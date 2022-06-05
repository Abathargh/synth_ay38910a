#ifndef AY38910A_SYNTH_1602A_LCD_H
#define AY38910A_SYNTH_1602A_LCD_H

#include <stdint.h>

void lcd1602a_init(void);
void lcd1602a_display_on(void);
void lcd1602a_display_off(void);
void lcd1602a_home(void);
void lcd1602a_clear(void);
void lcd1602_clear_row(uint8_t row);
void lcd1602a_set_cursor(uint8_t x, uint8_t y);
void lcd1602a_putchar(unsigned char c);
void lcd1602a_print(const char *str);
void lcd1602a_print_row(const char *str, uint8_t row);
void delay_us(uint16_t us);
void delay_ms(uint16_t ms);
#endif
