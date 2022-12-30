/** @file 1602a_lcd.h
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

#include <stdint.h>

/************************************************************************/
/* Defines                                                              */
/************************************************************************/

/**
 * @brief Initializes the lcd peripheral following the protocol
 * described in the HD44780U datasheet
 */
void lcd1602a_init(void);

/**
 * @brief Puts the display on
 */
void lcd1602a_display_on(void);

/**
 * @brief Puts the display off
 */
void lcd1602a_display_off(void);

/**
 * @brief Puts the cursor back to the starting position
 */
void lcd1602a_home(void);

/**
 * @brief Clears the display completely
 */
void lcd1602a_clear(void);

/**
 * @brief Clears a row of the display
 * @param row the row to clear, 0-1
 */
void lcd1602_clear_row(uint8_t row);

/**
 * @brief Sets the cursor position
 * @param x the x coordinate of the new position
 * @param y the y coordinate of the new position
 */
void lcd1602a_set_cursor(uint8_t x, uint8_t y);

/**
 * @brief Prints a single char to the last set position of the cursor.
 * @param c the char to print, as an ASCII character
 */
void lcd1602a_putchar(unsigned char c);

/**
 * @brief Prints a c-string to the display
 * @param str a null terminated ASCII string
 */
void lcd1602a_print(const char *str);

/**
 * @brief Prints a c-string to the display on the specified row
 * @param str a null terminated ASCII string
 * @param row the row where to print the string
 */
void lcd1602a_print_row(const char *str, uint8_t row);

#endif
