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

#ifndef F_CPU
#error "F_CPU must be defined mor the logger to work"
#endif

#include "logger.h"

#include <stdint.h>
#include <avr/io.h>

/************************************************************************/
/* Defines                                                              */
/************************************************************************/

// the default logger uses USART3
#ifndef LOGGER_PORT
#define LOGGER_PORT 3
#endif

#define UBRRH UBRR ## LOGGER_PORT ## H
#define UBRRL UBRR ## LOGGER_PORT ## L
#define UCSRA UCSR ## LOGGER_PORT # A
#define UCSRB UCSR ## LOGGER_PORT # B
#define UCSRC UCSR ## LOGGER_PORT # C
#define UDR UDR ## LOGGER_PORT

#define UDRE UDRE ## LOGGER_PORT


/************************************************************************/
/* Function implementations                                             */
/************************************************************************/


void logger_init(baudrate_t baud)
{
  UBRR3H = (uint8_t)(baud >> 8); 
  UBRR3L = (uint8_t) baud;
  UCSR3B = (1 << TXEN3);  

  // 8-bit 1-stop bit
  UCSR3C = (1 << UCSZ32) | (1 << UCSZ31) | (1 << UCSZ30);  
}

void print_error(const char *msg)
{
  while(*msg)
  {
    // Wait for the TX buffer to be empty
    while (!(UCSR3A & (1<<UDRE3)));
    UDR3 = *msg;
    msg++;
  }
}
