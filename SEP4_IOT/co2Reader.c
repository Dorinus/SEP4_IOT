#include "co2Reader.h"

typedef struct CO2Reader
{
	uint16_t co2_value;
	TaskHandle_t co2_task_handle;
}CO2Reader;

static EventGroupHandle_t _event_group_start_measure;
static EventBits_t _bit_start_measure;
static EventGroupHandle_t _event_group_ready;
static EventBits_t _bit_ready;

void co2Reader_execute_task(void* self)
{
	
	for(;;)
		co2Reader_measure((co2Reader_t)self);
}

co2Reader_t co2Reader_create(UBaseType_t priority,
							UBaseType_t stack,
							EventGroupHandle_t event_group_start_measure,
							EventBits_t bit_start_measure,
							EventGroupHandle_t event_group_ready,
							EventBits_t bit_ready)
{
	co2Reader_t _new_reader = malloc(sizeof(CO2Reader));
	if(_new_reader == NULL)
		return NULL;
		
	_new_reader->co2_value = 0;
	
	_event_group_start_measure = event_group_start_measure;
	_bit_start_measure = bit_start_measure;
	_event_group_ready = event_group_ready;
	_bit_ready = bit_ready;
	
	mh_z19_initialise(ser_USART3);
	
	xTaskCreate(
		co2Reader_execute_task,
		"co2Reader",
		stack + 200,
		_new_reader,
		priority,
		&_new_reader->co2_task_handle
	);
	
	return _new_reader;				
								
}

void co2Reader_destroy(co2Reader_t self)
{
	if(self != NULL)
		return;
	vTaskDelete(self->co2_task_handle);
	vPortFree(self);	
}

void co2Reader_measure(co2Reader_t self)
{
	EventBits_t uxBits = xEventGroupWaitBits(_event_group_start_measure, _bit_start_measure, pdTRUE, pdTRUE, portMAX_DELAY);
	
	if((uxBits & (_bit_start_measure)) == (_bit_start_measure))
	{
		mh_z19_returnCode_t co2_measurement_return_code = mh_z19_takeMeassuring();
		vTaskDelay(350);
		puts("CO2");
		if(co2_measurement_return_code == MHZ19_OK)
			mh_z19_getCo2Ppm(&self->co2_value);
		xEventGroupSetBits(_event_group_ready, _bit_ready);
	}
}

uint16_t co2reader_getCO2(co2Reader_t self)
{
	return self->co2_value;
}
