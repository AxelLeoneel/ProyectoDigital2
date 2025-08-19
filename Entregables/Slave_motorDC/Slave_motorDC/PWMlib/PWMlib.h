/*
 * PWMlib.h
 *
 * Created: 16/08/2025 17:20:04
 *  Author: gerav
 */ 

#include <avr/io.h>
								//Config PWM
#define invert 1
#define non_invert 0

#ifndef PWMLIB_H_
#define PWMLIB_H_

/*-----------------------PWM-------------------------*/
void init_pwm1(uint8_t invertido, uint16_t prescaler);
/*---------------------------------------------------*/

/*----------------------DUTYCycle--------------------*/
void DutyCycle1(uint16_t duty_cycle1);
/*---------------------------------------------------*/

#endif /* PWMLIB_H_ */