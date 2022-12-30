/** @file logger.h
 *
 * This module implements a basic logger over USART.
 */

#ifndef LOGGER_H_
#define LOGGER_H_

/************************************************************************/
/* Defines                                                              */
/************************************************************************/

#ifndef F_CPU
#warning The logger module requires to define F_CPU, defaulting to 16MHz
#define F_CPU 16000000
#endif

/**
 * The baud rate formula was taken from section 22.5 of the ATMega2560
 * datasheet, and is based on having the F_CPU macro at disposal. This
 * formula works for the ATMega644 too.
 */
#define BAUD_VAL(b) (F_CPU / 16 / b - 1)

/************************************************************************/
/* Typedefs                                                             */
/************************************************************************/

/**
 * @brief Supported baud rates
 */
typedef enum {
  BAUD_RATE_9600  = BAUD_VAL(9600), /**< 9600 baud */
  BAUD_RATE_38400 = BAUD_VAL(38400) /**< 38400 baud */
} baudrate_t;

/************************************************************************/
/* Public functions                                                     */
/************************************************************************/

/**
 * @brief Initializes the USART perpipheral used by the logger module
 * @param baud one of the supported baud rates
 */
void logger_init(baudrate_t baud);

/**
 * @brief Prints a message through the initialized USART
 * @param msg the biffer containing the message
 */
void logger_print(const char *msg);

#endif
