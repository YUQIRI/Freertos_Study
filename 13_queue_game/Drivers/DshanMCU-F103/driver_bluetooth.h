#ifndef __DRIVER_BLUETOOTH_H
#define __DRIVER_BLUETOOTH_H

#include <stdint.h>

void BTReceiver_Init(void);

int BTReceiver_Read(uint8_t *pDev, uint8_t *pData);


void BT_UART_RxCpltCallback(uint8_t ch);

const char *BTReceiver_CodeToString(uint8_t code);

void BTReceiver_Test(void);

#endif