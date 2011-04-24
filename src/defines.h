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

// Events
	// Money events
	#define ADD_50 						0
	#define ADD_100 					1
	#define FROM_ACCOUNT 				2
	#define ACCOUNT_NUMBER_ENTERED		4
	#define ACCOUNT_PIN_ENTERED			5
	
	// Choosing product
	#define PRODUCT_BUTTON_CLICKED	 	6
	
	// Filling
	#define HANDLE_LIFTED			 	7
	#define HANDLE_REPLACED 			8
	#define MONEY_USED					9
	#define PUMP_STOPPED				10
	
	// Timeouts
	#define BUY_TIMEOUT 				11
	
	// Administrative events
	#define UART_SET_PRICE 				12
	#define UART_GET_REPORT 			13

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
	#define EVENT_ACCOUNT_RECEIVED		3
	#define EVENT_PIN_RECEIVED			4
	#define EVENT_PRODUCT_CLICK			5
	#define EVENT_HANDLE_LIFTET			6
	#define EVENT_CLOSE_TO_TARGET		7
	#define EVENT_LITTLE_TIMEOUT		8 // Determines when to jump from 40% to ramp up while pumping
	#define EVENT_BIG_TIMEOUT			9 // 15sec "global" timeout
	#define EVENT_PUMP_AT_100			10
	#define EVENT_HANDLE_REPLACED		11
	#define EVENT_RECEIPT_TIMEOUT		12 // How long should the receipt be shown?
	

/*****************************   Constants   *******************************/

/*****************************   Variables   *******************************/

// Structures
struct uart_command
		{
			INT8U command;	// From the list of defines above
			INT8U product;	// Only used when setting price
			INT16U price;	// Only used when setting price
		};
		
struct gasstation_event		// This might not need to be a struct
		{
			INT8U event;
		};

// all mutex used in this c program
extern xSemaphoreHandle lcd_buffer_mutex;
extern xSemaphoreHandle lcd_keyboard_port_mutex;

// all queues in this c code
extern xQueueHandle uart_output_queue;
extern xQueueHandle uart_input_queue;

/*****************************   Functions   *******************************/

/****************************** End Of Module *******************************/
#endif
