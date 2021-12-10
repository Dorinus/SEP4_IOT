/*
 * downLinkHandler.h
 *
 * Created: 09/12/2021 11.25.51
 *  Author: Hadi
 */ 
#pragma once

#include "ATMEGA_FreeRTOS.h"
#include "portmacro.h"
#include "message_buffer.h"
#include <lora_driver.h>

void lora_downLinkHandler_initialize(UBaseType_t lora_downLinkHandler_task_priority,
UBaseType_t stack, MessageBufferHandle_t messageBuffer);
void lora_downLinkHandler_task(MessageBufferHandle_t messageBuffer);
void lora_downLinkHandler_initiateTask(void* messageBuffer);