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

#include "types.h"

/************************************************************************/
/* Defines                                                              */
/************************************************************************/

#define INPUT 0
#define OUTPUT 1

#define B B

#define INIT_OUT_PIN(PORT,PIN)  do{ DDR ## PRT |= (1 << PIN); } while(0)
#define INIT_OUT_PORT(PRT)      (DDR ## PRT = 0xFF);

#define GET_PIN(PRT,PIN)        (PRT ## PRT &   (1 << PIN))
#define SET_PIN(PRT,PIN)        (PRT ## PRT |=  (1 << PIN))
#define RST_PIN(PRT,PIN)        (PRT ## PRT &= ~(1 << PIN))

/************************************************************************/
/* Typedefs                                                             */
/************************************************************************/

typedef enum {
	PORT_A,
	PORT_B,
	PORT_C,
	PORT_D,
	PORT_E,
	PORT_F,
	PORT_G,
	PORT_H,
	PORT_J,
	PORT_K,
	PORT_L,
} Port;

typedef struct {
	Port port;
	uint8_t pin_number;
} Pin;

#endif /* PIN_CONFIG_H_ */
