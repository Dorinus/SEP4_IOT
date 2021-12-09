#include <ATMEGA_FreeRTOS.h>
#include <stdint.h>
#include <stdlib.h>
#include "device.h"
#include <lora_driver.h>
#include <message_buffer.h>
#include "controller.h"
#include "servo.h"
#include "conditions.h"
#include "co2Reader.h"
#include "humidityTemperatureReader.h"
#include <stdio.h>
#include <avr/io.h>
#include <avr/sfr_defs.h>
#include <spi_iha.h>

static EventGroupHandle_t _startMeasureEventGroup;
static EventBits_t _startMeasureBit;

static EventGroupHandle_t _readyMeasuringEventGroup;
static EventBits_t _readyBit;

static MessageBufferHandle_t _uplinkmessageBuffer;

static lora_driver_payload_t _uplink_payload;

typedef struct device device;

typedef struct device { 
	co2Reader_t co2_reader;
	humTempReader_t hum_temper_reader;
	conditions_t conditions;
	TaskHandle_t handleTask;
}device;

void device_executeTask(void* self) {
	for (;;)
	{
		device_startMeasuring((device_t)self);
	}
}

device_t device_create(UBaseType_t priority, UBaseType_t stack, EventGroupHandle_t startMeasureEventGroup, EventBits_t startMeasureBit,
EventGroupHandle_t readyEventGroup, EventBits_t readyBit, co2Reader_t co2Reader, humTempReader_t humAndTempReader, MessageBufferHandle_t uplinkMessageBuffer){

	device_t _new_device = malloc(sizeof(device));
	if (_new_device == NULL)
	return NULL;

	_new_device->co2_reader = co2Reader;
	_new_device->hum_temper_reader = humAndTempReader;
	conditions_t currentCond = conditions_initialisation();
	_new_device->conditions = currentCond;

	_startMeasureEventGroup = startMeasureEventGroup;
	_startMeasureBit = startMeasureBit;

	_readyMeasuringEventGroup = readyEventGroup;
	_readyBit = readyBit;
	
	_uplinkmessageBuffer=uplinkMessageBuffer;

	xTaskCreate(
	device_executeTask,
	"Device",
	stack + 200,
	_new_device,
	priority,
	&_new_device->handleTask
	);

	return _new_device;
}

void device_startMeasuring(device_t self) {
	bool* works = pvPortMalloc(sizeof(bool));
	controller_getWorking(works);
	if (*works == true)
	{
		bool* ventilate = pvPortMalloc(sizeof(bool));
		controller_getVentilation(ventilate);
		if(*ventilate == true)
		{
			servo_open_window();
		}
		else
		{
			servo_close_window();
		}

		// start measuring
		xEventGroupSetBits(_startMeasureEventGroup, _startMeasureBit);

		//wait for sensors to read data
		EventBits_t uxBits = xEventGroupWaitBits(_readyMeasuringEventGroup,
		_readyBit,
		pdTRUE,
		pdTRUE,
		portMAX_DELAY);
		
		if ((uxBits & (_readyBit)) == (_readyBit)) {
			conditions_set_co2(self->conditions,co2reader_getCO2(self->co2_reader));
			
			conditions_set_temperature(self->conditions,humTempReader_getTemperature(self->hum_temper_reader));
			
			int16_t humidity = humTempReader_getHumidity(self->hum_temper_reader);
			conditions_set_humidity(self->conditions, humidity);
			
			
			printf("CO2 is: %u\n", co2reader_getCO2(self->co2_reader));
			printf("Temperature is: %d\n", humTempReader_getTemperature(self->hum_temper_reader));
			printf("Humidity is: %u\n", humTempReader_getHumidity(self->hum_temper_reader));
			
			/*Perhaps loraPayload is not a good idea to be here*/
			_uplink_payload = get_conditions_payload(self->conditions);
			
			xMessageBufferSend(_uplinkmessageBuffer,(void*) &_uplink_payload,sizeof(_uplink_payload),portMAX_DELAY);
			vTaskDelay(30000);
			
		}
		//if the device is not on, wait a bit and check if anything is changed
	}
	else
	{
		_uplink_payload = get_conditions_simple_payload(self->conditions);
		xMessageBufferSend(_uplinkmessageBuffer,(void*) &_uplink_payload,sizeof(_uplink_payload),portMAX_DELAY);
		vTaskDelay(30000);
	}
}
