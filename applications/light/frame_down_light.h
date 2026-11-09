#pragma once

#include <stdint.h>

#define FRAME_DOWN_LIGHT_LED_NUM 93

// 每颗 LED 占 24 个 uint16_t，尾部额外 10 颗用于复位信号
#define FRAME_DOWN_LIGHT_RESET_LED_NUM 10
#define FRAME_DOWN_LIGHT_FRAME_LED_NUM (FRAME_DOWN_LIGHT_LED_NUM + FRAME_DOWN_LIGHT_RESET_LED_NUM)

uint8_t FrameDownLight_IsReady(void);

void FrameDownLight_Clear(void);
void FrameDownLight_Draw_All(void);
void FrameDownLight_Draw_Strip(uint16_t start, uint16_t end);
