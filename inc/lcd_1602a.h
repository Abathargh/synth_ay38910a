/** @file lcd_1602a.h
 *
 * This module implements the logic that drives the 1602a lcd screen.
 * This driver is based on the contents of the HD44780U datasheet,
 * using the timing diagrams and modes of operations as references.
 *
 * The implementation is geared towards driving the display in 4-bit
 * mode and is limited to a certain subset of functionalities that
 * are actually needed for this firmware.
 *
 * The library offers the possibility to drive the contrast pin
 * through a PWM signal.
 */

#ifndef AY38910A_SYNTH_LCD_1602A_H
#define AY38910A_SYNTH_LCD_1602A_H

/************************************************************************/
/* Includes                                                             */
/************************************************************************/

#include "pin_config.h"
#include "timer.h"

#include <stdint.h>

/************************************************************************/
/* Typedefs                                                             */
/************************************************************************/

typedef struct lcd1602a_t {
	port_t * ctl_port;
	port_t * bus_port;
	uint8_t  register_sel;
	uint8_t  enable;
} lcd1602a_t;

/************************************************************************/
/* Public functions                                                     */
/************************************************************************/

/**
 * @brief Initializes the lcd peripheral following the protocol
 * described in the HD44780U datasheet
 */
void lcd1602a_init(const lcd1602a_t * lcd, const timer_t * t);

/**
 * @brief Puts the display on
 */
void lcd1602a_display_on(const lcd1602a_t * lcd);

/**
 * @brief Puts the display off
 */
void lcd1602a_display_off(const lcd1602a_t * lcd);

/**
 * @brief Puts the cursor back to the starting position
 */
void lcd1602a_home(const lcd1602a_t * lcd);

/**
 * @brief Clears the display completely
 */
void lcd1602a_clear(const lcd1602a_t * lcd);

/**
 * @brief Clears a row of the display
 * @param row the row to clear, 0-1
 */
void lcd1602_clear_row(const lcd1602a_t * lcd, uint8_t row);

/**
 * @brief Sets the cursor position
 * @param x the x coordinate of the new position
 * @param y the y coordinate of the new position
 */
void lcd1602a_set_cursor(const lcd1602a_t * lcd, uint8_t x, uint8_t y);

/**
 * @brief Prints a single char to the last set position of the cursor.
 * @param c the char to print, as an ASCII character
 */
void lcd1602a_putchar(const lcd1602a_t * lcd, unsigned char c);

/**
 * @brief Prints a c-string to the display
 * @param str a null terminated ASCII string
 */
void lcd1602a_print(const lcd1602a_t * lcd, const char *str);

/**
 * @brief Prints a c-string to the display on the specified row
 * @param str a null terminated ASCII string
 * @param row the row where to print the string
 */
void lcd1602a_print_row(const lcd1602a_t * lcd, const char *str, uint8_t row);

#endif
