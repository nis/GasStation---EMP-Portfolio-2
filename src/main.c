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
#include "buttons/buttons.h"
#include "pwm/pwm.h"
#include "fan/fan.h"
#include "defines.h"
#include "uart/uart.h"
#include "keyboard/keyboard.h"
#include "rtc/rtc.h"
#include "gasstation_controller/gasstation_controller.h"

#define USERTASK_STACK_SIZE configMINIMAL_STACK_SIZE

// all mutex used in this c program
xSemaphoreHandle lcd_buffer_mutex;
xSemaphoreHandle lcd_keyboard_port_mutex;

// All queues
xQueueHandle uart_output_queue;
xQueueHandle uart_command_queue;
xQueueHandle event_queue;

xTaskHandle ALIVE_TASK;
xTaskHandle LCD_TASK;
xTaskHandle BUTTON_TASK;
xTaskHandle PWM_TASK;
xTaskHandle FAN_TASK;
xTaskHandle KEYBOARD_TASK;
xTaskHandle UART0_SEND_TASK;
xTaskHandle UART0_RECEIVE_TASK;
xTaskHandle GASSTATION_CONTROLLER;

void __error__(char *pcFilename, unsigned long ulLine) {
}

static void setupHardware(void) {
	
	disable_global_int();
	clk_system_init();
	init_leds();
	init_lcd_write_task();
	init_buttons();
	init_pwm();
	init_fan();
	init_keyboard();
	init_uart0();

	enable_global_int();
}

/**
 * Alive task. Blinks the status LED at 4Hz.
 */
void alive_task(void *pvParameters)
{
	while (1)
	{
		led_status_toggle();
		vTaskDelay(250);
	}
}

/**
 * LCD update task
 */
void lcd_task(void *pvParameters)
{
	while (1)
	{
		lcd_write_task();
		vTaskDelay(10);
	}
}

/**
 * Button task
 */
void button_task_runner(void *pvParameters)
{
	while (1)
	{
		button_task();
		vTaskDelay(20);
	}
}

/**
 * Fan task
 */
void fan_task_runner(void *pvParameters)
{
	while (1)
	{
		fan_task();
		vTaskDelay(100);
	}
}

/**
 * PWM task
 */
void pwm_task_runner(void *pvParameters)
{
	while (1)
	{
		pwm_task();
		vTaskDelay(100);
	}
}

/**
 * Keyboard task
 */
void keyboard_task_runner(void *pvParameters)
{
	while (1)
	{
		keyboard_task();
		vTaskDelay(50);
	}
}

/**
 * UART0 send task
 */
void uart0_send_task_runner(void *pvParameters)
{
	while (1)
	{
		uart0_send_task();
		vTaskDelay(10);
	}
}

/**
 * UART0 receive task
 */
void uart0_receive_task_runner(void *pvParameters)
{
	while (1)
	{
		uart0_receive_task();
		vTaskDelay(10);
	}
}

/**
 * RTC task
 */
void rtc_task_runner(void *pvParameters)
{
	while (1)
	{
		rtc_task();
		vTaskDelay(100);
	}
}

/**
 * Grand Controller task.
 */
void gasstation_controller_runner(void *pvParameters)
{
	gasstation_controller_init();
	
	while (1)
	{
		gasstation_controller_task();
		vTaskDelay(10);
	}
}

/**
 * Program entry point 
 */
int main(void) {
	setupHardware();
	portBASE_TYPE create_check = pdTRUE;

	/* 
	 * Start the tasks defined within this file/specific to this demo. 
	 */
	create_check = xTaskCreate( alive_task, 					( signed portCHAR * ) "ALIVE_TASK"				, USERTASK_STACK_SIZE, NULL, tskIDLE_PRIORITY, NULL );
	if (create_check != pdTRUE)
	{
		led_red_on();
		while(1);
	}
	
	create_check = xTaskCreate( lcd_task, 						( signed portCHAR * ) "LCD_TASK"				, USERTASK_STACK_SIZE, NULL, tskIDLE_PRIORITY, NULL );
	if (create_check != pdTRUE)
	{
		led_red_on();
		while(1);
	}
	
	create_check = xTaskCreate( button_task_runner, 			( signed portCHAR * ) "BUTTON_TASK"				, USERTASK_STACK_SIZE, NULL, tskIDLE_PRIORITY, NULL );
	if (create_check != pdTRUE)
	{
		led_red_on();
		while(1);
	}
	
	create_check = xTaskCreate( pwm_task_runner, 				( signed portCHAR * ) "PWM_TASK"				, USERTASK_STACK_SIZE, NULL, tskIDLE_PRIORITY, NULL );
	if (create_check != pdTRUE)
	{
		led_red_on();
		while(1);
	}
	
	create_check = xTaskCreate( fan_task_runner, 				( signed portCHAR * ) "FAN_TASK"				, USERTASK_STACK_SIZE, NULL, tskIDLE_PRIORITY, NULL );
	if (create_check != pdTRUE)
	{
		led_red_on();
		while(1);
	}
	
	create_check = xTaskCreate( keyboard_task_runner, 			( signed portCHAR * ) "KEYBOARD_TASK"			, USERTASK_STACK_SIZE, NULL, tskIDLE_PRIORITY, NULL );
	if (create_check != pdTRUE)
	{
		led_red_on();
		while(1);
	}
	
	create_check = xTaskCreate( uart0_send_task_runner, 		( signed portCHAR * ) "UART0_SEND_TASK"			, USERTASK_STACK_SIZE, NULL, tskIDLE_PRIORITY, NULL );
	if (create_check != pdTRUE)
	{
		led_red_on();
		while(1);
	}
	
	create_check = xTaskCreate( uart0_receive_task_runner, 	( signed portCHAR * ) "UART0_RECEIVE_TASK"		, USERTASK_STACK_SIZE, NULL, tskIDLE_PRIORITY, NULL );
	if (create_check != pdTRUE)
	{
		led_red_on();
		while(1);
	}
	
	create_check = xTaskCreate( rtc_task_runner, 	( signed portCHAR * ) "RTC_TASK"		, USERTASK_STACK_SIZE, NULL, tskIDLE_PRIORITY, NULL );
	if (create_check != pdTRUE)
	{
		led_red_on();
		while(1);
	}
	
	create_check = xTaskCreate( gasstation_controller_runner, 	( signed portCHAR * ) "GASSTATION_CONTROLLER"	, 85, NULL, tskIDLE_PRIORITY, NULL );
	if (create_check != pdTRUE)
	{
		led_red_on();
		while(1);
	}
	
	
	/* 
	 * Setup semaphores.
	 */
	lcd_buffer_mutex = xSemaphoreCreateMutex();
	if ( lcd_buffer_mutex == NULL )
	{
		led_red_on();
		while(1);
	}
	
	lcd_keyboard_port_mutex = xSemaphoreCreateMutex();
	if ( lcd_keyboard_port_mutex == NULL )
	{
		led_red_on();
		while(1);
	}
	
	/* 
	 * Setup queues.
	 */
	uart_output_queue = xQueueCreate(128, sizeof( INT8U ) );
	if (uart_output_queue == NULL)
	{
		led_red_on();
		while(1);
	}
	
	uart_command_queue = xQueueCreate(16, sizeof( uart_command ) );
	if (uart_command_queue == NULL)
	{
		led_red_on();
		while(1);
	}
	
	event_queue = xQueueCreate(16, sizeof( gasstation_event ) );
	if (event_queue == NULL)
	{
		led_red_on();
		while(1);
	}
	
	
	/* 
	 * Start the scheduler. 
	 */
	vTaskStartScheduler();

	/* 
	 * Will only get here if there was insufficient memory to create the idle task. 
	 */
	return 1;
}
