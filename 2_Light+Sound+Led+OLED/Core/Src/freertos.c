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
#include "driver_led.h"
#include "driver_passive_buzzer.h"
#include "driver_lcd.h"
#include "driver_timer.h"
#include "driver_color_led.h"

#include "driver_oled.h"

/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
/* USER CODE BEGIN Variables */

static TaskHandle_t  SoundStaticTask;
static UBaseType_t reg;

static StackType_t  LightStaticBuffer[128];
static StaticTask_t   LightTaskBuffer;
static TaskHandle_t LightStaticTask;

static StackType_t  ColorStaticBuffer[128];
static StaticTask_t  ColorTaskBuffer;
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
void SoundTask(void *argument)
{

  PassiveBuzzer_Init();
  while(1)
  {
    BadApple_Play();
    vTaskDelay(50);
  }
  
}

void LedTask(void *argument)
{
  while(1)
  {
    Led_Test();
  }
  vTaskDelay(50); 
}

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
    
  }
    vTaskDelay(50);
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
  defaultTaskHandle = osThreadNew(StartDefaultTask, NULL, &defaultTask_attributes);

  /* USER CODE BEGIN RTOS_THREADS */
  /* add threads, ... */

  reg = xTaskCreate(SoundTask, "Sound", 128*4, NULL, osPriorityNormal , &SoundStaticTask);
  
  reg = xTaskCreate(LedTask, "Led", 128, NULL, osPriorityNormal , NULL);
  
  LightStaticTask = xTaskCreateStatic(LightTask, "Light", 128, NULL,  osPriorityNormal, LightStaticBuffer, &LightTaskBuffer);
 
  ColorStaticTask = xTaskCreateStatic(ColorTask, "Color", 128, NULL, osPriorityNormal, ColorStaticBuffer , &ColorTaskBuffer);
  // /* USER CODE END RTOS_THREADS */

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

