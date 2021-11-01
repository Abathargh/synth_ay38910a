/*
 * pin_config.h
 *
 * Author: mar
 */ 


#ifndef PIN_CONFIG_H_
#define PIN_CONFIG_H_

/************************************************************************/
/* Includes                                                             */
/************************************************************************/

#include <avr/io.h>

/************************************************************************/
/* Defines                                                              */
/************************************************************************/

#define INPUT 0
#define OUTPUT 1

#define OC2A_PIN                PORTB,4
#define BC1_PIN                 PORTH,5
#define BDIR_PIN                PORTH,6

#define INIT_OUT_PIN(PORT,PIN)  (PORT |= (1 << PIN))
#define INIT_OUT_PORT(PORT)     (PORT = 0xFF);

#define SET_PORT(PORT,DATA)     (PORT = DATA)

#define GET_PIN(PORT,PIN)       (PORT &   (1 << PIN))
#define SET_PIN(PORT,PIN)       (PORT |=  (1 << PIN))
#define RST_PIN(PORT,PIN)       (PORT &= ~(1 << PIN))

#define INIT_BUS_PORT()         INIT_OUT_PORT(PORTB)
#define SET_BUS_PORT(DATA)      SET_PORT(PORTB,DATA)

#define InitOutPin(PIN)         INIT_OUT_PIN(PIN)
#define SetPin(PIN)             SET_PIN(PIN)
#define ResetPin(PIN)           RST_PIN(PIN)

/************************************************************************/
/* Typedefs                                                             */
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
	InitOutPin(OC2A_PIN);
	// Reset the counter
	TCNT2 = 0x00;
	
	// Sets the frequency to 2MHz
	OCR2A = ocr2a_value;
	
	TCCR2A = (0 << COM2A1) | (1 << COM2A0) | // Enable output signal on OC2A pin
	(1 << WGM21)  | (0 << WGM20);   // Enable Clear Timer on Compare Mode
	
	TCCR2B = (0 << WGM22) |                            // MSB output enable
	(0 << CS22)  | (0 << CS21) | (1 << CS20); // Clock select with no prescaler
	
	// Disable the compare match interrupt for register A
	TIMSK2 = 0;
}

#endif /* PIN_CONFIG_H_ */
