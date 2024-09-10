#include <lcd_1602a.h>
#include <ay38910a.h>
#include <settings.h>
#include <avr/io.h>
#include <delay.h>

#define SIZE(x) ((uint8_t)(sizeof(x)/sizeof(x[0])))

#if defined(__AVR_ATmega2560__)
static port_t key_port1 = IO_PORT_K;
static port_t key_port2 = IO_PORT_B;
static port_t lcd_port  = IO_PORT_C;
#endif

static const timer_t * timer2 = &(timer_t) {
	.tccr_a     = &TCCR2A,
	.tccr_b     = &TCCR2B,
	.tim_sk     = &TIMSK2,
	.ocr_a_8    = &OCR2A,
#if defined(__AVR_ATmega2560__)
	.ocr_a_port = &(port_t) IO_PORT_B,
	.ocr_a_pin  = 4
#elif defined(__AVR_ATmega644__)
	.ocr_a_port = &(port_t) IO_PORT_D,
	.ocr_a_pin  = 7
#endif
};

static const timer_t * timer5 = &(timer_t) {
	.tccr_a     = &TCCR5A,
	.tccr_b     = &TCCR5B,
	.tim_sk     = &TIMSK5,
	.ocr_a_16   = &OCR5A,
	.ocr_a_port = &(port_t)IO_PORT_L,
	.ocr_a_pin = 3,
};

static const ay38910a_t * ay = &(ay38910a_t) {
#if defined(__AVR_ATmega2560__)
	.bus_port = &(port_t) IO_PORT_A,
	.ctl_port = &(port_t) IO_PORT_H,
	.bc1      = 4,
	.bdir     = 5
#elif defined(__AVR_ATmega644__)
	.bus_port = &port_a,
	.ctl_port = &port_c,
	.bc1      = 7,
	.bdir     = 6
#endif
};

static const lcd1602a_t * lcd = &(lcd1602a_t) {
	.ctl_port     = &lcd_port,
	.bus_port     = &lcd_port,
	.register_sel = 0,
	.enable       = 1
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
	{{.port=&key_port2, .pin=0}, .chan=UNMAPPED_CHAN},
	{{.port=&key_port2, .pin=1}, .chan=UNMAPPED_CHAN},
	{{.port=&key_port2, .pin=2}, .chan=UNMAPPED_CHAN},
	{{.port=&key_port2, .pin=3}, .chan=UNMAPPED_CHAN},
#endif
};


#define DEBOUNCE_RES (8)

static settings_t * settings = &(settings_t){
	.amplitude = AMP_DEF,
	.octave    = OCT_DEF,
	.env_shape = SHP_DEF,
};

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
			ay38910_set_amplitude(ay, i << 1, settings->amplitude);
			ay38910_play_note(ay, i << 1, NOTE(note, settings->octave));
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
	lcd1602a_init(lcd, timer5);

	ay38910_init(ay, timer2);
	stg_init();

	for(int i = 0; i < SIZE(keys); i++) {
		as_input_pull_up_pin(keys[i].pin.port, keys[i].pin.pin);
	}

	uint8_t state   = 0xff;

	for(;;) {
		if(stg_received_data()) {
			stg_update(settings);
			stg_send_frame(settings);
			if(settings->env_shape == 0) {
				settings->amplitude &= AMPL_ENV_DISABLE;
			} else {
				settings->amplitude |= AMPL_ENV_ENABLE;
				ay38910_set_envelope(ay, env_shapes[settings->env_shape], 1000);
			}
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
