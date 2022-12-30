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

/**
 * Generic register macros used as a general approach
 * to selecting the available registers depending on
 * MCU currently selected as the target.
 *
 * These can be overridden to use a specifict USART
 * register by defining the 'LOGGER_PORT' macro.
 *
 */

#define UBRRH__(x) UBRR ## x ## H
#define UBRRL__(x) UBRR ## x ## L
#define UCSRA__(x) UCSR ## x ## A
#define UCSRB__(x) UCSR ## x ## B
#define UCSRC__(x) UCSR ## x ## C
#define UCSZ2__(x) UCSZ ## x ## 2
#define UCSZ1__(x) UCSZ ## x ## 1
#define UCSZ0__(x) UCSZ ## x ## 0
#define UDR__(x)   UDR  ## x
#define UDRE__(x)  UDRE ## x
#define TXEN__(x)  TXEN ## x

#define UBRRH_(x) UBRRH__(x)
#define UBRRL_(x) UBRRL__(x)
#define UCSRA_(x) UCSRA__(x)
#define UCSRB_(x) UCSRB__(x)
#define UCSRC_(x) UCSRC__(x)
#define UCSZ2_(x) UCSZ2__(x)
#define UCSZ1_(x) UCSZ1__(x)
#define UCSZ0_(x) UCSZ0__(x)
#define UDR_(x)   UDR__(x)
#define UDRE_(x)  UDRE__(x)
#define TXEN_(x)  TXEN__(x)

#define UBRRH UBRRH_(LOGGER_PORT)
#define UBRRL UBRRL_(LOGGER_PORT)
#define UCSRA UCSRA_(LOGGER_PORT)
#define UCSRB UCSRB_(LOGGER_PORT)
#define UCSRC UCSRC_(LOGGER_PORT)
#define UCSZ2 UCSZ2_(LOGGER_PORT)
#define UCSZ1 UCSZ1_(LOGGER_PORT)
#define UCSZ0 UCSZ0_(LOGGER_PORT)
#define UDR   UDR_(LOGGER_PORT)
#define UDRE  UDRE_(LOGGER_PORT)
#define TXEN  TXEN_(LOGGER_PORT)


/************************************************************************/
/* Function implementations                                             */
/************************************************************************/

void logger_init(baudrate_t baud)
{
  UBRRH = (uint8_t)(baud >> 8);
  UBRRL = (uint8_t) baud;
  UCSRB = (1 << TXEN);

  // 8-bit 1-stop bit
  UCSRC = (1 << UCSZ1) | (1 << UCSZ0);
}

void logger_print(const char *msg)
{
  while(*msg) {
    // Wait for the TX buffer to be empty
    while (!(UCSRA & (1 << UDRE)));
    UDR = *msg;
    msg++;
  }
}
