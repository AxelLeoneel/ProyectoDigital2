/*
 * LCD_4_bits.c
 *
 * Created: 20/07/2025 21:20:15
 *  Author: axell
 */ 

// Include h file
#include "LCD_4_bits.h"

// Configuration of the LCD
void initLCD_4_bits(void)
{
	// Use PORTB for control of the LCD
	// Set DDRB bits as OUT
	DDRB |= (1 << DDB5) | (1 << DDB4) | (1 << DDB3) | (1 << DDB2) | (1 << DDB1) | (1 << DDB0);
	// Write them as LOW
	PORTB = 0x00;
	// Boot sequence (Check Datasheet)
	LCD_Port(0x00); // Clear LCD digital bits to begin boot sequence
	_delay_ms(30); // Wait for more than 15 ms to send commands
	LCD_CMD(0x03); // Write command 0b0000 0011
	_delay_ms(5); // Wait 5 ms
	LCD_CMD(0x03); // Write command 0b0000 0011
	_delay_ms(11); // Wait 11 ms
	LCD_CMD(0x03); // Write command 0b0000 0011
	LCD_CMD(0x02); // Write command 0b0000 0010
	// Function set
	LCD_CMD(0x02);
	LCD_CMD(0x08);
	// Display ON/OFF
	LCD_CMD(0x00);
	LCD_CMD(0x0C);
	// Entry mode
	LCD_CMD(0x00);
	LCD_CMD(0x06);
	// Clear display
	LCD_CMD(0x00);
	LCD_CMD(0x01);
	
}
// Send a byte to the port (Read all the bus bits)
void LCD_Port(uint8_t byte)
{
	// NOTE: go bit by bit, digital pins at 4 bits config are D4-D7
	
	// D4 = 1
	if (byte & 1) // Logic AND: 0bXXXX XXXX & 0b0000 0001 = 0b0000 000X
	{
		// If the bit is set, set PB2
		PORTB |= (1 << PORTB2);
	}
	else
	{
		// If not, clear PB2
		PORTB &= ~(1 << PORTB2);
	}
	
	// D5 = 1
	if (byte & 2) // Logic AND: 0bXXXX XXXX & 0b0000 0010 = 0b0000 00X0
	{
		// If the bit is set, set PB3
		PORTB |= (1 << PORTB3);
	}
	else
	{
		// If not, clear PB3
		PORTB &= ~(1 << PORTB3);
	}
	
	// D6 = 1
	if (byte & 4) // Logic AND: 0bXXXX XXXX & 0b0000 0100 = 0b0000 0X00
	{
		// If the bit is set, set PB4
		PORTB |= (1 << PORTB4);
	}
	else
	{
		// If not, clear PB4
		PORTB &= ~(1 << PORTB4);
	}
	
	// D7 = 1
	if (byte & 8) // Logic AND: 0bXXXX XXXX & 0b0000 1000 = 0b0000 X000
	{
		// If the bit is set, set PB5
		PORTB |= (1 << PORTB5);
	}
	else
	{
		// If not, clear PB5
		PORTB &= ~(1 << PORTB5);
	}

}
// Send a command
void LCD_CMD(char cmd)
{
	// RS = 0 (LCD will interpret cmd char as a command)
	PORTB &= ~(1 << PORTB0);
	// Interpret command
	LCD_Port(cmd);
	// EN = 1, E = 1
	PORTB |= (1 << PORTB1);
	_delay_ms(4);
	// EN = 0, E = 0
	PORTB &= ~(1 << PORTB1);
}
// Send and write character
void LCD_Write_Char(char character)
{
	// Define high and low parts of byte (char)
	char charLOW, charHIGH;
	charLOW = character & 0x0F; // Logic AND: 0bXXXX XXXX & 0b0000 1111 = 0b0000 XXXX
	charHIGH = (character & 0xF0) >> 4; // Logic AND: 0bXXXX XXXX & 0b1111 0000 = 0bXXXX 0000 --> 0b0000 XXXX
	// RS = 1 (LCD will interpret cmd char as a character)
	PORTB |= (1 << PORTB0);
	LCD_Port(charHIGH);
	// EN = 1, E = 1
	PORTB |= (1 << PORTB1);
	_delay_ms(4);
	// EN = 0, E = 0
	PORTB &= ~(1 << PORTB1);
	LCD_Port(charLOW);
	// EN = 1, E = 1
	PORTB |= (1 << PORTB1);
	_delay_ms(4);
	// EN = 0, E = 0
	PORTB &= ~(1 << PORTB1);
}
// Send and write string
void LCD_Write_String(char *text)
{
	// Travel the array of characters
	int i;
	// Call write char for each char until it finds and empty char
	for (i = 0; text[i] != '\0'; i++)
	{
		LCD_Write_Char(text[i]);
	}
}
// Move to the right
void LCD_Shift_Right(void)
{
	LCD_CMD(0x01);
	LCD_CMD(0x0C);
}
// Move to the left
void LCD_Shift_Left(void)
{
	LCD_CMD(0x01);
	LCD_CMD(0x08);
}
// Set cursor on a 16 x 2 position
void LCD_Set_Cursor(char column, char row)
{
	// Define temporal char and divide byte into high and low parts
	char temporal, tempHIGH, tempLOW;
	if (row == 1)
	{
		temporal = 0x80 + column - 1;
		tempHIGH = (temporal & 0xF0) >> 4;
		tempLOW = temporal & 0x0F;
		LCD_CMD(tempHIGH);
		LCD_CMD(tempLOW);
	}
	else if (row == 2)
	{
		temporal = 0xC0 + column - 1;
		tempHIGH = (temporal & 0xF0) >> 4;
		tempLOW = temporal & 0x0F;
		LCD_CMD(tempHIGH);
		LCD_CMD(tempLOW);
	}
}