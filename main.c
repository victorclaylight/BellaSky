#include "mcc_generated_files/system/system.h"
#include <xc.h>
#include <stdint.h>
#include <stdbool.h>

#include "dali.h"
#include "dali_fade.h"   // built-in fade control (FADE TIME / RATE)
#include "debug_led.h"
#include "app_init.h"
#include "buttons.h"
#include "pir.h"
#include "relay.h"

#ifndef _XTAL_FREQ
#define _XTAL_FREQ 32000000UL
#endif

// ======================== DALI constants ========================
#define DALI_BCAST_DAPC   0xFEu
#define DALI_BCAST_CMD    0xFFu
#define DALI_CMD_OFF      0x00u
#define DALI_CMD_UP       0x01u
#define DALI_CMD_DOWN     0x02u
#define DALI_CMD_GO_TO_SCENE_BASE 0x10u
#define DALI_CMD_STORE_SCENE_BASE 0x40u

// ======================== Policy / Timing =======================
#define STARTUP_DALI_SETTLE_MS 3000u
#define PROGRAM_SCENES_ON_STARTUP true
#define STARTUP_FADE_TIME_MS 900u
#define DALI_DEFAULT_FADE_RATE_CODE 4u
#define PROGRAMMED_SCENE_FIRST 1u
#define PROGRAMMED_SCENE_LAST 3u
#define NORMAL_LEVEL_PERCENT 89u
#define NORMAL_STEP_DEFAULT  9u           // about 89..90 % when step scale is 0..10
#define STANDBY_STEP         2u           // 20 % during standby
#define DALI_STEP_MAX        10u
#define NORMAL_HOLD_MS       300000UL     // 300000UL 5 minutes from last PIR trigger, 10000UL for quick testing
#define STANDBY_HOLD_MS      1200000UL    // 1200000UL 20 minutes, then OFF


// ======================== Helpers ===============================
static void delay_ms_blocking(uint16_t delay_ms);

static inline uint8_t dali_level_from_step(uint8_t step /*0..10*/) {
    if (step >= DALI_STEP_MAX) {
        return 254u;
    }

    return (uint8_t)((step * 254u + (DALI_STEP_MAX / 2u)) / DALI_STEP_MAX);
}
static inline void dali_send_step(uint8_t step) {
    DALI_SendForwardFrame_Block(DALI_BCAST_DAPC, dali_level_from_step(step));
}
static inline void dali_send_broadcast_cmd(uint8_t cmd) {
    DALI_SendForwardFrame_Block(DALI_BCAST_CMD, cmd);
}
static inline void dali_send_scene(uint8_t scene_number) {
    dali_send_broadcast_cmd((uint8_t)(DALI_CMD_GO_TO_SCENE_BASE + (scene_number & 0x0Fu)));
}

static uint8_t dali_scene_level(uint8_t scene_number)
{
    switch (scene_number) {
    case 1u:
        return 0u;
    case 2u:
        return (uint8_t)((20u * 254u + 50u) / 100u);
    case 3u:
        return (uint8_t)((NORMAL_LEVEL_PERCENT * 254u + 50u) / 100u);
    default:
        return 0u;
    }
}

static void dali_store_scene(uint8_t scene_number, uint8_t level)
{
    const uint8_t store_cmd = (uint8_t)(DALI_CMD_STORE_SCENE_BASE + (scene_number & 0x0Fu));

    DALI_WriteDTR0(level);
    DALI_BroadcastCmd(store_cmd);
    DALI_BroadcastCmd(store_cmd);
}

static void dali_program_test_scenes(void)
{
    if (!PROGRAM_SCENES_ON_STARTUP) {
        return;
    }

    for (uint8_t scene = PROGRAMMED_SCENE_FIRST; scene <= PROGRAMMED_SCENE_LAST; ++scene) {
        dali_store_scene(scene, dali_scene_level(scene));
        delay_ms_blocking(40u);
    }
}

static void delay_ms_blocking(uint16_t delay_ms)
{
    while (delay_ms--) {
        __delay_ms(1);
    }
}

static uint8_t dali_fade_time_code_from_duration(uint16_t duration_ms)
{
    static const uint16_t fade_time_ms[] = {
        0u, 707u, 1000u, 1414u, 2000u, 2828u, 4000u, 5657u,
        8000u, 11314u, 16000u, 22627u, 32000u, 45255u, 64000u, 65535u
    };

    for (uint8_t code = 1u; code < 15u; ++code) {
        if (duration_ms <= fade_time_ms[code]) {
            return code;
        }
    }

    return 15u;
}

// ======================== State Machine =========================
typedef enum { ST_OFF = 0, ST_NORMAL, ST_STANDBY } dali_mode_t;

static void enter_off(void);
static void enter_standby(void);
static void enter_normal(bool from_pir);

static dali_mode_t mode;
static uint32_t normal_ms_left = 0;
static uint32_t standby_ms_left = 0;
static uint8_t  current_step = 0;            // 0..10 (0 = off)

static void adjust_normal_level(bool brighter)
{
    if (mode != ST_NORMAL) {
        return;
    }

    if (brighter) {
        if (current_step >= DALI_STEP_MAX) {
            return;
        }

        dali_send_broadcast_cmd(DALI_CMD_UP);
        current_step++;
        LED_RB4_Toggle();
    } else {
        if (current_step == 0u) {
            return;
        }

        dali_send_broadcast_cmd(DALI_CMD_DOWN);
        current_step--;
        LED_RB5_Toggle();
    }
}

static void enter_off(void)
{
    mode = ST_OFF;

    // Kill timers so main loop knows we're "done"
    normal_ms_left  = 0;
    standby_ms_left = 0;

    dali_send_scene(1u);
    current_step = 0u;

    DALI_SendForwardFrame_Block(DALI_BCAST_CMD, DALI_CMD_OFF);
    Relay_Off();
}


static void enter_standby(void)
{
    mode = ST_STANDBY;

    // We?re leaving "normal", so cancel its hold timer
    normal_ms_left  = 0;
    standby_ms_left = STANDBY_HOLD_MS;

    Relay_On(); // make sure PSU/driver is powered

    dali_send_scene(2u);

    current_step = STANDBY_STEP;
    LED_RB5_Toggle();
}


static void enter_normal(bool from_pir)
{
    mode = ST_NORMAL;

    standby_ms_left = 0;

    Relay_On();

    dali_send_scene(3u);
    
    current_step = NORMAL_STEP_DEFAULT;

    if (from_pir) {
        // Refresh the NORMAL hold timer only when PIR fired.
        normal_ms_left = NORMAL_HOLD_MS;
    } else {
        // if not PIR-triggered (e.g. button/manual), you may still want a hold
        // keep this if you already had that logic; otherwise it can be left 0
    }
    LED_RB4_Toggle();
}

// ================================ MAIN ===================================
int main(void)
{
    SYSTEM_Initialize();
    App_Init_DALI();
    delay_ms_blocking(STARTUP_DALI_SETTLE_MS);
    DALI_SetFadeTime(dali_fade_time_code_from_duration(STARTUP_FADE_TIME_MS));
    DALI_SetFadeRate(DALI_DEFAULT_FADE_RATE_CODE);

    dali_program_test_scenes();
   
    // Start OFF
    mode = ST_OFF;
    enter_off();
    
    TRISAbits.TRISA2 = 0;
    LATAbits.LATA2 = 1;

    for (;;) {
        Buttons_Task();
        PIR_Task();

        const bool up1_pressed = Buttons_WasPressed(BTN_UP1);
        const bool up2_pressed = Buttons_WasPressed(BTN_UP2);
        const bool down1_pressed = Buttons_WasPressed(BTN_DOWN1);
        const bool down2_pressed = Buttons_WasPressed(BTN_DOWN2);
        const bool up_pressed = up1_pressed | up2_pressed;
        const bool down_pressed = down1_pressed | down2_pressed;

        if (up_pressed) {
            adjust_normal_level(true);
        }

        if (down_pressed) {
            adjust_normal_level(false);
        }

        const bool pir_triggered = PIR_AnyTriggered();
        const bool pir_active = PIR_AnyActive();

        if (pir_triggered || (pir_active && mode != ST_NORMAL)) {
            enter_normal(true);
        } else if (pir_active && mode == ST_NORMAL) {
            normal_ms_left = NORMAL_HOLD_MS;
        }

        // ---------------- Timers (5 ms tick) ----------------
        if (mode == ST_NORMAL) {
            if (normal_ms_left >= 5) normal_ms_left -= 5; else normal_ms_left = 0;
            if (normal_ms_left == 0) {
                // No fresh PIR for 5 minutes ? go to STANDBY
                enter_standby();
            }
        } else if (mode == ST_STANDBY) {
            
            if (standby_ms_left >= 5) standby_ms_left -= 5; else standby_ms_left = 0;
            if (standby_ms_left == 0) {
                // No PIR for additional 20 minutes ? OFF
                enter_off();
            }
        }
        
        __delay_ms(5); // 5 ms service tick (buttons/pir debounce & timers)
    }

    return 0;
}

