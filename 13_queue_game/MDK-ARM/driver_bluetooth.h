#ifndef __DRIVER_BLUETOOTH_H
#define __DRIVER_BLUETOOTH_H

#include <stdint.h>

void IRReceiver_Init(void);
int IRReceiver_Read(uint8_t *pDev, uint8_t *pData);
const char *IRReceiver_CodeToString(uint8_t code);
void IRReceiver_Test(void);

#endif