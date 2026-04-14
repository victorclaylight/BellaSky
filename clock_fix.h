// inc/clock_fix.h
#ifndef CLOCK_FIX_H
#define CLOCK_FIX_H


#include <xc.h>


static inline void Clock_Use_EXTOSC_26MHz(void)
{
// Enable external oscillator. In MCC, also ensure FEXTOSC=HS, RSTOSC=EXTOSC.
OSCENbits.EXTOEN = 1;
__delay_ms(5); // wait for crystal to settle


// Switch to EXTOSC, NDIV=1
OSCCON1 = (0u << _OSCCON1_NDIV_POSN) | (7u << _OSCCON1_NOSC_POSN);
}


#endif // CLOCK_FIX_H