#pragma once

#define BOARD_ID 3     // 灯板id 1~5
#define BRIGHTNESS 2 // 亮度值，最高255，最低0(这个已经没有实际作用,实际亮度由遥控器vrb发送控制)

#define KEY_1_PORT  GPIOA
#define KEY_1_PIN   GPIO_PIN_2

#define KEY_2_PORT  GPIOA
#define KEY_2_PIN   GPIO_PIN_3

#define KEY_3_PORT  GPIOA
#define KEY_3_PIN   GPIO_PIN_4

#define KEY_4_PORT  GPIOA
#define KEY_4_PIN   GPIO_PIN_5

#define KEY_5_PORT  GPIOA
#define KEY_5_PIN   GPIO_PIN_6

#define KEY_6_PORT  GPIOA
#define KEY_6_PIN   GPIO_PIN_7

#define KEY_7_PORT  GPIOB
#define KEY_7_PIN   GPIO_PIN_0

#define KEY_8_PORT  GPIOB
#define KEY_8_PIN   GPIO_PIN_1

#define KEY_9_PORT  GPIOB
#define KEY_9_PIN   GPIO_PIN_10

#define BOARD_LIGHT_TIM htim5
#define BOARD_LIGHT_CHANNEL TIM_CHANNEL_1
#define BOARD_LIGHT_DMA hdma_tim5_ch1

#define FRAME_DOWN_LIGHT_TIM htim5
#define FRAME_DOWN_LIGHT_CHANNEL TIM_CHANNEL_2
#define FRAME_DOWN_LIGHT_DMA hdma_tim5_ch2

#define FRAME_LIGHT_TIM htim1
#define FRAME_LIGHT_CHANNEL TIM_CHANNEL_2
#define FRAME_LIGHT_DMA hdma_tim1_ch2

#define FRAME_UP_LIGHT_TIM htim3
#define FRAME_UP_LIGHT_CHANNEL TIM_CHANNEL_1
#define FRAME_UP_LIGHT_DMA hdma_tim3_ch1_trig

#define RUNNING_LIGHT_TIM htim1
#define RUNNING_LIGHT_CHANNEL TIM_CHANNEL_1
#define RUNNING_LIGHT_DMA hdma_tim1_ch1
