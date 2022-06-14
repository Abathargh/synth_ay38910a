#include "keyboard_ifc.h"

#include <avr/interrupt.h>
#include <avr/io.h>

#include "pin_config.h"
#include "pinout.h"

#define DEBOUNCE_MAX 8

static volatile
int button_debounce = 0;


static void io_acquisition_timer(void);


ISR(TIMER1_COMPA_vect)
{
	if(GetPin(button) && button_debounce < DEBOUNCE_MAX)
	{
		button_debounce++;
	}

	if(!GetPin(button) && button_debounce > 0)
	{
		button_debounce--;
	}
}

void keyboard_init(void)
{
	InitInPin(button);
	io_acquisition_timer();
}

bool is_pressed(void)
{
	return button_debounce == DEBOUNCE_MAX;
}

static void io_acquisition_timer(void)
{
	OCR1A = 625;

	TCCR1A = (0 << COM1A1) | (0 << COM1A0) | // Normal operation
					 (1 << WGM11)  | (0 << WGM10);   // Normal mode

	TCCR1B = (0 << WGM12) |                            // No CTC
					 (1 << CS12)  | (0 << CS11) | (0 << CS10); // f_clk/256 prescaler

	// Enable the compare match interrupt for channel A
	TIMSK1 = (1 << OCIE1A) | (0 << TOIE1);
}