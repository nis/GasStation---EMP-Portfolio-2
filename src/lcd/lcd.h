/*
 * lcd.h
 *
 *  Created on: 23/02/2011
 *      Author: NS.
 */

#ifndef LCD_H_
#define LCD_H_

#include "../inc/emp_type.h"

/* LCD character size */
#define COL_SIZE				16
#define ROW_SIZE				2

/* LCD Function definemens */
//#define	CLEAR_DISPLAY		0x01
//#define BLINK_ON				0x0D
//#define DISPLAY_ON			0x0C

void lcd_add_char_to_buffer(INT8U col, INT8U row, INT8U ch);
void lcd_add_string_to_buffer(INT8U col, INT8U row, INT8S *str);

void init_lcd_write_task(void);

void lcd_write_buffer(void);

void lcd_io(void);
void lcd_init(void);
void lcd_port_setup(void);
void lcd_cmd(INT8U cmd);
void lcd_delay_us(INT32U i);
void lcd_delay_ms(INT32U i);

void set_rs_high();
void set_rs_low();
void lcd_e_high();
void lcd_e_low();
void lcd_cmd_write(INT8U cmd);
void lcd_data_write(INT8U cmd);

void lcd_write_task(void);


#define PA7 7
#define PA6 6

#define PB2 2
#define PB3 3
#define PB4 4
#define PB5 5
#define PB6 6


#define LCD_RS	 PA7
#define LCD_RW	 PA6

#define LCD_D4	 PB2
#define LCD_D5	 PB3
#define LCD_D6 	 PB4
#define LCD_D7 	 PB5
#define LCD_E	 PB6



#endif /* LCD_H_ */
