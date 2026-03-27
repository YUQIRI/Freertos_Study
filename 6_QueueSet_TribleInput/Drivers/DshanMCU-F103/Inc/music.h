#ifndef __MUSIC_H
#define __MUSIC_H

#include "FreeRTOS.h"                   // ARM.FreeRTOS::RTOS:Core
#include "task.h"                       // ARM.FreeRTOS::RTOS:Core
#include "driver_passive_buzzer.h"
#include "driver_timer.h"
#include "stm32f1xx_hal.h"
#include "tim.h"
#include "cmsis_os2.h"

#include "typedefs.h"
#include "driver_bluetooth.h"
#include "queue.h"

void Buzzer_Play(uint16_t freq, uint32_t time_ms);
void BadApple_Play(void);
void SoundTask(void *argument);

#endif