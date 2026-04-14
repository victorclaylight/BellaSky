#include <xc.h>
#include <stdint.h>
#include "dali.h"
#include "dali_fade.h"

// DALI addressing bytes (S-bit semantics):
#define DALI_ADDR_BCAST_CMD  0xFFu  // broadcast, S=1 (command)
#define DALI_ADDR_DTR0       0xA3u  // special: DATA TRANSFER REGISTER 0 write

// Config command opcodes (second byte when S=1): see IEC 62386-102
#define DALI_CMD_SET_FADE_TIME   0x2Eu
#define DALI_CMD_SET_FADE_RATE   0x2Fu

void DALI_WriteDTR0(uint8_t value)
{
    // Address 0xA3, data := value
    DALI_SendForwardFrame_Block(DALI_ADDR_DTR0, value);
}

void DALI_BroadcastCmd(uint8_t cmd)
{
    DALI_SendForwardFrame_Block(DALI_ADDR_BCAST_CMD, cmd);
}

void DALI_SetFadeTime(uint8_t code)
{
    DALI_WriteDTR0(code & 0x0Fu);          // DTR0 := fade time code (0..15)
    DALI_BroadcastCmd(DALI_CMD_SET_FADE_TIME);
    DALI_BroadcastCmd(DALI_CMD_SET_FADE_TIME);
}

void DALI_SetFadeRate(uint8_t code)
{
    uint8_t c = code & 0x0Fu;              // 1..15 valid; 0 = no-fade in some gear
    DALI_WriteDTR0(c);
    DALI_BroadcastCmd(DALI_CMD_SET_FADE_RATE);
    DALI_BroadcastCmd(DALI_CMD_SET_FADE_RATE);
}
