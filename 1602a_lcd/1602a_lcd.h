#ifndef AY38910A_SYNTH_1602A_LCD_H
#define AY38910A_SYNTH_1602A_LCD_H

#include <stdio.h>

#define CLEAR_DISPLAY     0x01
#define RETURN_HOME       0x02
#define ENTRY_SET_DEF_LAT 0x06
#define DISPLAY_OFF       0x08
#define DISPLAY_ON        0x0C
#define FUNCTION_SET_4B1L 0x20
#define FUNCTION_SET_4B2L 0x28
#define FUNCTION_SET_8B1L 0x30


void lcd1602a_init(void);
void lcd1602a_display_on(void);
void lcd1602a_display_off(void);
void lcd1602a_home(void);
void lcd1602a_clear(void);
void lcd1602a_putchar(unsigned char c);
void lcd1602a_print(const char *str, size_t len);
void delay_us(uint16_t us);
void delay_ms(uint16_t ms);
#endif
