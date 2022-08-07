#ifndef AY38910A_SYNTH_KEYBOARD_IFC_H
#define AY38910A_SYNTH_KEYBOARD_IFC_H

#include <stdint.h>
#include <stdbool.h>

#define KEYBOARD_KEYS 12

void keyboard_init(void);
bool keyboard_acquire(uint16_t *mask);
bool keyboard_get_browse(void);
bool keyboard_get_confirm(void);


void keyboard_init_adc(void);
void keyboard_enable_potentiometer(void);
void keyboard_disable_potentiometer(void);
uint8_t keyboard_get_potentiometer(void);

#endif
