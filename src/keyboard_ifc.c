/************************************************************************/
/* Includes                                                             */
/************************************************************************/

#include "keyboard_ifc.h"

#include "delay.h"
#include "pinout.h"
#include "pin_config.h"

#include <avr/interrupt.h>
#include <stdint.h>

/************************************************************************/
/* Defines                                                              */
/************************************************************************/

#define OCR_DEBOUNCE_VAL 250 // This w prescaler @256 => 4ms debounce
#define DEBOUNCE_MAX     5   // ~20ms total debounce

#define ROW_NUM 4
#define COL_NUM 3

#define COL_MASK 0x07 // 3 cols => 00000111
#define ROW_MASK 0x0F // 4 rows => 00001111

#define NUM_CHANS 15
#define POT_CHAN  0

/************************************************************************/
/* Private function declarations                                        */
/************************************************************************/

static void keyboard_init_timer(void);

/************************************************************************/
/* Private variables                                                    */
/************************************************************************/

static volatile uint8_t browse_debounce  = 0;
static volatile uint8_t confirm_debounce = 0;
static volatile uint8_t pot_data = 0;


static port_t * keyboard_ctl;
static uint8_t  browse;
static uint8_t  confirm;

/************************************************************************/
/* Function implementations                                             */
/************************************************************************/

void keyboard_init(keyboard_t * key)
{
	// Initialize keyboard rows as outputs
	// columns as pulled-up inputs
	setup_with_mask(key->output, ROW_MASK);
	setup_with_cleared_mask(key->input, COL_MASK);

	set_port(key->output, ROW_MASK);
	set_port(key->input, 0x0f & COL_MASK);

	as_input_pin(key->buttons_port, key->browse);
	as_input_pin(key->buttons_port, key->confirm);
	as_input_pin(key->adc_port, key->potentiometer);
	keyboard_init_timer();
}

bool keyboard_acquire(keyboard_t * key, uint16_t *mask)
{
	uint16_t cur_mask = 0;
	uint8_t columns;
	
	for(int i = 0; i < ROW_NUM; i++) {
		// ground one by one to check for key presses
		clear_pin(key->output, i);
		delay_ms(2);
		columns = read_port(key->input) & COL_MASK;
		
		for(int j = 0; j < COL_NUM; j++) {
			uint16_t key_idx = ((ROW_NUM-1)*i + j);
			if(!(columns & (1 << j))) {
				cur_mask |= 1 << key_idx;
			} else {
				cur_mask &= ~(1 << key_idx);
			}
		}
		set_pin(key->output, i);
	}

	if(cur_mask != *mask) {
		// New key press
		*mask = cur_mask & 0x0FFF;
		return true;
	}
	return false;
}

bool keyboard_get_browse(void)
{
	return browse_debounce == DEBOUNCE_MAX;
}

bool keyboard_get_confirm(void)
{
	return confirm_debounce == DEBOUNCE_MAX;
}

void keyboard_init_adc(void)
{
	ADMUX  = (1 << REFS1) | (1 << REFS0) |  // Internal 1.1V ref, ADC0
					 (1 << ADLAR);                  // Left adjust the result
	ADCSRA = (1 << ADIE)  |                 // Interrupt mode
					 (1 << ADPS2) | (1 << ADPS1) | (1 << ADPS0); // 128-prescaling

	ADMUX  |= (POT_CHAN);
	ADCSRA |= (1 << ADSC);
}

void keyboard_enable_potentiometer(void)
{
	ADCSRA |= (1 << ADEN);
}

void keyboard_disable_potentiometer(void)
{
	ADCSRA &= ~(1 << ADEN);
}

uint8_t keyboard_get_potentiometer(void)
{
	return pot_data;
}

/************************************************************************/
/* Private Helpers                                                      */
/************************************************************************/

/**
 * Initializes the confirm and browse keys, together with the
 * timer used to debounce them. 
 */
static void keyboard_init_timer(void)
{
	OCR0A = OCR_DEBOUNCE_VAL;

	TCCR0A = (0 << COM0A1) | (0 << COM0A0) | // Normal operation
	         (1 << WGM01)  | (0 << WGM00);   // Normal mode

	TCCR0B = (0 << WGM02) |                            // No CTC
	         (1 << CS02)  | (0 << CS01) | (0 << CS00); // f_clk/256 prescaler

	// Enable the compare match interrupt for channel A
	TIMSK0 = (1 << OCIE0A) | (0 << TOIE0);
}



ISR(ADC_vect)
{
	// TODO acquire in 16 bit mode and mask it? envelope frequency needs 16bit
	// with left adjustment this register contains ADC[2:9]
	// and can be read without update problems
	pot_data = ADCH;
	ADCSRA |= (1 << ADSC);
}


ISR(TIMER0_COMPA_vect)
{
	if(read_pin(keyboard_ctl, browse) && browse_debounce < DEBOUNCE_MAX) {
		browse_debounce++;
	}

	if(!read_pin(keyboard_ctl, browse) && browse_debounce > 0) {
		browse_debounce--;
	}
	
	if(read_pin(keyboard_ctl, confirm) && confirm_debounce < DEBOUNCE_MAX) {
		confirm_debounce++;
	}

	if(!read_pin(keyboard_ctl, confirm) && confirm_debounce > 0) {
		confirm_debounce--;
	}
}
