/** @file usart.h
 *
 * This module implements a basic interface over USART.
 */

#ifndef LOGGER_H_
#define LOGGER_H_

/************************************************************************/
/* Defines                                                              */
/************************************************************************/

#include <pin_config.h>

/************************************************************************/
/* Defines                                                              */
/************************************************************************/

#ifndef F_CPU
#warning The usart module requires to define F_CPU, defaulting to 16MHz
#define F_CPU 16000000
#endif

/**
 * The baud rate formula was taken from section 22.5 of the ATMega2560
 * datasheet, and is based on having the F_CPU macro at disposal. This
 * formula works for the ATMega644 too.
 */
#define BAUD_VAL(b) (F_CPU / 16 / b - 1)

#define USART_UNTIL_NEWLINE (0)

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

/**
 * @brief Supported baud rates
 */
typedef struct {
	map_io8 * baud_lo;
	map_io8 * baud_hi;
	map_io8 * ctl_a;
	map_io8 * ctl_b;
	map_io8 * ctl_c;
	map_io8 * udr;
} usart_t;


typedef enum {
	ctla_mpcm = 1 << 0,
	ctla_u2x  = 1 << 1,
	ctla_upe  = 1 << 2,
	ctla_dor  = 1 << 3,
	ctla_fe   = 1 << 4,
	ctla_udre = 1 << 5,
	ctla_txc  = 1 << 6,
	ctla_rxc  = 1 << 7,
} ctla_flag_e;


typedef enum {
	ctlb_txb8  = 1 << 0,
	ctlb_rxb8  = 1 << 1,
	ctlb_ucsz2 = 1 << 2,
	ctlb_txen  = 1 << 3,
	ctlb_rxen  = 1 << 4,
	ctlb_udrie = 1 << 5,
	ctlb_txcie = 1 << 6,
	ctlb_rxcie = 1 << 7,
} ctlb_flag_e;


typedef enum {
	ctlc_ucpol  = 1 << 0,
	ctlc_ucsz0  = 1 << 1,
	ctlc_ucsz1  = 1 << 2,
	ctlc_usbs   = 1 << 3,
	ctlc_upm0   = 1 << 4,
	ctlc_upm1   = 1 << 5,
	ctlc_umsel0 = 1 << 6,
	ctlc_umsel1 = 1 << 7,
} ctlc_flag_e;

/************************************************************************/
/* Public functions                                                     */
/************************************************************************/

/**
 * @brief Initializes the USART peripheral.
 * @param baud one of the supported baud rates
 */
void usart_init(const usart_t * usart, baudrate_t baud);

/**
 * @brief Prints a message through the initialized USART.
 * @param msg the buffer containing the message
 */
void usart_write(const usart_t * usart, const char *msg);

/**
 * @brief Reads data through the initialized USART
 * @param msg the buffer containing the message
 */
uint8_t usart_read(const usart_t * usart, char * buf, uint8_t len);

/**
 * @brief Reads a single byte through the initialized USART.
 * @param msg the buffer containing the message
 * @retval the received byte
 */
uint8_t usart_read_byte(const usart_t * usart);

/**
 * @brief Reads through the initialized USART, until a new line is found or
 * the max length passed is reached. A len of USART_UNTIL_NEWLINE (= 0) means
 * to just read until a new line character is received.
 * @param  msg the buffer containing the message
 * @retval the number of bytes read, excluding the new line character
 */
uint8_t usart_readline(const usart_t * usart, char * buf, uint8_t len);

#endif
