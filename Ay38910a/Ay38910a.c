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
#include "pinout.h"

/************************************************************************/
/* Defines                                                              */
/************************************************************************/

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
 * The frequencies used in the calculations refer to the notes in the
 * equal temperament tuning system, with A4 = 440 Hz.
 *
 * The range of notes is from a B0 to a B8 (8 octaves).
 *
 * Based on these frequencies https://pages.mtu.edu/~suits/notefreqs.html
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

/**
 * Initializes the ay38910a module
 *
 * @retval None
 */
void ay38910_init()
{
    InitOutPin(bc1_pin);
    InitOutPin(bdir_pin);
    InitOutPort(ay_bus_port);
}

/**
 * Plays a note on the specified channel
 *
 * @param chan the channel to program
 * @param note the note to play depending on the internal mapping (0 off)
 */
void ay38910_play_note(Channel chan, uint8_t note)
{
    write_to_data_bus((uint8_t)chan, magic_notes[note] & 0xFF);
    write_to_data_bus((uint8_t)chan + 1, (magic_notes[note] >> 8) & 0x0F);
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
    write_to_data_bus(CHAN_TO_AMP_REG(chan), amplitude & 0x1F);
}

void ay38910_set_envelope(EnvelopeShape shape, uint16_t frequency)
{
    write_to_data_bus(FINE_ENV_REG, frequency & 0xFF);
    write_to_data_bus(COARSE_ENV_REG, (frequency >> 8) & 0xFF);
    write_to_data_bus(SHAPE_ENV_REG, ((uint8_t)shape) & 0x0F);
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
static inline void inactive_mode(void)
{
    ResetPin(bc1_pin);
    ResetPin(bdir_pin);
}

/**
 * Set the PSG to write mode
 *
 * @retval None
 */
static inline void write_mode(void)
{
    ResetPin(bc1_pin);
    SetPin(bdir_pin);
}

/**
 * Set the PSG to latch address mode
 *
 * @retval None
 */
static inline void latch_address_mode(void)
{
    SetPin(bc1_pin);
    SetPin(bdir_pin);
}

static inline void write_to_data_bus(uint8_t address, uint8_t data)
{
    // Set the register address
    inactive_mode();
    SetPort(ay_bus_port, address);
    latch_address_mode();
    inactive_mode();

    //Write to the previously set register
    write_mode();
    SetPort(ay_bus_port, data);
    inactive_mode();
}
