#include <stdio.h>
#include <avr/io.h>

#include <ATMEGA_FreeRTOS.h>
#include <FreeRTOSTraceDriver.h>
#include <task.h>
#include <semphr.h>

#include <stdio_driver.h>
#include <serial.h>

 // Needed for LoRaWAN
#include <lora_driver.h>
#include <status_leds.h>

 
 #include "device.h"
 #include "co2Reader.h"
 #include "HumidityTemperatureReader.h"
 #include "upLinkHandler.h"
 #include "downLinkHandler.h"
 #include "controller.h"
 #include "conditions.h"
 #include "servo.h"

// define handlers
SemaphoreHandle_t xTestSemaphore;
EventGroupHandle_t _event_group_start_measure;
EventGroupHandle_t _event_group_ready;
MessageBufferHandle_t message_buffer_uplink;
MessageBufferHandle_t message_buffer_downlink;
lora_driver_payload_t loraPayload;

//define bits
#define BIT_MEASURE_CO2 (1<<3)
#define BIT_MEASURE_HT (1<<0)
#define ALL_MEASURE_BIT (BIT_MEASURE_CO2|BIT_MEASURE_HT)

#define BIT_DONE_MEASURE_CO2 (1<<5)
#define BIT_DONE_MEASURE_HT (1<<4)
#define ALL_READY_BIT (BIT_DONE_MEASURE_CO2|BIT_DONE_MEASURE_HT)

/*-----------------------------------------------------------*/
void create_tasks_and_semaphores(void)
{
	_event_group_start_measure = xEventGroupCreate();
	_event_group_ready = xEventGroupCreate();
	message_buffer_uplink = xMessageBufferCreate(sizeof(lora_driver_payload_t)*2);
	message_buffer_downlink = xMessageBufferCreate(sizeof(lora_driver_payload_t)*2);
	printf("event groups initialized\n");
	
	controller_initialise();
	co2Reader_t co2Reader = co2Reader_create(tskIDLE_PRIORITY+2,configMINIMAL_STACK_SIZE,
	_event_group_start_measure,BIT_MEASURE_CO2,_event_group_ready,BIT_DONE_MEASURE_CO2);
	
	humTempReader_t humTempReader = humTempReader_create(tskIDLE_PRIORITY+2,configMINIMAL_STACK_SIZE,
	_event_group_start_measure,BIT_MEASURE_HT,_event_group_ready,BIT_DONE_MEASURE_HT);
	
	servo_initialise();
	
	device_create(tskIDLE_PRIORITY+4, configMINIMAL_STACK_SIZE,_event_group_start_measure, ALL_MEASURE_BIT, 
	 _event_group_ready, ALL_READY_BIT, co2Reader, humTempReader, message_buffer_uplink);
	
	lora_upLinkHandler_initialize(tskIDLE_PRIORITY + 1, configMINIMAL_STACK_SIZE,message_buffer_uplink);
	lora_downLinkHandler_initialize(tskIDLE_PRIORITY + 1, configMINIMAL_STACK_SIZE, message_buffer_downlink);
	
	/*
	// Semaphores are useful to stop a Task proceeding, where it should be paused to wait,
	// because it is sharing a resource, such as the Serial port.
	// Semaphores should only be used whilst the scheduler is running, but we can set it up here.
	if ( xTestSemaphore == NULL )  // Check to confirm that the Semaphore has not already been created.
	{
		xTestSemaphore = xSemaphoreCreateMutex();  // Create a mutex semaphore.
		if ( ( xTestSemaphore ) != NULL )
		{
			xSemaphoreGive( ( xTestSemaphore ) );  // Make the mutex available for use, by initially "Giving" the Semaphore.
		}
	}

	xTaskCreate(
	task1
	,  "Task1"  // A name just for humans
	,  configMINIMAL_STACK_SIZE  // This stack size can be checked & adjusted by reading the Stack Highwater
	,  NULL
	,  2  // Priority, with 3 (configMAX_PRIORITIES - 1) being the highest, and 0 being the lowest.
	,  NULL );

	xTaskCreate(
	task2
	,  "Task2"  // A name just for humans
	,  configMINIMAL_STACK_SIZE  // This stack size can be checked & adjusted by reading the Stack Highwater
	,  NULL
	,  1  // Priority, with 3 (configMAX_PRIORITIES - 1) being the highest, and 0 being the lowest.
	,  NULL );
	*/
}

/*-----------------------------------------------------------*/
void task1( void *pvParameters )
{
	TickType_t xLastWakeTime;
	const TickType_t xFrequency = 500/portTICK_PERIOD_MS; // 500 ms

	// Initialise the xLastWakeTime variable with the current time.
	xLastWakeTime = xTaskGetTickCount();

	for(;;)
	{
		xTaskDelayUntil( &xLastWakeTime, xFrequency );
		puts("Task1"); // stdio functions are not reentrant - Should normally be protected by MUTEX
		PORTA ^= _BV(PA0);
	}
	printf("task1 method");
}

/*-----------------------------------------------------------*/
void task2( void *pvParameters )
{
	TickType_t xLastWakeTime;
	const TickType_t xFrequency = 1000/portTICK_PERIOD_MS; // 1000 ms

	// Initialise the xLastWakeTime variable with the current time.
	xLastWakeTime = xTaskGetTickCount();

	for(;;)
	{
		xTaskDelayUntil( &xLastWakeTime, xFrequency );
		puts("Task2"); // stdio functions are not reentrant - Should normally be protected by MUTEX
		PORTA ^= _BV(PA7);
	}
	printf("task2 method");
}

/*-----------------------------------------------------------*/
void initialiseSystem()
{
	//printf("intialise started\n");
	// Set output ports for leds used in the example
	DDRA |= _BV(DDA0) | _BV(DDA7);

	// Make it possible to use stdio on COM port 0 (USB) on Arduino board - Setting 57600,8,N,1
	stdio_initialise(ser_USART0);
	// Let's create some tasks
	//create_tasks_and_semaphores();

	// vvvvvvvvvvvvvvvvv BELOW IS LoRaWAN initialisation vvvvvvvvvvvvvvvvvvvvvvvvvvvvvv
	// Status Leds driver
	status_leds_initialise(5); // Priority 5 for internal task
	// Initialise the LoRaWAN driver without down-link buffer
	lora_driver_initialise(1, NULL);
	// Create LoRaWAN task and start it up with priority 3
	lora_handler_initialise(3);
}

/*-----------------------------------------------------------*/
int main(void)
{
	initialiseSystem(); // Must be done as the very first thing!!
	create_tasks_and_semaphores();
	printf("Program Started!!\n");
	vTaskStartScheduler(); // Initialise and run the freeRTOS scheduler. Execution should never return from here.

	/* Replace with your application code */
	while (1)
	{
	}
}

