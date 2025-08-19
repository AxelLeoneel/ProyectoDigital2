/*
 * ADClib.c
 *
 * Created: 16/08/2025 0:03:02
 *  Author: gerav
 */ 

#include <avr/io.h>
#include "ADClib.h"

void init_adc()
{
	ADMUX = 0;
	ADMUX |= (1 << MUX0);
	ADMUX |= (1 << REFS0);
	ADMUX |= (1 << ADLAR);

	ADCSRA = 0;
	ADCSRA |= (1 << ADPS0) | (1 << ADPS1) | (1 << ADPS2);
	ADCSRA |= (1 << ADIE);								//Interrupcion
	ADCSRA |= (1 << ADEN);
	
	ADCSRA |= (1 << ADSC);							//ADC
}
