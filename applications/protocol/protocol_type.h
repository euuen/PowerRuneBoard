#pragma once
#include <stdint.h>

#pragma pack(1)

typedef struct {
    uint8_t board_state;
    uint8_t score;
} BoardStateFrame; // board to center

typedef struct {
    uint8_t target_color;
    uint8_t request_code;
    uint8_t brightness;
} CenterRequestFrame; // center to board

#pragma pack()

typedef enum {
    REQUEST_M3508_CAN_ID = 0x200,
    M3508_1_FEEDBACK_CAN_ID = 0x201,

    BOARD_1_STATE_CAN_ID = 0x101,
    BOARD_2_STATE_CAN_ID = 0x102,
    BOARD_3_STATE_CAN_ID = 0x103,
    BOARD_4_STATE_CAN_ID = 0x104,
    BOARD_5_STATE_CAN_ID = 0x105,

    CENTER_REQUEST_1_CAN_ID = 0x301,
    CENTER_REQUEST_2_CAN_ID = 0x302,
    CENTER_REQUEST_3_CAN_ID = 0x303,
    CENTER_REQUEST_4_CAN_ID = 0x304,
    CENTER_REQUEST_5_CAN_ID = 0x305,
} FrameCanID;

typedef enum {
    CENTER_REQUEST_NOTHING, // 中心板无任何请求
    CENTER_REQUEST_READY,   // 中心板请求灯板进入待射击状态
    CENTER_REQUEST_RESET,   // 中心板请求灯板状态复位为IGNORE状态
} CenterRequestCode;