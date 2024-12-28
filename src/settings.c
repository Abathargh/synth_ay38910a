/************************************************************************/
/* Includes                                                             */
/************************************************************************/

#include "settings.h"

#include <avr/interrupt.h>
#include <ay38910a.h>
#include <avr/io.h>
#include <stdio.h>
#include <usart.h>

/************************************************************************/
/* Defines                                                              */
/************************************************************************/

#define LCD_BUF_SIZE (20)
#define CHAR_CVT_ERR (10)
#define BUF_SIZE     (3)
#define POT_CHAN     (0)

#define IS_DIGIT(n)  (n >= 0x30 && n <= 0x39)
#define IS_HEX_HI(n) (n >= 0x41 && n <= 0x46)
#define IS_HEX_LO(n) (n >= 0x61 && n <= 0x66)

/************************************************************************/
/* Private function declarations                                        */
/************************************************************************/

static uint8_t u8_from_hex_char(char c);
static char hex_char_from_u8(uint8_t u);
static void adc_init(void);

/************************************************************************/
/* Private variables                                                    */
/************************************************************************/

/**
 * Frame format (as hex digits encoded chars)
 * | amplitude | octave   | envelope shape |
 * | frame[0]  | frame[1] | frame[2]       |
 */
static volatile char    recv_buf[BUF_SIZE] = {0};
static volatile uint8_t idx                =  0;
static volatile uint8_t pot_data           =  0;


static char lcd_buf[LCD_BUF_SIZE] = {0};

static const usart_t * serial = &(usart_t) {
	.baud_hi=&UBRR0H,
	.baud_lo=&UBRR0L,
	.ctl_a=  &UCSR0A,
	.ctl_b=  &UCSR0B,
	.ctl_c=  &UCSR0C,
	.udr=    &UDR0,
};

/************************************************************************/
/* Function implementations                                             */
/************************************************************************/

void stg_init(void) {
	// as_input_pull_up_pin(stg->nav_pin.port, stg->nav_pin.pin);
	// as_input_pull_up_pin(stg->sel_pin.port, stg->sel_pin.pin);
	usart_init(serial, BAUD_VAL(9600));
	adc_init();
	sei();
}

void stg_update_from_frame(settings_t * s) {
	cli();
	uint8_t ret;
	ret = u8_from_hex_char(*recv_buf);
	s->amplitude = ret == CHAR_CVT_ERR ? AMP_DEF : ret;

	ret = u8_from_hex_char(*(recv_buf + 1));
	s->octave = ret == CHAR_CVT_ERR ? OCT_DEF : ret;

	ret = u8_from_hex_char(*(recv_buf + 2));
	s->env_shape = ret == CHAR_CVT_ERR ? SHP_DEF : ret;
}

void stg_send_frame(const settings_t * s) {
	char buf[5];
	buf[0] = hex_char_from_u8(s->amplitude);
	buf[1] = hex_char_from_u8(s->octave);
	buf[2] = hex_char_from_u8(s->env_shape);
	buf[3] = '\n';
	buf[4] = '\0';
	usart_write(serial, buf);
	idx = 0;
	sei();
}

uint8_t stg_received_data(void) {
	return idx == BUF_SIZE;
}

void stg_print_settings(const lcd1602a_t * lcd, const settings_t * s) {
	snprintf(lcd_buf, LCD_BUF_SIZE, "amp: %d oct: %d", s->amplitude, s->octave);
	lcd1602a_print_row(lcd, lcd_buf, 0);
}

void stg_enable_potentiometer(void)
{
	ADCSRA |= (1 << ADEN);
	ADCSRA |= (1 << ADSC);
}

void stg_disable_potentiometer(void)
{
	ADCSRA &= ~(1 << ADEN);
}

uint8_t stg_read_potentiometer(void)
{
	return pot_data;
}

//static uint8_t menu_cardinality[] = {
//	[MENU_AMPLITUDE] = AMPLITUDE_CARD,
//	[MENU_OCTAVE]    = OCTAVE_CARD,
//	[MENU_WAVEFORM]  = WAVEFORM_CARD,
//};

void stg_menu_loop(settings_t * stg) {
	uint8_t nav_pressed = read_debounced(stg->nav_pin) == 0x00;
	uint8_t sel_pressed = read_debounced(stg->sel_pin) == 0x00;

	static uint8_t last_nav_pressed = 0xff;
	static uint8_t last_sel_pressed = 0xff;

	if(nav_pressed && last_nav_pressed != 0x00) {
		stg->selected = (stg->selected + 1) % 3;
		last_nav_pressed = 0x00;
	} else {
		last_nav_pressed = 0xff;
	}

	if(sel_pressed && last_sel_pressed != 0x00) {
		stg->selected = (stg->selected + 1) % 3;
		last_sel_pressed = 0x00;
	} else {
		last_sel_pressed = 0xff;
	}

}

/************************************************************************/
/* Private Helpers                                             	         */
/************************************************************************/

static void adc_init(void) {
	ADMUX  = (1 << REFS1) | (1 << REFS0) |  // Internal 1.1V ref, ADC0
					 (1 << ADLAR);                  // Left adjust the result
	ADCSRA = (1 << ADIE)  |                 // Interrupt mode
					 (1 << ADPS2) | (1 << ADPS1) | (1 << ADPS0); // 128-prescaling

	ADMUX  |= (POT_CHAN);
	ADCSRA |= (1 << ADSC);
}

static uint8_t u8_from_hex_char(char c) {
	if(IS_DIGIT(c))  return c - 0x30;
	if(IS_HEX_HI(c)) return c - 0x37;
	if(IS_HEX_LO(c)) return c - 0x57;
	return CHAR_CVT_ERR;
}

static char 	hex_char_from_u8(uint8_t u) {
	if(u <= 9)             return (char)(u + 0x30);
	if(u >= 10 && u <= 15) return (char)(u + 0x57);
	return 0;
}

ISR(USART0_RX_vect,) {
	char recv = (char)*serial->udr;
	if(idx < BUF_SIZE) {
		recv_buf[idx++] = recv;
	}
}

ISR(ADC_vect,) {
	// TODO acquire in 16 bit mode and mask it? envelope frequency needs 16bit
	// with left adjustment this register contains ADC[2:9]
	// and can be read without update problems
	pot_data = ADCH;
	ADCSRA |= (1 << ADSC);
}
