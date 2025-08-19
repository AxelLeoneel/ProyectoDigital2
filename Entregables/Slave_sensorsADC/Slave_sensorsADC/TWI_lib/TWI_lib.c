/*
 * TWI_lib.c
 *
 * Created: 18/08/2025 1:55:35
 *  Author: gerav
 */ 

// Include header file
#include "TWI_Lib.h"

/********************************************************************/
/*--------------------------Setup functions-------------------------*/

// Initialize pins for a Two Wire MCU (works for master and slave)
void init_TWI_pins(void)
{
	// Define I2C bits as IN
	DDRC &= ~( (1 << DDC5) | (1 << DDC4) );
	// Do not activate pull-up
	PORTC &= ~( (1 << PORTC4) | (1 << PORTC5) );
}

// Load the corresponding value in terms of desired frequency (Recommended f = 100kHz) and prescaler (1, 4, 16, 64), only in master
void Two_Wire_Bit_Rate(unsigned long SCL, uint8_t prescaler)
{
	// PreScaler select
	switch(prescaler)
	{
		case 1:
		TWSR &= ~( (1 << TWPS1) | (1 << TWPS0) );
		break;
		case 4:
		TWSR &= ~(1 << TWPS1);
		TWSR |= (1 << TWPS0);
		break;
		case 16:
		TWSR &= ~(1 << TWPS0);
		TWSR |= (1 << TWPS1);
		break;
		case 64:
		TWSR |= (1 << TWPS1) | (1 << TWPS0);
		break;
		default:
		prescaler = 1;
		TWSR &= ~( (1 << TWPS1) | (1 << TWPS0) );
		break;
	}
	// Bit rate register
	TWBR = ((F_CPU/SCL) - 16)/(prescaler * 2);
}

// Initialize slave
void TWI_Slave_Init(uint8_t slave_Address)
{
	// Assign address to slave
	TWAR = (slave_Address << 1);
	// Enable Two Wire, Interruption and ACK
	TWCR = (1 << TWEA) | (1 << TWEN) | (1 << TWIE);
}

/********************************************************************/
/*------------------------Recurrent functions-----------------------*/

// Read the status register
uint8_t Two_Wire_State(void)
{
	uint8_t state;
	state = TWSR & 0xF8;
	return state;
}

// Write a byte via Two Wire
uint8_t Two_Wire_Write(uint8_t data)
{
	// Load data to register
	TWDR = data;
	// Enable TWI and Interruption (send data to bus)
	TWCR = (1 << TWEN) | (1 << TWINT);
	// Wait until interrupt flag is cleared
	while(!(TWCR & (1 << TWINT)));
	// Return state
	return Two_Wire_State();
}

// Read a byte via Two Wire
uint8_t TWI_Read_Byte(uint8_t ack) 
{
	// Enable TWI, clear flag, and ACK/NACK bit
	TWCR = (1 << TWINT) | (1 << TWEN) | (ack ? (1 << TWEA) : 0);
	// Wait until operation is complete
	while(!(TWCR & (1 << TWINT)));
	// Verify successful read and save into buffer
	uint8_t state = Two_Wire_State();
	if (state == 0x50 || state == 0x58)
	{
		// Returns data
		return TWDR;
	}
	else
	{
		// Stop communication
		Two_Wire_Stop();
		return state;
	}
}

// Send Start Condition
uint8_t Two_Wire_Start(void)
{
	// Enable Two Wire, send start condition, set flag (starts operation)
	TWCR = (1 << TWINT) | (1 << TWSTA) | (1 << TWEN);
	// Wait until TWINT is cleared by hardware (stops an operation)
	while (!(TWCR & (1 << TWINT)));
	// Verify START condition received
	uint8_t state = Two_Wire_State();
	if (state == 0x08)
	{
		// If the START is complete, return 1
		return 1;
	}
	else
	{
		// If not, return the TWSR state
		return state;
	}
}

// Send Stop Condition
void Two_Wire_Stop(void)
{
	// Enable Two Wire, send stop condition, set flag
	TWCR = (1 << TWINT) | (1 << TWEN) | (1 << TWSTO);
	// Wait until stop is done
	while(TWCR & (1 << TWSTO));
}


/********************************************************************/
/*---------------------------Mode functions-------------------------*/

void TWI_Master_Transmitter(uint8_t slave_Address)
{
	// Enable Two Wire and Interruption
	TWCR = (1 << TWEN) | (1 << TWIE);
	// Send start condition
	uint8_t start = Two_Wire_Start();
	// If START fails, stop communication
	if (start != 1) Two_Wire_Stop();
	// Send SLA + W (Write = 0, Read = 1)
	Two_Wire_Write(slave_Address << 1 & 0b11111110);
	// Verify SLA + W sent and ACK received
	uint8_t state = Two_Wire_State();
	// If SLA + W + ACK fails, stop communication
	if (state != 0x18) Two_Wire_Stop();
	// Now send the useful data with write
}

void TWI_Master_Receiver(uint8_t slave_Address)
{
	// Enable Two Wire and Interruption
	TWCR = (1 << TWEN) | (1 << TWIE);
	// Send start condition
	uint8_t start = Two_Wire_Start();
	// If START fails, stop communication
	if (start != 1) Two_Wire_Stop();
	// Send SLA + W (Write = 0, Read = 1)
	Two_Wire_Write((slave_Address << 1) | 0x01);
	// Verify SLA + W sent and ACK received
	uint8_t state = Two_Wire_State();
	// If SLA + W + ACK fails, stop communication
	if (state != 0x40) Two_Wire_Stop();
	// Now read the useful data with read
}

/*ISR(TWI_vect) {
	twi_slave_status = TWSR & 0xF8;  // Lee estado actual

	switch(twi_slave_status) {
		// Slave Receiver
		case 0x60:  // Dirección + WRITE recibida
		case 0x80:  // Dato recibido
		twi_slave_data = TWDR;  // Lee dato
		TWCR = (1 << TWEA) | (1 << TWEN) | (1 << TWIE);  // Prepara ACK
		break;

		// Slave Transmitter
		case 0xA8:  // Dirección + READ recibida
		TWDR = twi_slave_data;  // Envía dato
		TWCR = (1 << TWEA) | (1 << TWEN) | (1 << TWIE);  // Prepara ACK
		break;

		case 0xC0:  // STOP recibido
		TWCR = (1 << TWEA) | (1 << TWEN) | (1 << TWIE);  // Prepárate para siguiente comunicación
		break;
	}
}*/