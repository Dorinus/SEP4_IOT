#pragma once
#include <ATMEGA_FreeRTOS.h>
#include <rc_servo.h>

void servo_initialise();
void servo_open_window();
void servo_close_window();
void servo_half_opened_window();