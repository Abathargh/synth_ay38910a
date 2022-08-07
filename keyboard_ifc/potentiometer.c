
/**
 * potentiometer.c
 * 
 */

/************************************************************************/
/* Includes                                                             */
/************************************************************************/

#include "keyboard_ifc.h"
#include "pin_config.h"
#include "pinout.h"

#include <avr/io.h>
#include <avr/interrupt.h>
#include <stdint.h>

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

static volatile uint8_t pot_data = 0;

/************************************************************************/
/* Function implementations                                             */
/************************************************************************/


void keyboard_init_adc(void)
{
	InitInPin(potentiometer);

	ADMUX  = (1 << REFS1) | (1 << REFS0) |  // Internal 1.1V Voltage reference, ADC0
	         (1 << ADLAR);                  // Left adjust the conversion result
	ADCSRA = (1 << ADIE)  |                 // Enable ADC in interrupt mode
	         (1 << ADPS2) | (1 << ADPS1) | (1 << ADPS0); // freq prescaling = 128
	
	ADMUX  |= (POT_CHAN);
	ADCSRA |= (1 << ADSC);
}

void keyboard_enable_potentiometer(void)
{
	ADCSRA |= (1 << ADEN);
}

void keyboard_disable_potentiometer(void)
{
	ADCSRA &= ~(1 << ADEN);
}

inline __attribute__((always_inline))
uint8_t keyboard_get_potentiometer(void)
{
	return pot_data;
}

ISR(ADC_vect)
{
	// with left adjustment this register contains ADC[2:9]
	// and can be read without update problems
	pot_data = ADCH;
	ADCSRA |= (1 << ADSC);
}
