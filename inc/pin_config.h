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

/************************************************************************/
/* Defines                                                              */
/************************************************************************/

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

void set_port(port_t * p, uint8_t val);

void set_port_mask(port_t * p, uint8_t mask);

void clear_port_mask(port_t * p, uint8_t mask);

uint8_t read_port(port_t * p);

uint8_t read_port_mask(port_t * p, uint8_t mask);

#endif /* PIN_CONFIG_H_ */
