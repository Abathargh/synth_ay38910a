/************************************************************************/
/* Includes                                                             */
/************************************************************************/

#include "pin_config.h"
#include <delay.h>

/************************************************************************/
/* Defines                                                              */
/************************************************************************/

#define INLINED __attribute__((always_inline)) inline

#define DEBOUNCE_RES (8)

/************************************************************************/
/* Function implementations                                             */
/************************************************************************/

INLINED
void as_output_pin(port_t * p, uint8_t pin) {
	*p->direction |= (1 << pin);
}

INLINED
void as_input_pin(port_t * p, uint8_t pin) {
	*p->direction &= ~(1 << pin);
}

INLINED
void as_output_port(port_t * p) {
	*p->direction = 0xff;
}

INLINED
void as_input_port(port_t * p) {
	*p->direction = 0x00;
}

INLINED
void setup_with_mask(port_t * p, uint8_t mask) {
	*p->direction |= mask;
}

INLINED
void setup_with_cleared_mask(port_t * p, uint8_t mask) {
	*p->direction &= ~mask;
}

INLINED
void as_input_pull_up_pin(port_t * p, uint8_t pin) {
	*p->direction &= ~(1 << pin);
	*p->output |= (1 << pin);
}

INLINED
void as_input_pull_up_port(port_t * p) {
	*p->direction = 0x00;
	*p->output = 0xff;
}

INLINED
void disable_pull_up(port_t * p, uint8_t pin)
{
	*p->output &= ~(1 << pin);
}

INLINED
void set_pin(port_t * p, uint8_t pin)
{
	*p->output |= (1 << pin);
}

INLINED
void clear_pin(port_t * p, uint8_t pin) {
	*p->output &= ~(1 << pin);
}

INLINED
void toggle_pin(port_t * p, uint8_t pin) {
	*p->output ^= (1 << pin);
}

INLINED
uint8_t read_pin(port_t * p, uint8_t pin){
 return (*p->input & (1 << pin)) >> pin;
}

INLINED
void set_port(port_t * p, uint8_t val) {
	*p->output = val;
}

INLINED
void put_hi_port(port_t * p, uint8_t val) {
	*p->output = (*p->output & 0x0f) | (val & 0xf0);
}

INLINED
void set_port_mask(port_t * p, uint8_t mask) {
	*p->output |= mask;
}

INLINED
void clear_port_mask(port_t * p, uint8_t mask) {
	*p->output &= ~mask;
}

INLINED
uint8_t read_port(port_t * p) {
	return *p->input;
}

INLINED
uint8_t read_port_mask(port_t * p, uint8_t mask) {
	return *p->input & mask;
}

uint8_t read_debounced(pin_t p) {
	uint8_t acc = 0;
	uint8_t ctr;
	do {
		ctr = 0;
		while (ctr++ < DEBOUNCE_RES) {
			acc = (acc << 1) | (read_pin(p.port, p.pin));
			delay_us(500);
		}
	} while(acc != 0x00 && acc != 0xff);
	return acc;
}
