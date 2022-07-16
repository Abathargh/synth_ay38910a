#ifndef PIN_CONFIG_H_
#define PIN_CONFIG_H_

/************************************************************************/
/* Includes                                                             */
/************************************************************************/

#include <avr/io.h>

/************************************************************************/
/* Defines                                                              */
/************************************************************************/

#define INIT_OUT_PIN(PRT,PIN)   ((DDR ## PRT)  |= (1 << PIN))
#define INIT_IN_PIN(PRT,PIN)    ((DDR ## PRT)  &=  ~(1 << PIN))
#define INIT_PULL_PIN(PRT,PIN)  ((PORT ## PRT) |= (1 << PIN))

#define GET_PIN(PRT,BIT)        ((PIN ## PRT)  &   (1 << BIT))
#define SET_PIN(PRT,PIN)        ((PORT ## PRT) |=  (1 << PIN))
#define RST_PIN(PRT,PIN)        ((PORT ## PRT) &= ~(1 << PIN))

#define INIT_OUT_PORT(PRT)      ((DDR ## PRT)  = 0xFF)
#define INIT_IN_PORT(PRT)       ((DDR ## PRT)  = 0x00)
#define INIT_PORT(PRT,DATA)     ((DDR ## PRT)  = DATA)
#define INIT_OR_PORT(PRT,DATA)  ((DDR ## PRT) |= DATA)
#define INIT_PULL_PORT(PRT,PIN) ((PORT ## PRT) = 0xFF)

#define SET_PORT(PRT,DATA)      ((PORT ## PRT) = DATA)
#define GET_PORT(PRT)           ((PIN ## PRT))


#define InitOutPin(PIN)         INIT_OUT_PIN(PIN)
#define InitInPin(PIN)          INIT_IN_PIN(PIN)
#define InitPullPin(PIN)        INIT_PULL_PIN(PIN)

#define SetPin(PIN)             SET_PIN(PIN)
#define ResetPin(PIN)           RST_PIN(PIN)
#define GetPin(PIN)             GET_PIN(PIN)

#define InitOutPort(PORT)       INIT_OUT_PORT(PORT)
#define InitInPort(PORT)        INIT_IN_PORT(PORT)
#define InitPort(PORT,DATA)     INIT_PORT(PORT,DATA)
#define InitOrPort(PORT,DATA)   INIT_OR_PORT(PORT,DATA)
#define InitPullPort(PORT)      INIT_PULL_PORT(PORT)

#define SetPort(PORT,DATA)      SET_PORT(PORT,DATA)
#define GetPort(PORT)           GET_PORT(PORT)

#endif /* PIN_CONFIG_H_ */
