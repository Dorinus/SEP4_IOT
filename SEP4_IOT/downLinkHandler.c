/*
 * downLinkHandler.c
 *
 * Created: 09/12/2021 11.25.30
 *  Author: Hadi
 */
#include "downLinkHandler.h"

static lora_driver_payload_t _downlink_payload;

void lora_downLinkHandler_initiateTask(void* messageBuffer){
for(;;)
{
	lora_downLinkHandler_task((MessageBufferHandle_t)messageBuffer);
}
}

void lora_downLinkHandler_initialize(UBaseType_t lora_downLinkHandler_task_priority, 
UBaseType_t stack, MessageBufferHandle_t messageBuffer)
{
	xTaskCreate(
	lora_downLinkHandler_task
	,  "LRULHand"  // A name just for humans
	,  configMINIMAL_STACK_SIZE+200  // This stack size can be checked & adjusted by reading the Stack Highwater
	,  (void*) messageBuffer
	,  lora_downLinkHandler_task_priority  // Priority, with 3 (configMAX_PRIORITIES - 1) being the highest, and 0 being the lowest.
	,  NULL );
}

void lora_downLinkHandler_task(MessageBufferHandle_t messageBuffer)
{
	xMessageBufferReceive(messageBuffer,(void*)&_downlink_payload,sizeof(lora_driver_payload_t),portMAX_DELAY);
	printf("PayLoad bytes are: %d, %d\n Payload lenght is %d\n", _downlink_payload.bytes[0] ,_downlink_payload.bytes[1],_downlink_payload.len);
	
	if (_downlink_payload.len == 2)
	{
		return NULL;
	}
}