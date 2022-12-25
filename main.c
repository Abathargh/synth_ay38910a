#include "lcd_1602a.h"
#include "delay.h"
#include "ay38910a.h"
#include "keyboard_ifc.h"

#include <stdint.h>
#include <string.h>
/* WRITE_BUF for easy printf-like debug through the lcd */
#include <stdio.h>

#include <avr/interrupt.h>


char buf[32];
#define WRITE_BUF(s, ...) snprintf(buf, 32, s, __VA_ARGS__)

enum menu_selection {
	OCTAVE_MENU,
	ENVELOPE_HOLD_MENU,
	ENVELOPE_ALTERNATE_MENU,
	ENVELOPE_ATTACK_MENU,
	ENVELOPE_CONTINUE_MENU,
	CHAN_A_TONE_MENU,
	CHAN_B_TONE_MENU,
	CHAN_C_TONE_MENU,
	RESET_MENU,
	EXIT_MENU,
	SIZE_MENU
};

static const uint8_t sample_levels[] = {
	[OCTAVE_MENU] = 8,
	[ENVELOPE_HOLD_MENU] = 1,
	[ENVELOPE_ALTERNATE_MENU] = 1,
	[ENVELOPE_ATTACK_MENU] = 1,
	[ENVELOPE_CONTINUE_MENU] = 1,
	[CHAN_A_TONE_MENU] = 2,
	[CHAN_B_TONE_MENU] = 2,
	[CHAN_C_TONE_MENU] = 2,
};

// TODO move to module, num cols, rows in .h to expose them
// TODO frequency maybe 0 disable env non zeroe enable with that freq? but 16 bit..
static const char messages[][16] = {
		[OCTAVE_MENU] = "Octave:",
		[ENVELOPE_HOLD_MENU] = "Hold envelope:",
		[ENVELOPE_ALTERNATE_MENU] = "Alt. envelope:",
		[ENVELOPE_ATTACK_MENU] = "Attack envelope:",
		[ENVELOPE_CONTINUE_MENU] = "Cont. envelope:",
		[CHAN_A_TONE_MENU] = "Channel A tone:",
		[CHAN_B_TONE_MENU] = "Channel B tone:",
		[CHAN_C_TONE_MENU] = "Channel C tone:",
		[RESET_MENU] = "Reset defaults",
		[EXIT_MENU] = "Exit",
};

static const char* tone_messages[] = 
{
	"tone",
	"noise"
};

struct keyboard_config
{
	uint8_t octave;
	uint8_t envelope_level[3];
	uint8_t tone_config[CHANNEL_NUM];
};


void handle_menu(bool *play_state, struct keyboard_config *conf);

int main(void)
{
	ay38910_init();
	lcd1602a_init();
	keyboard_init();
	keyboard_init_adc();

	lcd1602a_display_on();
	lcd1602a_clear();
	lcd1602a_home();


	bool     play_state   = true;  // by default we are in play mode

	// TODO maybe implement this to avoid double clicks?
	// bool     last_browse  = false;
	// bool     last_confirm = false;

	
	uint8_t  channel_mask = 0;
	uint16_t mask         = 0;

	struct keyboard_config config = {0};
	config.octave = 5;

	sei();

	while(1)
	{
		if(play_state && keyboard_get_browse())
		{
			play_state = false;
			keyboard_enable_potentiometer();
		}
		
		if(!play_state)
		{
			handle_menu(&play_state, &config);
			continue;
		}

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
					ay38910_play_note(chan, NOTE(1+i, config.octave));
					count++;
				}
			}
			
			// TODO this for is called anyways once the mask changes
			// even in a case where one channel is already closed
			// probably best to leave it instead of checking performance-wise
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


void uint8_to_str(uint8_t num, char *str, uint8_t len)
{
	// TODO benchmark with loopless version
	// TODO this adds zeros at the beginning
	// assert(str != NULL && len == 4); 
	str[len-1] = '\0';
	for(int i = len-2; i >= 0; i--, num /= 10)
	{
		str[i] = (num % 10) + '0';
	}
}

void handle_menu(bool *play_state, struct keyboard_config *conf)
{
	static char row1_buf[4]; // max num colons
	static enum menu_selection menu_state = OCTAVE_MENU;

	/*
		TODO
		prob better to actuate these two once edge changes to falling down
		otherwise executed n times wihle being pressed
	*/
	if(keyboard_get_confirm())
	{
		if(menu_state == RESET_MENU)
		{
			memset(conf, 0, sizeof(struct keyboard_config));
		}
		keyboard_disable_potentiometer();
		*play_state = true;
		return;
	}

	if(keyboard_get_browse())
	{
		menu_state = (menu_state + 1) % SIZE_MENU;
	}

	// acquire adc
	const uint8_t pot_levels = sample_levels[menu_state];
	uint8_t pot_value = keyboard_get_potentiometer() / pot_levels;
	
	// print menu message depending on selection
	lcd1602a_print_row(messages[menu_state], 0);


	switch(menu_state)
	{
		case OCTAVE_MENU:
			conf->octave = pot_value;
			uint8_to_str(pot_value, row1_buf, sizeof(row1_buf));
			lcd1602a_print_row(row1_buf, 1);
			break;

		case ENVELOPE_HOLD_MENU:
		case ENVELOPE_ALTERNATE_MENU:
		case ENVELOPE_ATTACK_MENU:
		case ENVELOPE_CONTINUE_MENU:
			// catch all the three cases
			// TODO actuate envelope
			conf->envelope_level[menu_state-ENVELOPE_HOLD_MENU] = pot_value;
			uint8_to_str(pot_value, row1_buf, sizeof(row1_buf));
			lcd1602a_print_row(row1_buf, 1);
			break;
		
		case CHAN_A_TONE_MENU:
		case CHAN_B_TONE_MENU:
		case CHAN_C_TONE_MENU:
			// catch all the three cases
			// TODO cleanup magic nums
			conf->tone_config[menu_state-CHAN_A_TONE_MENU] = pot_levels;
			lcd1602a_print_row(tone_messages[pot_levels ? 0 : 1], 1);
			break;

		default:
			return;
	}
}
