
#ifndef LOGGER_H_
#define LOGGER_H_

/************************************************************************/
/* Includes                                                             */
/************************************************************************/

/************************************************************************/
/* Defines                                                              */
/************************************************************************/

/**
 * These baud rate values to be passed to the UBRR register were taken 
 * section 22.11 of theATMega2560 datasheet, and are valid with a 16MHz
 * clock signal.
 */

#ifndef F_CPU
#error The logger module requires to define the F_CPU 
#endif

// Based on the ATMega2560 datasheet
#define BAUD_VAL(b) (F_CPU / 16 / b - 1)

/**********************
**************************************************/
/* Typedefs                                                             */
/************************************************************************/

typedef enum {
  BAUD_RATE_9600  = BAUD_VAL(9600),
  BAUD_RATE_38400 = BAUD_VAL(38400)
} baudrate_t;

/************************************************************************/
/* Public functions                                                     */
/************************************************************************/

void logger_init(baudrate_t baud);
void print_error(const char *msg);

#endif
