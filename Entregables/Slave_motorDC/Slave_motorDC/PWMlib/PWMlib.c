/*
 * PWBlib.c
 *
 * Created: 16/08/2025 17:20:17
 *  Author: gerav
 */ 

#include "PWMlib.h"


void init_pwm1(uint8_t invertido, uint16_t prescaler)
{
	DDRB |= (1 << PORTB1);
	
	TCCR1A = 0;						//Limpieza de bits
	TCCR1B = 0;
	
	//-----------------------------------------------//
	if(invertido == invert){
		TCCR1A |= (1 << COM1A0) | (1 << COM1A1);
	}
	else{
		TCCR1A |= (1 << COM1A1);
	}
	//-----------------------------------------------//
	TCCR1A |= (1 << WGM11);						//Seleccionamos FAST MODE
	TCCR1B |= (1 << WGM12) | (1 << WGM13);
	//-----------------------------------------------//
	switch(prescaler){
		case 1:
		TCCR1B |= (1 << CS10);
		break;
		
		case 8:
		TCCR1B |= (1 << CS11);
		break;
		
		case 64:
		TCCR1B |= (1 << CS11) | (1 << CS10);
		break;
		
		case 256:
		TCCR1B |= (1 << CS12);
		break;
		
		case 1024:
		TCCR1B |= (1 << CS12) | (1 << CS10);
		break;
		
		default:
		TCCR1B |= (1 << CS10);
		break;
	}
	
	ICR1 = 3999;
	
	OCR1A = 0;

}

void DutyCycle1(uint16_t DT1)
{
	if(DT1 > ICR1) DT1 = ICR1;
	OCR1A = DT1;
}
