/*
 * Ay38910a.h
 *
 * Author: mar
 */ 


#ifndef AY38910A_H_
#define AY38910A_H_

/************************************************************************/
/* Includes                                                             */
/************************************************************************/

#include "types.h"

/************************************************************************/
/* Defines                                                              */
/************************************************************************/

#define CHA_TONE  0
#define CHB_TONE  1
#define CHC_TONE  2
#define CHA_NOISE 3
#define CHB_NOISE 4
#define CHC_NOISE 5

#define ENVELOPE_ENABLE 4

#define CHA_TONE_ENABLE  (0 << CHA_TONE)
#define CHA_TONE_DISABLE (1 << CHA_TONE)

#define CHB_TONE_ENABLE  (0 << CHB_TONE)
#define CHB_TONE_DISABLE (1 << CHB_TONE)

#define CHC_TONE_ENABLE  (0 << CHC_TONE)
#define CHC_TONE_DISABLE (1 << CHC_TONE)

#define CHA_NOISE_ENABLE  (0 << CHA_NOISE)
#define CHA_NOISE_DISABLE (1 << CHA_NOISE)

#define CHB_NOISE_ENABLE  (0 << CHB_NOISE)
#define CHB_NOISE_DISABLE (1 << CHB_NOISE)

#define CHC_NOISE_ENABLE  (0 << CHC_NOISE)
#define CHC_NOISE_DISABLE (1 << CHC_NOISE)

#define AMPL_ENVELOPE_ENABLE  (1 << ENVELOPE_ENABLE)
#define AMPL_ENVELOPE_DISABLE (0 << ENVELOPE_ENABLE)

/************************************************************************/
/* Typedefs                                                             */
/************************************************************************/

typedef enum {
	CHANNEL_A = 0x00,
	CHANNEL_B = 0x02,
	CHANNEL_C = 0x04
} Channel;

typedef enum {
	SHAPE_HOLD      = 0x01,
	SHAPE_ALTERNATE = 0x02,
	SHAPE_ATTACK    = 0x04,
	SHAPE_CONTINUE  = 0x08
} EnvelopeShape;

/************************************************************************/
/* Public functions                                                     */
/************************************************************************/

void ay38910_init(Port AyParallelPort, Pin b1_pin, Pin bdir_pin);
void ay38910_play_note(Channel chan, u8 note);
void ay38910_play_noise(u8 sound);
void ay38910_channel_mode(u8 mode);
void ay38910_set_amplitude(Channel chan, u8 amplitude);
void ay38910_set_envelope(EnvelopeShape shape, u16 frequency);

#endif /* AY38910A_H_ */