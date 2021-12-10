#pragma once
#include <stdint.h>
#include <stdlib.h>
#include <ATMEGA_FreeRTOS.h>
#include "event_groups.h"
#include "portmacro.h"

typedef struct humidityTemperatureReader* humTempReader_t;

humTempReader_t humTempReader_create(UBaseType_t priority,
										UBaseType_t stack,
										EventGroupHandle_t event_group_start_measure,
										EventBits_t bit_start_measure,
										EventGroupHandle_t event_group_ready_measurment,
										EventBits_t bit_ready);
void humTempReader_destroy(humTempReader_t self);
void humTempReader_executeTask(void* self);
void humTempReader_measure(humTempReader_t self);
uint16_t humTempReader_getHumidity(humTempReader_t self);
int16_t humTempReader_getTemperature(humTempReader_t self);
