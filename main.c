#include "delay.h"
#include "Ay38910a.h"
#include "keyboard_ifc.h"

#include <stdint.h>

int main(void)
{
	ay38910_init();
	ay38910_channel_mode(CHA_TONE_ENABLE | CHB_TONE_ENABLE | CHC_TONE_ENABLE);
	
	keyboard_init();

	uint16_t mask   = 0;
	uint8_t  octave = 4; // octave is fixed to 4 for testing purposes
	
	while(1)
	{
		if(keyboard_acquire(&mask))
		{
			char count = 0;
			for(uint8_t i = 0; (i < KEYBOARD_KEYS) && (count <= CHANNEL_NUM); i++)
			{
				if(mask & (1 << i))
				{
					// volatile to prevent optimizations in debug mode
					volatile Channel chan = (Channel)(2*count);
					ay38910_set_amplitude(chan, 15);
					ay38910_play_note(chan, NOTE(1+i, octave));
					count++;
				}
			}
			
			// close unused channels due to a key not being pressed anymore
			for(uint8_t ch = count ; ch < CHANNEL_NUM; ch++)
			{
				ay38910_set_amplitude((Channel)(2*ch), 0);
			}
		}
		delay_ms(2);
	}
}
