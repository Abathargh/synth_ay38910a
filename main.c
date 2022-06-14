#include "Ay38910a.h"
#include "1602a_lcd.h"
#include <stdio.h>
#include <avr/interrupt.h>

#include "keyboard_ifc.h"

static char buf[32];
#define WRITE_BUF(s, ...) snprintf(buf, 32, s, __VA_ARGS__)

volatile uint8_t note = 20;

/*
 * TODO:
 *  - button test w debounce w and wo ISR (or better polling in timer)
 *  - adc acquisition w potentiometer w ISR
 *  - matrix keyboard
 *  - i2c w pcf8574
 *  - debug uart?
 * 	- application logic
 */


int main(void)
{
	ay38910_init();

	lcd1602a_init();
	lcd1602a_display_on();
	lcd1602a_home();
	lcd1602a_clear();


	ay38910_channel_mode(CHA_TONE_ENABLE);
	ay38910_set_amplitude(CHANNEL_A, 15);

	keyboard_init();

	sei();

	bool pressed = false;
	bool last = is_pressed();
	while(1)
	{
		pressed = is_pressed();
		if(pressed != last)
		{
			if(pressed)
			{
				ay38910_set_amplitude(CHANNEL_A, 15);
				ay38910_play_note(CHANNEL_A, note);
				WRITE_BUF("Note: %d", note);
				lcd1602a_print_row(buf, 0);
				note++;
			}
			else
			{
				ay38910_set_amplitude(CHANNEL_A, 0);
			}

			WRITE_BUF("Button: %d", pressed);
			lcd1602a_print_row(buf, 1);
		}
		last = pressed;
	}
}


