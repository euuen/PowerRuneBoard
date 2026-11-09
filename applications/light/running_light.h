#pragma once

#include <stdint.h>

#define RUNNING_LIGHT_LED_NUM 300

// 每颗 LED 占 24 个 uint16_t，尾部额外 10 颗用于复位信号
#define RUNNING_LIGHT_RESET_LED_NUM 10
#define RUNNING_LIGHT_FRAME_LED_NUM (RUNNING_LIGHT_LED_NUM + RUNNING_LIGHT_RESET_LED_NUM)

uint8_t RunningLight_IsReady(void);

void RunningLight_Clear(void);
void RunningLight_Draw_All(void);
void RunningLight_Draw_Arrow(uint16_t offset);
void RunningLight_Draw_Strip(uint16_t start, uint16_t end);
