// src/app_init.c
#include "dali_init.h"
#include "debug_led.h"
#include "app_init.h"
#include "buttons.h"
#include "pir.h"
#include "relay.h"

void App_Init_DALI(void)
{
// Using HFINTOSC 32 MHz (set in MCC)
//UART1_Initialize(); // MCC-generated init
UART1_DALI_Apply(); // enforce DALI mode + 1200 bps @ _XTAL_FREQ


// Optional: uncomment to verify RC5 (DALIOUT) wiring on a scope
// RC5_WiringProbe_200ms();


// --- PPS remap: UART1 TX to RC5 (DALIOUT) ---
// Keep RX mapping as configured by MCC. Only TX is moved.
PPSLOCK = 0x55; PPSLOCK = 0xAA; PPSLOCKbits.PPSLOCKED = 0; // unlock
RC5PPS = 0x13; // 0x13 = U1TX function
PPSLOCK = 0x55; PPSLOCK = 0xAA; PPSLOCKbits.PPSLOCKED = 1; // lock


ANSELCbits.ANSELC5 = 0; // digital mode
TRISCbits.TRISC5 = 0; // output
DebugLED_Init(); // RB4/RB5 as digital outputs, start off
Buttons_Init();
Buttons_InterruptInit();
PIR_Init(); 
Relay_Init();
}