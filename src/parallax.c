#include "ay38910a.h"
#include "delay.h"

#define SIZE(x) ((uint8_t)(sizeof(x)/sizeof(x[0])))

struct note {
	int note;
	int duration;
};

const struct note parallax[] = {
	{NOTE(C_NOTE, 2), 500},
	{NOTE(F_SHARP_NOTE, 2), 250},
	{NOTE(A_SHARP_NOTE, 2), 550},
	{NOTE(F_SHARP_NOTE, 2), 500},
	{NOTE(C_NOTE, 2), 500},
	{NOTE(F_SHARP_NOTE, 2), 250},
	{NOTE(A_SHARP_NOTE, 2), 550},
	{NOTE(F_SHARP_NOTE, 2), 500},
	{NOTE(C_NOTE, 2), 250},
	{NOTE(F_SHARP_NOTE, 2), 250},

	{NOTE(C_NOTE, 2), 500},
	{NOTE(F_SHARP_NOTE, 2), 250},
	{NOTE(A_SHARP_NOTE, 2), 550},
	{NOTE(F_SHARP_NOTE, 2), 500},
	{NOTE(C_NOTE, 2), 500},
	{NOTE(F_SHARP_NOTE, 2), 250},
	{NOTE(A_SHARP_NOTE, 2), 550},
	{NOTE(F_SHARP_NOTE, 2), 500},
	{NOTE(C_NOTE, 2), 250},
	{NOTE(C_SHARP_NOTE, 2), 250},

	{NOTE(D_NOTE, 2), 500},
	{NOTE(F_SHARP_NOTE, 2), 250},
	{NOTE(A_SHARP_NOTE, 2), 550},
	{NOTE(F_SHARP_NOTE, 2), 500},
	{NOTE(D_NOTE, 2), 500},
	{NOTE(F_SHARP_NOTE, 2), 250},
	{NOTE(A_SHARP_NOTE, 2), 550},
	{NOTE(F_SHARP_NOTE, 2), 500},
	{NOTE(D_NOTE, 2), 250},
	{NOTE(F_SHARP_NOTE, 2), 250},

	{NOTE(D_NOTE, 2), 500},
	{NOTE(F_SHARP_NOTE, 2), 250},
	{NOTE(A_SHARP_NOTE, 2), 550},
	{NOTE(F_SHARP_NOTE, 2), 500},
	{NOTE(D_NOTE, 2), 500},
	{NOTE(F_SHARP_NOTE, 2), 250},
	{NOTE(A_SHARP_NOTE, 2), 250},
	{NOTE(D_NOTE, 2), 250},
	{NOTE(D_SHARP_NOTE, 2), 250},
	{NOTE(D_NOTE, 2), 250},
	{NOTE(C_NOTE, 2), 250},
	{NOTE(D_NOTE, 2), 250},
};

const struct note verse[] = {
	{NOTE(G_NOTE, 3), 300},
	{NOTE(C_NOTE, 3), 500},
	{NOTE(G_NOTE, 3), 300},
	{NOTE(C_NOTE, 3), 500},
	{NOTE(G_NOTE, 3), 250},
	{NOTE(C_NOTE, 3), 250},
	{NOTE(G_NOTE, 3), 300},
	{NOTE(C_NOTE, 3), 500},
	{NOTE(G_NOTE, 3), 300},
	{NOTE(C_NOTE, 3), 500},
	{NOTE(G_NOTE, 3), 250},
	{NOTE(C_NOTE, 3), 250},

	{NOTE(F_SHARP_NOTE, 3), 300},
	{NOTE(C_NOTE, 3), 500},
	{NOTE(F_SHARP_NOTE, 3), 300},
	{NOTE(C_NOTE, 3), 500},
	{NOTE(F_SHARP_NOTE, 3), 250},
	{NOTE(C_NOTE, 3), 250},
	{NOTE(F_SHARP_NOTE, 3), 300},
	{NOTE(C_NOTE, 3), 500},
	{NOTE(F_SHARP_NOTE, 3), 300},
	{NOTE(C_NOTE, 3), 500},
	{NOTE(F_SHARP_NOTE, 3), 250},
	{NOTE(C_NOTE, 3), 250},

	{NOTE(F_NOTE, 3), 300},
	{NOTE(G_NOTE, 2), 500},
	{NOTE(F_NOTE, 3), 300},
	{NOTE(G_NOTE, 2), 500},
	{NOTE(F_NOTE, 3), 250},
	{NOTE(G_NOTE, 2), 250},
	{NOTE(F_NOTE, 3), 300},
	{NOTE(G_NOTE, 2), 500},
	{NOTE(F_NOTE, 3), 300},
	{NOTE(G_NOTE, 2), 500},
	{NOTE(F_NOTE, 3), 250},
	{NOTE(G_NOTE, 2), 250},

	{NOTE(D_SHARP_NOTE, 3), 300},
	{NOTE(F_SHARP_NOTE, 2), 500},
	{NOTE(D_SHARP_NOTE, 3), 300},
	{NOTE(F_SHARP_NOTE, 2), 500},
	{NOTE(D_SHARP_NOTE, 3), 250},
	{NOTE(F_SHARP_NOTE, 2), 250},
	{NOTE(D_SHARP_NOTE, 3), 300},
	{NOTE(F_SHARP_NOTE, 2), 500},
	{NOTE(D_SHARP_NOTE, 3), 300},
	{NOTE(F_SHARP_NOTE, 2), 500},
	{NOTE(D_SHARP_NOTE, 3), 250},
	{NOTE(F_SHARP_NOTE, 2), 250},

	{NOTE(G_NOTE, 2), 300},
	{NOTE(C_NOTE, 2), 500},
	{NOTE(G_NOTE, 2), 300},
	{NOTE(C_NOTE, 2), 500},
	{NOTE(G_NOTE, 2), 250},
	{NOTE(C_NOTE, 2), 250},
	{NOTE(G_NOTE, 2), 300},
	{NOTE(C_NOTE, 2), 500},
	{NOTE(G_NOTE, 2), 300},
	{NOTE(C_NOTE, 2), 500},
	{NOTE(G_NOTE, 2), 250},
	{NOTE(C_NOTE, 2), 250},

	{NOTE(F_SHARP_NOTE, 2), 300},
	{NOTE(C_NOTE, 2), 500},
	{NOTE(F_SHARP_NOTE, 2), 300},
	{NOTE(C_NOTE, 2), 500},
	{NOTE(F_SHARP_NOTE, 2), 250},
	{NOTE(C_NOTE, 2), 250},
	{NOTE(F_SHARP_NOTE, 2), 300},
	{NOTE(C_NOTE, 2), 500},
	{NOTE(F_SHARP_NOTE, 2), 300},
	{NOTE(C_NOTE, 2), 500},
 	{NOTE(F_SHARP_NOTE, 2), 250},
	{NOTE(C_NOTE, 2), 250},
};


void play_parallax(const ay38910a_t * ay, const timer_t * timer) {
	ay38910_init(ay, timer);
	ay38910_channel_mode(ay, CHAN_ENABLE(CHA_TONE));
	ay38910_set_amplitude(ay, CHANNEL_A, MAX_AMPL | AMPL_ENVELOPE_DISABLE);

	while(1) {
		ay38910_set_amplitude(ay, CHANNEL_A, MAX_AMPL | AMPL_ENVELOPE_DISABLE);
		for(uint8_t i = 0; i < SIZE(parallax); i++) {
			ay38910_play_note(ay, CHANNEL_A, parallax[i].note);
			delay_ms(parallax[i].duration);
		}

		ay38910_set_amplitude(ay, CHANNEL_A, MAX_AMPL | AMPL_ENVELOPE_ENABLE);
		ay38910_set_envelope(ay, FUNC_ATTACK | FUNC_CONTINUE, 1000);
		for(uint8_t j = 0; j < SIZE(verse); j++) {
			ay38910_play_note(ay, CHANNEL_A, verse[j].note);
			delay_ms(verse[j].duration);
		}
	}
}
