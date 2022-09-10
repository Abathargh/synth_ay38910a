/**
 * delay.c
 *
 * This module implements custom delay routines.
 * These delay aims to be as precise as possible, and are implemented in
 * inline assembly.
 *
 * Author: mar
 */

/************************************************************************/
/* Includes                                                             */
/************************************************************************/

#include "delay.h"

#include <stdint.h>

/**
 * Performs a blocking delay with a microsecond granularity
 * @param us the number of microseconds to wait
 */
inline __attribute__((always_inline))
void delay_us(uint16_t us)
{
	// The small error is due to using a 16 bit input
	// T_del = 	T_clk * #cycles = (n + 0,0625) us
	asm volatile(
		"MOV ZH,%B0\n\t"  // MOV: 1 cycle
		"MOV ZL,%A0\n\t"  // MOV: 1 cycle
 		"%=:\n\t"         // 16 cycles (last BRNE = 1 evens out with MOV)
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
		: "r" (us)
	);
}

/**
 * Performs a blocking delay with a millisecond granularity
 * @param ms the number of milliseconds to wait.
 */
inline __attribute__((always_inline))
void delay_ms(uint16_t ms)
{
	/**
	 * TODO inform the compiler via clobber that the registers are used and check
	 * if it works refer to inline assembly guide optimization => try to remove
	 * MOVs since R25:R24 should be used as first input register
	 */
	// TODO change with info on ms delay T_del = 	T_clk * #cycles = (n + 0,0625) us
	
	asm volatile(
		"MOV ZH,%B0\n\t"  // MOV: 1 cycle
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
		: "r" (ms)
	);
}