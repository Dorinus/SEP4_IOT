#include <ATMEGA_FreeRTOS.h>
#include <stdbool.h>
#include <stdio.h>
#include <stddef.h>
#include <stdlib.h>
#include "controller.h"
#include <semphr.h>

static bool _working;
static bool _ventilation;
static SemaphoreHandle_t _semaphore;

void controller_initialise() {
	_working = malloc(sizeof(bool));
	_ventilation = malloc(sizeof(bool));

	_working = true;
	_ventilation = false;

	_semaphore = xSemaphoreCreateMutex();
}

void controller_setWorking(bool value) {
	if (_semaphore != NULL) {
		xSemaphoreTake(_semaphore, portMAX_DELAY);
		_working = value;
		xSemaphoreGive(_semaphore);
	}
	else _working = value;
}

void controller_getWorking(bool* working) {
	if (_semaphore != NULL) {
		xSemaphoreTake(_semaphore, portMAX_DELAY);
		*working = _working;
		xSemaphoreGive(_semaphore);
	}
	else *working = _working;
}

void controller_getVentilation(bool* ventilation) {
	if (_semaphore != NULL) {
		xSemaphoreTake(_semaphore, portMAX_DELAY);
		*ventilation = _ventilation;
		xSemaphoreGive(_semaphore);
	}
	else *ventilation = _ventilation;
}

void controller_setVentilation(bool value) {
	if (_semaphore != NULL) {
		xSemaphoreTake(_semaphore, portMAX_DELAY);
		_ventilation = value;
		xSemaphoreGive(_semaphore);
	}
	else _ventilation = value;
}