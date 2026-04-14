#ifndef UART_BRG_H
#define UART_BRG_H

#include <stdint.h>

#define LO8(x) ((uint8_t)((uint32_t)(x) & 0xFFu))
#define HI8(x) ((uint8_t)(((uint32_t)(x) >> 8) & 0xFFu))

#define EUSART_DIVISOR(brg16, high) \
    ((brg16) ? ((high) ? 4u : 16u) : ((high) ? 16u : 64u))

#define EUSART_BRG_ROUNDED(fosc, baud, brg16, high) \
    ( ((uint32_t)(fosc) + ((uint32_t)EUSART_DIVISOR((brg16),(high)) * (uint32_t)(baud))/2u) / \
      ((uint32_t)EUSART_DIVISOR((brg16),(high)) * (uint32_t)(baud)) - 1u )

#define EUSART_BRG_CHECK(fosc, baud, brg16, high)                                      \
    enum { _EUSART_BRG_VAL_ = (int)EUSART_BRG_ROUNDED((fosc),(baud),(brg16),(high)) };  \
    typedef char _eusart_brg_oob_[ (_EUSART_BRG_VAL_ >= 0 && _EUSART_BRG_VAL_ <= 65535) ? 1 : -1 ]

#define EUSART_BRG16_HS(fosc, baud) EUSART_BRG_ROUNDED((fosc),(baud),1,1)
#define EUSART_BRG16_LS(fosc, baud) EUSART_BRG_ROUNDED((fosc),(baud),1,0)
#define EUSART_BRG8_HS(fosc, baud)  EUSART_BRG_ROUNDED((fosc),(baud),0,1)
#define EUSART_BRG8_LS(fosc, baud)  EUSART_BRG_ROUNDED((fosc),(baud),0,0)

static inline uint32_t eusart_actual_baud(uint32_t fosc, uint16_t brg, uint8_t brg16, uint8_t high)
{
    const uint32_t div = EUSART_DIVISOR(brg16, high);
    return fosc / (div * ((uint32_t)brg + 1u));
}

#endif // UART_BRG_H