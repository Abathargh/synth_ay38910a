#include "keyboard_ifc.h"
#include "1602a_lcd.h"
#include "pin_config.h"
#include "pinout.h"

#include <avr/interrupt.h>

#define DEBOUNCE_MAX 8
#define ROW(n) keyboard,(2+n)


void keyboard_init(void)
{
	InitPort(keyboard, 0b00001100);
	SetPort(keyboard,  0b00001111);
}

bool keyboard_acquire(uint8_t *mask)
{
	uint8_t columns;

	/**
		* x x x x o o i i input pullups
		* 0 0 0 0 1 1 0 0
		*/

	uint8_t cur_mask = 0;

	columns = GetPort(keyboard) & 0x03;
	while(columns == 0x03)
	{
		delay_ms(2);
		columns = GetPort(keyboard);
	}

	for(int i = 0; i < 2; i++)
	{
		// ground one by one to check for key presses
		ResetPin(ROW(i));
		delay_ms(2);
		columns = GetPort(keyboard) & 0x03;
		
		for(int j = 0; j < 2; j++)
		{
			if(!(columns & (1 << j)))
			{
				cur_mask |= 1 << (2*i + j);
			}
			else
			{
				cur_mask &= ~(1 << (2*i + j));
			}
		}
		SetPin(ROW(i));
	}

	if(cur_mask != *mask)
	{
		// New key press
		*mask = cur_mask;
		return true;
	}
	return false;
}
