#include <lcd_1602a.h>
#include <ay38910a.h>
#include <settings.h>
#include <avr/interrupt.h>
#include <stdio.h>


#define SIZE(x) ((uint8_t)(sizeof(x)/sizeof(x[0])))

#if defined(__AVR_ATmega2560__)
static port_t key_port1     = IO_PORT_K;
static port_t key_port2     = IO_PORT_B;
static port_t lcd_bus_port  = IO_PORT_C;
static port_t lcd_ctl_port  = IO_PORT_C;
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
	.ctl_port     = &lcd_ctl_port,
	.bus_port     = &lcd_bus_port,
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


static settings_t * settings = &(settings_t){
	.nav_pin={.port=&(port_t)IO_PORT_L, .pin=0},
	.sel_pin={.port=&(port_t)IO_PORT_L, .pin=1},
	.amplitude = AMP_DEF,
	.octave    = OCT_DEF,
	.env_shape = SHP_DEF,
};


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


struct shape_meta {
	uint8_t value;
	const char * figure;
} env_shapes[] = {
	{0,                "________________"},
	{REVERSE_SAWTOOTH, "\x7|\x7|\x7|\x7|\x7|\x7|\x7|\x7|"},
	{TRIANGULAR_OOP,   "\x7/\x7/\x7/\x7/\x7/\x7/\x7/\x7/"},
	{UP_DOWN_CUP,      "\x7/\x6\x6\x6\x6\x6\x6\x6\x6\x6\x6\x6\x6\x6\x6\x6\x6\x6"
	                   "\x6\x6\x6\x6"},
	{SAWTOOTH,         "/|/|/|/|/|/|/|/|"},
	{DOWN_CUP,         "/\x6\x6\x6\x6\x6\x6\x6\x6\x6\x6\x6\x6\x6\x6\x6"},
	{TRIANGULAR,       "/\x7/\x7/\x7/\x7/\x7/\x7/\x7/\x7"},
};

const char b_slash[] = {0, 0x10, 0x8, 0x4, 0x2, 0x1, 0, 0};
const char overline[] = {0x1f, 0, 0, 0, 0, 0, 0, 0};

void print_settings(void) {
	static char buf[20] = {0};
	snprintf(buf, 20, "amp: %d oct: %d", settings->amplitude, settings->octave);
	lcd1602a_print_row(lcd, buf, 0);
}


void print_shape_figure(const struct shape_meta shape) {
	static char buf[20] = {0};
	snprintf(buf, 20, "%s", shape.figure);
	lcd1602a_print_row(lcd, buf, 1);
}

int main(void) {
	lcd1602a_init(lcd, timer5);
	lcd1602a_display_on(lcd);
	lcd1602a_clear(lcd);
	lcd1602a_home(lcd);

	lcd1602a_new_char(lcd, 6, overline);
	lcd1602a_new_char(lcd, 7, b_slash);

	ay38910_init(ay, timer2);
	stg_init();

	for(int i = 0; i < SIZE(keys); i++) {
		as_input_pull_up_pin(keys[i].pin.port, keys[i].pin.pin);
	}

	uint8_t state   = 0xff;

	print_settings();
	print_shape_figure(env_shapes[0]);

	for(;;) {
		if(stg_received_data()) {
			stg_update_from_frame(settings);
			stg_send_frame(settings);
			print_settings();
			if(settings->env_shape == 0) {
				settings->amplitude &= AMPL_ENV_DISABLE;
				print_shape_figure(env_shapes[0]);
			} else {
				struct shape_meta shape = env_shapes[settings->env_shape];
				settings->amplitude |= AMPL_ENV_ENABLE;
				ay38910_set_envelope(ay, shape.value, 1000);
				print_shape_figure(shape);
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
