// inc/dali.h
#ifndef DALI_H
#define DALI_H

#include <stdint.h>
#include <stdbool.h>

void DALI_SendForwardFrame(uint8_t address, uint8_t command);
void DALI_SendForwardFrame_Block(uint8_t address, uint8_t command);
bool DALI_ModeIsSane(void);

#endif // DALI_H