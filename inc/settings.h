#ifndef SETTINGS_H
#define SETTINGS_H

#include <lcd_1602a.h>
#include <stdint.h>

#define AMP_DEF (7)
#define OCT_DEF (4)
#define SHP_DEF (0)

#define STG_ENVELOPE_OFF (0)
#define STG_ENVELOPE_NUM (7)

#define AMPLITUDE_CARD (15)
#define OCTAVE_CARD    (8)
#define WAVEFORM_CARD  (6)

enum menu_state {
	MENU_AMPLITUDE,
	MENU_OCTAVE,
	MENU_WAVEFORM,
};

typedef struct settings_ctl {
	pin_t   nav_pin;
	pin_t   sel_pin;
} settings_ctl_t;


typedef struct settings {
	uint8_t amplitude;
	uint8_t octave;
	uint8_t env_shape;
} settings_t;

void    stg_init(settings_ctl_t * sctl);
void    stg_update_from_frame(settings_t * s);
void    stg_send_frame(const settings_t * s);
uint8_t stg_received_data(void);
uint8_t stg_shape_val(uint8_t i);

/**
 * @brief Initializes the adc for the keyboard potentiometer
 */
void keyboard_init_adc(void);

/**
 * @brief Enables the adc peripheral to acquire data from the
 * potentiometer
 */
void stg_enable_potentiometer(void);

/**
 * @brief Disables the adc peripheral to acquire data from the
 * potentiometer
 */
void stg_disable_potentiometer(void);

/**
 * @brief Gets the latest potentiometer adc sample
 *
 * @return the sample as an 8-bit value
 */
uint8_t stg_read_potentiometer(void);

bool stg_menu_loop(const lcd1602a_t * lcd, settings_ctl_t * ctl, settings_t * stg);
void stg_print_settings(const lcd1602a_t * lcd, const settings_t * stg);
void stg_print_shape(const lcd1602a_t * lcd, const settings_t * stg);
uint8_t stg_get_shape_value(const settings_t * stg);

#endif
