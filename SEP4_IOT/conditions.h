/**
\author Dorin Chira
**/

#pragma once
#include <stdint.h>
#include <lora_driver.h>

typedef struct conditions* conditions_t;

conditions_t conditions_initialisation();
void conditions_set_co2(conditions_t self, uint16_t value);
void conditions_set_humidity(conditions_t self, uint16_t value);
void conditions_set_temperature(conditions_t self, int16_t value);
void conditions_destroy(conditions_t self);
uint16_t conditions_get_co2(conditions_t self);
int16_t conditions_get_temperature(conditions_t self);
uint16_t conditions_get_humidity(conditions_t self);
lora_driver_payload_t get_conditions_payload(conditions_t self);
lora_driver_payload_t get_conditions_simple_payload(conditions_t self);



