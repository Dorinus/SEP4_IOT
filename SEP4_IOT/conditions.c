/**
\author Dorin Chira
**/

#include <stdlib.h>
#include <stdint.h>
#include "conditions.h"
#include <lora_driver.h>

typedef struct conditions conditions;

typedef struct conditions {
	uint16_t humidity;
	uint16_t co2;
	int16_t temperature;	
}conditions;


conditions_t conditions_initialisation() {
	conditions_t _new_conditions = malloc(sizeof(conditions_t));
	if (_new_conditions == NULL)
	return NULL;
	_new_conditions->co2 = 0;
	_new_conditions->temperature = 0;
	_new_conditions->humidity = 0;	
	return _new_conditions;
}

void conditions_set_co2(conditions_t self, uint16_t value) {
	if(self!=NULL)
	{
		if(value >= 200 && value <= 10000)
		self->co2 = value;
	}
}


void conditions_set_humidity(conditions_t self, uint16_t value) {
	if (self != NULL)
	{
		if (value >= 0 && value <= 1000)
		{
			self->humidity= value;
		}
	}	
}

void conditions_set_temperature(conditions_t self, int16_t value) {
	if (self != NULL)
	{
		if (value >= -400 && value <= 550)
		{
			self->temperature = value;
		}
	}
}

void conditions_destroy(conditions_t self) {
	if(self == NULL)
	return;
	vPortFree(self);
}

// for testing
uint16_t conditions_get_co2(conditions_t self)
{
	return self->co2;
}

int16_t conditions_get_temperature(conditions_t self)
{
	return self->temperature;
}

uint16_t conditions_get_humidity(conditions_t self)
{
	return self->humidity;
}

lora_driver_payload_t get_conditions_payload(conditions_t self)
{
	lora_driver_payload_t payload;
	payload.portNo = 1;
	payload.len = 6;
	
	int16_t tempData = self->temperature;
	
	payload.bytes[0] = tempData >> 8;
	payload.bytes[1] = tempData & 0xFF;

	payload.bytes[2] = self->humidity >> 8;
	payload.bytes[3] = self->humidity & 0xFF;
	
	payload.bytes[4] = self->co2 >> 8;
	payload.bytes[5] = self->co2 & 0xFF;
	
	return payload;	
}

lora_driver_payload_t get_conditions_simple_payload(conditions_t self)
{
	lora_driver_payload_t payload;
	payload.portNo = 1;
	payload.len = 2;
	
	uint16_t one = 0;
	payload.bytes[0] = one >> 6;
	payload.bytes[1] = one & 0xFF;	
	return payload;
}