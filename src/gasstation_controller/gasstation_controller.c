/*****************************************************************************
* Odense University College of Enginerring
* Embedded C Programming (ECP)
*
* MODULENAME.: gasstation_controller.c
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
#include "FreeRTOS.h"
#include "Task.h"
#include "queue.h"
#include "semphr.h"
#include "../defines.h"
#include "gasstation_controller.h"
#include "../lcd/lcd.h"
#include "../led/led.h"
#include "rtc/rtc.h"

/*****************************    Defines    *******************************/

/*****************************   Constants   *******************************/

/*****************************   Variables   *******************************/

lineitem current_item;
product_price octane_92;
product_price octane_95;
product_price E_85;

/*****************************   Functions   *******************************/

void gasstation_controller_init( void )
/*****************************************************************************
*   Function : See module specification (.h-file).
*****************************************************************************/
{
	octane_92.product = OCTANE_92;
	octane_92.price = 1000;
	
	octane_95.product = OCTANE_95;
	octane_95.price = 1003;
	
	E_85.product = OCTANE_85;
	E_85.price = 1200;
}

void deascii_pin( INT16U a[4] )
/*****************************************************************************
*   Function : Converts the account from ascii to int.
*****************************************************************************/
{
	current_item.pin = ((a[0] - 0x30) * 1000) + ((a[1] - 0x30) * 100) + ((a[2] - 0x30) * 10) + (a[3] - 0x30);
}

void deascii_account( INT16U a[6] )
/*****************************************************************************
*   Function : Converts the account from ascii to int.
*****************************************************************************/
{
	current_item.account = ((a[0] - 0x30) * 100000) + ((a[1] - 0x30) * 10000) + ((a[2] - 0x30) * 1000) + ((a[3] - 0x30) * 100) + ((a[4] - 0x30) * 10) + (a[5] - 0x30);
}

void add_money( INT8U m )
/*****************************************************************************
*   Function : Adds money to the current lineitem.
*****************************************************************************/
{
	current_item.money += m;
}

void reset_line_item()
/*****************************************************************************
*   Function : Resets the limeite, for a new sale
*****************************************************************************/
{
	current_item.money = 0;
	current_item.pay_method = PAYMENT_CASH;
	current_item.account = 0;
	current_item.pin = 0;
	current_item.product = 0;
}

void gasstation_controller_task()
/*****************************************************************************
*   Function : See module specification (.h-file).
*****************************************************************************/
{
	static INT8U gasstation_state = STATE_IDLE;
	portBASE_TYPE status;
	gasstation_event event;
	
	static INT8U big_timer_running 		= 0;
	static INT16U big_timer 			= 0;
	static INT16U account[6]			= { 0x20, 0x20, 0x20, 0x20, 0x20, 0x20 };
	static INT8U account_pointer 		= 0;
	static INT16U pin[4]				= { 0x20, 0x20, 0x20, 0x20 };
	static INT8U pin_pointer			= 0;
	
	switch ( gasstation_state )
	{
		case STATE_IDLE:
		
		// Nice welcome
		lcd_add_string_to_buffer(0, 0, "Insert money or ");
		lcd_add_string_to_buffer(0, 1, "choose account. ");
		
		if(uxQueueMessagesWaiting(event_queue) != 0)
		{
			status = xQueueReceive(event_queue, &event, 0);
			
			if(status == pdPASS)
			{
				switch ( event.event )
				{
					case EVENT_ADD_50_KR:
					current_item.pay_method = 0;
					add_money(50);
					gasstation_state = STATE_ADD_MONEY;
					break;                               
					                                     
					case EVENT_ADD_100_KR:
					current_item.pay_method = 0;
					add_money(100);
					gasstation_state = STATE_ADD_MONEY;
					break;
					
					case EVENT_ACCOUNT_CLICK:
					current_item.pay_method = 1;
					gasstation_state = STATE_ENTER_ACCOUNT;
					INT8U i;
					for(i = 0; i < 6; ++i)
					{
						account[i] = 0x20;
					}
					account_pointer = 0;
					break;
				}
			}
		}
		
		break;
		
		case STATE_ENTER_ACCOUNT:
		
		if(account_pointer == 6)
		{
			deascii_account( account );
			gasstation_state = STATE_ENTER_PIN;
			INT8U i;
			for(i = 0; i < 4; ++i)
			{
				pin[i] = 0x20;
			}
			pin_pointer = 0;
		}
		
		lcd_add_string_to_buffer(0, 0, "Enter account:  ");
		lcd_add_string_to_buffer(6, 1, "         ");
		lcd_add_char_to_buffer(0, 1, account[0]);
		lcd_add_char_to_buffer(1, 1, account[1]);
		lcd_add_char_to_buffer(2, 1, account[2]);
		lcd_add_char_to_buffer(3, 1, account[3]);
		lcd_add_char_to_buffer(4, 1, account[4]);
		lcd_add_char_to_buffer(5, 1, account[5]);
		
		if(uxQueueMessagesWaiting(event_queue) != 0)
		{
			status = xQueueReceive(event_queue, &event, 0);
			
			if(status == pdPASS)
			{
				switch ( event.event )
				{
					case EVENT_KEYBOARD_DIGIT:
					account[account_pointer] = event.value;
					account_pointer++;
					break;
				}
			}
		}
		break;
		
		case STATE_ENTER_PIN:
		
		if(pin_pointer == 4)
		{
			deascii_pin( pin );
			gasstation_state = STATE_CHOOSE_PRODUCT;
		}
		
		lcd_add_string_to_buffer(0, 0, "Enter PIN:      ");
		lcd_add_string_to_buffer(4, 1, "         ");
		lcd_add_char_to_buffer(0, 1, pin[0]);
		lcd_add_char_to_buffer(1, 1, pin[1]);
		lcd_add_char_to_buffer(2, 1, pin[2]);
		lcd_add_char_to_buffer(3, 1, pin[3]);
		
		if(uxQueueMessagesWaiting(event_queue) != 0)
		{
			status = xQueueReceive(event_queue, &event, 0);
			
			if(status == pdPASS)
			{
				switch ( event.event )
				{
					case EVENT_KEYBOARD_DIGIT:
					pin[pin_pointer] = event.value;
					pin_pointer++;
					break;
				}
			}
		}
		break;
		
		case STATE_ADD_MONEY:
		lcd_add_string_to_buffer(0, 0, "Money added:    ");
		lcd_add_string_to_buffer(5, 1, "          ");
		write_5_char_int_to_buffer (0, 1, current_item.money );
		
		if(uxQueueMessagesWaiting(event_queue) != 0)
		{
			status = xQueueReceive(event_queue, &event, 0);
			
			if(status == pdPASS)
			{
				switch ( event.event )
				{
					case EVENT_ADD_50_KR:
					add_money(50);
					break;                               
					                                     
					case EVENT_ADD_100_KR:
					add_money(100);
					break;
					
					case EVENT_PRODUCT_CLICK:
					gasstation_state = STATE_CHOOSE_PRODUCT;
					break;
				}
			}
		}
		
		break;
		
		case STATE_CHOOSE_PRODUCT:
		lcd_add_string_to_buffer(0, 0, "Choose product: ");
		lcd_add_string_to_buffer(3, 1, "             ");
		if(current_item.product == OCTANE_92)
		{
			lcd_add_string_to_buffer(0, 1, " 92");
		} else if(current_item.product == OCTANE_95)
		{
			lcd_add_string_to_buffer(0, 1, " 95");
		} else	if(current_item.product == OCTANE_85)
		{
			lcd_add_string_to_buffer(0, 1, "E85");
		}
		
		if(uxQueueMessagesWaiting(event_queue) != 0)
		{
			status = xQueueReceive(event_queue, &event, 0);
			
			if(status == pdPASS)
			{
				switch ( event.event )
				{
					case EVENT_PRODUCT_CLICK:
					switch ( current_item.product )
					{
						case OCTANE_92:
						current_item.product = OCTANE_95;
						break;
						
						case OCTANE_95:
						current_item.product = OCTANE_85;
						break;
						
						case OCTANE_85:
						current_item.product = OCTANE_92;
						break;
					}
					break;
				}
			}
		}
		
		break;
	}
}

/****************************** End Of Module *******************************/
