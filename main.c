#include "1602a_lcd.h"
#include "delay.h"
#include "Ay38910a.h"
#include "keyboard_ifc.h"

#include <stdint.h>

/* WRITE_BUF for easy printf-like debug through the lcd */
#include <stdio.h>

char buf[32];
#define WRITE_BUF(s, ...) snprintf(buf, 32, s, __VA_ARGS__)



int main(void)
{
	ay38910_init();
	lcd1602a_init();
	keyboard_init();

	lcd1602a_display_on();
	lcd1602a_clear();
	lcd1602a_home();

	uint16_t mask   = 0;
	uint8_t  octave = 5; // octave is fixed to 5 for testing purposes
	uint8_t  channel_mask = 0;
	
	while(1)
	{
		if(keyboard_acquire(&mask))
		{
			uint8_t count = 0;
			for(uint8_t i = 0; (i < KEYBOARD_KEYS) && (count < CHANNEL_NUM); i++)
			{
				if(mask & (1 << i))
				{
					// With noise channels, this works with count+OFFSET
					channel_mask &= ~(1 << count);
					Channel chan = (Channel)(2*count);
					ay38910_channel_mode(channel_mask);
					ay38910_set_amplitude(chan, 15);
					ay38910_play_note(chan, NOTE(1+i, octave));
					count++;
				}
			}
			
			// this for is called anyways even if no new closed 
			// close unused channels due to a key not being pressed anymore
			for(uint8_t ch = count ; ch < CHANNEL_NUM; ch++)
			{
				channel_mask |= (1 << count);
				Channel chan = (Channel)(2*ch);
				ay38910_channel_mode(channel_mask);
				ay38910_set_amplitude(chan, 0);
			}
			WRITE_BUF("%x", mask);
			lcd1602a_print_row(buf, 0);
		}
		delay_ms(2);
	}
}
