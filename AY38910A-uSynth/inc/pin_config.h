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

#include <stdint.h>

/************************************************************************/
/* Defines                                                              */
/************************************************************************/

#define INPUT 0
#define OUTPUT 1

#define INIT_OUT_PIN(PORT,PIN)  (*(PORT) |= (1 << PIN))
#define INIT_OUT_PORT(PORT)     (*(PORT) = 0xFF);

#define SET_PORT(PORT,DATA)     (*(PORT) = DATA)

#define GET_PIN(PORT,PIN)       (*(PORT) &   (1 << PIN))
#define SET_PIN(PORT,PIN)       (*(PORT) |=  (1 << PIN))
#define RST_PIN(PORT,PIN)       (*(PORT) &= ~(1 << PIN))

#define InitOutPin(PIN)         INIT_OUT_PIN(PIN.ddr,PIN.pin)
#define SetPin(PIN)             SET_PIN(PIN.port,PIN.pin)
#define ResetPin(PIN)           RST_PIN(PIN.port,PIN.pin)

#define InitOutPort(PORT)       INIT_OUT_PORT(PORT.ddr)
#define SetPort(PORT,DATA)      SET_PORT(PORT.port,DATA)

/************************************************************************/
/* Typedefs                                                             */
/************************************************************************/

typedef struct {
	volatile uint8_t *ddr;
	volatile uint8_t *port;
	volatile uint8_t  pin;
} Pin;

typedef struct {
	volatile uint8_t *ddr;
	volatile uint8_t *port;
} Port;


#endif /* PIN_CONFIG_H_ */
