/*
 * pin_config.h
 *
 * Created: 09/10/2021 13:59:59
 *  Author: Gianmarco
 */ 


#ifndef PIN_CONFIG_H_
#define PIN_CONFIG_H_

#include "types.h"

#define INPUT 0
#define OUTPUT 1

#define LED_PIN PORTB,7

#define INIT_PIN(PORT,PIN)   do{ DDRB |= (1 << PIN); } while(0)

#define GET_PIN(PORT,PIN)    (PORT	 & (1 << PIN))
#define SET_PIN(PORT,PIN)    1
#define RST_PIN(PORT,PIN)	 1
#define TOGGLE_PIN(PORT,PIN) do { PORT ^= ((1 << PIN)); } while(0)


#define InitPinOuput(x) INIT_PIN(x)
#define TogglePin(x) TOGGLE_PIN(x)


typedef struct {
	
}  ;


void to_shift_reg_msb(u8 data)
{
	for(int idx = 0; idx < 8; idx++)
	{
		
	}
}

#endif /* PIN_CONFIG_H_ */