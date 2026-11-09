#pragma once

#include <stdint.h>

// 28个LED

#define FRAME_LIGHT_LED_NUM 32

// 每颗 LED 占 24 个 uint16_t，尾部额外 10 颗用于复位信号
#define FRAME_LIGHT_RESET_LED_NUM 10
#define FRAME_LIGHT_FRAME_LED_NUM (FRAME_LIGHT_LED_NUM + FRAME_LIGHT_RESET_LED_NUM)

uint8_t FrameLight_IsReady(void);

void FrameLight_Clear(void);
void FrameLight_Draw_All(void);
void FrameLight_Draw_Strip(uint16_t start, uint16_t end);
