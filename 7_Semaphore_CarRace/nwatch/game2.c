/*
 * Project: N|Watch
 * Author: Zak Kemble, contact@zakkemble.co.uk
 * Copyright: (C) 2013 by Zak Kemble
 * License: GNU GPL v3 (see License.txt)
 * Web: http://blog.zakkemble.co.uk/diy-digital-wristwatch/
 */
#include <stdlib.h>
#include <stdio.h>

#include "cmsis_os.h"
#include "FreeRTOS.h"                   // ARM.FreeRTOS::RTOS:Core
#include "task.h"                       // ARM.FreeRTOS::RTOS:Core
#include "event_groups.h"               // ARM.FreeRTOS::RTOS:Event Groups
#include "semphr.h"                     // ARM.FreeRTOS::RTOS:Core

#include "draw.h"
#include "resources.h"

#include "driver_lcd.h"
#include "driver_bluetooth.h"
#include "driver_mpu6050.h"

#include <string.h>
#include <sys/_intsup.h>

#define NOINVERT	false
#define INVERT		true

#define sprintf_P  sprintf
#define PSTR(a)  a

#define CAR_MOVE_NONE 0
#define CAR1MOVE	1
#define CAR2MOVE	2
#define CAR3MOVE	3

#define CARLENTH 15
#define CARHEIGHT 16
#define CARSPEED 6

static const byte roadMarking[] ={
	0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01,
};

static const byte carImg[] ={
	0x40,0xF8,0xEC,0x2C,0x2C,0x38,0xF0,0x10,0xD0,0x30,0xE8,0x4C,0x4C,0x9C,0xF0,
	0x02,0x1F,0x37,0x34,0x34,0x1C,0x0F,0x08,0x0B,0x0C,0x17,0x32,0x32,0x39,0x0F,
};

static const byte clearImg[30] ={
	0
};

struct car_data {
	int x;
	int y;
	int ControlKey;
};

struct car_data g_cars[3] = {
	{0, 0, BT_KEY_1},
	{0, 17, BT_KEY_2},	
	{0, 34, BT_KEY_3},
};



static uint32_t g_xres, g_yres, g_bpp;
static uint8_t *g_framebuffer;

static void ShowCar(struct car_data *car)
{
	draw_bitmap(car->x, car->y, carImg, 15, 16, NOINVERT, 0);
	draw_flushArea(car->x, car->y, 15, 16);
}
static void HideCar(struct car_data *car)
{
	draw_bitmap(car->x, car->y, clearImg, 15, 16, NOINVERT, 0);
	draw_flushArea(car->x, car->y, 15, 16);
}
static void car_task(void *params)
{
	struct car_data *car = params;
	struct bt_data idata;
	//创建队列
	QueueHandle_t g_xQueueCar = xQueueCreate(10, sizeof(struct bt_data));
	//注册队列
	RegisterQueueHandle(g_xQueueCar);

	ShowCar(car);
    
	while (1)
	{
		/* 读取汽车队列 */
		xQueueReceive(g_xQueueCar, &idata, portMAX_DELAY);
			 	
			// Move cars
			if(idata.val == car->ControlKey)
			{
				// Hide cars
				HideCar(car);
				if(car->x < g_xres - CARLENTH)
				{
					car->x += CARSPEED;
					if(car->x > g_xres - CARLENTH)
						car->x = g_xres - CARLENTH;
				}
				// Draw cars
				ShowCar(car);
			}
	
		vTaskDelay(50);
	}
}

void car_game(void)
{
	int i,j;
   
    g_framebuffer = LCD_GetFrameBuffer(&g_xres, &g_yres, &g_bpp);
    draw_init();
    draw_end();
	//画道路
	for(i = 0; i < 3; i++)
	{
		for(j = 0; j < 8; j++)
		{
			draw_bitmap(16*j, 16+i*17, roadMarking, 8, 1, NOINVERT, 0);
			draw_flushArea(16*j, 16+i*17, 8, 1);
		}
	}
	
	//创建任务
	xTaskCreate(car_task, "car1_task", 128 , &g_cars[0], osPriorityNormal + 1 , NULL);
	xTaskCreate(car_task, "car2_task", 128 , &g_cars[1], osPriorityNormal + 1 , NULL);
	xTaskCreate(car_task, "car3_task", 128 , &g_cars[2], osPriorityNormal + 1 , NULL);

}