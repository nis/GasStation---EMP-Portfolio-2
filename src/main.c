/**
 * Name        : main.c
 * Version     :
 * Description : main definition for FreeRTOS application
 */

/*
 * FreeRTOS includes
 */
#include "FreeRTOS.h"
#include "Task.h"
#include "queue.h"
#include "semphr.h"
#include "cpu/cpu.h"
#include "led/led.h"
#include "lcd/lcd.h"

#define USERTASK_STACK_SIZE configMINIMAL_STACK_SIZE

void __error__(char *pcFilename, unsigned long ulLine) {
}

static void setupHardware(void) {
	
	disable_global_int();
	clk_system_init();
	init_leds();
	init_lcd_write_task();
	
	enable_global_int();
}

/**
 * Alive task. Blinks the status LED at 4Hz.
 */
void vUserTask1(void *pvParameters) {
	while (1) {
		led_status_toggle();
		vTaskDelay(250) ;
/**
 * LCD update task
 */
void vUserTask2(void *pvParameters)
{
	while (1)
	{
		lcd_write_task();
		vTaskDelay(10);
	}
}

/**
 * Simple task that increments a counter
 */
// void vUserTask2(void *pvParameters) {
// 	int count = 0;
// 	while (1) {
// 		count++;
// 		vTaskDelay(101) ;
// 	}
// }

/**
 * Program entry point 
 */
int main(void) {
	setupHardware();

	/* 
	 * Start the tasks defined within this file/specific to this demo. 
	 */
	xTaskCreate( vUserTask1, ( signed portCHAR * ) "Task1", USERTASK_STACK_SIZE, NULL, tskIDLE_PRIORITY, NULL );
	xTaskCreate( vUserTask2, ( signed portCHAR * ) "Task2", USERTASK_STACK_SIZE, NULL, tskIDLE_PRIORITY, NULL );

	/* 
	 * Start the scheduler. 
	 */
	vTaskStartScheduler();

	/* 
	 * Will only get here if there was insufficient memory to create the idle task. 
	 */
	return 1;
}
