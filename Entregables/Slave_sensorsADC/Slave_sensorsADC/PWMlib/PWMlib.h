/*
 * PWMlib.h
 *
 * Created: 18/08/2025 16:20:07
 *  Author: gerav
 */ 

#include <avr/io.h>

#define invert 1					//Config PWM
#define non_invert 0


#ifndef PWMLIB_H_
#define PWMLIB_H_



/*------------------PWM---------------------------*/
void init_pwm1(uint8_t invertido, uint16_t prescaler);
/*------------------------------------------------*/

/*----------------DUTYCycle-----------------------*/
void DutyCycle1(uint16_t duty_cycle1);
/*------------------------------------------------*/



#endif /* PWMLIB_H_ */