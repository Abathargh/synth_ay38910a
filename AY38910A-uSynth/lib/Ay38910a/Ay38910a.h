/*
 * Ay38910a.h
 *
 * Created: 08/10/2021 19:41:05
 *  Author: Gianmarco
 */ 


#ifndef AY38910A_H_
#define AY38910A_H_

#include "types.h"

typedef struct {
	u8 data_pin;
	u8 latch_pin;
	u8 clock_pin;
	u8 pin_BC1;
	u8 pin_BCDIR;
	u8 freq_output_pin;
	u8 prescale;
	u8 ocr2aval;
	float period;
	float freq;
} ay_pin_config;

void ay_init(ay_pin_config conf)
{
		
}





#endif /* AY38910A_H_ */