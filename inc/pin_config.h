/** @file pin_config.h
 *
 * Macro collections to ease the writing of pin operations.
 */

#ifndef PIN_CONFIG_H_
#define PIN_CONFIG_H_

/************************************************************************/
/* Includes                                                             */
/************************************************************************/

#include <stdint.h>
#include <avr/io.h>

/************************************************************************/
/* Defines                                                              */
/************************************************************************/

#define IO_PORT_A {&DDRA, &PORTA, &PINA}
#define IO_PORT_B {&DDRB, &PORTB, &PINB}
#define IO_PORT_C {&DDRC, &PORTC, &PINC}
#define IO_PORT_D {&DDRD, &PORTD, &PIND}

#if defined(__AVR_ATmega2560__)
#define IO_PORT_E {&DDRE, &PORTE, &PINE}
#define IO_PORT_F {&DDRF, &PORTF, &PINF}
#define IO_PORT_G {&DDRG, &PORTG, &PING}
#define IO_PORT_H {&DDRH, &PORTH, &PINH}
#define IO_PORT_J {&DDRJ, &PORTJ, &PINJ}
#define IO_PORT_K {&DDRK, &PORTK, &PINK}
#define IO_PORT_L {&DDRL, &PORTL, &PINL}
#endif

/************************************************************************/
/* Typedefs                                                             */
/************************************************************************/

typedef volatile uint8_t map_io8;
typedef volatile uint16_t map_io16;

typedef struct port_t {
	map_io8 * direction;
	map_io8 * output;
	map_io8 * input;
} port_t;

typedef struct pin_t {
	port_t * port;
	uint8_t   pin;
} pin_t;

/************************************************************************/
/* Public functions                                                     */
/************************************************************************/

void as_output_pin(port_t * p, uint8_t pin);

void as_input_pin(port_t * p, uint8_t pin);

void as_output_port(port_t * p);

void as_input_port(port_t * p);

void setup_with_mask(port_t * p, uint8_t mask);

void setup_with_cleared_mask(port_t * p, uint8_t mask);

void as_input_pull_up_pin(port_t * p, uint8_t pin);

void as_input_pull_up_port(port_t * p);

void disable_pull_up(port_t * p, uint8_t pin);

void set_pin(port_t * p, uint8_t pin);

void clear_pin(port_t * p, uint8_t pin);

void toggle_pin(port_t * p, uint8_t pin);

uint8_t read_pin(port_t * p, uint8_t pin);

void put_hi_port(port_t * p, uint8_t val);

void set_port(port_t * p, uint8_t val);

void set_port_mask(port_t * p, uint8_t mask);

void clear_port_mask(port_t * p, uint8_t mask);

uint8_t read_port(port_t * p);

uint8_t read_port_mask(port_t * p, uint8_t mask);

uint8_t read_debounced(pin_t p);

#endif /* PIN_CONFIG_H_ */
