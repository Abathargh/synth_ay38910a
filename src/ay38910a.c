/************************************************************************/
/* Includes                                                             */
/************************************************************************/

#include "ay38910a.h"
#include "pin_config.h"
#include "delay.h"

#include <assert.h>

/************************************************************************/
/* Defines                                                              */
/************************************************************************/

#define INLINED __attribute__((always_inline)) inline

#define NOISE_REG      0x06
#define MIXER_REG      0x07
#define FINE_ENV_REG   0x0B
#define COARSE_ENV_REG 0x0C
#define SHAPE_ENV_REG  0x0D

#define MIXER_MASK     0xC0

#define CHAN_TO_AMP_REG(c) (((uint8_t)c / 2) + 8)

/**
 * Setting up the clock signal
 * -----------------------------------------------------------------
 * We want to generate a 2MHz square wave as the input signal for
 * the clock pin of the AY38910A. Consider the following:
 *   f_clk = 16 MHz
 *   T_clk = 0.0625 us
 *
 * and knowing that:
 *   f_desired = 2MHz
 *   T_desired = 0.5 us
 *
 * we know that we have to generate a signal that peaks every:
 *   T_desired/2 = T_del = 0.25 us
 *   OCRxx = T_del/T_clk - 1 = (0.25 us / 0.0625 us) - 1 = 3
 *
 * In general, if F_CPU is defined and known in advance, the value
 * for the output compare register can be obtained with the following
 * formula:
 *   OCRxx = (F_CPU/(2*F_TIM)) - 1
 */
#if defined(F_CPU)
#define F_TIM        2000000
#define AY_CLK_OCR   ((F_CPU/(2*F_TIM)) - 1)
#else
#define AY_CLK_OCR   3 /* Fallback: 16MHz if F_CPU is not defined */
#endif

/************************************************************************/
/* Private function declarations                                        */
/************************************************************************/

static void inactive_mode(const ay38910a_t * ay);
static void latch_address_mode(const ay38910a_t * ay);
static void write_mode(const ay38910a_t * ay);
static void write_to_data_bus(const ay38910a_t * ay, uint8_t address, uint8_t data);
static void oc2a_pin_config(const timer_t * t);

/************************************************************************/
/* Private variables                                                    */
/************************************************************************/

/**
 * The AY38910A tone generation works in the following way: the clock
 * signal gets scaled by a factor of 16 and by an additional factor
 * depending on the value contained in the 12 less significant bits of
 * the two registers related to the channel that is being used.
 *
 * f_clk  = 2MHz
 * f_high = f / 16 = 125 KHz          => Highest note programmable
 * f_low  = (f / 16) / 2^12 ~ 30.5 Hz => Lowest note programmable
 *
 * This array contains some pre-computed magic numbers that correspond
 * to real notes following the mathematical equation just introduced.
 * The frequencies used in the calculations refer to the notes in the
 * equal temperament tuning system, with A4 = 440 Hz.
 *
 * The range of notes is from a B0 to a B8 (8 octaves).
 *
 * Based on these frequencies https://pages.mtu.edu/~suits/notefreqs.html.
 */
static const unsigned int magic_notes[] = {
	4049,
	3823, 3607, 3405, 3214, 3034, 2864, 2703, 2551, 2408, 2273, 2145, 2025,
	1911, 1804, 1703, 1607, 1517, 1432, 1351, 1276, 1204, 1136, 1073, 1012,
	956, 902, 851, 804, 758, 716, 676, 638, 602, 568, 536, 506,
	478, 451, 426, 402, 379, 358, 338, 319, 301, 284, 268, 253,
	239, 225, 213, 201, 190, 179, 169, 159, 150, 142, 134, 127,
	119, 113, 106, 100, 95, 89, 84, 80, 75, 71, 67, 63,
	60, 56, 53, 50, 47, 45, 42, 40, 38, 36, 34, 32, 30,
	28, 27, 25, 24, 22, 21, 20, 19, 18, 17, 16,
};

/************************************************************************/
/* Function implementations                                             */
/************************************************************************/

void ay38910_init(const ay38910a_t * ay, const timer_t * t)
{
	as_output_pin(ay->ctl_port, ay->bc1);
	as_output_pin(ay->ctl_port, ay->bdir);
	as_output_port(ay->bus_port);
	oc2a_pin_config(t);
}

void ay38910_play_note(const ay38910a_t * ay, channel_t chan, uint8_t note)
{
	assert(note <= N_NOTES);
	write_to_data_bus(ay, (uint8_t)chan, magic_notes[note] & 0xFF);
	write_to_data_bus(ay, (uint8_t)chan + 1, (magic_notes[note] >> 8) & 0x0F);
}

void ay38910_play_noise(const ay38910a_t * ay, uint8_t divider)
{
	write_to_data_bus(ay, NOISE_REG, 0x1F & divider);
}

void ay38910_channel_mode(const ay38910a_t * ay,uint8_t mode)
{
	write_to_data_bus(ay, MIXER_REG, MIXER_MASK | mode);
}

void ay38910_set_amplitude(const ay38910a_t * ay, channel_t chan, uint8_t amplitude)
{
	write_to_data_bus(ay, CHAN_TO_AMP_REG(chan), amplitude & 0x1F);
}

void ay38910_set_envelope(const ay38910a_t * ay, envelope_func_t shape, uint16_t frequency)
{
	write_to_data_bus(ay, FINE_ENV_REG, frequency & 0xFF);
	write_to_data_bus(ay, COARSE_ENV_REG, (frequency >> 8) & 0xFF);
	write_to_data_bus(ay, SHAPE_ENV_REG, ((uint8_t)shape) & 0x0F);
}

/************************************************************************/
/* Private Helpers                                                      */
/************************************************************************/

/**
 * Set the PSG to inactive mode
 */
static INLINED
void inactive_mode(const ay38910a_t * ay)
{
	clear_pin(ay->ctl_port, ay->bc1);
	delay_us(1);
	clear_pin(ay->ctl_port, ay->bdir);
	delay_us(1);
}

/**
 * Set the PSG to write mode
 */
static INLINED
void write_mode(const ay38910a_t * ay)
{
	clear_pin(ay->ctl_port, ay->bc1);
	delay_us(1);
	set_pin(ay->ctl_port, ay->bdir);
	delay_us(1);
}

/**
 * Set the PSG to latch address mode
 */
static INLINED
void latch_address_mode(const ay38910a_t * ay)
{
	set_pin(ay->ctl_port, ay->bc1);
	delay_us(1);
	set_pin(ay->ctl_port, ay->bdir);
	delay_us(1);
}

/**
 * Writes to the AY38910a data bus
 *
 * @param address the address of the register to use
 * @param data the payload to write
 */
static INLINED
void write_to_data_bus(const ay38910a_t  * ay, uint8_t address, uint8_t data)
{
	// Set the register address
	inactive_mode(ay);
	set_port(ay->bus_port, address);
	latch_address_mode(ay);
	inactive_mode(ay);

	// Write to the previously set register
	write_mode(ay);
	set_port(ay->bus_port, data);
	inactive_mode(ay);
}


/**
 * Initializes Timer2 in Toggle on Compare Match mode, in order
 * to output a 2MHz square wave on the OC2A Pin (PB4).
 *
 * @param ocr2a_value the value of the timer threshold
 */
static void oc2a_pin_config(const timer_t * t)
{
	as_output_pin(t->ocr_a_port, t->ocr_a_pin);
	*t->ocr_a_8  = AY_CLK_OCR; // 8-bit register: ocr_a_8 active
	*t->tccr_a = 0x42;         // Enable output signal on OC2A pin, CTC Mode
	*t->tccr_b = 0x01;         // MSB output enable, clock select with no prescaler
	*t->tim_sk = 0;            // Disable the compare match interrupt for register A
}
