/** @file keyboard_ifc.h
 *
 * This module implements the keyboard interface logic.
 * The keyboard is a NxM matrix made of push buttons; this approach is
 * a classical one that allows to use NxM key buttons or generic key
 * inputs with only N+M pins.
 * In the case of this module, a whole octave is implemented as a 4x3
 * matrix, of 4 output rows and 3 input columns
 *
 *          c0  c1  c2
 *       ---|---|---|-------
 *       |  |   |   |      |
 *     r0---O---O---O------|
 *       |   |   |   |     |
 *     r1----O---O---O-----|
 *       |    |   |   |    |
 *     r2-----O---O---O----|
 *       |     |   |   |   |
 *     r3------O---O---O---|
 *
 * These keys are sampled by iterating on each row, and checking on the
 * columns which are initialized with a pull-up configuration. On each
 * row iteration, the row is set to the low state, and the algorithm
 * checks if any column is driven low  meaning that the key (i, j) is
 * being pressed. These configuration can be used to check if more than
 * one key is being pressed, and avoids errors thanks to the hardware
 * configuration, where each key connection that goes back to the output
 * row is done through a diode.
 *
 */

#ifndef AY38910A_SYNTH_KEYBOARD_IFC_H
#define AY38910A_SYNTH_KEYBOARD_IFC_H

/************************************************************************/
/* Includes                                                             */
/************************************************************************/

#include <stdint.h>
#include <stdbool.h>

/************************************************************************/
/* Defines                                                              */
/************************************************************************/

#define KEYBOARD_KEYS 12 /**< The number of supported keys */

/************************************************************************/
/* Public functions                                                     */
/************************************************************************/

/**
 * @brief Initializes the keyboard matrix peripheral
 */
void keyboard_init(void);


/**
 * @brief Keyboard acquisition matrix sampling routine.
 *
 * The pressed buttons are coded into a mask that has the ith bit set to
 * 1 if the ith key is pressed.
 *
 * The mask pointer argument that is passed acts as the old mask from
 * the last iteration, and the new mask if a different combination of
 * keys was pressed.
 *
 * @param mask the old mask, modified on key changes.
 * @return true a new key combination was detected
 * @return false same key combination was detected
 */
bool keyboard_acquire(uint16_t *mask);


/**
 * @brief Returns the last reading from the debounced browse key
 *
 * @return true if pressed
 */
bool keyboard_get_browse(void);

/**
 * @brief Returns the last reading from the debounced confirm key
 *
 * @return true if pressed
 */
bool keyboard_get_confirm(void);


/**
 * @brief Initializes the adc for the keyboard potentiometer
 */
void keyboard_init_adc(void);

/**
 * @brief Enables the adc peripheral to acquire data from the
 * potentiometer
 */
void keyboard_enable_potentiometer(void);

/**
 * @brief Disables the adc peripheral to acquire data from the
 * potentiometer
 */
void keyboard_disable_potentiometer(void);

/**
 * @brief Gets the latest potentiometer adc sample
 *
 * @return the sample as an 8-bit value
 */
uint8_t keyboard_get_potentiometer(void);

#endif
