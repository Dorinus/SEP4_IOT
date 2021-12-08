#include "humidityTemperatureReader.h"
#include <stdint.h>
#include <stdlib.h>
#include <stdbool.h>
#include <hih8120.h>
#include <ATMEGA_FreeRTOS.h>
#include "task.h"
#include "event_groups.h"
#include "portmacro.h"

static EventGroupHandle_t _startMeasureEventGroup;
static EventBits_t _startMeasureBit;

static EventGroupHandle_t _readyMeasuringEventGroup;
static EventBits_t _readyBit;

typedef struct humidityTemperatureReader humidityTemperatureReader;

typedef struct humidityTemperatureReader {
	uint16_t humidity;
	int16_t temperature;
	TaskHandle_t handleTask;
}humidityTemperatureReader;

humTempReader_t humTempReader_create(UBaseType_t priority, 
										UBaseType_t stack,
										EventGroupHandle_t startMeasureEventGroup,
										EventBits_t startMeasureBit,
										EventGroupHandle_t readyEventGroup, 
										EventBits_t readyBit) {
	humTempReader_t _new_reader = calloc(1, sizeof(humidityTemperatureReader));
	if (_new_reader == NULL){
		return NULL;
	}
	

	_new_reader->humidity = 0;
	_new_reader->temperature = 0;

	_startMeasureEventGroup = startMeasureEventGroup;
	_startMeasureBit = startMeasureBit;

	_readyMeasuringEventGroup = readyEventGroup;
	_readyBit = readyBit;

	hih8120_initialise();
	
	if ( HIH8120_OK == hih8120_initialise() )
	{
		xTaskCreate(
		humTempReader_executeTask,
		"HumAndTempReader",
		stack + 200,
		_new_reader,
		priority,
		&_new_reader->handleTask
		);
	}
	
	return _new_reader;
}

void humTempReader_destroy(humTempReader_t self) {
	if (self == NULL)
	return;
	vTaskDelete(self->handleTask);
	vPortFree(self);
}

void humTempReader_executeTask(void* self) {
	for (;;) {
		humTempReader_measure((humTempReader_t)self);
	}
}

void humTempReader_measure(humTempReader_t self) {
	EventBits_t uxBits = xEventGroupWaitBits(_startMeasureEventGroup,
	_startMeasureBit,
	pdTRUE,
	pdTRUE,
	portMAX_DELAY);

	if ((uxBits & (_startMeasureBit)) == (_startMeasureBit)) {
		hih8120_wakeup();
		vTaskDelay(50);
		if ( HIH8120_OK != hih8120_wakeup() )
		{
			for(;;){
				hih8120_wakeup();
				vTaskDelay(50);
			}
		}	
		hih8120_measure();
		vTaskDelay(5);
		if(hih8120_isReady())
		{
			self->temperature = hih8120_getTemperature_x10();
			self->humidity = hih8120_getHumidityPercent_x10();			
		}
		vTaskDelay(1000);
		xEventGroupSetBits(_readyMeasuringEventGroup, _readyBit);
	}
}

uint16_t humTempReader_getHumidity(humTempReader_t self){
	return self->humidity;
}

int16_t humTempReader_getTemperature(humTempReader_t self){
	return self->temperature;
}

