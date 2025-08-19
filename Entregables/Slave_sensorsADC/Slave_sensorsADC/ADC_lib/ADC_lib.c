/*
 * CFile1.c
 *
 * Created: 18/08/2025 1:59:52
 *  Author: gerav
 */ 

#include "ADC_lib.h"

void configADC(uint8_t canal)
{
	// Begin ADC Multiplexer
	ADMUX = 0x00;
	// AVcc
	ADMUX |= (1	<< REFS0);
	// Activate justification L
	ADMUX &= ~(1 << ADLAR);
	// Select channel
	configChannel(canal);
	// Begin ADC Control & Status Register
	ADCSRA = 0x00;
	// Enable interruptions
	ADCSRA |= (1 << ADIE);
	// PreScaler = 128 --> f_adc = 125kHz
	ADCSRA |= (1 << ADPS2) | (1 << ADPS1) | (1 << ADPS0);
	// ADC enable
	ADCSRA |= (1 << ADEN);
	// Start first conversion
	ADCSRA |= (1 << ADSC);
}

void configChannel(uint8_t canal)
{
	switch(canal)
	{
		case 0:
		ADMUX &= ~( (1 << MUX3) | (1 << MUX2) | (1 << MUX1) | (1 << MUX0) ); // (MUX0123 = 0000)
		break;
		case 1:
		ADMUX |=  (1 << MUX0) ; // (MUX0123 = 0001)
		break;
		case 2:
		ADMUX |=  (1 << MUX1) ; // (MUX0123 = 0010)
		break;
		case 6:
		ADMUX |=  (1 << MUX2) | (1 << MUX1) ; // (MUX0123 = 0110)
		ADMUX &= ~( (1 << MUX3) | (1 << MUX0) );
		break;
		case 7:
		ADMUX |=  (1 << MUX2) | (1 << MUX1) | (1 << MUX0) ; // (MUX0123 = 0111)
		ADMUX &= ~(1 << MUX3);
		break;
		default:
		ADMUX &= ~( (1 << MUX3) | (1 << MUX2) | (1 << MUX1) | (1 << MUX0) ); // Channel 0
		break;
	}
}
