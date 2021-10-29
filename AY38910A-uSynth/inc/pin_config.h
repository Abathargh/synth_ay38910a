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

#define INIT_OUT_PIN(PORT,PIN)  do{ DDR ## PRT |= (1 << PIN); } while(0)
#define INIT_OUT_PORT(PRT)      (DDR ## PRT = 0xFF);

#define GET_PIN(PRT,PIN)        (PRT ## PRT &   (1 << PIN))
#define SET_PIN(PRT,PIN)        (PRT ## PRT |=  (1 << PIN))
#define RST_PIN(PRT,PIN)        (PRT ## PRT &= ~(1 << PIN))

#endif /* PIN_CONFIG_H_ */
