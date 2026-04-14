/*
© [2025] Microchip Technology Inc. and its subsidiaries.

    Subject to your compliance with these terms, you may use Microchip 
    software and any derivatives exclusively with Microchip products. 
    You are responsible for complying with 3rd party license terms  
    applicable to your use of 3rd party software (including open source  
    software) that may accompany Microchip software. SOFTWARE IS ?AS IS.? 
    NO WARRANTIES, WHETHER EXPRESS, IMPLIED OR STATUTORY, APPLY TO THIS 
    SOFTWARE, INCLUDING ANY IMPLIED WARRANTIES OF NON-INFRINGEMENT,  
    MERCHANTABILITY, OR FITNESS FOR A PARTICULAR PURPOSE. IN NO EVENT 
    WILL MICROCHIP BE LIABLE FOR ANY INDIRECT, SPECIAL, PUNITIVE, 
    INCIDENTAL OR CONSEQUENTIAL LOSS, DAMAGE, COST OR EXPENSE OF ANY 
    KIND WHATSOEVER RELATED TO THE SOFTWARE, HOWEVER CAUSED, EVEN IF 
    MICROCHIP HAS BEEN ADVISED OF THE POSSIBILITY OR THE DAMAGES ARE 
    FORESEEABLE. TO THE FULLEST EXTENT ALLOWED BY LAW, MICROCHIP?S 
    TOTAL LIABILITY ON ALL CLAIMS RELATED TO THE SOFTWARE WILL NOT 
    EXCEED AMOUNT OF FEES, IF ANY, YOU PAID DIRECTLY TO MICROCHIP FOR 
    THIS SOFTWARE.
*/

#include <xc.h>
#include "../uart1.h"

// Set the UART1 module to the options selected in the user interface.
void UART1_Initialize(void) {
   // Ensure DALI Control Device mode (MODE=0b1000), RX/TX enable, BRGS=0 (normal)
    U1CON0bits.MODE = 0b1000; // DALI Control Device
    U1CON0bits.RXEN = 1;
    U1CON0bits.TXEN = 1;
    U1CON0bits.BRGS = 0;      // BRGS not supported in DALI; force normal speed

    // Two stop bits, polarity per interface (MCC sets these; override only if needed)
    U1CON2bits.STP = 0b10;    // 2 Stop bits
    // U1CON2bits.TXPOL = 1;  // invert if your line driver inverts
    // U1CON2bits.RXPOL = 0;  // not inverted by default

    // Inter-frame timing: at least 22 half-bit times
    U1P1H = 0x00;
    U1P1L = 0x16;             // 22 half-bit periods minimum

    // Forward/backward threshold example (keep your MCC value if it suits your timing)
    // U1P2H = 0x00; U1P2L = 0x03;

    // Baud for 1200 bps @ 30 MHz in normal rate
    U1BRGH = 0x06;
    U1BRGL = 0x1A;

    // Enable module at the end
    U1CON1bits.ON = 1;
}
