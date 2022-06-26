/**
 * logger.c
 *
 * This module implements a basic logger over USART.
 *
 * Author: mar
 */

/************************************************************************/
/* Includes                                                             */
/************************************************************************/

#include "logger.h"

#include <stdint.h>
#include <avr/io.h>

/************************************************************************/
/* Defines                                                              */
/************************************************************************/

// the default logger uses USART3
#ifndef LOGGER_PORT

#if defined(__AVR_ATmega2560__)
#define LOGGER_PORT 3
#elif defined(__AVR_ATmega644__)
#define LOGGER_PORT 0 
#endif

#endif


#define __UBRRH(x) UBRR ## x ## H
#define __UBRRL(x) UBRR ## x ## L
#define __UCSRA(x) UCSR ## x ## A
#define __UCSRB(x) UCSR ## x ## B
#define __UCSRC(x) UCSR ## x ## C
#define __UCSZ2(x) UCSZ ## x ## 2
#define __UCSZ1(x) UCSZ ## x ## 1
#define __UCSZ0(x) UCSZ ## x ## 0
#define __UDR(x)   UDR  ## x
#define __UDRE(x)  UDRE ## x

#define _UBRRH(x) __UBRRH(x) 
#define _UBRRL(x) __UBRRL(x) 
#define _UCSRA(x) __UCSRA(x) 
#define _UCSRB(x) __UCSRB(x) 
#define _UCSRC(x) __UCSRC(x) 
#define _UCSZ2(x) __UCSZ2(x) 
#define _UCSZ1(x) __UCSZ1(x) 
#define _UCSZ0(x) __UCSZ0(x) 
#define _UDR(x)   __UDR(x)
#define _UDRE(x)  __UDRE(x)

#define UBRRH _UBRRH(LOGGER_PORT)
#define UBRRL _UBRRL(LOGGER_PORT)
#define UCSRA _UCSRA(LOGGER_PORT)
#define UCSRB _UCSRB(LOGGER_PORT)
#define UCSRC _UCSRC(LOGGER_PORT)
#define UCSZ2 _UCSZ2(LOGGER_PORT)
#define UCSZ1 _UCSZ1(LOGGER_PORT)
#define UCSZ0 _UCSZ0(LOGGER_PORT)
#define UDR   _UDR(LOGGER_PORT)
#define UDRE  _UDRE(LOGGER_PORT)


/************************************************************************/
/* Function implementations                                             */
/************************************************************************/


void logger_init(baudrate_t baud)
{
  UBRRH = (uint8_t)(baud >> 8); 
  UBRRL = (uint8_t) baud;
  UCSRB = (1 << TXEN3);  

  // 8-bit 1-stop bit
  UCSRC = (1 << UCSZ2) | (1 << UCSZ1) | (1 << UCSZ0);  
}

void logger_print(const char *msg)
{
  while(*msg)
  {
    // Wait for the TX buffer to be empty
    while (!(UCSRA & (1 << UDRE)));
    UDR = *msg;
    msg++;
  }
}
