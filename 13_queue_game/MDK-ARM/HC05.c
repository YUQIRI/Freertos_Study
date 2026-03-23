/*  Copyright (s) 2019 深圳百问网科技有限公司
 *  All rights reserved
 * 
 * 文件名称：driver_bluetooth.c
 * 摘要：使用USART1蓝牙接收器替换红外接收器,接口完全兼容
 *  
 * 修改历史     版本号        Author       修改内容
 *--------------------------------------------------
 * 2026.03.22      v01         替换为蓝牙
 *--------------------------------------------------
*/

#include "driver_bluetooth.h"
#include "driver_lcd.h"
#include "driver_timer.h"
#include "stm32f1xx_hal.h"
#include "tim.h"
#include "FreeRTOS.h"  
#include "queue.h"  
#include "typedefs.h"
#include <usart.h>

/* 环形缓冲区: 保存蓝牙数据 */
#define BT_BUF_LEN 128
static unsigned char g_KeysBuf[BT_BUF_LEN];
static int g_KeysBuf_R, g_KeysBuf_W;

/* 接收缓存 */
static uint8_t g_BT_RxData;

extern QueueHandle_t g_xQueuePlatform; 

#define BT_NEXT_POS(x) ((x+1) % BT_BUF_LEN)

/* 环形缓冲区操作 */
static int isKeysBufEmpty(void)
{
	return (g_KeysBuf_R == g_KeysBuf_W);
}

static int isKeysBufFull(void)
{
	return (g_KeysBuf_R == BT_NEXT_POS(g_KeysBuf_W));
}

static void PutKeyToBuf(unsigned char key)
{
	if (!isKeysBufFull())
	{
		g_KeysBuf[g_KeysBuf_W] = key;
		g_KeysBuf_W = BT_NEXT_POS(g_KeysBuf_W);
	}
}

static unsigned char GetKeyFromBuf(void)
{
	unsigned char key = 0xff;
	if (!isKeysBufEmpty())
	{
		key = g_KeysBuf[g_KeysBuf_R];
		g_KeysBuf_R = BT_NEXT_POS(g_KeysBuf_R);
	}
	return key;
}

/**********************************************************************
 * 函数名称： BT_UART_RxCpltCallback
 * 功能描述： USART1中断服务函数,蓝牙数据接收
 **********************************************************************/
void BT_UART_RxCpltCallback(uint8_t ch)
{
    struct input_data idata;

    /* 按原来红外格式发送到队列：dev=0xAA，val=蓝牙收到的字符 */
    idata.dev = 0xAA;
    idata.val = ch;
    xQueueSendToBackFromISR(g_xQueuePlatform, &idata, NULL);

    /* 兼容原来的环形缓冲区 */
    PutKeyToBuf(0xAA);
    PutKeyToBuf(ch);
}
/**********************************************************************
 * 函数名称： IRReceiver_Init
 * 功能描述： 蓝牙初始化(兼容旧接口名)
 ***********************************************************************/
void IRReceiver_Init(void)
{
    /* 开启USART1中断 */
    HAL_NVIC_SetPriority(USART1_IRQn, 5, 0);
    HAL_NVIC_EnableIRQ(USART1_IRQn);
    __HAL_UART_ENABLE_IT(&huart1, UART_IT_RXNE);
}

/**********************************************************************
 * 函数名称： IRReceiver_Read
 * 功能描述： 读取蓝牙数据(兼容旧接口名)
 ***********************************************************************/
int IRReceiver_Read(uint8_t *pDev, uint8_t *pData)
{
    if (isKeysBufEmpty())
        return -1;
    
    *pDev  = GetKeyFromBuf();
    *pData = GetKeyFromBuf();
    return 0;
}

/**********************************************************************
 * 函数名称： IRReceiver_CodeToString
 * 功能描述： 蓝牙字符转名字(兼容游戏控制)
 ***********************************************************************/
const char *IRReceiver_CodeToString(uint8_t code)
{
    static char buf[2];
    if(code >= 0x20 && code <= 0x7E)
    {
        buf[0] = code;
        buf[1] = 0;
        return buf;
    }
    return "BTKey";
}

/**********************************************************************
 * 函数名称： IRReceiver_Test
 * 功能描述： 蓝牙测试(兼容旧接口)
 ***********************************************************************/
void IRReceiver_Test(void)
{
    uint8_t dev, data;
    int len;
	
    IRReceiver_Init();

    while (1)
    {
        LCD_PrintString(0, 0, "BT Receiver: ");        
        LCD_PrintString(0, 2, "Device  Data");

        if (!IRReceiver_Read(&dev, &data))
        {
            LCD_PrintString(0, 4, "                ");
            LCD_PrintHex(0, 4, dev, 1);
            LCD_PrintHex(8, 4, data, 1);
            LCD_PrintString(0, 6, "                ");
            len = LCD_PrintString(0, 6, "BT Key: ");
            LCD_PrintString(len, 6, IRReceiver_CodeToString(data));
        }
        HAL_Delay(20);
    }
}