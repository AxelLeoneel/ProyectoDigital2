/*
 * ADC_lib.h
 *
 * Created: 18/08/2025 2:00:11
 *  Author: gerav
 */ 


#ifndef ADC_LIB_H_
#define ADC_LIB_H_


#define F_CPU 16000000UL
#include <avr/io.h>
#include <util/delay.h>

void configADC(uint8_t canal);
void configChannel(uint8_t canal);


#endif /* ADC_LIB_H_ */