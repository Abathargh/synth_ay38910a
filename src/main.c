#include <ay38910a.h>
#include <delay.h>
#include <avr/io.h>

#define SIZE(x) ((uint8_t)(sizeof(x)/sizeof(x[0])))

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
#if defined(__AVR_ATmega2560__)
	.bus_port = &(port_t) {&DDRA, &PORTA, &PINA},
	.ctl_port = &(port_t) {&DDRH, &PORTH, &PINH},
	.bc1      = 4,
	.bdir     = 5
#elif defined(__AVR_ATmega644__)
	.bus_port = &port_a,
	.ctl_port = &port_c,
	.bc1      = 7,
	.bdir     = 6
#endif
};

#ifdef USE_PARALLAX
extern void play_parallax(const ay38910a_t * ay, const timer_t * timer);

int main(void) {
	play_parallax(ay, timer2);
}

#else

#define UNMAPPED_CHAN ((channel_t)-1)


typedef struct {
	pin_t     pin;
	channel_t chan;
} key_t;


#if defined(__AVR_ATmega2560__)
static port_t key_port1 = {&DDRK, &PORTK, &PINK};
static port_t key_port2 = {&DDRF, &PORTF, &PINF};
#endif

static key_t keys[] = {
#if defined(__AVR_ATmega2560__)
	{{.port=&key_port1, .pin=0}, .chan=UNMAPPED_CHAN},
	{{.port=&key_port1, .pin=1}, .chan=UNMAPPED_CHAN},
	{{.port=&key_port1, .pin=2}, .chan=UNMAPPED_CHAN},
	{{.port=&key_port1, .pin=3}, .chan=UNMAPPED_CHAN},
	{{.port=&key_port1, .pin=4}, .chan=UNMAPPED_CHAN},
	{{.port=&key_port1, .pin=5}, .chan=UNMAPPED_CHAN},
	{{.port=&key_port1, .pin=6}, .chan=UNMAPPED_CHAN},
	{{.port=&key_port1, .pin=7}, .chan=UNMAPPED_CHAN},
	{{.port=&key_port2, .pin=1}, .chan=UNMAPPED_CHAN},
	{{.port=&key_port2, .pin=2}, .chan=UNMAPPED_CHAN},
	{{.port=&key_port2, .pin=3}, .chan=UNMAPPED_CHAN},
	{{.port=&key_port2, .pin=4}, .chan=UNMAPPED_CHAN},
#endif
};


#define DEBOUNCE_RES (8)

static uint8_t ampl   = MAX_AMPL & AMPL_ENV_DISABLE;
static uint8_t octave = 4;

uint8_t read_debounced(pin_t p) {
	uint8_t acc = 0;
	uint8_t ctr;
	do {
		ctr = 0;
		while (ctr++ < DEBOUNCE_RES) {
			acc = (acc << 1) | (read_pin(p.port, p.pin));
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
			ay38910_set_amplitude(ay, i << 1, ampl);
			ay38910_play_note(ay, i << 1, NOTE(note, octave));
			key->chan = i;
			return;
		}
	}
}

void close_channel(key_t * key, uint8_t * state) {
	*state |= CHAN_DISABLE(key->chan);
	ay38910_set_amplitude(ay, 1 << key->chan, 0);
	ay38910_channel_mode(ay, *state);
	key->chan = UNMAPPED_CHAN;
}

uint8_t env_shapes[] = {
	0,
	REVERSE_SAWTOOTH,
	TRIANGULAR_OOP,
	UP_DOWN_CUP,
	SAWTOOTH,
	DOWN_CUP,
	TRIANGULAR,
};


int main(void) {
	ay38910_init(ay, timer2);
	as_input_pull_up_port(&key_port1);
	as_input_pull_up_port(&key_port2);

	port_t * settings_port = &(port_t) {&DDRF, &PORTF, &PINF};
	pin_t    settings_pin  = {settings_port, 7};
	pin_t    settings_led  = {settings_port, 6};
	as_input_pull_up_pin(settings_port, 7);
	as_output_pin(settings_port, 6);

	uint8_t setting   = 0x00;
	uint8_t last_read = 0xff;
	uint8_t curr_read;

	uint8_t state   = 0xff;

	for(;;) {
		curr_read = read_debounced(settings_pin);
		if(curr_read == 0 && last_read != 0) {
			setting = (setting + 1) % (SIZE(env_shapes));
			last_read = curr_read;
			if(setting == 0) {
				ampl &= AMPL_ENV_DISABLE;
				set_pin(settings_led.port, settings_led.pin);
			} else {
				ampl |= AMPL_ENV_ENABLE;
				ay38910_set_envelope(ay, env_shapes[setting], 1000);
				clear_pin(settings_led.port, settings_led.pin);
			}
		} else if(curr_read != 0 && last_read == 0) {
			last_read = 0xff;
		}

		for(int i = 0; i < SIZE(keys); i++) {
			key_t * key = &keys[i];
			uint8_t pressed = read_debounced(keys[i].pin) == 0x00;
			if(pressed && key->chan == UNMAPPED_CHAN) {
				play_note(key, i, &state);
			} else if(!pressed && key->chan != UNMAPPED_CHAN) {
				close_channel(key, &state);
			}
		}
	}
}

#endif
