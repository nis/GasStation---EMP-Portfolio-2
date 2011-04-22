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
#include "qu_mu.h"

/*****************************    Defines    *******************************/

/*****************************   Constants   *******************************/

/*****************************   Variables   *******************************/

/*****************************   Functions   *******************************/

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

void uart0_receive_task()
/*****************************************************************************
*   Function : See module specification (.h-file).
*****************************************************************************/
{
	INT8U ch = 0;
	while(UART0_FR_R & UART_FR_RXFE);
	ch = UART0_DR_R;
	xQueueSend(uart_input_queue, &ch, 0);
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
