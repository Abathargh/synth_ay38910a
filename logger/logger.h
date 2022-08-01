
#ifndef LOGGER_H_
#define LOGGER_H_

/************************************************************************/
/* Includes                                                             */
/************************************************************************/

/************************************************************************/
/* Defines                                                              */
/************************************************************************/

/**
 * The baud rate formula was taken from section 22.5 of the ATMega2560 
 * datasheet, and is based on having the F_CPU macro at disposal. This 
 * formula works for the ATMega644 too.
 */

#ifndef F_CPU
#warning The logger module requires to define F_CPU, defaulting to 16MHz
#define F_CPU 16000000
#endif

#define BAUD_VAL(b) (F_CPU / 16 / b - 1)

/************************************************************************/
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
void logger_print(const char *msg);

#endif
