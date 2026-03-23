/* USER CODE BEGIN Header */
#include "driver_led.h"
#include "driver_lcd.h"
#include "driver_mpu6050.h"
#include "driver_timer.h"
#include "driver_ds18b20.h"
#include "driver_dht11.h"
#include "driver_active_buzzer.h"
#include "driver_passive_buzzer.h"
#include "driver_color_led.h"
//#include "driver_ir_receiver.h"
#include "driver_ir_sender.h"
//#include "driver_light_sensor.h"
//#include "driver_ir_obstacle.h"
//#include "driver_ultrasonic_sr04.h"
//#include "driver_spiflash_w25q64.h"
#include "driver_rotary_encoder.h"
#include "driver_motor.h"
#include "driver_key.h"
#include "driver_uart.h"
#include "driver_bluetooth.h"
#include <usart.h>
#include "typedefs.h"

/**
  ******************************************************************************
  * File Name          : freertos.c
  * Description        : Code for freertos applications
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2023 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */
/* USER CODE END Header */

/* Includes ------------------------------------------------------------------*/
#include "FreeRTOS.h"
#include "task.h"
#include "main.h"
#include "cmsis_os.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "queue.h"
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

//static uint8_t rxData; 
//static uint8_t rxMusicData; 

//// 串口接收完成回调函数
//void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
//{
//    if(huart->Instance == USART1)  // 判断是哪个串口
//    {
//        // 在这里处理收到的数据
//          rxMusicData = rxData;
//          rxData = 0x00;
//        // 重要！如果要继续接收，必须重新开启
//        HAL_UART_Receive_IT(&huart1, &rxData, 1);
//    }
//}

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
/* USER CODE BEGIN Variables */
static TaskHandle_t  SoundStaticTask ;
static TaskHandle_t  SoundInsideTask = NULL;
static UBaseType_t reg;


static StackType_t g_pucStackOfLightTask[128];
static StaticTask_t g_TCBofLightTask;
static TaskHandle_t xLightTaskHandle;

static StackType_t g_pucStackOfColorTask[128];
static StaticTask_t g_TCBofColorTask;
static TaskHandle_t xColorTaskHandle;

void game1_task(void *params);



/* USER CODE END Variables */
/* Definitions for defaultTask */
osThreadId_t defaultTaskHandle;
const osThreadAttr_t defaultTask_attributes = {
  .name = "defaultTask",
  .stack_size = 128 * 4,
  .priority = (osPriority_t) osPriorityNormal,
};

/* Private function prototypes -----------------------------------------------*/
/* USER CODE BEGIN FunctionPrototypes */

QueueHandle_t g_xQueueMusic; /* 音乐队列 */

void MusicTask(void *argument)
{
  while(1)
  {
    BadApple_Play();
    vTaskDelay(50);
  }
 
}


void SoundTask(void *argument)
{

  PassiveBuzzer_Init();
  uint8_t Running = 1;
 //

  struct input_data bdata;
// HAL_UART_Receive_IT(&huart1, &rxData, 1);
//  LCD_Clear();
//  LCD_PrintString(6, 0, "Ready");
   uint8_t  b_data, b_last_data;
  while(1)
  {
  
  		if (pdPASS == xQueueReceive(g_xQueueMusic, &bdata, 10))
		{
		  b_data = bdata.val;
            if (b_data == 0x00)
            {
                b_data = b_last_data;
            }
            
            if (b_data == 0x01) /* 暂停/启动 */
            {
				b_data = 0x00;
                 extern void MusicTask(void *argument);

				  if(SoundInsideTask == NULL)
				  {
					/* 创建任务 */

			//        LCD_Clear();
			//        LCD_PrintString(0, 0, "Creat Task");

					reg = xTaskCreate(MusicTask, "BadApple", 128, NULL, osPriorityNormal + 1 , &SoundInsideTask);
					
					  }
				  else 
				  {
						if(Running)
						{
				//          LCD_Clear();
				//          LCD_PrintString(0, 0, "Suspend Task");

						  vTaskSuspend(SoundInsideTask);
						  PassiveBuzzer_Control(0);
						  Running = 0;
						}
						else 
						{
				//          LCD_Clear();
				//          LCD_PrintString(0, 0, "Resume Task");

						  vTaskResume(SoundInsideTask);
						  Running = 1;
						}
					}
			}
            if (b_data == 0x02)  /* 删除 */
            {
				b_data = 0x00;
                if(SoundInsideTask != NULL)
				  {
			//        LCD_Clear();
			//        LCD_PrintString(0, 0, "Delete Task");

					vTaskDelete(SoundInsideTask);
					SoundInsideTask = NULL;   
					PassiveBuzzer_Control(0);
				  }
			
			vTaskDelay(50);
            }
			}
            b_last_data = b_data;

//    /* 启动/暂停 信号 */
//    if(0x01 == rxMusicData)
//    {
//      rxMusicData = 0x00;
//      /* 创建任务 */
//     
//      
//    }
//    /* 删除信号 */
//    if(0x02 == rxMusicData)
//    {
//      rxMusicData = 0x00;

      
  }

}

/* USER CODE END FunctionPrototypes */

void StartDefaultTask(void *argument);

void MX_FREERTOS_Init(void); /* (MISRA C 2004 rule 8.1) */

/**
  * @brief  FreeRTOS initialization
  * @param  None
  * @retval None
  */
void MX_FREERTOS_Init(void) {
  /* USER CODE BEGIN Init */
  LCD_Init();
  LCD_Clear();
  
  
  IRReceiver_Init();
  LCD_PrintString(0, 0, "Starting");

  /* USER CODE END Init */

  /* USER CODE BEGIN RTOS_MUTEX */
  /* add mutexes, ... */
  /* USER CODE END RTOS_MUTEX */

  /* USER CODE BEGIN RTOS_SEMAPHORES */
  /* add semaphores, ... */
  /* USER CODE END RTOS_SEMAPHORES */

  /* USER CODE BEGIN RTOS_TIMERS */
  /* start timers, add new ones, ... */
  /* USER CODE END RTOS_TIMERS */

  /* USER CODE BEGIN RTOS_QUEUES */
  /* add queues, ... */
  /* USER CODE END RTOS_QUEUES */

  /* Create the thread(s) */
  /* creation of defaultTask */
  //defaultTaskHandle = osThreadNew(StartDefaultTask, NULL, &defaultTask_attributes);

  /* USER CODE BEGIN RTOS_THREADS */
  /* add threads, ... */
  
  /* 创建任务: 声 */
//  extern void PlayMusic(void *params);
//  xTaskCreate(PlayMusic, "MusicTask", 128, NULL, osPriorityNormal, NULL);
  xTaskCreate(game1_task, "GameTask", 128, NULL, osPriorityNormal, NULL);
  //reg = xTaskCreate(SoundTask, "Sound", 128, NULL, osPriorityNormal , &SoundStaticTask);
  
  /* 创建任务: 光 */
  //xLightTaskHandle = xTaskCreateStatic(Led_Test, "LightTask", 128, NULL, osPriorityNormal, g_pucStackOfLightTask, &g_TCBofLightTask);

  /* 创建任务: 色 */
  //xColorTaskHandle = xTaskCreateStatic(ColorLED_Test, "ColorTask", 128, NULL, osPriorityNormal, g_pucStackOfColorTask, &g_TCBofColorTask);

  /* USER CODE END RTOS_THREADS */

  /* USER CODE BEGIN RTOS_EVENTS */
  /* add events, ... */
  /* USER CODE END RTOS_EVENTS */

}

/* USER CODE BEGIN Header_StartDefaultTask */

/**
  * @brief  Function implementing the defaultTask thread.
  * @param  argument: Not used
  * @retval None
  */
/* USER CODE END Header_StartDefaultTask */
void StartDefaultTask(void *argument)
{
  /* USER CODE BEGIN StartDefaultTask */
  /* Infinite loop */
	vTaskDelay(20);
  /* USER CODE END StartDefaultTask */
}

/* Private application code --------------------------------------------------*/
/* USER CODE BEGIN Application */

/* USER CODE END Application */

