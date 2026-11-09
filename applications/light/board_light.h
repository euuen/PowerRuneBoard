#pragma once

#include <stdint.h>

#define BOARD_LIGHT_LED_NUM 272 // 不知道为什么多加一个led就能解决最后一个led控制异常的问题。。。

#define RING1_START 0
#define RING1_END   48
#define RING2_START 48
#define RING2_END   90
#define RING3_START 90
#define RING3_END   134
#define RING4_START 134
#define RING4_END   174
#define RING5_START 174
#define RING5_END   210
#define RING6_START 210
#define RING6_END   230
#define RING7_START 230
#define RING7_END   250
#define RING8_START 250
#define RING8_END   262
#define RING9_START 262
#define RING9_END   271

// 每颗 LED 占 24 个 uint16_t，尾部额外 10 颗用于复位信号
#define BOARD_LIGHT_RESET_LED_NUM 10
#define BOARD_LIGHT_FRAME_LED_NUM (BOARD_LIGHT_LED_NUM + BOARD_LIGHT_RESET_LED_NUM)

extern uint16_t board_light_buffer[BOARD_LIGHT_FRAME_LED_NUM * 24];
uint8_t BoardLight_IsReady(void);

void BoardLight_Clear(void);
void BoardLight_Draw_All(void);
void BoardLight_Draw_Ring(uint8_t ring_id);
void BoardLight_Draw_Cross(void);
void BoardLight_Draw_Strip(uint16_t start, uint16_t end);
