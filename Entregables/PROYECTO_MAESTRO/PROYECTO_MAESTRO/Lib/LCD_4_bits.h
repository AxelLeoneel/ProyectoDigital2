/*
 * LCD_4_bits.h
 *
 * Created: 20/07/2025 21:20:27
 *  Author: axell
 */ 


#ifndef LCD_4_BITS_H_
#define LCD_4_BITS_H_

// Include existing libraries
#define F_CPU 16000000UL
#include <avr/io.h>
#include <util/delay.h>

// E pin set in PC1
#define E (1 << PORTC1)
// Configuration of the LCD
void initLCD_4_bits(void);
// Send a byte to the port
void LCD_Port(uint8_t byte);
// Send a command
void LCD_CMD(char cmd);
// Send and write character 
void LCD_Write_Char(char character);
// Send and write string
void LCD_Write_String(char *text);
// Move to the right
void LCD_Shift_Right(void);
// Move to the left
void LCD_Shift_Left(void);
// Set cursor on a 16 x 2 position
void LCD_Set_Cursor(char column, char row);

#endif /* LCD_4_BITS_H_ */