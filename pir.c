// src/pir.c
#include <xc.h>
#include <stdint.h>
#include <stdbool.h>
#include "pir.h"


#ifndef _XTAL_FREQ
#define _XTAL_FREQ 32000000UL
#endif

#ifndef PIR_DEBOUNCE_SAMPLES
#define PIR_DEBOUNCE_SAMPLES 4      // 4*5ms = ~20 ms; faster + responsive
#endif

// mask for N stable samples
#define STABLE_MASK   ((uint8_t)((1u << PIR_DEBOUNCE_SAMPLES) - 1u))

static uint8_t hist[4];
static uint8_t stable[4];
static uint8_t latched;  // rising-edge latches (auto-cleared by PIR_AnyTriggered)

// returns 1 when motion asserted, 0 when idle; applies PIR_ACTIVE_LOW
static inline uint8_t phys_read_norm(uint8_t idx)
{
    uint8_t v;
    switch (idx) {
        case 0: v = PIR1_READ(); break;
        case 1: v = PIR2_READ(); break;
        case 2: v = PIR3_READ(); break;
        default: v = PIR4_READ(); break;
    }
#if PIR_ACTIVE_LOW
    v = !v;                     // normalize: 1 = motion, 0 = idle
#endif
    return v & 1u;
}

void PIR_Init(void)
{
    PIR_ANSEL_INIT();
    PIR_WPU_INIT();

    // seed histories from live levels to avoid delayed/false edges at power-up
    for (int i = 0; i < 4; ++i) {
        const uint8_t a = phys_read_norm(i);
        hist[i]   = a ? STABLE_MASK : 0u;
        stable[i] = a ? 1u : 0u;
    }
    latched = 0u;
}

void PIR_Task(void)
{
    for (int i = 0; i < 4; ++i) {
        hist[i] = (uint8_t)(((hist[i] << 1) | phys_read_norm(i)) & STABLE_MASK);

        if (hist[i] == STABLE_MASK) {             // stable asserted (motion)
            if (!stable[i]) {                     // rising edge
                latched |= (1u << i);
            }
            stable[i] = 1u;
        } else if (hist[i] == 0u) {               // stable idle
            stable[i] = 0u;
        }
        // otherwise in transition ? keep previous stable[i]
    }
}


bool PIR_AnyTriggered(void)
{
const bool any = (latched != 0);
latched = 0u; // auto-clear
return any;
}

bool PIR_AnyActive(void)
{
    // stable[] is the debounced state: 1 = motion asserted, 0 = idle
    return (stable[0] | stable[1] | stable[2] | stable[3]) != 0u;
}

uint8_t PIR_GetActiveMask(void)
{
    return (stable[0]?1u:0u) | (stable[1]?2u:0u) | (stable[2]?4u:0u) | (stable[3]?8u:0u);
}