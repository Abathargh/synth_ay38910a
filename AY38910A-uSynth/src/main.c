/*
 * EmbeddedProject-AY38910A.c
 *
 * Author : mar
 */ 

#include "Ay38910a.h"


int main(void)
{
	u8 note = 60;
	ay38910_init();

	ay38910_channel_mode(CHA_TONE_ENABLE);
	ay38910_set_amplitude(CHANNEL_A, 15);
	ay38910_play_note(CHANNEL_A, note);

	while(1)
	{
	}
	return 0;
}

