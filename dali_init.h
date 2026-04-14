// inc/dali_init.h
#ifndef DALI_INIT_H
#define DALI_INIT_H


#include <stdint.h>


#ifndef _XTAL_FREQ
#define _XTAL_FREQ 32000000U
#endif


#define DALI_BAUD 1200UL


void UART1_DALI_Apply(void); // force DALI mode @ 1200 bps
void UART1_DALI_SelfCheck(uint32_t *actual, int32_t *ppm);
uint32_t UART1_DALI_ActualBaud(void);
uint32_t UART1_DALI_ActualBaud_HW(void);
int32_t UART1_DALI_BaudErrorPpm(void);


#endif // DALI_INIT_H