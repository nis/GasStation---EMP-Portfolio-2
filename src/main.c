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
#include "qu_mu.h"
#include "uart/uart.h"

#define USERTASK_STACK_SIZE configMINIMAL_STACK_SIZE

// all mutex used in this c program
xSemaphoreHandle lcd_buffer_mutex;
xSemaphoreHandle lcd_keyboard_port_mutex;

// All queues
xQueueHandle uart_output_queue;
xQueueHandle uart_input_queue;

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
 * Working task.
 */
void vUserTask3(void *pvParameters)
{
	INT8U speed;
	while (1)
	{
		fan_set_speed(speed);
		write_3_char_int_to_buffer (0, 0, speed);
		write_3_char_int_to_buffer (0, 1, fan_get_pulse_count());
		
		if(speed >= 100)
		{
			speed = 0;
		} else {
			speed++;
		}
		
		vTaskDelay(100);
	}
}

/**
 * Program entry point 
 */
int main(void) {
	setupHardware();
	

	/* 
	 * Start the tasks defined within this file/specific to this demo. 
	 */
	xTaskCreate( alive_task, 				( signed portCHAR * ) "ALIVE_TASK"		, USERTASK_STACK_SIZE, NULL, tskIDLE_PRIORITY, NULL );
	xTaskCreate( lcd_task, 					( signed portCHAR * ) "LCD_TASK"		, USERTASK_STACK_SIZE, NULL, tskIDLE_PRIORITY, NULL );
	xTaskCreate( button_task_runner, 		( signed portCHAR * ) "BUTTON_TASK"		, USERTASK_STACK_SIZE, NULL, tskIDLE_PRIORITY, NULL );
	xTaskCreate( pwm_task_runner, 			( signed portCHAR * ) "PWM_TASK"		, USERTASK_STACK_SIZE, NULL, tskIDLE_PRIORITY, NULL );
	xTaskCreate( fan_task_runner, 			( signed portCHAR * ) "FAN_TASK"		, USERTASK_STACK_SIZE, NULL, tskIDLE_PRIORITY, NULL );
	xTaskCreate( keyboard_task_runner, 		( signed portCHAR * ) "KEYBOARD_TASK"	, USERTASK_STACK_SIZE, NULL, tskIDLE_PRIORITY, NULL );
	xTaskCreate( uart0_send_task_runner, 	( signed portCHAR * ) "UART0_SEND_TASK"		, USERTASK_STACK_SIZE, NULL, tskIDLE_PRIORITY, NULL );
	xTaskCreate( uart0_receive_task_runner, ( signed portCHAR * ) "UART0_RECEIVE_TASK"		, USERTASK_STACK_SIZE, NULL, tskIDLE_PRIORITY, NULL );
	xTaskCreate( vUserTask3, 				( signed portCHAR * ) "Task3"			, USERTASK_STACK_SIZE, NULL, tskIDLE_PRIORITY, NULL );

	
	
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
	uart_output_queue = xQueueCreate(16, sizeof( INT8U ) );
	if (uart_output_queue == NULL)
	{
		led_red_on();
		while(1);
	}
	
	uart_input_queue = xQueueCreate(16, sizeof( INT8U ) );
	if (uart_input_queue == NULL)
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
