// inc/pir.h
#ifndef PIR_H
#define PIR_H


#include <stdint.h>
#include <stdbool.h>



// Using PORTC: PIR1..PIR4 on RC0..RC3 (RC5 is DALItx; leave it alone)
#define PIR1_READ() (PORTCbits.RC0)
#define PIR2_READ() (PORTCbits.RC1)
#define PIR3_READ() (PORTCbits.RC2)
#define PIR4_READ() (PORTCbits.RC3)
#define PIR_ANSEL_INIT() do { \
ANSELCbits.ANSELC0 = 0; ANSELCbits.ANSELC1 = 0; \
ANSELCbits.ANSELC2 = 0; ANSELCbits.ANSELC3 = 0; \
TRISCbits.TRISC0 = 1; TRISCbits.TRISC1 = 1; \
TRISCbits.TRISC2 = 1; TRISCbits.TRISC3 = 1; \
} while(0)
#define PIR_WPU_INIT() do { \
WPUCbits.WPUC0 = 1; WPUCbits.WPUC1 = 1; \
WPUCbits.WPUC2 = 1; WPUCbits.WPUC3 = 1; \
} while(0)



// Set 1 if your PIR output is active-LOW (open collector to GND). From your schematic with 10k pull-ups ? likely 1.
#ifndef PIR_ACTIVE_LOW
#define PIR_ACTIVE_LOW 1
#endif


void PIR_Init(void);
void PIR_Task(void); // call every ~5 ms
bool PIR_AnyTriggered(void); // latched edge, auto-clears on read
bool PIR_AnyActive(void);        // true while any PIR is asserted (debounced)
uint8_t PIR_GetActiveMask(void); // bit0..bit3 show which PIRs are active



#endif // PIR_H