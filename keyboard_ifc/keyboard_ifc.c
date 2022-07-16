#include "keyboard_ifc.h"

#include "delay.h"
#include "pinout.h"
#include "pin_config.h"

#include <avr/interrupt.h>

#define ROW_NUM 4
#define COL_NUM 3

#define ROW(n) keyboard_out,(2+n)


void keyboard_init(void)
{
	InitPort(keyboard_out, 0x0F);

	// Set the keyboard cols as pullup inputs
	InitPort(keyboard_in, 0x07);
	SetPort(keyboard_in, 0x03); 

	SetPort(keyboard_out, 0x0F);
}

bool keyboard_acquire(uint16_t *mask)
{
	uint8_t cur_mask = 0;
	uint8_t columns = GetPort(keyboard_in) & 0x03;
	
	while(columns == 0x03)
	{
		delay_ms(2);
		columns = GetPort(keyboard_in) & 0x03;
	}

	for(int i = 0; i < ROW_NUM; i++)
	{
		// ground one by one to check for key presses
		ResetPin(ROW(i));
		delay_ms(2);
		columns = GetPort(keyboard_in) & 0x03;
		
		for(int j = 0; j < COL_NUM; j++)
		{
			if(!(columns & (1 << j)))
			{
				cur_mask |= 1 << ((ROW_NUM-1)*i + j);
			}
			else
			{
				cur_mask &= ~(1 << ((ROW_NUM-1)*i + j));
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
