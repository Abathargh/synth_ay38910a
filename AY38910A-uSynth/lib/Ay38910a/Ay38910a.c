/**
 * Ay38910a.c
 *
 * This module implements the logic for that drives the AY38910A
 * Programmable sound Generator Chip. This is achieved by  
 * implementing the following features:
 *   - a 2MHz clock signal to use as the PSG input
 *   - interfacing with the AY38910A address/data bus using PORTA
 *   - interfacing with the AY38910A bus control line using PORTC[0:2]
 *
 * Author: mar
 */ 

/************************************************************************/
/* Includes                                                             */
/************************************************************************/

#include "Ay38910a.h"

#include <avr/io.h>

/************************************************************************/
/* Defines                                                              */
/************************************************************************/

/**
 * We want to generate a 2MHz square wave as the input signal for
 * the clock pin of the AY38910A. Consider the following:
 *
 * f_clk = 16 MHz => T_clk = 0.0625 us
 * 
 * and knowing that:
 *
 * f_desired 2MHz => T_desired = 0.5 us
 *
 * we know that we have to generate a signal that peaks every 
 * T_desired/2, so T_del = 0.25 us
 *
 * OCR2VAL = T_clk/T_del - 1 = (0.0625 us / 0.25 us) - 1 = 3 
 */
#define OCR2AVAL       3

#define BC1            7
#define BDIR           6

#define NOISE_REG      0x06
#define MIXER_REG      0x07
#define FINE_ENV_REG   0x0B
#define COARSE_ENV_REG 0x0C
#define SHAPE_ENV_REG  0x0D

#define CHAN_TO_AMP_REG(c) (((u8)c / 2) + 8)


/************************************************************************/
/* Private function declarations                                        */
/************************************************************************/

static void clock_init(void);
static void inactive_mode(void);
static void latch_address_mode(void);
static void write_mode(void);
static void write_to_data_bus(u8 address, u8 data);

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
 */
static const u16 magic_notes[] = {
	
};

/************************************************************************/
/* Function implementations                                             */
/************************************************************************/

/**
 * Initializes the ay38910a module
 *
 * @retval None
 */
void ay38910_init(void)
{
	DDRA = 0xFF; // 11111111
	DDRC = 0xA0; // 10100000
	
	clock_init();
}

/**
 * Plays a note on the specified channel
 *
 * @param chan the channel to program 
 * @ note the note to play depending on the internal mapping (0 off)
 */
void ay38910_play_note(Channel chan, u8 note)
{
	u8 reg1, reg2;
	
	reg1 = (u8) chan;
	reg2 = ((u8) chan) + 1;
	
	write_to_data_bus(reg1, 0xFF & magic_notes[note]);
	write_to_data_bus(reg2, 0x0F & (magic_notes[note] >> 8));
}

/**
 * Plays a sound on the noise channel
 *
 * @param divider the divider value to manipulate the noise
 *
 * @retval None
 */
void ay38910_play_noise(u8 divider)
{
	write_to_data_bus(NOISE_REG, 0x1F & divider);
}

/**
 * Enables/Disables tone/noise generation on one or more channels
 *
 * @param mode the mode(s) to enable, using the ENABLE/DISABLE defines
 *
 * @retval None
 */
void ay38910_channel_mode(u8 mode)
{
	write_to_data_bus(MIXER_REG, mode);
}

/**
 * Sets the amplitude for the specified channel
 *
 * @param chan the channel that will have the passed amplitude
 * @param amplitude the amplitude to set, the first 4 bytes
 *        refer to the amplitude itself (0-16), the fifth byte 
 *        enables the envelope filter and disables the use of the 
 *        first three bytes. Use with the enable/disable macros.
 
 * @retval None
 */
void ay38910_set_amplitude(Channel chan, u8 amplitude)
{
	u8 amplitude_reg = CHAN_TO_AMP_REG(chan);
	write_to_data_bus(amplitude_reg, 0x1F & amplitude);
}

void ay38910_set_envelope(EnvelopeShape shape, u16 frequency)
{
	write_to_data_bus(FINE_ENV_REG, 0xFF & frequency);
	write_to_data_bus(COARSE_ENV_REG, 0xFF & (frequency >> 8));
	write_to_data_bus(SHAPE_ENV_REG, 0x07 & ((u8)shape));
}

/************************************************************************/
/* Private Helpers                                                      */
/************************************************************************/

/**
 * Initializes the TIMER2A so that the OC2A pin outputs a 
 * 2MHz clock signal that can be used to drive the AY38910A
 *
 * @retval None
 */
static void clock_init(void)
{
	// Reset the counter
	TCNT2 = 0x00;
	
	// Sets the frequency to 2MHz 
	OCR2A = OCR2AVAL;
	
	TCCR2A = (0 << COM2A1) | (1 << COM2A0) | // Enable output signal on OC2A pin 
			 (1 << WGM21)  | (1 << WGM20);   // Enable Clear Timer on Compare Mode
	
	TCCR2B = (0 << WGM22) |                            // MSB output enable 
			 (0 << CS22)  | (0 << CS21) | (1 << CS20); // Clock select with no prescaler
	
	// Disable the compare match interrupt for register A
	TIMSK2 = 0;
}

/**
 * Set the PSG to inactive mode
 *
 * @retval None
 */
static inline void inactive_mode(void)
{
	PORTC = (0 << BC1) | (0 << BDIR); 
}

/**
 * Set the PSG to write mode
 *
 * @retval None
 */
static inline void write_mode(void)
{
	PORTC = (0 << BC1) | (1 << BDIR);
}

/**
 * Set the PSG to latch address mode
 *
 * @retval None
 */
static inline void latch_address_mode(void)
{
	PORTC = (1 << BC1) | (1 << BDIR);
}

static void write_to_data_bus(u8 address, u8 data)
{
	// Set the register address
	inactive_mode();
	PORTA = address;
	latch_address_mode();
	inactive_mode();
	
	//Write to the previously set register
	write_mode();
	PORTA = data;
	inactive_mode();	
}
