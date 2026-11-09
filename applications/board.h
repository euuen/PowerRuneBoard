#pragma once
#include "protocol/protocol_type.h"
#include <stdint.h>

#define true 1
#define false 0

typedef enum { COLOR_BLUE, COLOR_RED } BoardColor;

typedef enum {
    BOARD_READY,   // 等待命中
    BOARD_HIT,     // 已命中
    BOARD_TIMEOUT, // 保留占位，灯板不再使用，超时由中心板计算
    BOARD_IGNORE   // 无需做任何处理（如果击中会忽略）
} BoardState;

typedef struct {
    uint8_t request_ready;
} CenterRequest;

typedef struct {
    uint8_t board_id;             // 灯板id
    BoardColor board_color;       // 灯板颜色
    BoardState board_state;       // 灯板状态
    CenterRequestCode center_request_code; // 中心板请求
    uint8_t score;         // 命中环数得分，10环为10，未命中为0
    uint8_t brightness;    // 灯板亮度，0~255
} Board;

extern Board board;

void Board_Init ();
void Board_Loop ();