
/*
 * upLinkHandler.h
 *
 * Created: 08/12/2021 13.01.08
 *  Author: Hadi
 */ 

#pragma once

#include <stddef.h>
#include <stdio.h>
#include <ATMEGA_FreeRTOS.h>
#include <lora_driver.h>
#include <status_leds.h>

#include "portmacro.h"
#include "message_buffer.h"

void lora_upLinkHandler_initialize(UBaseType_t lora_handler_task_priority,UBaseType_t stack,MessageBufferHandle_t messageBuffer);
void lora_upLinkHandler_task(MessageBufferHandle_t messageBuffer);
void lora_upLinkHandler_initiateTask(void* messageBuffer);
