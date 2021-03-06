#ifndef AY38910A_SYNTH_KEYBOARD_IFC_H
#define AY38910A_SYNTH_KEYBOARD_IFC_H

#include <stdint.h>
#include <stdbool.h>

#define KEYBOARD_KEYS 12

void keyboard_init(void);
bool keyboard_acquire(uint16_t *mask);

void init_adc(void);
uint8_t get_potentiometer(void);

#endif //AY38910A_SYNTH_KEYBOARD_IFC_H
