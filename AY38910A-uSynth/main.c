/*
 * EmbeddedProject-AY38910A.c
 *
 * Author : mar
 */ 

#define F_CPU 16000000

#include <avr/io.h>
#include <util/delay.h>

#include "pin_config.h"

int main(void)
{
	InitPinOuput(LED_PIN);
    while (1) 
    {
		TogglePin(LED_PIN);
		_delay_ms(100);
    }
}

