#ifndef DALI_FADE_H
#define DALI_FADE_H
#include <stdint.h>

// Broadcast helpers to configure DALI gear's internal fade engine.
// Sequence: write DTR0 with value, then issue command (per IEC 62386-102).
// Address/data bytes are sent with your existing DALI forward-frame TX.

#ifdef __cplusplus
extern "C" { 
#endif

// --- Configuration commands (address byte uses S=1 "command" semantics) ---
// DATA TRANSFER REGISTER 0 write (special address 0xA3): DTR0 := value
void DALI_WriteDTR0(uint8_t value);

// Broadcast command (S=1) ? sends cmd to all control gear (address byte 0xFF)
void DALI_BroadcastCmd(uint8_t cmd);

// Set FADE TIME := code in DTR0  (0..15, see table in docs)
// Typical: 4 ? ~2.0 s, 7 ? ~5.7 s, 10 ? ~16 s, 15 ? ~90.5 s
void DALI_SetFadeTime(uint8_t code);

// Set FADE RATE := code in DTR0 (1..15 ? 358..2.8 steps/s) ? mostly used by UP/DOWN
void DALI_SetFadeRate(uint8_t code);

// Convenience: commission typical behavior used in this project
//  - fade time ? 2 s (code 4)
//  - fade rate default (code 7 ? 45 steps/s)
static inline void DALI_Commission_DefaultFade(void)
{
    DALI_SetFadeTime(4u);   // ?2.0 s to go between MIN and MAX on DAPC/scene
    DALI_SetFadeRate(7u);   // default rate for UP/DOWN
}

#ifdef __cplusplus
}
#endif

#endif // DALI_FADE_H
