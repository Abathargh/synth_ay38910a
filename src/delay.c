/************************************************************************/
/* Includes                                                             */
/************************************************************************/

#include "delay.h"

#include <stdint.h>

/************************************************************************/
/* Function implementations                                             */
/************************************************************************/

inline __attribute__((always_inline))
void delay_us(uint16_t us)
{
	/**
	 * Blocking delay that is hand written around a 16MHz clock source.
	 * The internal loop is exactly 16 clock cycles long. In the last
	 * iteration, when the loop breaks, the last BRNE that gets executed
	 * only needs 1 clock cycle. If we consider the last BRNE together with
	 * one of the first MOV, then we can observe that this delay executes
	 * only one cycle more than it should, giving a delay of:
	 *   T_del = 	T_clk * #cycles = (n + 0,0625) us
	 * Note that this implementation is inlined, so there should not be any
	 * additional function call overhead.
	 */
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
		"SBIW Z,1\n\t"    // SBIW: 2 cycles
		"BRNE %=b\n\t"    // BRNE: 2 if condition is false, 1 otherwise
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
	 * Blocking delay that is hand written around a 16MHz clock source.
	 * The approach is similar to the microsecond delay, but with some
	 * more loops. Let us consider the inner loop as having varying
	 * number of cycles 'N'. Then, by following a similar approach in
	 * counting cycles together as for the previous delay, we come to
	 * the following:
	 *   #cycles = n(1000N + 16) + 1
	 * Where 'n' is the number of repetitions passed by the user (the
	 * 'ms' parameter). Dividing everything by the clock frequency yields
	 * the actual elapsed time in seconds:
	 *   T_del(n) = #cycles/f_clk = n(0,0625N ms + 0,001 ms) + 1 us
	 * That, with N = 16, is equal to:
	 *   T_del(n) = n * 1,001 ms + 0,001 ms
	 * Note that this implementation is inlined, so there should not be any
	 * additional function call overhead.
	 *
	 * TODO inform the compiler via clobber that the registers are used and check
	 * if it works refer to inline assembly guide optimization => try to remove
	 * MOVs since R25:R24 should be used as first input register
	 */
	asm volatile(
		"MOV ZH,%B0\n\t"  // MOV: 1 cycle
		"MOV ZL,%A0\n\t"  // MOV: 1 cycle => 1 + (16012 + 4) * ms = rep*16016 + 1
		"OUTER%=:\n\t"    // (4000 + 3) * 4 = 16012
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
		"NOP\n\t"
		"DEC R17\n\t"
		"BRNE MICROSEC%=\n\t"
		"DEC R18\n\t"
		"BRNE MILLISEC%=\n\t"
		"SBIW Z,1\n\t"        // SBIW: 2 cycles
		"BRNE OUTER%=\n\t"    // BRNE: 2 if condition is false, 1 otherwise
		:
		: "r" (ms)
	);
}