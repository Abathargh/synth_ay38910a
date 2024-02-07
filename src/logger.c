/************************************************************************/
/* Includes                                                             */
/************************************************************************/

#include "logger.h"

/************************************************************************/
/* Function implementations                                             */
/************************************************************************/

static uint8_t read_byte(const usart_t * usart);

void logger_init(const usart_t * usart, baudrate_t baud) {
  *usart->baud_hi = (uint8_t)(baud >> 8);
	*usart->baud_hi = (uint8_t) baud;
  *usart->ctl_b = ctlb_rxen | ctlb_txen;

  // 8-bit 1-stop bit
  *usart->ctl_c = ctlc_ucsz1 | ctlc_ucsz0;
}

void logger_write(const usart_t * usart, const char * msg) {
  while(*msg) {
    // Wait for the TX buffer to be empty
    while (!(*usart->ctl_a & ctla_udre));
    *usart->udr = *msg;
    msg++;
  }
}

uint8_t logger_readline(const usart_t * usart, char * buf, uint8_t len) {
  uint8_t count = 0;
	uint8_t byte  = read_byte(usart);
	while(byte != '\n' && count < len) {
		buf[count++] = (char)byte;
		byte = read_byte(usart);
  }
	buf[count] = '\0';
	return count-1;
}

static uint8_t read_byte(const usart_t * usart) {
	while (!(*usart->ctl_a & ctla_rxc));
	return *usart->udr;
}
