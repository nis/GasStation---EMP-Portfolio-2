/*****************************************************************************
* Odense University College of Enginerring
* Embedded C Programming (ECP)
*
* MODULENAME.: defines.h
*
* PROJECT....: ECP
*
* DESCRIPTION:
*
* Change Log:
******************************************************************************
* Date    Id    Change
* YYMMDD
* --------------------
* 050128  NS    Module created.
*
*****************************************************************************/

#ifndef _DEFINES_H
  #define _DEFINES_H

/***************************** Include files *******************************/

/*****************************    Defines    *******************************/

// Pump states
	#define PUMP_IDLE 					0
	#define PUMP_SLOW 					1
	#define PUMP_RAMP_UP 				2
	#define PUMP_SS 					3
	#define PUMP_RAMP_DOWN 				4
	#define PUMP_TARGET_REACHED 		5

// Products
	#define OCTANE_92  					0
	#define OCTANE_95  					1
	#define OCTANE_85  					2
	
// UART commands
	#define UART_SET_PRICE				0
	#define UART_GET_REPORT				1
	
// Gasstation events
	#define EVENT_ACCOUNT_CLICK			0
	#define EVENT_ADD_50_KR				1
	#define EVENT_ADD_100_KR			2
	#define EVENT_KEYBOARD_DIGIT		3
	//#define EVENT_PIN_DIGIT				4
	#define EVENT_PRODUCT_CLICK			5
	#define EVENT_HANDLE_LIFTET			6
	#define EVENT_CLOSE_TO_TARGET		7
	#define EVENT_LITTLE_TIMEOUT		8 // Determines when to jump from 40% to ramp up while pumping
	#define EVENT_BIG_TIMEOUT			9 // 15sec "global" timeout
	#define EVENT_PUMP_AT_100			10
	#define EVENT_HANDLE_REPLACED		11
	#define EVENT_RECEIPT_TIMEOUT		12 // How long should the receipt be shown?
	
// Main states
	#define STATE_IDLE					0
	#define STATE_ENTER_ACCOUNT			1
	#define STATE_ENTER_PIN				2
	#define STATE_ADD_MONEY				3
	#define STATE_CHOOSE_PRODUCT		4
	#define STATE_PUMP_SLOW 			5
	#define STATE_PUMP_RAMP_UP 			6
	#define STATE_PUMP_SS 				7
	#define STATE_PUMP_RAMP_DOWN 		8
	#define STATE_SHUTDOWN_PUMP			9
	#define STATE_RECEIPT			 	10
	
// Lineitem defines
	#define PAYMENT_CASH				0
	#define PAYMENT_ACCOUNT				1

/*****************************   Constants   *******************************/

/*****************************   Variables   *******************************/

// Structures
typedef struct {INT8U command; INT8U product; INT16U price;} uart_command;
typedef struct {INT8U event; INT16U key; INT16U value;} gasstation_event;
typedef struct {INT8U pay_method; 
				INT32U money; 
				INT16U price; 
				INT32U account; 
				INT16U pin; 
				INT8U product; 
				INT16U pumped;
				INT32U time;
				} lineitem;
typedef struct {INT8U product; INT16U price;} product_price;


// all mutex used in this c program
extern xSemaphoreHandle lcd_buffer_mutex;
extern xSemaphoreHandle lcd_keyboard_port_mutex;

// all queues in this c code
extern xQueueHandle uart_output_queue;
extern xQueueHandle uart_command_queue;
extern xQueueHandle event_queue;

extern xTaskHandle ALIVE_TASK;
extern xTaskHandle LCD_TASK;
extern xTaskHandle BUTTON_TASK;
extern xTaskHandle PWM_TASK;
extern xTaskHandle FAN_TASK;
extern xTaskHandle KEYBOARD_TASK;
extern xTaskHandle UART0_SEND_TASK;
extern xTaskHandle UART0_RECEIVE_TASK;
extern xTaskHandle GASSTATION_CONTROLLER;

/*****************************   Functions   *******************************/

/****************************** End Of Module *******************************/
#endif
