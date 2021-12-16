/**
\author Dorin Chira
**/

#pragma once
#include <ATMEGA_FreeRTOS.h>
#include <stdbool.h>
#include <semphr.h>

void controller_initialise();

void controller_setWorking(bool value);
void controller_getWorking(bool* working);

void controller_setVentilation(bool value);
void controller_getVentilation(bool* ventilation);