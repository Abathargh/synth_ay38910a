/************************************************************************/
/* Includes                                                             */
/************************************************************************/

#include "lcd_1602a.h"

#include "delay.h"
#include "pinout.h"
#include "pin_config.h"

#include <string.h>

/************************************************************************/
/* Defines                                                              */
/************************************************************************/

#define INLINED inline __attribute__((always_inline))

#define CONTRAST_DT 220

#define NUM_ROWS    2
#define NUM_COLS    16

#define ROW0_OFFSET 0x00
#define ROW1_OFFSET 0x40

#define CLEAR_DISPLAY     0x01
#define RETURN_HOME       0x02
#define ENTRY_SET_DEF_LAT 0x06
#define DISPLAY_OFF       0x08
#define DISPLAY_ON        0x0C
#define FUNCTION_SET_4B1L 0x20
#define FUNCTION_SET_4B2L 0x28
#define FUNCTION_SET_8B1L 0x30
#define SET_DDRAM_ADDR    0x80

/************************************************************************/
/* Private function declarations                                        */
/************************************************************************/

static void forward_data(const lcd1602a_t * lcd);
static void init_contrast(const timer_t * t);
static void send_command(const lcd1602a_t * lcd, unsigned char cmd);

/************************************************************************/
/* Private variables                                                    */
/************************************************************************/

static const uint8_t ROW_OFFSETS[] = {ROW0_OFFSET, ROW1_OFFSET};

/************************************************************************/
/* Function implementations                                             */
/************************************************************************/

void lcd1602a_init(const lcd1602a_t * lcd, const timer_t * t)
{
	as_output_pin(lcd->ctl_port, lcd->register_sel);
	as_output_pin(lcd->ctl_port, lcd->enable);
	setup_with_mask(lcd->bus_port, 0xf0);

	// Pull the rs pin down and wait for
	// more than 40ms at init time
	clear_pin(lcd->ctl_port, lcd->register_sel);
	delay_ms(2000);

	// Set the interface to 8-bit waiting > 4.1 ms
	set_port_mask(lcd->bus_port, FUNCTION_SET_8B1L);
	forward_data(lcd);
	delay_ms(16);

	// Set the interface to 8-bit waiting > 100 us
	set_port_mask(lcd->bus_port, FUNCTION_SET_8B1L);
	forward_data(lcd);
	delay_us(400);

	// Set the interface to 8-bit waiting > 100 us
	set_port_mask(lcd->bus_port, FUNCTION_SET_8B1L);
	forward_data(lcd);
	delay_us(400);

	// Set the interface to 4-bit 1 line waiting > 100 us
	set_port_mask(lcd->bus_port, FUNCTION_SET_4B1L);
	forward_data(lcd);
	delay_us(160);

	// The lcd is initialized, these last operations
	// allows the use of the lcd in 4-bit mode with 2 lines
	send_command(lcd, FUNCTION_SET_4B2L);
	send_command(lcd, DISPLAY_OFF);
	send_command(lcd, CLEAR_DISPLAY);
	send_command(lcd, ENTRY_SET_DEF_LAT);
	init_contrast(t);
}

void lcd1602a_display_on(const lcd1602a_t * lcd)
{
	send_command(lcd, DISPLAY_ON);
}

void lcd1602a_display_off(const lcd1602a_t * lcd)
{
	send_command(lcd, DISPLAY_OFF);
}

void lcd1602a_home(const lcd1602a_t * lcd)
{
	send_command(lcd, RETURN_HOME);
}

void lcd1602a_clear(const lcd1602a_t * lcd)
{
	send_command(lcd, CLEAR_DISPLAY);
}

void lcd1602_clear_row(const lcd1602a_t * lcd, uint8_t row)
{
	/**
	 * 	N.B. the HD44780U does not support single row clears, so this
	 * 	just prints	spaces, which is suboptimal => O(n)
	 */
	lcd1602a_set_cursor(lcd, row, 0);
	for(size_t idx = 0; idx < NUM_COLS; idx++) {
		lcd1602a_putchar(lcd, ' ');
	}
}

void lcd1602a_set_cursor(const lcd1602a_t * lcd, uint8_t x, uint8_t y)
{
	send_command(lcd, SET_DDRAM_ADDR | ((y % NUM_COLS) + ROW_OFFSETS[x % NUM_ROWS]));
}

void lcd1602a_putchar(const lcd1602a_t * lcd, unsigned char c)
{
	set_pin(lcd->ctl_port, lcd->register_sel);

	set_port_mask(lcd->bus_port, c & 0xf0);
	forward_data(lcd);

	set_port_mask(lcd->bus_port, (c << 4) & 0xf0);
	forward_data(lcd);

	delay_us(2000);
}

void lcd1602a_print(const lcd1602a_t * lcd, const char *str)
{
	lcd1602a_clear(lcd);
	lcd1602a_print_row(lcd, str, 0);
}

void lcd1602a_print_row(const lcd1602a_t * lcd, const char *str, uint8_t row)
{
	// TODO maybe an option to clear only if the user wants
	// TODO strlen heavy, maybe change api and pass len?
	size_t len = strlen(str);
	size_t idx = 0;

	lcd1602_clear_row(lcd, row);
	lcd1602a_set_cursor(lcd, row, 0);
	for(; (idx < len) && (idx < NUM_COLS); idx++)  {
		lcd1602a_putchar(lcd, str[idx]);
	}
}

/************************************************************************/
/* Private Helpers                                                      */
/************************************************************************/

/**
 * Sends a command to the lcd display
 * @param cmd the command to actuate
 */
static void send_command(const lcd1602a_t * lcd, unsigned char cmd)
{
	clear_pin(lcd->ctl_port, lcd->register_sel);

	set_port_mask(lcd->bus_port, cmd & 0xf0);
	forward_data(lcd);

	set_port_mask(lcd->bus_port, (cmd << 4) & 0xf0);
	forward_data(lcd);

	delay_us(2000);
}

/**
 * Forwards the data currently in the lcd bus to the display
 */
static INLINED
void forward_data(const lcd1602a_t * lcd)
{
	set_pin(lcd->ctl_port, lcd->enable);
	delay_us(2);
	clear_pin(lcd->ctl_port, lcd->enable);
	delay_us(2);
}

/**
 * Initializes the a PWM signal to be used to control the contrast
 * of the lcd display.
 */
static void init_contrast(const timer_t * t)
{
	as_output_pin(t->ocr_a_port, t->ocr_a_pin);

	// N.B. if using 2 rows, you need a higher V for the same
	// contrast level
	*t->ocr_a_16 = CONTRAST_DT; // 16-bit register: ocr_a_16 active
	*t->tccr_a = 0x83;          // PWM, Phase Correct, 10-bit 0x03FF, Clear OCnA/OCnB on Compare Match when up-
		                          // counting. Set OCnA/OCnB on Compare Match when down-counting.
	*t->tccr_b = 0x01;          // No prescaling
}
