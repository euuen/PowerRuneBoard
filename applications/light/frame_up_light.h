#pragma once

#include <stdint.h>

#define FRAME_UP_LIGHT_LED_NUM 87

// 每颗 LED 占 24 个 uint16_t，尾部额外 10 颗用于复位信号
#define FRAME_UP_LIGHT_RESET_LED_NUM 10
#define FRAME_UP_LIGHT_FRAME_LED_NUM (FRAME_UP_LIGHT_LED_NUM + FRAME_UP_LIGHT_RESET_LED_NUM)

uint8_t FrameUpLight_IsReady(void);

void FrameUpLight_Clear(void);
void FrameUpLight_Draw_All(void);
void FrameUpLight_Draw_Strip(uint16_t start, uint16_t end);
