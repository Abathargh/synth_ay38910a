/************************************************************************/
/* Includes                                                             */
/************************************************************************/

#include "pin_config.h"

/************************************************************************/
/* Defines                                                              */
/************************************************************************/

#define INLINED __attribute__((always_inline)) inline

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
