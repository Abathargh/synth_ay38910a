/**
 * Ay38910a.c
 *
 * Author: mar
 */ 

/************************************************************************/
/* Includes                                                             */
/************************************************************************/

#include "Ay38910a.h"

#include <avr/io.h>

/************************************************************************/
/* Defines                                                              */
/************************************************************************/

/**
 * We want to generate a 2MHz square wave as the input signal for
 * the clock pin of the AY38910A. Consider the following:
 *
 * f_clk = 16 MHz => T_clk = 0.0625 us
 * 
 * and knowing that:
 *
 * f_desired 2MHz => T_desired = 0.5 us
 *
 * we know that we have to generate a signal that peaks every 
 * T_desired/2, so T_del = 0.25 us
 *
 * OCR2VAL = T_clk/T_del - 1 = (0.0625 us / 0.25 us) - 1 = 3 
 */
#define OCR2AVAL 3

/************************************************************************/
/* Private function declarations                                        */
/************************************************************************/

static void clock_init(void);

/************************************************************************/
/* Private variables                                                    */
/************************************************************************/

ay_pin_config _sConfig;

/************************************************************************/
/* Function implementations                                             */
/************************************************************************/


void ay38910_init(ay_pin_config conf)
{
	_sConfig = conf;
	clock_init();
}

/************************************************************************/
/* Private Helpers                                                      */
/************************************************************************/

/**
 * Initializes the TIMER2A so that the OC2A pin outputs a 
 * 2MHz clock signal that can be used to drive the AY38910A
 *
 * @retval None
 */
static void clock_init(void)
{
	// Reset the counter
	TCNT2 = 0x00;
	
	// Sets the frequency to 2MHz 
	OCR2A = OCR2AVAL;
	
	TCCR2A = (0 << COM2A1) | (1 << COM2A0) | // Enable output signal on OC2A pin 
			 (1 << WGM21)  | (1 << WGM20);   // Enable Clear Timer on Compare Mode
	
	TCCR2B = (0 << WGM22) |                            // MSB output enable 
			 (0 << CS22)  | (0 << CS21) | (1 << CS20); // Clock select with no prescaler
	
	TIMSK2 = 0; // Disable the compare match interrupt for register A
}

