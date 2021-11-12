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

/************************************************************************/
/* Defines                                                              */
/************************************************************************/

#define INPUT 0
#define OUTPUT 1

#define INIT_OUT_PIN(PORT,PIN)  (PORT |= (1 << PIN))
#define INIT_OUT_PORT(PORT)     (PORT = 0xFF);

#define SET_PORT(PORT,DATA)     (PORT = DATA)

#define GET_PIN(PORT,PIN)       (PORT &   (1 << PIN))
#define SET_PIN(PORT,PIN)       (PORT |=  (1 << PIN))
#define RST_PIN(PORT,PIN)       (PORT &= ~(1 << PIN))

#define InitOutPin(PIN)         INIT_OUT_PIN(PIN)
#define SetPin(PIN)             SET_PIN(PIN)
#define ResetPin(PIN)           RST_PIN(PIN)



#endif /* PIN_CONFIG_H_ */
