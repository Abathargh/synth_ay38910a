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
#include "pin_config.h"

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

#define NOISE_REG      0x06
#define MIXER_REG      0x07
#define FINE_ENV_REG   0x0B
#define COARSE_ENV_REG 0x0C
#define SHAPE_ENV_REG  0x0D

#define MIXER_MASK     0xC0

#define CHAN_TO_AMP_REG(c) (((uint8_t)c / 2) + 8)


/************************************************************************/
/* Private function declarations                                        */
/************************************************************************/

static void inactive_mode(void);
static void latch_address_mode(void);
static void write_mode(void);
static void write_to_data_bus(uint8_t address, uint8_t data);

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
static const unsigned int magic_notes[] = {
	  15289, 14431, 13621, 12856, 12135, 11454, 10811, 10204,//0-o7
	  9631, 9091, 8581, 8099, 7645, 7215, 6810, 6428,//8-15
	  6067, 5727, 5405, 5102, 4816, 4545, 4290, 4050,//16-23
	  3822, 3608, 3405, 3214, 3034, 2863, 2703, 2551,//24-31
	  2408, 2273, 2145, 2025, 1911, 1804, 1703, 1607,//32-39
	  1517, 1432, 1351, 1276, 1204, 1136, 1073, 1012,//40-47
	  956, 902, 851, 804, 758, 716, 676, 638,//48-55
	  602, 568, 536, 506, 478, 451, 426, 402,//56-63
	  379, 358, 338, 319, 301, 284, 268, 253,//64-71
	  239, 225, 213, 201, 190, 179, 169, 159,//72-79
	  150, 142, 134, 127, 119, 113, 106, 100,//80-87
	  95, 89, 84, 80, 75, 71, 67, 63,//88-95
	  60, 56, 53, 50, 47, 45, 42, 40,//96-103
	  38, 36, 34, 32, 30, 28, 27, 25,//104-111
	  24, 22, 21, 20, 19, 18, 17, 16,//112-119
	  15, 14, 13, 13, 12, 11, 11, 10,//120-127
	  0//off
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
	InitOutPin(BC1_PIN);
	InitOutPin(BDIR_PIN);
	INIT_BUS_PORT();
	oc2a_pin_config(OCR2AVAL);
}

/**
 * Plays a note on the specified channel
 *
 * @param chan the channel to program 
 * @ note the note to play depending on the internal mapping (0 off)
 */
void ay38910_play_note(Channel chan, uint8_t note)
{
	write_to_data_bus((uint8_t)chan, 0xFF & magic_notes[note]);
	write_to_data_bus((uint8_t)chan + 1, 0x0F & (magic_notes[note] >> 8));
}

/**
 * Plays a sound on the noise channel
 *
 * @param divider the divider value to manipulate the noise
 *
 * @retval None
 */
void ay38910_play_noise(uint8_t divider)
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
void ay38910_channel_mode(uint8_t mode)
{
	write_to_data_bus(MIXER_REG, MIXER_MASK | mode);
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
void ay38910_set_amplitude(Channel chan, uint8_t amplitude)
{
	write_to_data_bus(CHAN_TO_AMP_REG(chan), 0x1F & amplitude);
}

void ay38910_set_envelope(EnvelopeShape shape, uint16_t frequency)
{
	write_to_data_bus(FINE_ENV_REG, 0xFF & frequency);
	write_to_data_bus(COARSE_ENV_REG, 0xFF & (frequency >> 8));
	write_to_data_bus(SHAPE_ENV_REG, 0x07 & ((uint8_t)shape));
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


/**
 * Set the PSG to inactive mode
 *
 * @retval None
 */
static void inactive_mode(void)
{
	ResetPin(BC1_PIN);
	ResetPin(BDIR_PIN);
}

/**
 * Set the PSG to write mode
 *
 * @retval None
 */
static void write_mode(void)
{
	ResetPin(BC1_PIN);
	SetPin(BDIR_PIN);
}

/**
 * Set the PSG to latch address mode
 *
 * @retval None
 */
static void latch_address_mode(void)
{
	SetPin(BC1_PIN);
	SetPin(BDIR_PIN);
}

static void write_to_data_bus(uint8_t address, uint8_t data)
{
	// Set the register address
	inactive_mode();
	SET_BUS_PORT(address);
	latch_address_mode();
	inactive_mode();
	
	//Write to the previously set register
	write_mode();
	SET_BUS_PORT(data);
	inactive_mode();	
}

