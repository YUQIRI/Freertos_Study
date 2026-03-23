// SPDX-License-Identifier: GPL-3.0-only
/*
 * Copyright (c) 2008-2023 100askTeam : Dongshan WEI <weidongshan@qq.com> 
 * Discourse:  https://forums.100ask.net
 */
 
/*  Copyright (C) 2008-2023 深圳百问网科技有限公司
 *  All rights reserved
 *
 * 免责声明: 百问网编写的文档, 仅供学员学习使用, 可以转发或引用(请保留作者信息),禁止用于商业用途！
 * 免责声明: 百问网编写的程序, 可以用于商业用途, 但百问网不承担任何后果！
 * 
 * 本程序遵循GPL V3协议, 请遵循协议
 * 百问网学习平台   : https://www.100ask.net
 * 百问网交流社区   : https://forums.100ask.net
 * 百问网官方B站    : https://space.bilibili.com/275908810
 * 本程序所用开发板 : DShanMCU-F103
 * 百问网官方淘宝   : https://100ask.taobao.com
 * 联系我们(E-mail): weidongshan@qq.com
 *
 *          版权所有，盗版必究。
 *  
 * 修改历史     版本号           作者        修改内容
 *-----------------------------------------------------
 * 2023.08.04      v01         百问科技      创建文件
 *-----------------------------------------------------
 */

#include "driver_passive_buzzer.h"
#include "driver_lcd.h"
#include "driver_timer.h"
#include "stm32f1xx_hal.h"
#include "tim.h"
#include "FreeRTOS.h"
#include "task.h"

/*
  Melody
  Plays a melody

  circuit:

  - 8 ohm speaker on digital pin 8

  created 21 Jan 2010

  modified 30 Aug 2011

  by Tom Igoe
  
  This example code is in the public domain.
  https://www.arduino.cc/en/Tutorial/Tone

  Another code referenced for dotted note logic by Robson Couto
  https://github.com/robsoncouto/arduino-songs
*/

/*
  MODIFIED

  BadAppleSEKAIver

  Github instruction: https://github.com/lenpai0/BadAppleSEKAIver_Buzzer_Cover_Code/

  Song: Bad Apple!!feat.SEKAI by Nightcord at 25:00 https://www.youtube.com/watch?v=v-fc1zv31zE
  Piano cover reference by @Sapphire_Titech used: https://www.youtube.com/watch?v=VVsgW76z00o

  This is a modified program from my most popular TikTok video. This code should work for any Arduino board.
  When uploaded, it will simply play the whole main melody of Bad Apple. 
  
  Press the RESET button on the arduino board to have the song play again. Enjoy!

  by Lenpai (YouTube: https://www.youtube.com/@lenpai0 | TikTok: https://www.tiktok.com/@lenpai0)

*/
extern TIM_HandleTypeDef htim1;

static TIM_HandleTypeDef *g_HPWM_PassiveBuzzer = &htim1;


#define REST      0

#define B0        31
#define C1        33
#define CS1       35
#define D1        37
#define DS1       39
#define E1        41
#define F1        44
#define FS1       46
#define G1        49
#define GS1       52
#define A1        55
#define AS1       58
#define B1        62
#define C2        65
#define CS2       69
#define D2        73
#define DS2       78
#define E2        82
#define F2        87
#define FS2       93
#define G2        98
#define GS2       104
#define A2        110
#define AS2       117
#define B2        123
#define C3        131
#define CS3       139
#define D3        147
#define DS3       156
#define E3        165
#define F3        175
#define FS3       185
#define G3        196
#define GS3       208
#define A3        220
#define AS3       233
#define B3        247
#define C4        262
#define CS4       277
#define D4        294
#define DS4       311
#define E4        330
#define F4        349
#define FS4       370
#define G4        392
#define GS4       415
#define A4        440
#define AS4       466
#define B4        494
#define C5        523
#define CS5       554
#define D5        587
#define DS5       622
#define E5        659
#define F5        698
#define FS5       740
#define G5        784
#define GS5       831
#define A5        880
#define AS5       932
#define B5        988
#define C6        1047
#define CS6       1109
#define D6        1175
#define DS6       1245
#define E6        1319
#define F6        1397
#define FS6       1480
#define G6        1568
#define GS6       1661
#define A6        1760
#define AS6       1865
#define B6        1976
#define C7        2093
#define CS7       2217
#define D7        2349
#define DS7       2489
#define E7        2637
#define F7        2794
#define FS7       2960
#define G7        3136
#define GS7       3322
#define A7        3520
#define AS7       3729
#define B7        3951
#define C8        4186
#define CS8       4435
#define D8        4699
#define DS8       4978

const uint16_t BadApple_Melody[] = {
  C4, D4, DS4, F4, G4, REST, C5, AS4, G4, C4, G4, F4, DS4, D4, C4, D4, DS4, F4, G4,
  F4, DS4, D4, C4, D4, DS4, D4, C4, B3, D4, C4, D4, DS4, F4, G4, REST, C5, AS4, G4, C4, G4,
  F4, DS4, D4, C4, D4, DS4, F4, G4, F4, DS4, D4, REST, DS4, F4, REST, G4, C5, D5, DS5, F5,
  G5, C6, AS5, G5, C5, G5, F5, DS5, D5, C5, D5, DS5, F5, G5, F5, DS5, D5, C5, D5, DS5, D5,
  C5, B4, D5, C5, D5, DS5, F5, G5, C6, AS5, G5, C5, G5, F5, DS5, D5, C5, D5, DS5, F5, G5,
  F5, DS5, D5, REST, DS5, F5, G5,

  AS5, C6, G5, F5, G5, F5, G5, AS5, C6, G5, F5, G5, F5, G5, F5, DS5, D5, AS4, C5, AS4, C5, D5, DS5, F5, G5, C5,
  G5, AS5, AS5, C6, G5, F5, G5, F5, G5, AS5, C6, G5, F5, G5, F5, G5, F5, DS5, D5, AS4, C5, AS4, C5, D5, DS5, F5, G5, C5,
  G5, AS5, AS5, C6, G5, F5, G5, F5, G5, AS5, C6, G5, F5, G5, F5, G5, F5, DS5, D5, AS4, C5, AS4, C5, D5, DS5, F5, G5, C5,
  G5, AS5, AS5, C6, G5, F5, G5, F5, G5, AS5, C6, G5, F5, G5, C6, D6, DS6, D6, C6, AS5, G5, F5, G5, F5, DS5, D5, AS4, C5,
  G5, AS5, AS5, C6, G5, F5, G5, F5, G5, AS5, C6, G5, F5, G5, F5, G5, F5, DS5, D5, AS4, C5, AS4, C5, D5, DS5, F5, G5, C5,
  G5, AS5, AS5, C6, G5, F5, G5, F5, G5, AS5, C6, G5, F5, G5, F5, G5, F5, DS5, D5, AS4, C5, AS4, C5, D5, DS5, F5, G5, C5,
  G5, AS5, AS5, C6, G5, F5, G5, F5, G5, AS5, C6, G5, F5, G5, F5, G5, F5, DS5, D5, AS4, C5, AS4, C5, D5, DS5, F5, G5, C5,
  G5, AS5, AS5, C6, G5, F5, G5, F5, G5, AS5, C6, G5, F5, G5, C6, D6, DS6, D6, C6, AS5, G5, F5, G5, F5, DS5, D5, AS4, C5,
  REST,

  C6, AS5, C6, C6, AS5, C6, C6, AS5, C6, C6, AS5, C6, AS5, C6, AS5, C6, AS5, C6, DS6, C6,
  AS5, C6, C6, C6, REST, C4, DS4, F4, G4, FS4, G4, FS4, FS4, F4, F4, DS4,
  AS3, REST, DS4, REST, C4, C5,
  C6, C5, AS5, C5, G5, C5, FS5, C5, F5, C5, DS5,
  AS4, C5, DS5, DS5, C5, AS4, DS5, DS5, C5, AS4, DS5, F5, REST, DS5, AS4,

  C5, G4, AS4, C5, DS5, G4, AS4, C5, DS5, F5, AS4, C5, DS5, F5, G5, C5, DS5, F5, G5,
  AS5, C5, DS5, F5, AS5, C6, F5, G5, AS5, C6, DS6, G5, AS5, C6, DS6, G6, C7, DS7, G7,
  REST,

  C4, D4, DS4, F4, G4, REST, C5, AS4, G4, C4, G4, F4, DS4, D4, C4, D4, DS4, F4, G4,
  F4, DS4, D4, C4, D4, DS4, D4, C4, B3, D4, C4, D4, DS4, F4, G4, REST, C5, AS4, G4, C4, G4,
  F4, DS4, D4, C4, D4, DS4, F4, G4, F4, DS4, D4, REST, DS4, F4, REST, G4, C5, D5, DS5, F5,
  G5, C6, AS5, G5, C5, G5, F5, DS5, D5, C5, D5, DS5, F5, G5, F5, DS5, D5, C5, D5, DS5, D5,
  C5, B4, D5, C5, D5, DS5, F5, G5, C6, AS5, G5, C5, G5, F5, DS5, D5, C5, D5, DS5, F5, G5,
  F5, DS5, D5, REST, DS5, F5, G5,

  AS5, C6, G5, F5, G5, F5, G5, AS5, C6, G5, F5, G5, F5, G5, F5, DS5, D5, AS4, C5, AS4, C5, D5, DS5, F5, G5, C5,
  G5, AS5, AS5, C6, G5, F5, G5, F5, G5, AS5, C6, G5, F5, G5, F5, G5, F5, DS5, D5, AS4, C5, AS4, C5, D5, DS5, F5, G5, C5,
  G5, AS5, AS5, C6, G5, F5, G5, F5, G5, AS5, C6, G5, F5, G5, F5, G5, F5, DS5, D5, AS4, C5, AS4, C5, D5, DS5, F5, G5, C5,
  G5, AS5, AS5, C6, G5, F5, G5, F5, G5, AS5, C6, G5, F5, G5, C6, D6, DS6, D6, C6, AS5, G5, F5, G5, F5, DS5, D5, AS4, C5,

  GS5, B5, B5, CS6, GS5, FS5, GS5, FS5, GS5, B5, CS6, GS5, FS5, GS5, FS5, GS5, FS5, E5, DS5, B4, CS5, B4, CS5, DS5, E5, FS5, GS5, CS5,
  GS5, B5, B5, CS6, GS5, FS5, GS5, FS5, GS5, B5, CS6, GS5, FS5, GS5, FS5, GS5, FS5, E5, DS5, B4, CS5, B4, CS5, DS5, E5, FS5, GS5, CS5,
  GS5, B5, B5, CS6, GS5, FS5, GS5, FS5, GS5, B5, CS6, GS5, FS5, GS5, FS5, GS5, FS5, E5, DS5, B4, CS5, B4, CS5, DS5, E5, FS5, GS5, CS5,
  GS5, B5, B5, CS6, GS5, FS5, GS5, FS5, GS5, B5, CS6, GS5, FS5, GS5, CS6, DS6, E6, DS6, CS6, B5, GS5, FS5, GS5, FS5, E5, DS5, B4, CS5,

  B5, CS6, GS5, FS5, GS5, FS5, GS5, FS5, REST, B5, DS6, FS6, CS6, DS6, E6, DS6, CS6, B5, GS5, FS5, GS5, FS5, B5,
  E7, DS7, CS7, B6, GS6, B6, FS6, B5, CS6, GS5, FS5, GS5, FS5, GS5, B5, CS6, GS5, FS5, GS5, CS6, DS6, E6, DS6, CS6, B5, GS5,
  FS5, GS5, FS5, GS5, FS5, GS5, E6, DS6, REST, B5, E6, B6, REST, B6, E7, FS7, REST
};

const int8_t BadApple_Rhythm[] = {
  8, 8, 8, 8, -8, 16, 8, 8, 4, 4, 8, 8, 8, 8, 8, 8, 8, 8, 4,
  8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, -8, 16, 8, 8, 4, 4, 8, 8, 8, 8, 8, 8, 8, 8, 4,
  8, 8, 8, 8, 4, 8, 8, 4, 8, 8, 8, 8, 4, 8, 8, 4, 4, 8, 8, 8, 8, 8, 8, 8, 8, 4, 8, 8, 8, 8,
  8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 4, 8, 8, 4, 4, 8, 8, 8, 8, 8, 8, 8, 8, 4, 8, 8, 8, 8, 4, 4, 4,

  8, 8, 8, 8, 4, 8, 8, 8, 8, 8, 8, 4, 8, 8, 8, 8, 8, 8, 4, 8, 8, 8, 8, 8, 8, 4,
  8, 8, 8, 8, 8, 8, 4, 8, 8, 8, 8, 8, 8, 4, 8, 8, 8, 8, 8, 8, 4, 8, 8, 8, 8, 8, 8, 4,
  8, 8, 8, 8, 8, 8, 4, 8, 8, 8, 8, 8, 8, 4, 8, 8, 8, 8, 8, 8, 4, 8, 8, 8, 8, 8, 8, 4,
  8, 8, 8, 8, 8, 8, 4, 8, 8, 8, 8, 8, 8, 4, 8, 8, 8, 8, 8, 8, 4, 8, 8, 8, 8, 8, 8, 4,
  8, 8, 8, 8, 8, 8, 4, 8, 8, 8, 8, 8, 8, 4, 8, 8, 8, 8, 8, 8, 4, 8, 8, 8, 8, 8, 8, 4,
  8, 8, 8, 8, 8, 8, 4, 8, 8, 8, 8, 8, 8, 4, 8, 8, 8, 8, 8, 8, 4, 8, 8, 8, 8, 8, 8, 4,
  8, 8, 8, 8, 8, 8, 4, 8, 8, 8, 8, 8, 8, 4, 8, 8, 8, 8, 8, 8, 4, 8, 8, 8, 8, 8, 8, 4,
  8, 8, 8, 8, 8, 8, 4, 8, 8, 8, 8, 8, 8, 4, 8, 8, 8, 8, 8, 8, 4, 8, 8, 8, 8, 8, 8, -4,
  4,

  8, 16, 8, 8, 16, 16, 8, 16, 16, 8, 16, 16, 16, 16, 16, 16, 16, 8, 8, 16,
  8, 8, 16, 8, 16, 16, 16, 16, 4, 8, 8, 16, 8, 16, 8, 8,
  -16, -16, -16, -16, -8, 16,
  16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 8,
  16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 8, 16, 16, 8, 8,

  8, 16, 16, -16, 16, 32, 32, 32, 16, 16, 32, 32, 32, 16, 16, 32, 32, 32, 16, 16, 32, 32,
  32, 16, 16, 32, 32, 32, 16, 32, 32, 32, 32, 16, 32, 32, 32, -16, -8,
  REST,

  8, 8, 8, 8, -8, 16, 8, 8, 4, 4, 8, 8, 8, 8, 8, 8, 8, 8, 4,
  8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, -8, 16, 8, 8, 4, 4, 8, 8, 8, 8, 8, 8, 8, 8, 4,
  8, 8, 8, 8, 4, 8, 8, 4, 8, 8, 8, 8, 4, 8, 8, 4, 4, 8, 8, 8, 8, 8, 8, 8, 8, 4, 8, 8, 8, 8,
  8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 4, 8, 8, 4, 4, 8, 8, 8, 8, 8, 8, 8, 8, 4, 8, 8, 8, 8, 4, 4, 4,

  8, 8, 8, 8, 4, 8, 8, 8, 8, 8, 8, 4, 8, 8, 8, 8, 8, 8, 4, 8, 8, 8, 8, 8, 8, 4,
  8, 8, 8, 8, 8, 8, 4, 8, 8, 8, 8, 8, 8, 4, 8, 8, 8, 8, 8, 8, 4, 8, 8, 8, 8, 8, 8, 4,
  8, 8, 8, 8, 8, 8, 4, 8, 8, 8, 8, 8, 8, 4, 8, 8, 8, 8, 8, 8, 4, 8, 8, 8, 8, 8, 8, 4,
  8, 8, 8, 8, 8, 8, 4, 8, 8, 8, 8, 8, 8, 4, 8, 8, 8, 8, 8, 8, 4, 8, 8, 8, 8, 8, 8, 4,

  8, 8, 8, 8, 8, 8, 4, 8, 8, 8, 8, 8, 8, 4, 8, 8, 8, 8, 8, 8, 4, 8, 8, 8, 8, 8, 8, 4,
  8, 8, 8, 8, 8, 8, 4, 8, 8, 8, 8, 8, 8, 4, 8, 8, 8, 8, 8, 8, 4, 8, 8, 8, 8, 8, 8, 4,
  8, 8, 8, 8, 8, 8, 4, 8, 8, 8, 8, 8, 8, 4, 8, 8, 8, 8, 8, 8, 4, 8, 8, 8, 8, 8, 8, 4,
  8,
  8, 8, 8, 8, 8, 4, 8, 8, 8, 8, 8, 8, 4, 8, 8, 8, 8, 8, 8, 4, 8, 8, 8, 8, 8, 8, 2,

  8, 8, 8, 8, 4, 8, 8, -4, 16, 32, 32, 4, 8, 8, 8, 8, 8, 8, 4, 8, 8, 4, 4,
  16, 16, 16, 16, 16, 16, 8, 8, 8, 8, 8, 4, 8, 8, 8, 8, 8, 8, 4, 8, 8, 8, 8, 8, 8, 4,
  8, 8, 8, 8, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, -4, 2
};

#define MELODY_LEN  (sizeof(BadApple_Melody)/sizeof(BadApple_Melody[0]))
#define TEMPO       138
#define WHOLE_NOTE  ((60000*4)/TEMPO)

void Buzzer_Play(uint16_t freq, uint32_t time_ms)
{
  if(freq == REST){
    vTaskDelay(time_ms);
    return;
  }

  uint32_t arr = 1000000 / freq - 1;
  __HAL_TIM_SET_AUTORELOAD(&htim1, arr);
  __HAL_TIM_SET_COMPARE(&htim1, TIM_CHANNEL_1, arr/2);
  HAL_TIM_PWM_Start(&htim1, TIM_CHANNEL_1);

  vTaskDelay(time_ms);

  HAL_TIM_PWM_Stop(&htim1, TIM_CHANNEL_1);
}

void BadApple_Play(void)
{
  int note_duration;

  for(int i=0; i<MELODY_LEN; i++){
    uint16_t note = BadApple_Melody[i];
    int8_t rhythm = BadApple_Rhythm[i];

    if(rhythm > 0){
      note_duration = WHOLE_NOTE / rhythm;
    }else{
      note_duration = (WHOLE_NOTE / (-rhythm)) * 1.5;
    }

    Buzzer_Play(note, note_duration * 0.9);
    vTaskDelay(note_duration * 0.1);
  }
}

/**********************************************************************
 * 函数名称： PassiveBuzzer_Init
 * 功能描述： 无源蜂鸣器的初始化函数
 * 输入参数： 无
 * 输出参数： 无
 * 返 回 值： 无
 * 修改日期：      版本号     修改人	      修改内容
 * -----------------------------------------------
 * 2023/08/04	     V1.0	  韦东山	      创建
 ***********************************************************************/
void PassiveBuzzer_Init(void)
{
    TIM_OC_InitTypeDef sConfig = { 0 };
    GPIO_InitTypeDef GPIO_InitStruct = {0};

    /**TIM1 GPIO Configuration
    PA8     ------> TIM1_CH1
    */
    GPIO_InitStruct.Pin = GPIO_PIN_8;
    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
    HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);
    
    //HAL_TIM_Base_DeInit(g_HPWM_PassiveBuzzer);
    g_HPWM_PassiveBuzzer->Instance = TIM1;
    g_HPWM_PassiveBuzzer->Init.Prescaler = 71;
    g_HPWM_PassiveBuzzer->Init.CounterMode = TIM_COUNTERMODE_UP;
    g_HPWM_PassiveBuzzer->Init.Period = 999;  /* 1KHz */
    g_HPWM_PassiveBuzzer->Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
    g_HPWM_PassiveBuzzer->Init.RepetitionCounter = 0;
    g_HPWM_PassiveBuzzer->Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_ENABLE;
    //HAL_TIM_Base_Init(g_HPWM_PassiveBuzzer);
    TIM_Base_SetConfig(g_HPWM_PassiveBuzzer->Instance, &g_HPWM_PassiveBuzzer->Init);
    //return;


    sConfig.OCMode = TIM_OCMODE_PWM1;        // PWM 输出的两种模式:PWM1 当极性为低,CCR<CNT,输出低电平,反之高电平
    sConfig.OCPolarity = TIM_OCPOLARITY_LOW; // 设置极性为低(硬件上低电平亮灯)
    sConfig.OCFastMode = TIM_OCFAST_DISABLE; // 输出比较快速使能禁止(仅在 PWM1 和 PWM2 可设置)
    sConfig.Pulse      = 499;                // 在 PWM1 模式下,50%占空比
    
    HAL_TIM_PWM_ConfigChannel(g_HPWM_PassiveBuzzer, &sConfig, TIM_CHANNEL_1);
}


/**********************************************************************
 * 函数名称： PassiveBuzzer_Control
 * 功能描述： 无源蜂鸣器控制函数
 * 输入参数： on - 1-响, 0-不响
 * 输出参数： 无
 * 返 回 值： 无
 * 修改日期：      版本号     修改人	      修改内容
 * -----------------------------------------------
 * 2023/08/04	     V1.0	  韦东山	      创建
 ***********************************************************************/
void PassiveBuzzer_Control(int on)
{
    if (on)
    {
        HAL_TIM_PWM_Start(g_HPWM_PassiveBuzzer, TIM_CHANNEL_1);
    }        
    else
    {
        HAL_TIM_PWM_Stop(g_HPWM_PassiveBuzzer, TIM_CHANNEL_1);
    }
}

/**********************************************************************
 * 函数名称： PassiveBuzzer_Set_Freq_Duty
 * 功能描述： 无源蜂鸣器控制函数: 设置频率和占空比
 * 输入参数： freq - 频率, duty - 占空比
 * 输出参数： 无
 * 返 回 值： 无
 * 修改日期：      版本号     修改人	      修改内容
 * -----------------------------------------------
 * 2023/08/04	     V1.0	  韦东山	      创建
 ***********************************************************************/
void PassiveBuzzer_Set_Freq_Duty(int freq, int duty)
{
    TIM_OC_InitTypeDef sConfig = { 0 };

    HAL_TIM_PWM_Stop(g_HPWM_PassiveBuzzer, TIM_CHANNEL_1);

    HAL_TIM_Base_DeInit(g_HPWM_PassiveBuzzer);
    g_HPWM_PassiveBuzzer->Instance = TIM1;
    g_HPWM_PassiveBuzzer->Init.Prescaler = 71;
    g_HPWM_PassiveBuzzer->Init.CounterMode = TIM_COUNTERMODE_UP;
    g_HPWM_PassiveBuzzer->Init.Period = 1000000 / freq - 1;  
    g_HPWM_PassiveBuzzer->Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
    g_HPWM_PassiveBuzzer->Init.RepetitionCounter = 0;
    g_HPWM_PassiveBuzzer->Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_ENABLE;
    HAL_TIM_Base_Init(g_HPWM_PassiveBuzzer);


    sConfig.OCMode = TIM_OCMODE_PWM1;        // PWM 输出的两种模式:PWM1 当极性为低,CCR<CNT,输出低电平,反之高电平
    sConfig.OCPolarity = TIM_OCPOLARITY_LOW; // 设置极性为低(硬件上低电平亮灯)
    sConfig.OCFastMode = TIM_OCFAST_DISABLE; // 输出比较快速使能禁止(仅在 PWM1 和 PWM2 可设置)
    sConfig.Pulse      = (1000000 / freq - 1) * duty / 100;                // 在 PWM1 模式占空比
    
    HAL_TIM_PWM_ConfigChannel(g_HPWM_PassiveBuzzer, &sConfig, TIM_CHANNEL_1);

    HAL_TIM_PWM_Start(g_HPWM_PassiveBuzzer, TIM_CHANNEL_1);
}


/**********************************************************************
 * 函数名称： PassiveBuzzer_Test
 * 功能描述： 无源蜂鸣器测试程序
 * 输入参数： 无
 * 输出参数： 无
 *            无
 * 返 回 值： 无
 * 修改日期        版本号     修改人        修改内容
 * -----------------------------------------------
 * 2023/08/04        V1.0     韦东山       创建
 ***********************************************************************/
void PassiveBuzzer_Test(void)
{
    PassiveBuzzer_Init();
	LCD_Init();
	LCD_Clear();

    while (1)
    {
        LCD_PrintString(0, 0, "PassiveBuzzer");
        LCD_PrintString(0, 2, "ON ");
        PassiveBuzzer_Control(1);
        mdelay(1000);

        LCD_PrintString(0, 0, "PassiveBuzzer");
        LCD_PrintString(0, 2, "OFF");
        PassiveBuzzer_Control(0);
        mdelay(1000);
    }
}

