/**
 * potentiometer.c
 *
 * Author: mar
 */

/************************************************************************/
/* Includes                                                             */
/************************************************************************/


#include "keyboard_ifc.h"
#include "pin_config.h"
#include "pinout.h"

#include <avr/io.h>
#include <avr/interrupt.h>

/************************************************************************/
/* Defines                                                              */
/************************************************************************/

#define NUM_CHANS 15
#define POT_CHAN  0

/************************************************************************/
/* Private function declarations                                        */
/************************************************************************/

/************************************************************************/
/* Private variables                                                    */
/************************************************************************/

static uint16_t readings[NUM_CHANS] = {};

/************************************************************************/
/* Function implementations                                             */
/************************************************************************/


void init_adc(void)
{

	InitInPin(potentiom);

	ADMUX  = (1 << REFS1) | (1 << REFS0);  // Internal 1.1V Voltage reference, ADC0

	ADCSRA = (1 << ADEN)  | (1 << ADIE)  | // Enable ADC in interrupt mode
           (1 << ADPS2) | (1 << ADPS1) | (1 << ADPS0); // freq prescaling = 128
	
	ADCSRA |= (1 << ADSC);
}

uint8_t get_potentiometer(void)
{
	return readings[POT_CHAN];
}

ISR(ADC_vect)
{
	readings[ADMUX & 0x1F] = ADCL | (ADCH << 8);
	ADCSRA |= (1 << ADSC);
}
