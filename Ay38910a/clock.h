#ifndef AY3891_SYNTH_CLOCK_H
#define AY3891_SYNTH_CLOCK_H

/************************************************************************/
/* Includes                                                             */
/************************************************************************/

#include <avr/io.h>
#include "pin_config.h"
#include "pinout.h"

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
#define OCR2AVAL       3

/************************************************************************/
/* Private function declarations                                        */
/************************************************************************/

/************************************************************************/
/* Private variables                                                    */
/************************************************************************/

/************************************************************************/
/* Public functions                                                     */
/************************************************************************/

/**
 * Initializes Timer2 in Toggle on Compare Match mode, in order
 * to output a square wave on the OC2A Pin (PB4).
 *
 * @param ocr2a_value the value of the timer threshold
 * @retval None
 */
void oc2a_pin_config(uint8_t ocr2a_value)
{
    InitOutPin(oc2a_pin);

    OCR2A = ocr2a_value;

    TCCR2A = (0 << COM2A1) | (1 << COM2A0) | // Enable output signal on OC2A pin
             (1 << WGM21)  | (0 << WGM20);   // Enable Clear Timer on Compare Mode

    TCCR2B = (0 << WGM22) |                            // MSB output enable
             (0 << CS22)  | (0 << CS21) | (1 << CS20); // Clock select with no prescaler

    // Disable the compare match interrupt for register A
    TIMSK2 = 0;
}


#endif //AY3891_SYNTH_CLOCK_H
