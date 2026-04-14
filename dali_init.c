// src/dali_init.c
#include <xc.h>
#include <stdint.h>
#include "uart_brg.h"
#include "dali_init.h"


// Compute BRG from the actual project clock so timing stays correct if _XTAL_FREQ changes
enum { U1BRG_DALI = (int)EUSART_BRG16_LS(_XTAL_FREQ, DALI_BAUD) }; // normal rate (÷16)


void UART1_DALI_Apply(void)
{
U1CON0bits.MODE = 0b1000; // DALI Control Device
U1CON0bits.RXEN = 1;
U1CON0bits.TXEN = 1;
U1CON0bits.BRGS = 0; // normal rate in DALI


U1CON2bits.STP = 0b10; // 2 stop bits
 U1CON2bits.TXPOL = 1; // enable only if your line driver inverts
// U1CON2bits.RXPOL = 0; // default not inverted


U1P1H = 0x00; // ? 22 half-bits idle
U1P1L = 0x16;


U1BRGH = HI8(U1BRG_DALI);
U1BRGL = LO8(U1BRG_DALI); // At 26 MHz, U1BRG_DALI ? 1354 (0x054A)


U1CON1bits.ON = 1;
}


uint32_t UART1_DALI_ActualBaud(void)
{
return eusart_actual_baud(_XTAL_FREQ, (uint16_t)U1BRG_DALI, 1, 0);
}


uint32_t UART1_DALI_ActualBaud_HW(void)
{
const uint16_t brg = ((uint16_t)U1BRGH << 8) | U1BRGL;
const uint8_t brgs = U1CON0bits.BRGS; // should be 0 in DALI
return eusart_actual_baud(_XTAL_FREQ, brg, 1, brgs);
}


int32_t UART1_DALI_BaudErrorPpm(void)
{
const uint32_t actual = UART1_DALI_ActualBaud();
const int32_t diff = (int32_t)actual - (int32_t)DALI_BAUD;
return (int32_t)((int64_t)diff * 1000000LL / (int64_t)DALI_BAUD);
}


void UART1_DALI_SelfCheck(uint32_t *actual_baud, int32_t *error_ppm)
{
if (actual_baud) *actual_baud = UART1_DALI_ActualBaud_HW();
if (error_ppm) *error_ppm = UART1_DALI_BaudErrorPpm();
}