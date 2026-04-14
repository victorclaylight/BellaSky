// src/buttons.c
#include <xc.h>
#include <stdint.h>
#include <stdbool.h>
#include "buttons.h"


#ifndef _XTAL_FREQ
#define _XTAL_FREQ 32000000UL
#endif


// Active level: inputs pulled up, pressed = LOW
#define BTN_ACTIVE_LOW 1


// Map indices ? PORTB bits
#define RB_BIT_UP1 1 // RB1
#define RB_BIT_DOWN1 0 // RB0
#define RB_BIT_UP2 3 // RB3
#define RB_BIT_DOWN2 2 // RB2


// Debounce window ~40 ms with 5 ms service period
#define BTN_SERVICE_MS 5u
#define BTN_DEBOUNCE_MS 40u
#define BTN_DEBOUNCE_TICKS ((BTN_DEBOUNCE_MS + BTN_SERVICE_MS - 1u)/BTN_SERVICE_MS)

static uint8_t reenable_ticks[BTN_COUNT];
static uint8_t pressed_flags;

static inline uint8_t read_pin_bit(uint8_t bit)
{
uint8_t v = (PORTB >> bit) & 1u;
#if BTN_ACTIVE_LOW
v = !v; // pressed=1
#endif
return v;
}

static inline uint8_t id_to_bit(button_id_t id)
{
switch (id) {
case BTN_UP1: return RB_BIT_UP1;
case BTN_DOWN1: return RB_BIT_DOWN1;
case BTN_UP2: return RB_BIT_UP2;
default: return RB_BIT_DOWN2;
}
}

static inline void enable_ioc_on_bit(uint8_t bit)
{
switch (bit) {
case 0: IOCBNbits.IOCBN0 = 1; break;
case 1: IOCBNbits.IOCBN1 = 1; break;
case 2: IOCBNbits.IOCBN2 = 1; break;
case 3: IOCBNbits.IOCBN3 = 1; break;
default: break;
}
}


static inline void disable_ioc_on_bit(uint8_t bit)
{
switch (bit) {
case 0: IOCBNbits.IOCBN0 = 0; break;
case 1: IOCBNbits.IOCBN1 = 0; break;
case 2: IOCBNbits.IOCBN2 = 0; break;
case 3: IOCBNbits.IOCBN3 = 0; break;
default: break;
}
}


void Buttons_Init(void)
{
// Digital inputs on RB0..RB3
ANSELBbits.ANSELB0 = 0; ANSELBbits.ANSELB1 = 0; ANSELBbits.ANSELB2 = 0; ANSELBbits.ANSELB3 = 0;
TRISBbits.TRISB0 = 1; TRISBbits.TRISB1 = 1; TRISBbits.TRISB2 = 1; TRISBbits.TRISB3 = 1;


for (int i = 0; i < BTN_COUNT; ++i) reenable_ticks[i] = 0;
pressed_flags = 0;
}


void Buttons_InterruptInit(void)
{
// Falling-edge IOC (active-LOW press) on RB0..RB3
IOCBPbits.IOCBP0 = 0; IOCBPbits.IOCBP1 = 0; IOCBPbits.IOCBP2 = 0; IOCBPbits.IOCBP3 = 0;
IOCBNbits.IOCBN0 = 1; IOCBNbits.IOCBN1 = 1; IOCBNbits.IOCBN2 = 1; IOCBNbits.IOCBN3 = 1;


// Clear flags and mismatch
IOCBFbits.IOCBF0 = 0; IOCBFbits.IOCBF1 = 0; IOCBFbits.IOCBF2 = 0; IOCBFbits.IOCBF3 = 0;
(void)PORTB;
PIR0bits.IOCIF = 0;
PIE0bits.IOCIE = 1; // enable IOC interrupt
}


void Buttons_IOC_ISR(void)
{
// Edge: treat as PRESS event; disable this pin's IOC until debounced
if (IOCBFbits.IOCBF0) { pressed_flags |= (1u << BTN_DOWN1); disable_ioc_on_bit(0); reenable_ticks[BTN_DOWN1] = BTN_DEBOUNCE_TICKS; IOCBFbits.IOCBF0 = 0; }
if (IOCBFbits.IOCBF1) { pressed_flags |= (1u << BTN_UP1); disable_ioc_on_bit(1); reenable_ticks[BTN_UP1] = BTN_DEBOUNCE_TICKS; IOCBFbits.IOCBF1 = 0; }
if (IOCBFbits.IOCBF2) { pressed_flags |= (1u << BTN_DOWN2); disable_ioc_on_bit(2); reenable_ticks[BTN_DOWN2] = BTN_DEBOUNCE_TICKS; IOCBFbits.IOCBF2 = 0; }
if (IOCBFbits.IOCBF3) { pressed_flags |= (1u << BTN_UP2); disable_ioc_on_bit(3); reenable_ticks[BTN_UP2] = BTN_DEBOUNCE_TICKS; IOCBFbits.IOCBF3 = 0; }


PIR0bits.IOCIF = 0;
}


void Buttons_Task(void)
{
// Re-enable IOC after debounce window and once released
for (int i = 0; i < BTN_COUNT; ++i) {
if (reenable_ticks[i]) {
if (--reenable_ticks[i] == 0) {
// Only re-arm when button is released (not held)
if (!read_pin_bit(id_to_bit((button_id_t)i))) {
// still pressed ? extend debounce window
reenable_ticks[i] = BTN_DEBOUNCE_TICKS;
} else {
enable_ioc_on_bit(id_to_bit((button_id_t)i));
}
}
}
}
}


bool Buttons_WasPressed(button_id_t id)
{
const uint8_t mask = (1u << id);
const bool was = (pressed_flags & mask) != 0;
pressed_flags &= (uint8_t)~mask; // auto-clear
return was;
}