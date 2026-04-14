// inc/buttons.h
#ifndef BUTTONS_H
#define BUTTONS_H


#include <stdbool.h>
#include <stdint.h>


// Two connectors, each with UP/DOWN buttons
// CN1: RB1=UP1, RB0=DOWN1
// CN2: RB3=UP2, RB2=DOWN2


typedef enum {
BTN_UP1 = 0,
BTN_DOWN1,
BTN_UP2,
BTN_DOWN2,
BTN_COUNT
} button_id_t;


void Buttons_Init(void); // config pins as digital inputs
void Buttons_InterruptInit(void); // enable IOC on RB0..RB3 (falling edge)
void Buttons_Task(void); // 5 ms service to re-enable after debounce
bool Buttons_WasPressed(button_id_t id); // edge (press) + auto-clear


// Call from MCC's IOC interrupt handler (PIN_MANAGER_IOC or InterruptManager)
void Buttons_IOC_ISR(void);


#endif // BUTTONS_H