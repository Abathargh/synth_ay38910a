#include "1602a_lcd.h"
#include "Ay38910a.h"
#include "keyboard_ifc.h"

#include <stdint.h>

int main(void)
{
	ay38910_init();
	ay38910_channel_mode(CHA_TONE_ENABLE);
	ay38910_set_amplitude(CHANNEL_A, 15);
	
	keyboard_init();

	uint8_t mask = 0;
	uint8_t octave = 4;
	
	while(1)
	{
		if(keyboard_acquire(&mask))
		{
			char count = 0;
			for(uint8_t i = 0; (i < sizeof(mask) * 8) && count <=3; i++)
			{
				if(mask & (1 << i))
				{
					Channel chan = (Channel)(2*count); // TODO define as macro?
					ay38910_set_amplitude(chan, 15);
					ay38910_play_note(chan, NOTE(1+i, octave));
					count++;
				}
			}
			
			// Close unused channels due to a key not being pressed anymore
			for(uint8_t ch = count ; ch < CHANNEL_NUM; ch++)
			{
				ay38910_set_amplitude((Channel)ch, 0);
			}
		}
		delay_ms(2);
	}
}
