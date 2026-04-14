#include <xc.h>
#include "dali.h"

#ifndef _XTAL_FREQ
#define _XTAL_FREQ 32000000U
#endif

#ifndef DALI_INTERFRAME_IDLE_MS
#define DALI_INTERFRAME_IDLE_MS 10u
#endif

static inline void dali_wait_txbuf_empty(void)
{
    while (!PIR3bits.U1TXIF) { }
}

static inline void dali_wait_tx_complete(void)
{
    while (!U1ERRIRbits.TXMTIF) { }
}

bool DALI_ModeIsSane(void)
{
    const unsigned mode_ok = (U1CON0bits.MODE == 0b1000);
    const unsigned stp_ok  = (U1CON2bits.STP  == 0b10);
    const unsigned brgs_ok = (U1CON0bits.BRGS == 0);
    return (mode_ok && stp_ok && brgs_ok);
}

void DALI_SendForwardFrame(uint8_t address, uint8_t command)
{
    dali_wait_txbuf_empty();
    U1TXB = address;

    dali_wait_txbuf_empty();
    U1TXB = command;

    dali_wait_tx_complete();
}

void DALI_SendForwardFrame_Block(uint8_t address, uint8_t command)
{
    DALI_SendForwardFrame(address, command);
    __delay_ms(DALI_INTERFRAME_IDLE_MS);
}

