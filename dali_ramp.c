#include <xc.h>
#include <stdint.h>
#include "dali.h"

#ifndef _XTAL_FREQ
#define _XTAL_FREQ 32000000UL
#endif

// -----------------------------------------------------------------------------
// Simple, blocking DALI ramp helpers for test/demos.
// Depends on DALI_SendForwardFrame_Block() providing ~10 ms inter-frame idle.
// -----------------------------------------------------------------------------

#define DALI_BCAST_DAPC                 0xFEu
#define DALI_TX_INTERFRAME_MS           10u   // must match dali_tx.c

