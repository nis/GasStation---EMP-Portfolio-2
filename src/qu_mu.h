/*****************************************************************************
* University of Southern Denmark
* Embedded Programming (EMP)
*
* MODULENAME.: emp.h
*
* PROJECT....: EMP
*
* DESCRIPTION: Test.
*
* Change Log:
******************************************************************************
* Date    Id    Change
* 
* --------------------
* 30/03/2011 NS    Module created.
*
*****************************************************************************/

#ifndef QU_MU_H_
#define QU_MU_H_

/***************************** Include files *******************************/

/*****************************    Defines    *******************************/

// all mutex used in this c program
extern xSemaphoreHandle lcd_buffer_mutex;
extern xSemaphoreHandle lcd_keyboard_port_mutex;

// all queues in this c code
extern xQueueHandle uart_output_queue;
extern xQueueHandle uart_input_queue;

/*****************************   Constants   *******************************/

/*****************************   Functions   *******************************/

/****************************** End Of Module *******************************/
#endif
