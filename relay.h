#ifndef RELAY_H
#define RELAY_H

#include <stdbool.h>

void Relay_Init(void);      // RC4 output, default OFF
void Relay_On(void);
void Relay_Off(void);
void Relay_Set(bool on);

#endif // RELAY_H