#include <ay38910a.h>
#include <delay.h>
#include <avr/io.h>

#define SIZE(x) ((uint8_t)(sizeof(x)/sizeof(x[0])))

static port_t port_a = {.direction=&DDRA,	.input=&PINA, .output=&PORTA};
static port_t port_b = {.direction=&DDRB,	.input=&PINB, .output=&PORTB};
static port_t port_c = {.direction=&DDRC,	.input=&PINC, .output=&PORTC};

static const timer_t * timer2 = &(timer_t) {
	.tccr_a     = &TCCR2A,
	.tccr_b     = &TCCR2B,
	.tim_sk     = &TIMSK2,
	.ocr_a_8    = &OCR2A,
#if defined(__AVR_ATmega2560__)
	.ocr_a_port = &(port_t) {&DDRB, &PORTB, &PINB},
	.ocr_a_pin  = 4
#elif defined(__AVR_ATmega644__)
	.ocr_a_port = &(port_t) {&DDRD, &PORTD, &PIND},
	.ocr_a_pin  = 7
#endif
};

static const ay38910a_t * ay = &(ay38910a_t) {
	.bus_port = &port_a,
	.ctl_port = &port_c,
	.bc1      = 7,
	.bdir     = 6
};

#ifdef USE_PARALLAX
extern void play_parallax(const ay38910a_t * ay, const timer_t * timer);

int main(void) {
	play_parallax(ay, timer2);
}

#else

#define UNMAPPED_CHAN ((channel_t)-1)



typedef struct {
	port_t *  port;
	uint8_t   pin;
	channel_t chan;
} key_t;

static key_t keys[] = {
	{.port=&port_b, .pin=0, .chan=UNMAPPED_CHAN},
	{.port=&port_b, .pin=1, .chan=UNMAPPED_CHAN},
	{.port=&port_b, .pin=2, .chan=UNMAPPED_CHAN},
	{.port=&port_b, .pin=3, .chan=UNMAPPED_CHAN},
	{.port=&port_b, .pin=4, .chan=UNMAPPED_CHAN},
	{.port=&port_b, .pin=5, .chan=UNMAPPED_CHAN},
	{.port=&port_b, .pin=6, .chan=UNMAPPED_CHAN},
	{.port=&port_b, .pin=7, .chan=UNMAPPED_CHAN},
	{.port=&port_b, .pin=0, .chan=UNMAPPED_CHAN},
	{.port=&port_c, .pin=1, .chan=UNMAPPED_CHAN},
	{.port=&port_c, .pin=2, .chan=UNMAPPED_CHAN},
	{.port=&port_c, .pin=3, .chan=UNMAPPED_CHAN},
};


#define DEBOUNCE_RES (8)

uint8_t read_debounced(key_t k) {
	uint8_t acc = 0;
	uint8_t ctr;
	do {
		ctr = 0;
		while (ctr++ < DEBOUNCE_RES) {
			acc = (acc << 1) | (read_pin(k.port, k.pin));
			delay_ms(1);
		}
	} while(acc != 0x00 && acc != 0xff);
	return acc;
}


void play_note(key_t * key, uint8_t note, uint8_t * state) {
	for(uint8_t i = 0; i < CHANNEL_NUM; i++) {
		if((*state) & (1 << i)) {
			*state &= CHAN_ENABLE(i);
			ay38910_channel_mode(ay, *state);
			ay38910_set_amplitude(ay, i << 1, MAX_AMPL);
			ay38910_play_note(ay, i << 1, NOTE(note, 4));
			key->chan = i;
			return;
		}
	}
}

void close_channel(key_t * key, uint8_t * state) {
	*state |= CHAN_DISABLE(key->chan);
	ay38910_channel_mode(ay, *state);
	key->chan = UNMAPPED_CHAN;
}


int main(void) {
	ay38910_init(ay, timer2);
	for(uint8_t i = 0; i < SIZE(keys); i++) {
		as_input_pull_up_pin(keys[i].port, keys[i].pin);
	}

	uint8_t state = 0xff;

	for(;;) {
		for(int i = 0; i < SIZE(keys); i++) {
			key_t * key = &keys[i];
			uint8_t pressed = read_debounced(keys[i]) == 0x00;
			if(pressed && key->chan == UNMAPPED_CHAN) {
				play_note(key, i, &state);
			} else if(!pressed && key->chan != UNMAPPED_CHAN) {
				close_channel(key, &state);
			}
		}
	}
}

#endif
