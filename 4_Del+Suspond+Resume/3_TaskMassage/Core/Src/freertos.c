/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * File Name          : freertos.c
  * Description        : Code for freertos applications
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2026 STMicroelectronics.
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
#include "usart.h"
#include "driver_led.h"
#include "driver_passive_buzzer.h"
#include "driver_lcd.h"
#include "driver_timer.h"
#include "driver_color_led.h"

#include "driver_oled.h"

#include <stdint.h>
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

static uint8_t rxData; 
static uint8_t rxMusicData; 

// 串口接收完成回调函数
void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
{
    if(huart->Instance == USART1)  // 判断是哪个串口
    {
        // 在这里处理收到的数据
          rxMusicData = rxData;
          rxData = 0x00;
        // 重要！如果要继续接收，必须重新开启
        HAL_UART_Receive_IT(&huart1, &rxData, 1);
    }
}
/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
/* USER CODE BEGIN Variables */
static TaskHandle_t  SoundStaticTask ;
static TaskHandle_t  SoundInsideTask = NULL;
static UBaseType_t reg;

static StackType_t  LightStaticBuffer[128];
static StaticTask_t   LightTaskBuffer;
static TaskHandle_t LightStaticTask;

static StackType_t  ColorStaticBuffer[128];
static StaticTask_t   ColorTaskBuffer;
static TaskHandle_t ColorStaticTask;



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
  HAL_UART_Receive_IT(&huart1, &rxData, 1);
  LCD_Clear();
  LCD_PrintString(6, 0, "Ready");

  while(1)
  {
    /* 启动/暂停 信号 */
    if(0x01 == rxMusicData)
    {
      rxMusicData = 0x00;
      /* 创建任务 */
      extern void MusicTask(void *argument);

      if(SoundInsideTask == NULL)
      {
        /* 创建任务 */

        LCD_Clear();
        LCD_PrintString(0, 0, "Creat Task");

        reg = xTaskCreate(MusicTask, "BadApple", 128, NULL, osPriorityNormal + 1 , &SoundInsideTask);
    
      }
      else 
      {
        if(Running)
        {
          LCD_Clear();
          LCD_PrintString(0, 0, "Suspend Task");

          vTaskSuspend(SoundInsideTask);
          PassiveBuzzer_Control(0);
          Running = 0;
        }
        else 
        {
          LCD_Clear();
          LCD_PrintString(0, 0, "Resume Task");

          vTaskResume(SoundInsideTask);
          Running = 1;
        }
      }
    }
    /* 删除信号 */
    if(0x02 == rxMusicData)
    {
      rxMusicData = 0x00;

      if(SoundInsideTask != NULL)
      {
        LCD_Clear();
        LCD_PrintString(0, 0, "Delete Task");

        vTaskDelete(SoundInsideTask);
        SoundInsideTask = NULL;   
        PassiveBuzzer_Control(0);
      }
    }
   vTaskDelay(50);
  }

}

// void LedTask(void *argument)
// {
//   while(1)
//   {
//     Led_Test();
//   }
//   vTaskDelay(50);
// }


void LightTask(void *argument)
{
  while(1)
  {
    Led_Test();
     vTaskDelay(50);
  }
 
}

void ColorTask(void *argument)
{
  while(1)
  {
    ColorLED_Test();
    vTaskDelay(50);
  }

}

/* 多任务调用同一函数 */
// struct Task_Record{
//   uint8_t x;
//   uint8_t y;
//   char Task[16];
// };

// static struct Task_Record Task1Info = {0, 0, "Task1"};
// static struct Task_Record Task2Info = {0, 3, "Task2"};
// static struct Task_Record Task3Info = {0, 6, "Task3"};

// static uint8_t LcdCanUse = 1;

// void RecordTask(void *params)
// {
//   LCD_Init();
//   LCD_Clear();
//   struct Task_Record *Info = params;
//   uint32_t cnt = 0;
//   int len ;
//   while(1)
//   {
//     if(LcdCanUse)
//     {
//       // LcdCanUse = 0;
//       len = LCD_PrintString(Info->x, Info->y, Info->Task);
//       len += LCD_PrintString(len,Info->y,":");
//       LCD_PrintSignedVal(len, Info-> y, cnt++);
//       // LcdCanUse = 1;
//     }

//     vTaskDelay(50);
//   }
// }

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
  // defaultTaskHandle = osThreadNew(StartDefaultTask, NULL, &defaultTask_attributes);

  /* USER CODE BEGIN RTOS_THREADS */
  /* add threads, ... */
  reg = xTaskCreate(SoundTask, "Sound", 128*4, NULL, osPriorityNormal , &SoundStaticTask);
  
  // reg = xTaskCreate(LedTask, "Led", 128, NULL, osPriorityNormal , NULL);
  
  LightStaticTask = xTaskCreateStatic(LightTask, "Light", 128, NULL,  osPriorityNormal, LightStaticBuffer, &LightTaskBuffer);
 
  ColorStaticTask = xTaskCreateStatic(ColorTask, "Color", 128, NULL, osPriorityNormal, ColorStaticBuffer , &ColorTaskBuffer);
 
  // xTaskCreate(RecordTask, "Task1", 128, &Task1Info, osPriorityNormal, NULL);
  // xTaskCreate(RecordTask, "Task2", 128, &Task2Info, osPriorityNormal, NULL);
  // xTaskCreate(RecordTask, "Task3", 128, &Task3Info, osPriorityNormal, NULL);

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
  for(;;)
  {
    OLED_Test();
    vTaskDelay(100);
  }
  /* USER CODE END StartDefaultTask */
}

/* Private application code --------------------------------------------------*/
/* USER CODE BEGIN Application */

/* USER CODE END Application */

