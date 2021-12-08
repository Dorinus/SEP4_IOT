#pragma once

#include <ATMEGA_FreeRTOS.h>
#include "task.h"
#include "event_groups.h"

#include <stdint.h>
#include <stdlib.h>
#include <mh_z19.h>


typedef struct CO2Reader* co2Reader_t;

co2Reader_t co2Reader_create(UBaseType_t priority,
							UBaseType_t stack,
							EventGroupHandle_t event_group_start_measure,
							EventBits_t bit_start_measure,
							EventGroupHandle_t event_group_ready,
							EventBits_t bit_ready);
							
void co2Reader_destroy(co2Reader_t self);
void co2Reader_execute_task(void* self);
void co2Reader_measure(co2Reader_t self);
uint16_t co2reader_getCO2(co2Reader_t self);
