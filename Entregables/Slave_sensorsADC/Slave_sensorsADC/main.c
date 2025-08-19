/*
 * Slave_sensorsADC.c
 *
 * Created: 18/08/2025 1:47:18
 * Author : gerav
 */ 


/********************************************************************/
/*--------------------- Header (Libraries)--------------------------*/
#define F_CPU 16000000UL
#include <avr/interrupt.h>
#include <avr/io.h>
#include <util/delay.h>
#include <stdlib.h>

#include "TWI_lib/TWI_lib.h"
#include "ADC_lib/ADC_lib.h"
#include "PWMlib/PWMlib.h"

/********************************************************************/



/********************************************************************/
/*-----------------------Function Prototypes------------------------*/
void setup();
void traffic_light();
uint8_t calculate_rain_percentage(uint16_t adc_value);
void update_servo_position(uint8_t percentage);

/********************************************************************/



/********************************************************************/
/*-----------------------Variable Declaration-----------------------*/
uint16_t channel1 = 1;
uint16_t channelRain = 0;

volatile uint16_t adc_val1 = 0;		// Guardamos el valor de ADC1
volatile uint16_t rain_adc = 0;		// Valor del sensor de lluvia
volatile uint8_t rain_percent = 0;	// Porcentaje de lluvia calculado

uint8_t buffer;
uint8_t slave_Address = 0x40;

// Variables traffic light
volatile uint8_t G = 0;
volatile uint8_t R = 0;
volatile uint8_t Y = 0;

volatile uint8_t current_channel = 1;		// 1 = MQ-135, 0 = YL-83

// Variables servo
#define SERVO_MIN 2000						// 0 grados
#define SERVO_MAX 4000						// 90 grados
volatile uint8_t servo_moved = 0;

/********************************************************************/



/********************************************************************/
/*------------------------------Main--------------------------------*/
int main(void)
{
	setup();
	
	while (1)
	{
		traffic_light();
		
		// Control del servomotor
		if(rain_percent >= 45 && !servo_moved){
			update_servo_position(rain_percent);
			servo_moved = 1;
		}
		else if(rain_percent < 45 && servo_moved){
			update_servo_position(0);
			servo_moved = 0;
		}
	}
}

/********************************************************************/



/********************************************************************/
/*---------------------Non-interrupt Subroutines--------------------*/
void setup()
{
	cli();
	
	// Entradas
	


	// Salidas
	DDRD |= (1 << PORTD2) | (1 << PORTD3) | (1 << PORTD4);							// Led indicadores
	PORTD &= ~((1 << PORTD2) | (1 << PORTD3) | (1 << PORTD4));
	
	
	// Configuracion ADC
	configADC(channel1);
	
	// I2C configuracion
	TWI_Slave_Init(slave_Address);
	
	// Config PWM
	init_pwm1(non_invert, 64);
	
	// Posicion inicial
	DutyCycle1(SERVO_MIN);
		
	
	sei();
}

void traffic_light()
{
	PORTD &= ~((1 << PORTD2) | (1 << PORTD3) | (1 << PORTD4));
	
	if(G){
		PORTD |= (1 << PORTD2);
	}
	if(Y){
		PORTD |= (1 << PORTD3);
	}
	if(R){
		PORTD |= (1 << PORTD4);
	}
}

// Calculamos el porcentaje de lluvia basado en el valor ADC
uint8_t calculate_rain_percentage(uint16_t adc_value)
{
	// Valores de referencia
	const uint16_t MAX_RESISTANCE = 100;
	const uint16_t MIN_RESISTANCE = 0;
	const uint16_t DRY_VALUE = 30;
	const uint16_t WET_VALUE = 600;
	
	if(adc_value < DRY_VALUE) return MAX_RESISTANCE;
	if(adc_value > WET_VALUE) return MIN_RESISTANCE;
	
	return MAX_RESISTANCE - ((adc_value - DRY_VALUE) * (MAX_RESISTANCE - MIN_RESISTANCE) / (WET_VALUE - DRY_VALUE));
}

void update_servo_position(uint8_t pecentage)
{
	uint16_t servo_pos;
	
	if(pecentage >= 45){
		DutyCycle1(SERVO_MAX);
	}else{
		DutyCycle1(SERVO_MIN);
	}
	
	_delay_ms(20);
}

/********************************************************************/



/********************************************************************/
/*----------------------Interrupt Subroutines-----------------------*/
ISR(ADC_vect)
{
	if(current_channel == 1){
		// Leemos el MQ-135
		adc_val1 = ADC;
		
		// Umbrales para MQ-135
		if(adc_val1 < 60){
			G = 1;
			Y = 0;
			R = 0;
		}
		else if(adc_val1 >= 60 && adc_val1 < 200){
			G = 0;
			Y = 1;
			R = 0;
		}
		else if(adc_val1 >= 200 &&  adc_val1 < 600){
			G = 0;
			Y = 0;
			R = 1;
		}
		
		// Cambiamos de sensor
		current_channel = 0;
		configChannel(current_channel);
	}
	else{
		rain_adc = ADC;
		
		// Porcentaje
		rain_percent = calculate_rain_percentage(rain_adc);
		
		// Volvemos a MQ-135
		current_channel = 1;
		configChannel(current_channel);
	}
	
	ADCSRA |= (1 << ADSC);							// Empezamos nueva conversion
}

ISR(TWI_vect)
{
	uint8_t state;
	state = TWSR & 0xFC;
	
	switch(state)
	{
		// Slave Receiver
		case 0x60:
		case 0x70:
		TWCR = (1 << TWINT) | (1 << TWEA) | (1 << TWEN) | (1 << TWIE);
		break;
		
		case 0x80:
		case 0x90:
		buffer = TWDR;
		TWCR = (1 << TWINT) | (1 << TWEA) | (1 << TWEN) | (1 << TWIE);
		break;
		
		// Slave Transmitter
		case 0xA8:
		
		if(buffer == 1){
			TWDR = adc_val1;
		}
		else if(buffer == 2){
			TWDR = rain_percent; 
		}
		else{
			TWDR = 0;
		}
		TWCR = (1 << TWINT) | (1 << TWEA) | (1 << TWEN) | (1 << TWIE);
		break;
		
		case 0xB8:
		TWCR = (1 << TWINT) | (1 << TWEA) | (1 << TWEN) | (1 << TWIE);
		break;
		
		default:
		TWCR = (1 << TWINT) | (1 << TWEA) | (1 << TWEN) | (1 << TWIE);
		break;
	}
}




/**********************************************************/


