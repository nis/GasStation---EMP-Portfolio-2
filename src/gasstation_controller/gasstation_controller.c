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
#include "../pwm/pwm.h"
#include "../fan/fan.h"
#include "../uart/uart.h"
#include "rtc/rtc.h"

/*****************************    Defines    *******************************/

/*****************************   Constants   *******************************/

/*****************************   Variables   *******************************/
lineitem sales[100];
INT16U sales_pointer = 0;
lineitem current_item;
product_price octane_92;
product_price octane_95;
product_price octane_85;

/*****************************   Functions   *******************************/

void update_tanking_display( void )
/*****************************************************************************
*   Function : See module specification (.h-file).
*****************************************************************************/
{
	write_4_char_int_to_buffer_with_comma (0, 0, current_item.pumped);
	lcd_add_char_to_buffer(5, 0, 'L');
	
	write_4_char_int_to_buffer_with_comma (7, 0, current_item.price);
	write_10_char_int_to_buffer_with_comma (3, 1, ((current_item.pumped * current_item.price)/100));
	
	lcd_add_string_to_buffer(12, 0, "kr/L");
	lcd_add_string_to_buffer(14, 1, "kr");
}

void gasstation_controller_init( void )
/*****************************************************************************
*   Function : See module specification (.h-file).
*****************************************************************************/
{
	octane_92.product = OCTANE_92;
	octane_92.price = 1000;
	
	octane_95.product = OCTANE_95;
	octane_95.price = 1003;
	
	octane_85.product = OCTANE_85;
	octane_85.price = 1200;
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
	current_item.money += m*100;
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
	current_item.pumped = 0;
	current_item.time = 0;
}

void gasstation_controller_task()
/*****************************************************************************
*   Function : See module specification (.h-file).
*****************************************************************************/
{
	static INT8U gasstation_state = STATE_IDLE;
	portBASE_TYPE status;
	gasstation_event event;
	uart_command command;
	
	static INT8U big_timer_running 		= 0;
	static INT16U big_timer 			= 0;
	static INT16U account[6]			= { 0x20, 0x20, 0x20, 0x20, 0x20, 0x20 };
	static INT8U account_pointer 		= 0;
	static INT16U pin[4]				= { 0x20, 0x20, 0x20, 0x20 };
	static INT8U pin_pointer			= 0;
	static INT16U pump_target			= 0;
	//static INT16U pump_pumped			= 0;
	static INT16U pump_timer				= 0;
	
	switch ( gasstation_state )
	{
		case STATE_IDLE:
		
		// Nice welcome
		lcd_add_string_to_buffer(0, 0, "Insert money or ");
		lcd_add_string_to_buffer(0, 1, "choose account. ");
		
		// Check for UART commands
		if(uxQueueMessagesWaiting(uart_command_queue) != 0)
		{
			status = xQueueReceive(uart_command_queue, &command, 0);
			
			if(status == pdPASS)
			{
				switch (command.command)
				{
					case UART_SET_PRICE:
					switch (command.product)
					{
						case OCTANE_92:
						uart_send_string("Changed price for 92 from ");
						uart_send_4_digit_int( octane_92.price );
						octane_92.price = command.price;
						uart_send_string(" to ");
						uart_send_4_digit_int( octane_92.price );
						uart_send_newline();
						break;
						
						case OCTANE_95:
						uart_send_string("Changed price for 95 from ");
						uart_send_4_digit_int( octane_95.price );
						octane_95.price = command.price;
						uart_send_string(" to ");
						uart_send_4_digit_int( octane_95.price );
						uart_send_newline();
						break;
						
						case OCTANE_85:
						uart_send_string("Changed price for E85 from ");
						uart_send_4_digit_int( octane_85.price );
						octane_85.price = command.price;
						uart_send_string(" to ");
						uart_send_4_digit_int( octane_85.price );
						uart_send_newline();
						break;
					}
					break;
					
					case UART_GET_REPORT:
					led_yellow_toggle();
					break;
				}
			}
		}
		
		// Check for button events
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
		lcd_add_string_to_buffer(0, 1, "     ");
		write_10_char_int_to_buffer_with_comma (5, 1, current_item.money );
		
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
			current_item.price = octane_92.price;
		} else if(current_item.product == OCTANE_95)
		{
			lcd_add_string_to_buffer(0, 1, " 95");
			current_item.price = octane_95.price;
		} else	if(current_item.product == OCTANE_85)
		{
			lcd_add_string_to_buffer(0, 1, "E85");
			current_item.price = octane_85.price;
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
					
					case EVENT_HANDLE_LIFTET:
					if(current_item.pay_method == PAYMENT_CASH)
					{
						pump_target = (current_item.money/current_item.price) * 100;
						current_item.pumped = 0;
						fan_get_pulse_count();
					}
					pump_timer = 0;
					gasstation_state = STATE_PUMP_SLOW;
					break;
				}
			}
		}
		
		break;
		
		case STATE_PUMP_SLOW:
		fan_set_speed(40);
		current_item.pumped += fan_get_pulse_count();
		update_tanking_display();
		
		if(current_item.pay_method == PAYMENT_CASH && (pump_target - current_item.pumped) < 1000)
		{
			gasstation_state = STATE_PUMP_RAMP_DOWN;
		}
		
		if(pump_timer >= 200)
		{
			pump_timer = 0;
			gasstation_state = STATE_PUMP_RAMP_UP;
		} else {
			pump_timer++;
		}
		
		if(uxQueueMessagesWaiting(event_queue) != 0)
		{
			status = xQueueReceive(event_queue, &event, 0);
			
			if(status == pdPASS)
			{
				switch ( event.event )
				{
					case EVENT_HANDLE_REPLACED:
					gasstation_state = STATE_SHUTDOWN_PUMP;
					break;
				}
			}
		}
		break;
		
		case STATE_PUMP_RAMP_UP:
		current_item.pumped += fan_get_pulse_count();
		update_tanking_display();
		
		if(current_item.pay_method == PAYMENT_CASH && (pump_target - current_item.pumped) < 1000)
		{
			gasstation_state = STATE_PUMP_RAMP_DOWN;
		}
		
		if(pump_timer >= 10)
		{
			pump_timer = 0;
			if(fan_get_ref_speed() < 100)
			{
				fan_speed_up( 1 );
			} else {
				gasstation_state = STATE_PUMP_SS;
			}
		} else {
			pump_timer++;
		}
		
		if(uxQueueMessagesWaiting(event_queue) != 0)
		{
			status = xQueueReceive(event_queue, &event, 0);
			
			if(status == pdPASS)
			{
				switch ( event.event )
				{
					case EVENT_HANDLE_REPLACED:
					gasstation_state = STATE_SHUTDOWN_PUMP;
					break;
				}
			}
		}
		break;
		
		case STATE_PUMP_SS:
		current_item.pumped += fan_get_pulse_count();
		update_tanking_display();
		
		if(current_item.pay_method == PAYMENT_CASH && (pump_target - current_item.pumped) < 1000)
		{
			gasstation_state = STATE_PUMP_RAMP_DOWN;
		}
		
		if(uxQueueMessagesWaiting(event_queue) != 0)
		{
			status = xQueueReceive(event_queue, &event, 0);
			
			if(status == pdPASS)
			{
				switch ( event.event )
				{
					case EVENT_HANDLE_REPLACED:
					gasstation_state = STATE_SHUTDOWN_PUMP;
					break;
				}
			}
		}
		
		break;
		
		case STATE_PUMP_RAMP_DOWN:
		current_item.pumped += fan_get_pulse_count();
		update_tanking_display();
		
		INT16U diff = pump_target - current_item.pumped;
		
		if(current_item.pumped > pump_target)
		{
			fan_set_speed(0);
			gasstation_state = STATE_SHUTDOWN_PUMP;
		} else if(diff < 12)
		{
			fan_set_speed(0);
			gasstation_state = STATE_SHUTDOWN_PUMP;
		} else if(diff < 30)
		{
			fan_set_speed(3);
		} else if(diff < 100)
		{
			fan_set_speed(5);
		} else if(diff < 250)
		{
			fan_set_speed(10);
		} else if(diff < 500)
		{
			fan_set_speed(30);
		} else if(diff < 1000)
		{
			fan_set_speed(50);
		}
		break;
		
		case STATE_SHUTDOWN_PUMP:
		fan_set_speed(0);
		INT16U old_pumped = current_item.pumped;
		current_item.pumped += fan_get_pulse_count();
		update_tanking_display();
		if(current_item.pumped == old_pumped)
		{
			current_item.time = rtc_get_time();
			sales[sales_pointer] = current_item;
			sales_pointer++;
			
			pump_timer = 0;
			gasstation_state = STATE_RECEIPT;
		}
		break;
		
		case STATE_RECEIPT:
		if(pump_timer >= 500)
		{
			pump_timer = 0;
			reset_line_item();
			gasstation_state = STATE_IDLE;
		} else {
			pump_timer++;
		}
		break;
	}
}

/****************************** End Of Module *******************************/
