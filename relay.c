#include <xc.h>
#include <stdbool.h>
#include "relay.h"

#ifndef _XTAL_FREQ
#define _XTAL_FREQ 32000000UL
#endif

void Relay_Init(void)
{
    ANSELCbits.ANSELC4 = 0;  // digital
    TRISCbits.TRISC4   = 0;  // output
    LATCbits.LATC4     = 0;  // OFF
}

void Relay_On(void)        { LATCbits.LATC4 = 1; }
void Relay_Off(void)       { LATCbits.LATC4 = 0; }
void Relay_Set(bool on)    { LATCbits.LATC4 = on ? 1 : 0; }