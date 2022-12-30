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

static void forward_data(void);
static void init_contrast(void);
static void send_command(unsigned char cmd);


/************************************************************************/
/* Private variables                                                    */
/************************************************************************/

static const uint8_t ROW_OFFSETS[] = {ROW0_OFFSET, ROW1_OFFSET};

/************************************************************************/
/* Function implementations                                             */
/************************************************************************/

void lcd1602a_init(void)
{
	InitOutPin(rs_pin);
	InitOutPin(en_pin);
	InitOrPort(lcd_bus, 0xF0);

	// Pull the rs pin down and wait for
	// more than 40ms at init time
	ResetPin(rs_pin);
	delay_ms(2000);

	// Set the interface to 8-bit waiting > 4.1 ms
	SetHiPort(lcd_bus, FUNCTION_SET_8B1L);
	forward_data();
	delay_ms(16);

	// Set the interface to 8-bit waiting > 100 us
	SetHiPort(lcd_bus, FUNCTION_SET_8B1L);
	forward_data();
	delay_us(400);

	// Set the interface to 8-bit waiting > 100 us
	SetHiPort(lcd_bus, FUNCTION_SET_8B1L);
	forward_data();
	delay_us(400);

	// Set the interface to 4-bit 1 line waiting > 100 us
	SetHiPort(lcd_bus, FUNCTION_SET_4B1L);
	forward_data();
	delay_us(160);

	// The lcd is initialized, these last operations
	// allows the use of the lcd in 4-bit mode with 2 lines
	send_command(FUNCTION_SET_4B2L);
	send_command(DISPLAY_OFF);
	send_command(CLEAR_DISPLAY);
	send_command(ENTRY_SET_DEF_LAT);
	init_contrast();
}

void lcd1602a_display_on(void)
{
	send_command(DISPLAY_ON);
}

void lcd1602a_display_off(void)
{
	send_command(DISPLAY_OFF);
}

void lcd1602a_home(void)
{
	send_command(RETURN_HOME);
}

void lcd1602a_clear(void)
{
	send_command(CLEAR_DISPLAY);
}

void lcd1602_clear_row(uint8_t row)
{
	/**
	 * 	N.B. the HD44780U does not support single row clears, so this
	 * 	just prints	spaces, which is suboptimal => O(n)
	 */
	lcd1602a_set_cursor(row, 0);
	for(size_t idx = 0; idx < NUM_COLS; idx++) {
		lcd1602a_putchar(' ');
	}
}

void lcd1602a_set_cursor(uint8_t x, uint8_t y)
{
	send_command(SET_DDRAM_ADDR | ((y % NUM_COLS) + ROW_OFFSETS[x % NUM_ROWS]));
}

void lcd1602a_putchar(unsigned char c)
{
	SetPin(rs_pin);

	SetHiPort(lcd_bus, c & 0xf0);
	forward_data();

	SetHiPort(lcd_bus, (c << 4) & 0xf0);
	forward_data();

	delay_us(2000);
}

void lcd1602a_print(const char *str)
{
	lcd1602a_clear();
	lcd1602a_print_row(str, 0);
}

void lcd1602a_print_row(const char *str, uint8_t row)
{
	// TODO maybe an option to clear only if the user wants
	// TODO strlen heavy, maybe change api and pass len?
	size_t len = strlen(str);
	size_t idx = 0;

	lcd1602_clear_row(row);
	lcd1602a_set_cursor(row, 0);
	for(; (idx < len) && (idx < NUM_COLS); idx++)  {
		lcd1602a_putchar(str[idx]);
	}
}

/************************************************************************/
/* Private Helpers                                                      */
/************************************************************************/

/**
 * Sends a command to the lcd display
 * @param cmd the command to actuate
 */
static void send_command(unsigned char cmd)
{
	ResetPin(rs_pin);

	SetHiPort(lcd_bus, cmd & 0xf0);
	forward_data();

	SetHiPort(lcd_bus, (cmd << 4) & 0xf0);
	forward_data();

	delay_us(2000);
}

/**
 * Forwards the data currently in the lcd bus to the display
 */
static inline __attribute__((always_inline))
void forward_data(void)
{
	SetPin(en_pin);
	delay_us(2);
	ResetPin(en_pin);
	delay_us(2);
}

/**
 * Initializes the a PWM signal to be used to control the contrast
 * of the lcd display.
 */
static void init_contrast(void)
{
	InitOutPin(pwm_ch0);

	// N.B. if using 2 rows, you need a higher V for the same
	// contrast level
	OCR1A = CONTRAST_DT;

	TCCR1A = (1 << COM1A1) | (0 << COM1A0) | (1 << WGM11) | (1 << WGM10);
	TCCR1B = (0 << WGM12)  | (0 << CS12)   | (0 << CS11)  | (1 << CS10);
}
