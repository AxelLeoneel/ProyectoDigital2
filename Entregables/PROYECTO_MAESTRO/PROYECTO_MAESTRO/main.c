/*
 * PROYECTO_MAESTRO.c
 *
 * Created: 14/08/2025 12:35:29
 * Author : axell
 */ 

/********************************************************************/
/*--------------------- Header (Libraries)--------------------------*/
#define F_CPU 16000000UL
#include <avr/io.h>
#include <stdlib.h>
#include <avr/interrupt.h>
#include <util/delay.h>

#include "Lib/LCD_4_bits.h"
#include "Lib/TWI_Lib.h"
#include "Lib/UARTlib.h"
/********************************************************************/



/********************************************************************/
/*-----------------------Function Prototypes------------------------*/
void setup();
void program();

/********************************************************************/



/********************************************************************/
/*-----------------------Variable Declaration-----------------------*/
#define slave_BME280 0x76
#define slave_Sensor 0x40
#define slave_AHT10 0x38

uint8_t adc = 0;		// ADC0 value of Atmega328p slave
uint8_t command = 0;	// Id of the BME280
float tempAHT10 = 0;	// AHT10 value of temperature
float humAHT10 = 0;		// AHT10 value of humidity
uint8_t rain_Sensor = 0;// Rain sensor value
uint8_t smoke_MQ135 = 0;// MQ135 value of impurity

char str[16];			// Buffer for integer to array conversion (itoa)

float Pressure;			// Converts to pressure magnitude
float Temperature;		// Converts to temperature magnitude
float Humidity;			// Converts to humidity magnitude

/********************************************************************/



/********************************************************************/
/*------------------------------Main--------------------------------*/
int main(void)
{
	// Global config of MCU
	setup();
	// Initialize LCD 4 bit mode and write index
	initLCD_4_bits();
	LCD_Set_Cursor(1, 1);
	LCD_Write_String("Jijo");
	// Main loop
	while (1)
	{
		program();
	}
}

/********************************************************************/



/********************************************************************/
/*---------------------Non-interrupt Subroutines--------------------*/
void setup()
{
	// Disable global interruption
	cli();
	// Initialize I2C Master Mode f = 100kHz, PreScaler = 1
	init_TWI_pins();
	Two_Wire_Bit_Rate(100000,1);
	// Initialize UART (asynchronous normal, no parity, 1 stop bit, 8 bit rate, 9600@16MHz)
	initUART();
	// Enable global interruption
	sei();
}

void program()
{
	/*---------------------Configuration of sensor--------------------*/
	/*// BMP280 communication sequence
	TWI_Master_Transmitter(slave_BME280);
	Two_Wire_Write(0xD0);
	TWI_Master_Receiver_Repeated(slave_BME280);
	command = TWI_Read_Byte(0); // NACK for only one byte
	Two_Wire_Stop();*/
	
	/*---------------------Atmega328p slave read--------------------*/
	/*// Atmega328p slave
	TWI_Master_Receiver(slaveADC);
	adc = TWI_Read_Byte(0); // NACK for only one byte
	Two_Wire_Stop();*/
	
	/*-------------------------AHT10 slave read---------------------*/
	// Needs at least 20 ms after power on
	_delay_ms(30);
	// Initialization commands
	TWI_Master_Transmitter(slave_AHT10);
	Two_Wire_Write(0xE1);
	Two_Wire_Write(0x08);
	Two_Wire_Write(0x00);
	Two_Wire_Stop();
	// Trigger measurement
	TWI_Master_Transmitter(slave_AHT10);
	Two_Wire_Write(0xAC);
	Two_Wire_Write(0x33);
	Two_Wire_Write(0x00);
	Two_Wire_Stop();
	// Needs at least 75 ms after trigger
	_delay_ms(80);
	// Read the data
	TWI_Master_Receiver(slave_AHT10);
	// Read all the values
	uint8_t rawDataAHT10[6];
	for (uint8_t i = 0; i < 6; i++)
	{
		if (i < 5)
		// Bytes 0 to 5
		rawDataAHT10[i] = TWI_Read_Byte(1);
		else
		// Last byte 6
		rawDataAHT10[i] = TWI_Read_Byte(0);
	}
	Two_Wire_Stop();
	
	/*-------------------------AHT10 data conversion---------------------*/
	// Mask 4 less significant bits of byte 3, byte 4, byte 5
	uint32_t rawTemp = (((uint32_t)rawDataAHT10[3] & 0x0F) << 16) | ((uint32_t)rawDataAHT10[4] << 8) | ((uint32_t)rawDataAHT10[5]);
	// Use the corresponding formula
	tempAHT10 = ((rawTemp * 200.0) / 1048576.0) - 50.0;
	// Mask the 20 humidity raw bits
	uint32_t rawHum = ((uint32_t)rawDataAHT10[1] << 12) | ((uint32_t)rawDataAHT10[2] << 4)  | ((uint32_t)rawDataAHT10[3] >> 4);
	// Convert to relative humidity using formula
	humAHT10 = (rawHum * 100.0) / 1048576.0;
	
	/*-------------------------Centrifuge slave communication---------------------*/
	
	
	/*-------------------------Sensors slave communication---------------------*/
	// Ask MQ-135 value (command = 1)
	TWI_Master_Transmitter(slave_Sensor);
	Two_Wire_Write(1);
	Two_Wire_Stop();
	// Save
	TWI_Master_Receiver(slave_Sensor);
	smoke_MQ135 = TWI_Read_Byte(0);
	Two_Wire_Stop();
	
	// Ask percent rain sensor (command = 2)
	TWI_Master_Transmitter(slave_Sensor);
	Two_Wire_Write(2);
	Two_Wire_Stop();
	// Save
	TWI_Master_Receiver(slave_Sensor);
	rain_Sensor = TWI_Read_Byte(0);
	Two_Wire_Stop();
	
	/*---------------------Show the data in LCD--------------------*/
	// Show smoke values in LCD
	LCD_Set_Cursor(1, 2);
	LCD_Write_String("   ");
	LCD_Set_Cursor(1, 2);
	itoa(smoke_MQ135, str, 10);
	LCD_Write_String(str);
	
	// Show rain values in LCD
	LCD_Set_Cursor(4, 2);
	LCD_Write_String("   ");
	LCD_Set_Cursor(4, 2);
	itoa(rain_Sensor, str, 10);
	LCD_Write_String(str);
	LCD_Write_String("%");
	
	// Show AHT10 temperature value in LCD
	dtostrf(tempAHT10, 5, 2, str); // dtostrf(float to convert, space needed, number of decimals, string to save)
	LCD_Set_Cursor(9, 2);
	LCD_Write_String("     C");
	LCD_Set_Cursor(9, 2);
	LCD_Write_String(str);
	
	// Show AHT10 humidity value in LCD
	dtostrf(humAHT10, 5, 2, str); // dtostrf(float to convert, space needed, number of decimals, string to save)
	LCD_Set_Cursor(7, 1);
	LCD_Write_String("     %");
	LCD_Set_Cursor(7, 1);
	LCD_Write_String(str);
	
	/*// Show ADC value in LCD
	LCD_Set_Cursor(13, 2);
	LCD_Write_String("   ");
	LCD_Set_Cursor(13, 2);
	itoa(adc, str, 10);
	LCD_Write_String(str);*/
	
	/*---------------------Send ADC data via UART--------------------*/
	// Convert adc integer to array
	/*itoa(adc, str, 10);
	// Send string to UART bus
	writeString(str);
	writeChar(',');*/
	
	// Convert temperature to array
	dtostrf(tempAHT10, 5, 2, str);
	// Send string to UART bus
	writeString(str);
	writeChar(',');
	
	// Convert humidity to array
	dtostrf(humAHT10, 5, 2, str);
	// Send string to UART bus
	writeString(str);
	writeChar(',');
	
	itoa(smoke_MQ135, str, 10);
	writeString(str);
	writeChar(',');
	
	itoa(rain_Sensor, str, 10);
	writeString(str);
	writeChar(',');
	
	// End "flag"
	writeChar('\n');
	// Anti saturation delay
	_delay_ms(1000);
}
/********************************************************************/



/********************************************************************/
/*----------------------Interrupt Subroutines-----------------------*/

/********************************************************************/

