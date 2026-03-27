/*  Copyright (s) 2019 ���ڰ������Ƽ����޹�˾
 *  All rights reserved
 * 
 * �ļ����ƣ�driver_bluetooth.c
 * ժҪ��ʹ��USART1�����������滻���������,�ӿ���ȫ����
 *  
 * �޸���ʷ     �汾��        Author       �޸�����
 *--------------------------------------------------
 * 2026.03.22      v01         �滻Ϊ����
 *--------------------------------------------------
*/


#include "driver_lcd.h"
#include "driver_timer.h"
#include "stm32f1xx_hal.h"
#include "stm32f1xx_hal_def.h"
#include "tim.h"

#include "typedefs.h"
#include <usart.h>
#include "driver_bluetooth.h"
#include "music.h"


/* ���λ�����: ������������ */
#define BT_BUF_LEN 128
static unsigned char g_KeysBuf[BT_BUF_LEN];
static int g_KeysBuf_R, g_KeysBuf_W;

static uint64_t g_BTReceiverIRQ_Timers[68];
static int g_BTReceiverIRQ_Cnt = 0;


/* ���ջ��� */
static uint8_t g_BT_RxData;

//extern QueueHandle_t g_xQueuePlatform; 
extern QueueHandle_t g_xQueueMusic; 
static BaseType_t xHigherPriorityTaskWoken = pdFALSE;

/* ������������ */
static QueueHandle_t g_xQueueBT; 

#define BT_NEXT_POS(x) ((x+1) % BT_BUF_LEN)

/* ���λ��������� */
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
 * �������ƣ� BT_UART_RxCpltCallback
 * ���������� USART1�жϷ�����,�������ݽ���
 **********************************************************************/
void BT_UART_RxCpltCallback(uint8_t ch)
{
	
    //struct input_data idata;
	struct music_data bdata;
	struct bt_data btdata;
    /* ��ԭ�������ʽ���͵����У�dev=0xAA��val=�����յ����ַ� */
    btdata.dev = 0xAA;
    btdata.val = ch;
	bdata.dev = 0xAA;
    bdata.val = ch;
	/* �ȴ����������У��ٴ������м�*/
    xQueueSendToBackFromISR(g_xQueueBT, &btdata, NULL);
	xQueueSendToBackFromISR(g_xQueueMusic, &bdata, NULL);


    /* ����ԭ���Ļ��λ����� */
    PutKeyToBuf(0xAA);
    PutKeyToBuf(ch);
	
}

/**********************************************************************
 * �������ƣ� IRReceiver_BTQTimes_Parse
 * ���������� �����жϻص��������¼��ʱ������,�õ���device��key���뻷�λ�����
 * ��������� ��
 * ��������� ��
 * �� �� ֵ�� 0 - �ɹ�, (-1) - ʧ��
 * �޸����ڣ�      �汾��     �޸���	      �޸�����
 * -----------------------------------------------
 * 2023/08/04	     V1.0	  Τ��ɽ	      ����
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

	/* 1. �ж�ǰ���� : 9ms�ĵ�����, 4.5ms������  */
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

	/* 2. �������� */
	for (i = 0; i < 32; i++)
	{
		m = 3 + i*2;
		n = m+1;
		time = g_BTReceiverIRQ_Timers[n] - g_BTReceiverIRQ_Timers[m];
		data <<= 1;
		bits++;
		if (time > 1000000)
		{
			/* �õ�������1 */
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

	/* �ж��������� */
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
	//	xQueueSendToBackFromISR(g_xQueuePlatform, &idata, NULL);
    return 0;
}

/**********************************************************************
 * �������ƣ� isRepeatedKey
 * ���������� �����жϻص��������¼��ʱ������,�ж��Ƿ��ظ���
 * ��������� ��
 * ��������� ��
 * �� �� ֵ�� 1 - ��, (0) - ����
 * �޸����ڣ�      �汾��     �޸���	      �޸�����
 * -----------------------------------------------
 * 2023/08/04	     V1.0	  Τ��ɽ	      ����
 ***********************************************************************/
static int isRepeatedKey(void)
{
	uint64_t time;

	/* 1. �ж��ظ��� : 9ms�ĵ�����, 2.25ms������  */
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
 * �������ƣ� BTReceiver_IRQ_Callback
 * ���������� �������������жϻص�����,��¼�ж�ʱ��
 * ��������� ��
 * ��������� ��
 * �� �� ֵ�� ��
 * �޸����ڣ�      �汾��     �޸���	      �޸�����
 * -----------------------------------------------
 * 2023/08/04	     V1.0	  Τ��ɽ	      ����
 ***********************************************************************/
void BTReceiver_IRQ_Callback(void)
{
    uint64_t time;
    static uint64_t pre_time = 0;
	  struct bt_data idata;
	  //struct music_data bdata;
BaseType_t xHigherPriorityTaskWoken = pdFALSE;       
	/* 1. ��¼�жϷ�����ʱ�� */	
	time = system_get_ns();
    
    /* һ�ΰ���������� = ������ + 32������"1" = 9+4.5+2.25*32 = 85.5ms
     * �����ǰ�жϵ�ʱ��, �����ϴ��жϵ�ʱ�̳������ʱ��, ��ǰ�����ݾ�����
     */
    if (time - pre_time > 100000000) 
    {
        g_BTReceiverIRQ_Cnt = 0;
    }
    pre_time = time;
    
	g_BTReceiverIRQ_Timers[g_BTReceiverIRQ_Cnt] = time;

	/* 2. �ۼ��жϴ��� */
	g_BTReceiverIRQ_Cnt++;

	/* 3. ��������, ��������, ����buffer */
	if (g_BTReceiverIRQ_Cnt == 4)
	{
		/* �Ƿ��ظ��� */
		if (isRepeatedKey())
		{
			/* device: 0, val: 0, ��ʾ�ظ��� */
			//PutKeyToBuf(0);
			//PutKeyToBuf(0);
			/* д���� */
			idata.dev = 0;
			idata.val = 0;
//			bdata.dev = 0;
//			bdata.val = 0;
			//xQueueSendToBackFromISR(g_xQueueMusic, &bdata, NULL);
			xQueueSendToBackFromISR(g_xQueueBT, &idata, NULL);
			
			
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
 * �������ƣ� GetQueueBT
 * ���������� ��g_xQueueBT���о�����������
 ***********************************************************************/
QueueHandle_t GetQueueBT(void)
{
	return g_xQueueBT;
}

/**********************************************************************
 * �������ƣ� IRReceiver_Init
 * ���������� ������ʼ��(���ݾɽӿ���)
 ***********************************************************************/
void BTReceiver_Init(void)
{
	/* �������� */	
	g_xQueueBT = xQueueCreate(LenthBT,sizeof(struct bt_data));
	
    /* ����USART1�ж� */
    HAL_NVIC_SetPriority(USART1_IRQn, 5, 0);
    HAL_NVIC_EnableIRQ(USART1_IRQn);
    __HAL_UART_ENABLE_IT(&huart1, UART_IT_RXNE);
	
}

/**********************************************************************
 * �������ƣ� BTReceiver_Read
 * ���������� ��ȡ��������(���ݾɽӿ���)
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
 * �������ƣ� BTReceiver_CodeToString
 * ���������� �ѽ��յ��İ�����ת��Ϊ��������
 * ��������� code - ������
 * ��������� ��
 * �� �� ֵ�� NULL - δʶ��İ�����; ����ֵ - ��������
 * �޸����ڣ�      �汾��     �޸���	      �޸�����
 * -----------------------------------------------
 * 2023/08/04	     V1.0	  Τ��ɽ	      ����
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
 * �������ƣ� IRReceiver_Test
 * ���������� ��������(���ݾɽӿ�)
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