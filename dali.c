#include <xc.h>
#include "dali.h"

#ifndef _XTAL_FREQ
#define _XTAL_FREQ 32000000U
#endif

/* DALI requires an idle (high) period of at least 22 half-bit times between frames.
   At 1200 bps ? bit = 833.333 µs ? 22 * 0.5 * bit ? 9.17 ms. Use 10 ms guard. */
#ifndef DALI_INTERFRAME_IDLE_MS
#define DALI_INTERFRAME_IDLE_MS 10u
#endif

static inline void dali_wait_txbuf_empty(void)
{
    while (!PIR3bits.U1TXIF) { /* wait */ }
}

static inline void dali_wait_tx_complete(void)
{
    // Shift register empty (complete, including stop bits). Read-only flag.
    while (!U1ERRIRbits.TXMTIF) { /* wait */ }
}


