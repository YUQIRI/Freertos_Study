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

static uint64_t g_BTReceiverIRQ_Timers[68];
static int g_BTReceiverIRQ_Cnt = 0;

/* 接收缓存 */
static uint8_t g_BT_RxData;

extern QueueHandle_t g_xQueuePlatform; 
extern QueueHandle_t g_xQueueMusic; 
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
	struct music_data bdata;

    /* 按原来红外格式发送到队列：dev=0xAA，val=蓝牙收到的字符 */
    idata.dev = 0xAA;
    idata.val = ch;
	bdata.dev = 0xAA;
    bdata.val = ch;
    xQueueSendToBackFromISR(g_xQueuePlatform, &idata, NULL);
	xQueueSendToBackFromISR(g_xQueueMusic, &bdata, NULL);

    /* 兼容原来的环形缓冲区 */
    PutKeyToBuf(0xAA);
    PutKeyToBuf(ch);
}

/**********************************************************************
 * 函数名称： IRReceiver_BTQTimes_Parse
 * 功能描述： 解析中断回调函数里记录的时间序列,得到的device和key放入环形缓冲区
 * 输入参数： 无
 * 输出参数： 无
 * 返 回 值： 0 - 成功, (-1) - 失败
 * 修改日期：      版本号     修改人	      修改内容
 * -----------------------------------------------
 * 2023/08/04	     V1.0	  韦东山	      创建
 ***********************************************************************/
static int BTReceiver_IRQTimes_Parse(void)
{
	uint64_t time;
	int i;
	int m, n;
	unsigned char datas[4];
	unsigned char data = 0;
	int bits = 0;
	int byte = 0;
	struct input_data idata;

	/* 1. 判断前导码 : 9ms的低脉冲, 4.5ms高脉冲  */
	time = g_BTReceiverIRQ_Timers[1] - g_BTReceiverIRQ_Timers[0];
	if (time < 8000000 || time > 10000000)
	{
		return -1;
	}

	time = g_BTReceiverIRQ_Timers[2] - g_BTReceiverIRQ_Timers[1];
	if (time < 3500000 || time > 55000000)
	{
		return -1;
	}

	/* 2. 解析数据 */
	for (i = 0; i < 32; i++)
	{
		m = 3 + i*2;
		n = m+1;
		time = g_BTReceiverIRQ_Timers[n] - g_BTReceiverIRQ_Timers[m];
		data <<= 1;
		bits++;
		if (time > 1000000)
		{
			/* 得到了数据1 */
			data |= 1;
		}

		if (bits == 8)
		{
			datas[byte] = data;
			byte++;
			data = 0;
			bits = 0;
		}
	}

	/* 判断数据正误 */
	datas[1] = ~datas[1];
	datas[3] = ~datas[3];
	
	if ((datas[0] != datas[1]) || (datas[2] != datas[3]))
	{
        g_BTReceiverIRQ_Cnt = 0;
        return -1;
	}

	//PutKeyToBuf(datas[0]);
	//PutKeyToBuf(datas[2]);
		idata.dev = datas[0];
		idata.val = datas[2];
		xQueueSendToBackFromISR(g_xQueuePlatform, &idata, NULL);
    return 0;
}

/**********************************************************************
 * 函数名称： isRepeatedKey
 * 功能描述： 解析中断回调函数里记录的时间序列,判断是否重复码
 * 输入参数： 无
 * 输出参数： 无
 * 返 回 值： 1 - 是, (0) - 不是
 * 修改日期：      版本号     修改人	      修改内容
 * -----------------------------------------------
 * 2023/08/04	     V1.0	  韦东山	      创建
 ***********************************************************************/
static int isRepeatedKey(void)
{
	uint64_t time;

	/* 1. 判断重复码 : 9ms的低脉冲, 2.25ms高脉冲  */
	time = g_BTReceiverIRQ_Timers[1] - g_BTReceiverIRQ_Timers[0];
	if (time < 8000000 || time > 10000000)
	{
		return 0;
	}

	time = g_BTReceiverIRQ_Timers[2] - g_BTReceiverIRQ_Timers[1];
	if (time < 2000000 || time > 2500000)
	{
		return 0;
	}	

	return 1;
}


/**********************************************************************
 * 函数名称： BTReceiver_IRQ_Callback
 * 功能描述： 蓝牙接收器的中断回调函数,记录中断时刻
 * 输入参数： 无
 * 输出参数： 无
 * 返 回 值： 无
 * 修改日期：      版本号     修改人	      修改内容
 * -----------------------------------------------
 * 2023/08/04	     V1.0	  韦东山	      创建
 ***********************************************************************/
void BTReceiver_IRQ_Callback(void)
{
    uint64_t time;
    static uint64_t pre_time = 0;
	  struct input_data idata;
	  struct music_data bdata;
BaseType_t xHigherPriorityTaskWoken = pdFALSE;       
	/* 1. 记录中断发生的时刻 */	
	time = system_get_ns();
    
    /* 一次按键的最长数据 = 引导码 + 32个数据"1" = 9+4.5+2.25*32 = 85.5ms
     * 如果当前中断的时刻, 举例上次中断的时刻超过这个时间, 以前的数据就抛弃
     */
    if (time - pre_time > 100000000) 
    {
        g_BTReceiverIRQ_Cnt = 0;
    }
    pre_time = time;
    
	g_BTReceiverIRQ_Timers[g_BTReceiverIRQ_Cnt] = time;

	/* 2. 累计中断次数 */
	g_BTReceiverIRQ_Cnt++;

	/* 3. 次数达标后, 解析数据, 放入buffer */
	if (g_BTReceiverIRQ_Cnt == 4)
	{
		/* 是否重复码 */
		if (isRepeatedKey())
		{
			/* device: 0, val: 0, 表示重复码 */
			//PutKeyToBuf(0);
			//PutKeyToBuf(0);
			/* 写队列 */
			idata.dev = 0;
			idata.val = 0;
			bdata.dev = 0;
			bdata.val = 0;
			xQueueSendToBackFromISR(g_xQueueMusic, &bdata, NULL);
			xQueueSendToBackFromISR(g_xQueuePlatform, &idata, NULL);
			
			
			g_BTReceiverIRQ_Cnt = 0;
		}
	}
	if (g_BTReceiverIRQ_Cnt == 68)
	{
		BTReceiver_IRQTimes_Parse();
		g_BTReceiverIRQ_Cnt = 0;
	}
}

/**********************************************************************
 * 函数名称： IRReceiver_Init
 * 功能描述： 蓝牙初始化(兼容旧接口名)
 ***********************************************************************/
void BTReceiver_Init(void)
{
    /* 开启USART1中断 */
    HAL_NVIC_SetPriority(USART1_IRQn, 5, 0);
    HAL_NVIC_EnableIRQ(USART1_IRQn);
    __HAL_UART_ENABLE_IT(&huart1, UART_IT_RXNE);
}

/**********************************************************************
 * 函数名称： BTReceiver_Read
 * 功能描述： 读取蓝牙数据(兼容旧接口名)
 ***********************************************************************/
int BTReceiver_Read(uint8_t *pDev, uint8_t *pData)
{
    if (isKeysBufEmpty())
        return -1;
    
    *pDev  = GetKeyFromBuf();
    *pData = GetKeyFromBuf();
    return 0;
}


/**********************************************************************
 * 函数名称： BTReceiver_CodeToString
 * 功能描述： 把接收到的按键码转换为按键名字
 * 输入参数： code - 按键码
 * 输出参数： 无
 * 返 回 值： NULL - 未识别的按键码; 其他值 - 按键名字
 * 修改日期：      版本号     修改人	      修改内容
 * -----------------------------------------------
 * 2023/08/04	     V1.0	  韦东山	      创建
 ***********************************************************************/
const char *BTReceiver_CodeToString(uint8_t code)
{
    const uint8_t codes[]= {0xa2, 0xe2, 0x22, 0x02, 0xc2, 0xe0, 0xa8, 0x90, \
                            0x68, 0x98, 0xb0, 0x30, 0x18, 0x7a, 0x10, 0x38, \
                            0x5a, 0x42, 0x4a, 0x52, 0x00,0x01,0x02};
    const char *names[]= {"Power", "Menu", "Test", "+", "Return", "Left", "Play", "Right", \
                            "0", "-", "C", "1", "2", "3", "4", "5", \
                            "6", "7", "8", "9", "None","Start/Suspend","Delete"};
    int i;
    
    for (i = 0; i < sizeof(codes)/sizeof(codes[0]); i++)
    {
        if (code == codes[i])
        {
            return names[i];
        }
    }
    return "Error";
}



/**********************************************************************
 * 函数名称： IRReceiver_Test
 * 功能描述： 蓝牙测试(兼容旧接口)
 ***********************************************************************/
void BTReceiver_Test(void)
{
    uint8_t dev, data;
    int len;
	
    BTReceiver_Init();

    while (1)
    {
        LCD_PrintString(0, 0, "BT Receiver: ");        
        LCD_PrintString(0, 2, "Device  Data");

        if (!BTReceiver_Read(&dev, &data))
        {
            LCD_PrintString(0, 4, "                ");
            LCD_PrintHex(0, 4, dev, 1);
            LCD_PrintHex(8, 4, data, 1);
            LCD_PrintString(0, 6, "                ");
            len = LCD_PrintString(0, 6, "BT Key: ");
            LCD_PrintString(len, 6, BTReceiver_CodeToString(data));
        }
        HAL_Delay(20);
    }
}