/** @file delay.h
 *
 * This module implements custom delay routines.
 * These delay aims to be as precise as possible, and are implemented in
 * assembly.
 */
#ifndef AY38910A_SYNTH_DELAY_LCD_H
#define AY38910A_SYNTH_DELAY_LCD_H

/************************************************************************/
/* Includes                                                             */
/************************************************************************/

#include <stdint.h>

/************************************************************************/
/* Defines                                                              */
/************************************************************************/

/**
 * @brief Performs a blocking delay with a microsecond granularity
 *
 * This delay has a small error of exactly one cycle and is best used
 * when a specific blocking delay with microsecond granularity is needed,
 * for example when writing peripheral drivers.
 *
 * It only works with a 16MHz clock source.
 *
 * @param us the number of microseconds to wait
 */
void delay_us(uint16_t us);

/**
 * @brief Performs a blocking delay with a millisecond granularity
 *
 * This delay has a small error of n microseconds + 1, and is best used
 * when a specific blocking delay with milliseconds granularity is needed,
 * for example when writing peripheral drivers.
 *
 * It only works with a 16MHz clock source.
 *
 * @param ms the number of milliseconds to wait
 */
void delay_ms(uint16_t ms);

#endif
