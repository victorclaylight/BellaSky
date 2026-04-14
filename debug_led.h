#ifndef DEBUG_LED_H
#define DEBUG_LED_H


#include <xc.h>


// RB4 = LED1 (state indicator), RB5 = LED2 (activity/error)
static inline void DebugLED_Init(void)
{
// Ensure digital mode and outputs
ANSELBbits.ANSELB4 = 0;
ANSELBbits.ANSELB5 = 0;
TRISBbits.TRISB4 = 0;
TRISBbits.TRISB5 = 0;
LATBbits.LATB4 = 0;
LATBbits.LATB5 = 0;
}


static inline void LED_RB4_On(void) { LATBbits.LATB4 = 1; }
static inline void LED_RB4_Off(void) { LATBbits.LATB4 = 0; }
static inline void LED_RB4_Toggle(void) { LATBbits.LATB4 = !LATBbits.LATB4; }


static inline void LED_RB5_On(void) { LATBbits.LATB5 = 1; }
static inline void LED_RB5_Off(void) { LATBbits.LATB5 = 0; }
static inline void LED_RB5_Toggle(void) { LATBbits.LATB5 = !LATBbits.LATB5; }


#endif // DEBUG_LED_H