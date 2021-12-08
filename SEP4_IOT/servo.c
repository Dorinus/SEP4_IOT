#include <ATMEGA_FreeRTOS.h>
#include <rc_servo.h>
#include "servo.h"

// TODO initialize by using the servoNr

// It is possible to use only 0 - J14 or 1 - J13 as servoNr
void servo_initialise()
{
	rc_servo_initialise();
}

void servo_open_window()
{
	rc_servo_setPosition(1, 100);
}
void servo_close_window()
{
	rc_servo_setPosition(1, -100);
}

void servo_half_opened_window(){
	rc_servo_setPosition(1, 0);
}