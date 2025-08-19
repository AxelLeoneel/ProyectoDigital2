/*
 * PWMlib.c
 *
 * Created: 18/08/2025 16:20:20
 *  Author: gerav
 */ 

#include "PWMlib.h"

uint16_t duty_cycle1 = 375;

void init_pwm1(uint8_t invertido, uint16_t prescaler){
	DDRB |= (1 << PORTB1);				//PD6 Y PD5
	
	TCCR1A = 0;
	TCCR1B = 0;			//Limpieza de bits
	
	//---------------------------------------------------------//
	if(invertido == invert){
		TCCR1A |= (1 << COM1A0) | (1 << COM1A1);
		TCCR1A |= (1 << COM1B0) | (1 << COM1B1);
	}
	else{
		TCCR1A |= (1 << COM1A1);
		TCCR1A |= (1 << COM1B1);
	}
	//---------------------------------------------------------//
	TCCR1A |= (1 << WGM11);							//Seleccionamos FAST MODE
	TCCR1B |= (1 << WGM13) | (1 << WGM12);
	//--------------------------------------------------------//
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
	
	ICR1 = 39999;
	//--------------------------------------------------------//
	OCR1A = 2000;					//Llamamos funcion de cargar duty cycle
}

void DutyCycle1(uint16_t DT1){
	OCR1A = DT1;
}