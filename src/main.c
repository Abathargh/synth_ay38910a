#include <ay38910a.h>
#include <avr/io.h>
#include <delay.h>


#define SIZE(x) ((uint8_t)(sizeof(x)/sizeof(x[0])))
#define MAX_AMPL (15)

static const timer_t * timer2 = &(timer_t) {
	.tccr_a     = &TCCR2A,
	.tccr_b     = &TCCR2B,
	.tim_sk     = &TIMSK2,
	.ocr_a_8    = &OCR2A,
	.ocr_a_port = &(port_t) {&DDRB, &PORTB, &PINB},
	.ocr_a_pin  = 4
};

static const ay38910a_t * ay = &(ay38910a_t) {
	.bus_port = &(port_t) {&DDRK, &PORTK, &PINK},
	.ctl_port = &(port_t) {&DDRH, &PORTH, &PINH},
	.bc1      = 4,
	.bdir     = 5
};

struct note {
	int note;
	int duration;
};

extern const struct note parallax[42];
extern const struct note verse[48];

int main(void) {
	ay38910_init(ay, timer2);
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
