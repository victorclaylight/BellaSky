// inc/dali_ramp.h
#ifndef DALI_RAMP_H
#define DALI_RAMP_H


#include <stdint.h>


/** Broadcast ramp 0?100%?0 over exactly 10 seconds (one cycle). */
void DALI_Ramp_0_100_0_10s(void);


/** Broadcast ramp 0?100%?0 over 'period_ms' milliseconds, repeat 'repeats' times. */
void DALI_Ramp_0_100_0(uint16_t period_ms, uint8_t repeats);


#endif // DALI_RAMP_H