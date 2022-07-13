#include <stdio.h>
#include "logger.h"
#include "pin_config.h"
#include "1602a_lcd.h"

static char buf[32];
#define WRITE_BUF(s, ...) snprintf(buf, 32, s, __VA_ARGS__)

#define pin_test A,0

void keyboard_acquire(void);

int main(void)
{
	while(1)
  {
    keyboard_acquire();
  }
}


#define keyboard_port A
#define row(n) A,(2+n)

void keyboard_acquire()
{
  InitPort(keyboard_port, 0b00001100);
  SetPort(keyboard_port,  0b00001111);

  unsigned char columns;

  logger_init(BAUD_RATE_9600);

  /**
   * x x x x o o i i input pullups
   * 0 0 0 0 1 1 0 0
   */
  while(1)
  {
    columns = GetPort(keyboard_port) & 0x03;
    while(columns == 0x03)
    {
      delay_ms(2);
      columns = GetPort(keyboard_port);
    }

    for(int i = 0; i < 2; i++)
    {
      // ground one by one to check for key presses
      ResetPin(row(i));
      delay_ms(2);
      columns = GetPort(keyboard_port) & 0x03;
      
      for(int j = 0; j < 2; j++)
      {
        if(!(columns & (1 << j)))
        {
          WRITE_BUF("%d %d\n", i, j);
          logger_print(buf);
        }
      }
      SetPin(row(i));
    }
  }
}

/*
bool pressed = false;
	bool last = is_pressed();
	while(1)
	{
		pressed = is_pressed();
		if(pressed != last)
		{
			if(pressed)
			{
				ay38910_set_amplitude(CHANNEL_A, 15);
				ay38910_play_note(CHANNEL_A, note);
				WRITE_BUF("Note: %d", note);
				lcd1602a_print_row(buf, 0);
				note++;
			}
			else
			{
				ay38910_set_amplitude(CHANNEL_A, 0);
			}

			WRITE_BUF("Button: %d", pressed);
			lcd1602a_print_row(buf, 1);
		}
		last = pressed;
	}
*/

