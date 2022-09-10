/**
 * potentiometer.c
 * 
 * This module handles the potentiometer sampling.
 * The adc can be enabled by thekeyboard_en/disable APIs; once it is 
 * active, it continously samples the potentiometer. The most recent 
 * value can be obtained by calling keyboard_get_potentiometer.
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

/**
 * Initializes the adc for the keyboard potentiometer 
 */
void keyboard_init_adc(void)
{
	InitInPin(potentiometer);

	ADMUX  = (1 << REFS1) | (1 << REFS0) |  // Internal 1.1V ref, ADC0
	         (1 << ADLAR);                  // Left adjust the result
	ADCSRA = (1 << ADIE)  |                 // Interrupt mode
	         (1 << ADPS2) | (1 << ADPS1) | (1 << ADPS0); // 128-prescaling
	
	ADMUX  |= (POT_CHAN);
	ADCSRA |= (1 << ADSC);
}

/**
 * Enables the adc peripheral to acquire data from the 
 * potentiometer
 */
void keyboard_enable_potentiometer(void)
{
	ADCSRA |= (1 << ADEN);
}

/**
 * Disables the adc peripheral to acquire data from the 
 * potentiometer
 */
void keyboard_disable_potentiometer(void)
{
	ADCSRA &= ~(1 << ADEN);
}

/**
 * Gets the latest potentiometer adc sample
 */
inline __attribute__((always_inline))
uint8_t keyboard_get_potentiometer(void)
{
	return pot_data;
}

// TODO acquire in 16 bit mode and mask it? envelope frequency needs 16bit
ISR(ADC_vect)
{
	// with left adjustment this register contains ADC[2:9]
	// and can be read without update problems
	pot_data = ADCH;
	ADCSRA |= (1 << ADSC);
}
