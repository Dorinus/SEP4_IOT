#pragma once
#include <ATMEGA_FreeRTOS.h>
#include <task.h>
#include <event_groups.h>
#include <message_buffer.h>


#include "conditions.h"
#include "co2Reader.h"
#include "humidityTemperatureReader.h"
#include <stdint.h>

typedef struct device* device_t;

device_t device_create(UBaseType_t priority, 
						UBaseType_t stack, 
						EventGroupHandle_t event_group_start_measure, 
						EventBits_t bit_start_measure,
						EventGroupHandle_t event_group_ready, 
						EventBits_t bit_ready, 
						co2Reader_t co2Reader,
						humTempReader_t humAndTempReader,
						MessageBufferHandle_t uplinkMessageBuffer);
void device_executeTask(void* self);
void device_startMeasuring(device_t self);
