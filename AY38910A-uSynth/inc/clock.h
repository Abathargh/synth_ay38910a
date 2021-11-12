/*
 * clock.h
 *
 *  Author: mar
 */ 


#ifndef CLOCK_H_
#define CLOCK_H_

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

#endif /* CLOCK_H_ */
