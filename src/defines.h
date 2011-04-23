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
	
	// Timeouts
	#define BUY_TIMEOUT 				10
	
	// Administrative events
	#define UART_SET_PRICE 				11
	#define UART_GET_REPORT 			12

// Products
#define OCTANE_92  0
#define OCTANE_95  1
#define OCTANE_85  2

/*****************************   Constants   *******************************/

/*****************************   Variables   *******************************/

// all mutex used in this c program
extern xSemaphoreHandle lcd_buffer_mutex;
extern xSemaphoreHandle lcd_keyboard_port_mutex;

// all queues in this c code
extern xQueueHandle uart_output_queue;
extern xQueueHandle uart_input_queue;

/*****************************   Functions   *******************************/

/****************************** End Of Module *******************************/
#endif
