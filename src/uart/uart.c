/*****************************************************************************
* Odense University College of Enginerring
* Embedded C Programming (ECP)
*
* MODULENAME.: uart.c
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

/***************************** Include files *******************************/
#include "inc/lm3s6965.h"
#include "../inc/emp_type.h"
#include "../inc/binary.h"
#include "uart.h"
#include "FreeRTOS.h"
#include "Task.h"
#include "queue.h"
#include "semphr.h"
#include "defines.h"
#include "defines.h"
#include "lcd/lcd.h"
#include "led/led.h"

/*****************************    Defines    *******************************/

#define UART_COMMAND_RECEIVE_TIMEOUT 200 	// How many ticks shoudl we wait before scrubbing the buffer?
											// 100 = 1 second. 200 = 2 seconds. Etc.

// UART states
#define UART_IDLE 0
#define UART_RECEIVE 1

/*****************************   Constants   *******************************/

/*****************************   Variables   *******************************/

INT8U uart_state = 0;
INT8U input_buffer[32];
INT8U buffer_pointer = 0;
INT8U input_timeout = 0;

/*****************************   Functions   *******************************/

INT8U uart_decipher_command( void)
{
	INT8U valid_command = 0;
	
	if(	(input_buffer[0] == 's') && (input_buffer[1] == 'e') && (input_buffer[2] == 't') && 
		(input_buffer[4] == '9' || input_buffer[4] == '8') &&
		(input_buffer[5] == '2' || input_buffer[5] == '5') &&
		(input_buffer[7] >= '0' && input_buffer[7] <= '9') &&
		(input_buffer[8] >= '0' && input_buffer[8] <= '9') &&
		(input_buffer[9] >= '0' && input_buffer[9] <= '9') &&
		(input_buffer[10] >= '0' && input_buffer[10] <= '9'))
	{
		lcd_add_string_to_buffer(0, 0, "Set ");
		lcd_add_char_to_buffer(4, 0, input_buffer[4]);
		lcd_add_char_to_buffer(5, 0, input_buffer[5]);
		
		// Find new price
		INT16U new_price = ((input_buffer[7] - 0x30)*1000) + ((input_buffer[8] - 0x30)*100) + ((input_buffer[9] - 0x30)*10) + (input_buffer[10] - 0x30);
		
		// Find which product to set the price for
		INT8U product;
		if(input_buffer[4] == '9' && input_buffer[5] == '2')
		{
			product = OCTANE_92;
		}
		
		if(input_buffer[4] == '9' && input_buffer[5] == '5')
		{
			product = OCTANE_95;
		}
		
		if(input_buffer[4] == '8' && input_buffer[5] == '5')
		{
			product = OCTANE_85;
		}
		
		// Set price of product to new_price
		// Send this command to the event queue.
		
		return 1;
	}
	
	if((input_buffer[0] == 'g') && (input_buffer[1] == 'e') && (input_buffer[2] == 't'))
	{
		// Send a report command to the event queue.
	}
	
	return valid_command;
}

void uart_clear_buffer( void )
/*****************************************************************************
*   Function : Clears the buffer and various other things.
*****************************************************************************/
{
	input_timeout = 0;
	buffer_pointer = 0;
	// Clear the buffer
	INT8U col;
	for (col=0; col < 32; col++)
	{
		input_buffer[col] = ' ';
	}
}

void uart0_receive_task()
/*****************************************************************************
*   Function : See module specification (.h-file).
*****************************************************************************/
{
	switch (uart_state)
	{
		case UART_IDLE:
			if(!(UART0_FR_R & UART_FR_RXFE))
			{
				uart_state = UART_RECEIVE;
			}
			break;
			
		case UART_RECEIVE:
			input_timeout++;
			if(input_timeout > UART_COMMAND_RECEIVE_TIMEOUT)
			{
				uart_clear_buffer();
			} else {
				if(!(UART0_FR_R & UART_FR_RXFE))
				{
					// reset timer
					input_timeout = 0;
					
					INT8U ch = 0;
					ch = UART0_DR_R;
					if(ch == 0xD)
					{
						uart_decipher_command();
						uart_state = UART_IDLE;
					} else {
						input_buffer[buffer_pointer] = ch;
						buffer_pointer++;
					}
				}
			}
			break;
	}
}

void uart_send_char(INT8U c)
/*****************************************************************************
*   Function : See module specification (.h-file).
*****************************************************************************/
{
	xQueueSend(uart_output_queue,&c, 0);
}

void uart_send_string(INT8S *str )
/*****************************************************************************
*   Function : See module specification (.h-file).
*****************************************************************************/
{
	while( *str )
	  {
		uart_send_char( *str );
		//xQueueSend(output_uart_queue,str, 0);
	    str++;
	  }
}

void uart_send_newline( void )
/*****************************************************************************
*   Function : See module specification (.h-file).
*****************************************************************************/
{
	uart_send_char('\n');
	uart_send_char('\r');
}

void uart0_send_task()
/*****************************************************************************
*   Function : See module specification (.h-file).
*****************************************************************************/
{
	INT8U ch = 0;
	xQueueReceive(uart_output_queue, &ch, portMAX_DELAY );
	while(UART0_FR_R & UART_FR_TXFF);
	UART0_DR_R = ch;
}

void init_uart0(void)
/*****************************************************************************
*   Function : See module specification (.h-file).
*****************************************************************************/
{
	INT8U dummy;
	
	//enable sysclok for uart0
	SYSCTL_RCGC1_R |= SYSCTL_RCGC1_UART0;
	dummy = SYSCTL_RCGC1_R;

	//Enable the pins used by the UART
	SYSCTL_RCGC2_R |= SYSCTL_RCGC2_GPIOA;
	dummy = SYSCTL_RCGC2_R;

	// PORTA [0:1] to alternative function
	SET_BIT_HIGH(GPIO_PORTA_AFSEL_R,0);
	SET_BIT_HIGH(GPIO_PORTA_AFSEL_R,1);

	// Enable digital function
	SET_BIT_HIGH(GPIO_PORTA_DEN_R,0);
	SET_BIT_HIGH(GPIO_PORTA_DEN_R,1);

	// Disable the UART0
	UART0_CTL_R &= ~(UART_CTL_UARTEN);

	// Clear UART pending interrupt.
	UART0_ICR_R |= 0x7F0;

	// set baud rate to 115200
	UART0_IBRD_R = 27;	// 50,000,000 / (16 * 115,200 ) = 27.1267 floored to 27
	UART0_FBRD_R = 8;  // integer(0.1267 * 64 + 0.5) = 8.6111 aprox to 8

	// uart length set to 8 bit, one stop bit
	UART0_LCRH_R |= UART_LCRH_WLEN_8;

	// enable the UART0, transmit and receive
	UART0_CTL_R |= UART_CTL_TXE; //write
	UART0_CTL_R |= UART_CTL_RXE; //read
	UART0_CTL_R |= UART_CTL_UARTEN; //enable
}

/****************************** End Of Module *******************************/
