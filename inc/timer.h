#ifndef AY38910A_SYNTH_TIMER_H
#define AY38910A_SYNTH_TIMER_H

#include "pin_config.h"
#include <stdbool.h>
#include <assert.h>

typedef struct timer8_t {
	map_io8 * tccr_a;
	map_io8 * tccr_b;
	map_io8 * tim_sk;
	union {
		map_io8  * ocr_a_8;
		map_io16 * ocr_a_16;
	};
	port_t  * ocr_a_port;
	uint8_t   ocr_a_pin;
} timer_t;

#define	TIMER_MODE_NORMAL                0x00
#define	TIMER_MODE_PWM_PHASE_CORRECT     0x01
#define	TIMER_MODE_CTC                   0x02
#define	TIMER_MODE_FAST_PWM              0x03
#define	TIMER_MODE_OCR_TOP               0x05
#define	TIMER_MODE_FAST_PWM_OCR_TOP      0x07

#define IS_VALID_MODE(m) (m == TIMER_MODE_NORMAL ||              \
                          m == TIMER_MODE_PWM_PHASE_CORRECT ||   \
													m == TIMER_MODE_CTC ||                 \
													m == TIMER_MODE_FAST_PWM ||            \
													m == TIMER_MODE_OCR_TOP  ||            \
													m == TIMER_MODE_FAST_PWM_OCR_TOP)


#define	TIMER_CLOCK_NO_SOURCE            0x00
#define	TIMER_CLOCK_NO_PRESCALER         0x01
#define	TIMER_CLOCK_PRESCALER_8          0x02
#define	TIMER_CLOCK_PRESCALER_32         0x03
#define	TIMER_CLOCK_PRESCALER_64         0x04
#define	TIMER_CLOCK_PRESCALER_128        0x05
#define	TIMER_CLOCK_PRESCALER_256        0x06
#define	TIMER_CLOCK_PRESCALER_1024       0x07

#define IS_VALID_CLOCK(c) (c >= TIMER_CLOCK_NO_SOURCE && \
                           c <= TIMER_CLOCK_PRESCALER_1024)


#define	TIMER_CLOCK_EXT_NO_SOURCE        0x00
#define	TIMER_CLOCK_EXT_NO_PRESCALER     0x01
#define	TIMER_CLOCK_EXT_PRESCALER_8      0x02
#define	TIMER_CLOCK_EXT_PRESCALER_64     0x03
#define	TIMER_CLOCK_EXT_PRESCALER_256    0x04
#define	TIMER_CLOCK_EXT_PRESCALER_1024   0x05
#define	TIMER_CLOCK_EXT_EXTERNAL_RISING  0x06
#define	TIMER_CLOCK_EXT_EXTERNAL_FALLING 0x07

#define IS_VALID_CLOCK_EXT(c) (c >= TIMER_CLOCK_EXT_NO_SOURCE && \
                               c <= TIMER_CLOCK_EXT_EXTERNAL_FALLING)

#endif //AY38910A_SYNTH_TIMER_H
