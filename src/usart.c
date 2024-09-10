/************************************************************************/
/* Includes                                                             */
/************************************************************************/

#include "usart.h"

/************************************************************************/
/* Function implementations                                             */
/************************************************************************/

void usart_init(const usart_t * usart, baudrate_t baud) {
  *usart->baud_hi = (uint8_t)(baud >> 8);
	*usart->baud_lo = (uint8_t) baud;
  *usart->ctl_b = (ctlb_rxen) | (ctlb_txen) | (ctlb_rxcie);

  // 8-bit 1-stop bit
  *usart->ctl_c = (ctlc_ucsz1) | (ctlc_ucsz0);
}

void usart_write(const usart_t * usart, const char * msg) {
  while(*msg) {
    // Wait for the TX buffer to be empty
    while (!(*usart->ctl_a & ctla_udre));
    *usart->udr = *msg;
    msg++;
  }
}

uint8_t usart_read_byte(const usart_t * usart) {
	while (!(*usart->ctl_a & ctla_rxc));
	return *usart->udr;
}

uint8_t usart_read(const usart_t * usart, char * buf, uint8_t len) {
	uint8_t count = 0;
	uint8_t byte  = usart_read_byte(usart);
	while(count < len) {
		buf[count++] = (char)byte;
		byte = usart_read_byte(usart);
	}
	buf[count] = '\0';
	return count-1;
}


uint8_t usart_readline(const usart_t * usart, char * buf, uint8_t len) {
  uint8_t count = 0;
	uint8_t byte  = usart_read_byte(usart);
	while(byte != '\n' && (len != USART_UNTIL_NEWLINE && count < len)) {
		buf[count++] = (char)byte;
		byte = usart_read_byte(usart);
  }
	buf[count] = '\0';
	return count-1;
}

