/** @file ay38910a.h
 *
 * This module implements the low level driver for the AY38910A
 * Programmable sound Generator Chip. This is achieved by
 * implementing the following features:
 *   - generating a 2MHz clock signal to use as the PSG input
 *   - interfacing with the AY38910A address/data bus
 *   - interfacing with the AY38910A bus control line
 */

#ifndef AY38910A_H_
#define AY38910A_H_

/************************************************************************/
/* Includes                                                             */
/************************************************************************/

#include <stdint.h>

/************************************************************************/
/* Defines                                                              */
/************************************************************************/

/**
 * @defgroup ToneNoiseMacros Tone and Noise channel macros
 *
 * Channel-related macros.
 */
/**@{*/
#define CHANNEL_NUM  3

#define CHA_TONE  0 /**< Channel A Tone bit */
#define CHB_TONE  1 /**< Channel B Tone bit */
#define CHC_TONE  2 /**< Channel C Tone bit */
#define CHA_NOISE 3 /**< Channel A Noise bit */
#define CHB_NOISE 4 /**< Channel B Noise bit */
#define CHC_NOISE 5 /**< Channel C Noise bit */

/** @def CHAN_ENABLE(c)
 *
 * @brief Computes the mask that enables the passed channel.
 *
 * Computes a mask that can be used to enable a specific channel.
 * A bit set to to '0' enables the channel corresponding to
 * that bit position.
 *
 * @param c the channel bit, as specified in the CH*_TONE/NOISE macros
 * @return the encoded bitmask that enables the channel
 */
#define CHAN_ENABLE(c)  (~(1 << c))


/** @def CHAN_DISABLE(c)
 *
 * @brief Computes the mask that disables the passed channel.
 *
 * Computes a mask that can be used to disable a specific channel.
 * A bit set to to '1' disables the channel corresponding to
 * that bit position.
 *
 * @param c the channel bit, as specified in the CH*_TONE/NOISE macros
 * @return the encoded bitmask that disables the channel
 */
#define CHAN_DISABLE(c) (1 << c)
/**@}*/

/**
 * @defgroup EnvelopeMacros Envelope enable macros
 */
/**@{*/
#define ENVELOPE_ENABLE 4 /**< Bit 4 if the amplitude word is used to control envelopes */

#define AMPL_ENVELOPE_DISABLE (~(1 << ENVELOPE_ENABLE)) /**< Envelope disable mask */
#define AMPL_ENVELOPE_ENABLE  (1 << ENVELOPE_ENABLE)    /**< Envelope enable mask */
/**@}*/

/**
 * @defgroup NoteMacros Note macros
 * Macros related to the ay38910a_play_note function
 */
/**@{*/

#define N_NOTES      97 /**< Total number of notes, from B0 to B8 */

/** @def NOTE(N,OCT)
 *
 * @brief Computes the index corresponding to the selected note
 *
 * The AY38910a PSG can reproduce notes in the 30.5-125k Hz. The
 * frequencies that are actually actuated by this library are in the
 * 30.5-7.8k Hz, corresponding to 8 octaves (B0-B8).
 *
 * This macro function allows for the safe selection of a note in that
 * range. Prefer calling ay38910a_play_note using this macro function,
 * along with the *_NOTE macros that map to the note of a single octave.
 * @code
 * assert(NOTE(0, 3) == NOTE(12, 2)); // => true
 * @endcode
 *
 * @param N the note to play (0 is a B, 11 is a Bb/A#)
 * @param OCT the octave to play it in
 * @return the encoded value for the selected note
 */
#define NOTE(N,OCT) (((N + 12*(OCT)) % N_NOTES + N_NOTES) % N_NOTES)
/**@}*/


/************************************************************************/
/* Typedefs                                                             */
/************************************************************************/

/**
 * @brief The channels physically present on the AY38910a chip
 *
 * Each channel is mapped onto its starting address that must be used
 * when communicating the note to play.
 *
 */
typedef enum {
	CHANNEL_A = 0x00, /**< Channel A selected  */
	CHANNEL_B = 0x02, /**< Channel B selected */
	CHANNEL_C = 0x04  /**< Channel C selected */
} channel_t;

/**
 * @brief The available envelope shapes/cycle functions
 *
 * The envelope available through the PSG can have its
 * waveform modified by setting one or more shape bits.
 * Setting the shape bit to one activates it.
 */
typedef enum {
	FUNC_HOLD      = 0x01, /**< Limits the shape to one cycle */
	FUNC_ALTERNATE = 0x02, /**< Reverses the count direction */
	FUNC_ATTACK    = 0x04, /**< Counts up if active, down otherwise */
	FUNC_CONTINUE  = 0x08  /**< If not active, the counter resets and holds */
} envelope_func_t;

/************************************************************************/
/* Public functions                                                     */
/************************************************************************/

/**
 * @brief Initializes the ay38910a module
 *
 * This function initializes the control pins, data bus and clock signal
 * for the PSG. Call before using any other library function.
 */
void ay38910_init(void);

/**
 * @brief Plays a note on the specified channel
 *
 * Plays the passed note on a specific channel; prefer using this function
 * with the *NOTE* macro function, which is safer than using raw integers.
 * @code
 * ay38910a_play_note(CHANNEL_A, NOTE(n, oct));
 * @endcode
 *
 * @param chan the channel to program
 * @param note the note to play depending on the internal mapping (0 off)
 */
void ay38910_play_note(channel_t chan, uint8_t note);


/**
 * @brief Plays a sound on the noise channel
 *
 * @param divider the divider value to manipulate the noise
 */
void ay38910_play_noise(uint8_t sound);


/**
 * @brief Enables/disables tone/noise generation on one or more channels
 *
 * When enabling channels, the six less significant bits of the mode word
 * are used to enable or disable a specific channel in a specific mode.
 * A bit set to to '0' enables the channel corresponding to that bit
 * position, while a bit set to '1' disables it.
 *
 * Prefer using this function with the CHAN_ENABLE/DISABLE macros for
 * clarity purposes:
 * @code
 * uint8_t mask = 0;
 * mask &= CHAN_ENABLE(CHA_TONE) & CHAN_ENABLE(CHC_TONE);
 * ay38910_channel_mode(mask);
 * // do stuff with the A and C channels
 * mask |= CHAN_DISABLE(CHA_TONE) | CHAN_DISABLE(CHC_TONE);
 * ay38910_channel_mode(mask);
 * @endcode
 *
 * @param mode the mode(s) to enable, using the ENABLE/DISABLE defines
 */
void ay38910_channel_mode(uint8_t mode);

/**
 * @brief Sets the amplitude for the specified channel
 *
 * When setting the amplitude, only the first five less significant
 * bits hold information, with the first four bits referring to the
 * amplitude itself (0-16), and the fifth bit referring to the envelope
 * filter being enabled.
 *
 * Enabling the envelope filter disables the usage of fixed amplitude
 * through bits[0:3].
 *
 * @param chan the channel that will have the passed amplitude
 * @param amplitude the amplitude to set. Use with the enable/disable macros.
 */
void ay38910_set_amplitude(channel_t chan, uint8_t amplitude);

/**
 * @brief Sets the envelope shape and frequency
 *
 * @param shape the shape of the envelop to enable
 * @param frequency the frequency of the envelope
 */
void ay38910_set_envelope(envelope_func_t shape, uint16_t frequency);

#endif /* AY38910A_H_ */