#include "1602a_lcd.h"

#include "pin_config.h"
#include "pinout.h"

static void forward_data(void);
static void init_contrast(void);
static void send_command(unsigned char cmd);



void lcd1602a_init(void)
{
	InitOutPin(rs_pin);
	InitOutPin(en_pin);
	InitOutPort(lcd_bus);

	ResetPin(rs_pin);
	delay_ms(2000);

	SetPort(lcd_bus, FUNCTION_SET_8B1L);
	forward_data();
	delay_ms(16);

	SetPort(lcd_bus, FUNCTION_SET_8B1L);
	forward_data();
	delay_us(400);

	SetPort(lcd_bus, FUNCTION_SET_8B1L);
	forward_data();
	delay_us(400);


	SetPort(lcd_bus, FUNCTION_SET_4B1L);
	forward_data();
	delay_us(160);

	send_command(FUNCTION_SET_4B2L);
	send_command(DISPLAY_OFF);
	send_command(CLEAR_DISPLAY);
	send_command(ENTRY_SET_DEF_LAT);
	init_contrast();
}


void lcd1602a_display_on(void)
{
	send_command(DISPLAY_ON);
}


void lcd1602a_display_off(void)
{
	send_command(DISPLAY_OFF);
}


void lcd1602a_home(void)
{
	send_command(RETURN_HOME);
}

void lcd1602a_clear(void)
{
	send_command(CLEAR_DISPLAY);
}


void lcd1602a_putchar(unsigned char c)
{
	SetPin(rs_pin);

	SetPort(lcd_bus, c & 0xf0);
	forward_data();

	SetPort(lcd_bus, (c << 4) & 0xf0);
	forward_data();

	delay_us(2000);
}

void lcd1602a_print(const char *str, size_t len)
{
	lcd1602a_clear();
	for(size_t idx = 0; idx < len; idx++)
	{
		lcd1602a_putchar(str[idx]);
	}
}


static void send_command(unsigned char cmd)
{
	ResetPin(rs_pin);

	SetPort(lcd_bus, cmd & 0xf0);
	forward_data();

	SetPort(lcd_bus, (cmd << 4) & 0xf0);
	forward_data();

	delay_us(2000);
}


static inline __attribute__((always_inline))
void forward_data(void)
{
	SetPin(en_pin);
	delay_us(2);
	ResetPin(en_pin);
	delay_us(2);
}


static void init_contrast(void)
{
	InitOutPin(pwm_ch0);

	// TODO put magic num in define, this sets the
	// TODO PWM DT => contrast level
	// N.B. if using 2 rows, you need an higher V for the same
	// contrast level
	OCR0A = 95;

	TCCR0A = (1 << COM0A1) | (0 << COM0A0) | (1 << WGM01) | (1 << WGM00);
	TCCR0B = (0 << WGM02)  | (0 << CS02)   | (0 << CS01)  | (1 << CS00);
}

inline __attribute__((always_inline))
void delay_us(uint16_t us)
{
	asm volatile("MOV ZH,%B0\n\t"  // MOV: 1 cycle
							 "MOV ZL,%A0\n\t"  // MOV: 1 cycle
							 "%=:\n\t"
							 "NOP\n\t"
							 "NOP\n\t"
							 "NOP\n\t"
							 "NOP\n\t"
							 "NOP\n\t"
							 "NOP\n\t"
							 "NOP\n\t"
							 "NOP\n\t"
							 "NOP\n\t"
							 "NOP\n\t"
							 "NOP\n\t"
							 "NOP\n\t"
							 "SBIW Z,1\n\t"
							 "BRNE %=b\n\t"
							 :
							 : "r" (us));
}


inline __attribute__((always_inline))
void delay_ms(uint16_t ms)
{
	// TODO optimization => try to remove MOVs since R25:R24 should be used as
	// TODO first input register
	asm volatile("MOV ZH,%B0\n\t"  // MOV: 1 cycle
							 "MOV ZL,%A0\n\t"  // MOV: 1 cycle => 1 + (16016 + 4) * ms = 16020 * ms + 1
							 "OUTER%=:\n\t"    // (4000 + 4) * 4 = 16016
							 "LDI R18,4\n\t"   // LDI: 1 cycle
							 "MILLISEC%=:\n\t" // 16 * 250 = 4000 cycles
							 "LDI R17,250\n\t" // LDI: 1 cycle
							 "MICROSEC%=:\n\t" // MICROSEC LOOP: 16 cycles tot (including previous LDI per cycle)
							 "NOP\n\t"
							 "NOP\n\t"
							 "NOP\n\t"
							 "NOP\n\t"
							 "NOP\n\t"
							 "NOP\n\t"
							 "NOP\n\t"
							 "NOP\n\t"
							 "NOP\n\t"
							 "NOP\n\t"
							 "NOP\n\t"
							 "NOP\n\t"
							 "DEC R17\n\t"
							 "BRNE MICROSEC%=\n\t"
							 "DEC R18\n\t"
							 "BRNE MILLISEC%=\n\t"
							 "SBIW Z,1\n\t"        // SBIW: 2 cycles
							 "BRNE OUTER%=\n\t"
		:
		: "r" (ms));
}