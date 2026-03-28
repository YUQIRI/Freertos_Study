#include "music.h"
#include "driver_bluetooth.h"

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
 * �������ƣ� Music��SoundTask
 * ���������� ���ֲ��ź���
 * 2026/03/23	     V1.0	  ������	      ����
 ***********************************************************************/
static TaskHandle_t  SoundInsideTask = NULL;

QueueHandle_t g_xQueueMusic; /* ���ֶ��� */

static void MusicTask(void *argument)
{
  while(1)
  {
    BadApple_Play();
    vTaskDelay(50);
  }
 
}


void SoundTask(void *argument)
{
  /* 栈溢出检查 */
  #if (configCHECK_FOR_STACK_OVERFLOW > 0)
  UBaseType_t uxHighWaterMark;
  #endif

  PassiveBuzzer_Init();
  uint8_t Running = 1;
 //
  g_xQueueMusic = xQueueCreate(10, sizeof(struct music_data));
    
  struct music_data bdata;
//  struct input_data bdata;
   uint8_t  b_data, b_last_data;

// HAL_UART_Receive_IT(&huart1, &rxData, 1);
//  LCD_Clear();
//  LCD_PrintString(6, 0, "Ready");

  while(1)
  {

  		if (pdPASS == xQueueReceive(g_xQueueMusic, &bdata, portMAX_DELAY))
		{
		  b_data = bdata.val;         
            if (b_data == BT_MUSIC_SET) /* ��ͣ/���� */
            {
				b_data = BT_KEY_REPEAT;

				  if(SoundInsideTask == NULL)
				  {
					/* �������� */

//			        LCD_Clear();
//			        LCD_PrintString(0, 0, "Creat Task");

						xTaskCreate(MusicTask, "BadApple", 128, NULL, osPriorityNormal + 1 , &SoundInsideTask);
					
					  }
				  else 
				  {
						if(Running)
						{
//				          LCD_Clear();
//				          LCD_PrintString(0, 0, "Suspend Task");

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
            if (b_data == BT_MUSIC_DEL)  /* ɾ */
            {
				b_data = BT_KEY_REPEAT;
                if(SoundInsideTask != NULL)
				  {
			//        LCD_Clear();
			//        LCD_PrintString(0, 0, "Delete Task");

					vTaskDelete(SoundInsideTask);
					SoundInsideTask = NULL;   
					PassiveBuzzer_Control(0);
				  }
			
            }
       

			}
            

//    /* ����/��ͣ �ź� */
//    if(0x01 == rxMusicData)
//    {
//      rxMusicData = 0x00;
//      /* �������� */
//     
//      
//    }
//    /* ɾ���ź� */
//    if(0x02 == rxMusicData)
//    {
//      rxMusicData = 0x00;

      vTaskDelay(50);
  }

}