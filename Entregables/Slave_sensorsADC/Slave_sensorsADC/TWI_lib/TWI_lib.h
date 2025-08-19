/*
 * TWI_lib.h
 *
 * Created: 18/08/2025 1:55:47
 *  Author: gerav
 */ 


#ifndef TWI_LIB_H_
#define TWI_LIB_H_


#define F_CPU 16000000UL
#include <avr/io.h>
#include <avr/interrupt.h>

/********************************************************************/
/*--------------------------Setup functions-------------------------*/

// Initialize pins for a Two Wire MCU (works for master and slave)
void init_TWI_pins(void);
// Load the corresponding value in terms of desired frequency (Recommended f = 100kHz) and prescaler (1, 4, 16, 64), only in master
void Two_Wire_Bit_Rate(unsigned long SCL, uint8_t prescaler);
// Initialize slave
void TWI_Slave_Init(uint8_t slave_Address);

/********************************************************************/
/*------------------------Recurrent functions-----------------------*/

// Read the status register
uint8_t Two_Wire_State(void);
// Write a byte via Two Wire
uint8_t Two_Wire_Write(uint8_t data);
// Read a byte via Two Wire
uint8_t TWI_Read_Byte(uint8_t ack);
// Send Start Condition
uint8_t Two_Wire_Start(void);
// Send Stop Condition
void Two_Wire_Stop(void);

/********************************************************************/
/*---------------------------Master functions-------------------------*/

void TWI_Master_Transmitter(uint8_t slave_Address);
void TWI_Master_Receiver(uint8_t slave_Address);


#endif /* TWI_LIB_H_ */