/*
 * pin_config.h
 *
 * Author: mar
 */ 


#ifndef PIN_CONFIG_H_
#define PIN_CONFIG_H_

#include "types.h"

#define INPUT 0
#define OUTPUT 1

#define LED_PIN PORTB,7

#define INIT_PIN(PORT,PIN)   do{ DDRB |= (1 << PIN); } while(0)
#define INIT_OUT_PORT(PORT) (PORT = 0xFF);

#define GET_PIN(PORT,PIN)    (PORT &   (1 << PIN))
#define SET_PIN(PORT,PIN)    (PORT |=  (1 << PIN))
#define RST_PIN(PORT,PIN)	 (PORT &= ~(1 << PIN))

#define 

// define here the pins to use and set them in the main/init so that ay module does not refer to io arv
#endif /* PIN_CONFIG_H_ */
