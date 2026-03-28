#ifndef __DRIVER_BLUETOOTH_H
#define __DRIVER_BLUETOOTH_H

#include <stdint.h>
#include "FreeRTOS.h"  
#include "Task.h" 
#include "queue.h" 
#include "cmsis_os.h"
#define LenthBT 10

#define BT_KEY_LEFT 	0xE0
#define BT_KEY_RIGHT 	0x90
#define BT_KEY_REPEAT 	0x00

#define BT_KEY_1	0x01
#define BT_KEY_2	0x02	
#define BT_KEY_3	0x03

#define BT_MUSIC_SET 0x10
#define BT_MUSIC_DEL 0x11

struct bt_data {
	uint32_t dev;
	uint32_t val;
};

void BTReceiver_Init(void);

int BTReceiver_Read(uint8_t *pDev, uint8_t *pData);

void RegisterQueueHandle(QueueHandle_t xQueue);

static void DispathKey(struct bt_data *btdata);
void BT_UART_RxCpltCallback(uint8_t ch);

const char *BTReceiver_CodeToString(uint8_t code);

void BTReceiver_Test(void);

QueueHandle_t GetQueueBT(void);


#endif