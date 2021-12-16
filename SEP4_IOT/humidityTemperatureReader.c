/**
\author Dorin Chira
**/

#include "humidityTemperatureReader.h"
#include <stdint.h>
#include <stdlib.h>
#include <stdbool.h>
#include <hih8120.h>
#include <ATMEGA_FreeRTOS.h>
#include "task.h"
#include "event_groups.h"
#include "portmacro.h"

static EventGroupHandle_t _event_group_start_measure;
static EventBits_t _bit_start_measure;

static EventGroupHandle_t _event_group_ready_measurment;
static EventBits_t _bit_ready;

typedef struct humidityTemperatureReader humidityTemperatureReader;

typedef struct humidityTemperatureReader {
	uint16_t humidity;
	int16_t temperature;
	TaskHandle_t handleTask;
}humidityTemperatureReader;

humTempReader_t humTempReader_create(UBaseType_t priority, 
										UBaseType_t stack,
										EventGroupHandle_t event_group_start_measure,
										EventBits_t bit_start_measure,
										EventGroupHandle_t event_group_ready_measurment, 
										EventBits_t bit_ready) {
	humTempReader_t _new_reader = malloc(sizeof(humidityTemperatureReader));
	if (_new_reader == NULL){
		return NULL;
	}
	

	_new_reader->humidity = 0;
	_new_reader->temperature = 0;

	_event_group_start_measure = event_group_start_measure;
	_bit_start_measure = bit_start_measure;

	_event_group_ready_measurment = event_group_ready_measurment;
	_bit_ready = bit_ready;

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
	EventBits_t uxBits = xEventGroupWaitBits(_event_group_start_measure,
	_bit_start_measure,
	pdTRUE,
	pdTRUE,
	portMAX_DELAY);

	if ((uxBits & (_bit_start_measure)) == (_bit_start_measure)) {
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
		xEventGroupSetBits(_event_group_ready_measurment, _bit_ready);
	}
}

uint16_t humTempReader_getHumidity(humTempReader_t self){
	return self->humidity;
}

int16_t humTempReader_getTemperature(humTempReader_t self){
	return self->temperature;
}

