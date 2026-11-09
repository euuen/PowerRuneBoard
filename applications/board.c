#include "board.h"
#include "config.h"
#include "fsm.h"
#include "light/board_light.h"
#include "light/frame_down_light.h"
#include "light/frame_light.h"
#include "light/frame_up_light.h"
#include "light/running_light.h"
#include "main.h"
#include "protocol/protocol.h"
#include <string.h>

Board board;

void Board_Init () {
    Protocol_Init ();

    memset (&board, 0, sizeof (board));
    board.board_id = BOARD_ID;
    board.board_color = COLOR_BLUE;
    board.board_state = BOARD_IGNORE;
    board.center_request_code = CENTER_REQUEST_NOTHING;
    board.brightness = BRIGHTNESS;

    BoardLight_Clear ();
    RunningLight_Clear ();
    FrameUpLight_Clear ();
    FrameLight_Clear ();
    FrameDownLight_Clear ();
}

void Board_Loop () {
    // 发送灯板状态
    UploadBoardState ();

    // 状态机处理
    FSM_Process ();

    // 灯效已在 FSM_Process 里直接调用各灯模块的全局函数，
    // 每个模块内部只在画面变化时才启动自己的 PWM/DMA
    HAL_Delay (20);
}

void HAL_CAN_RxFifo0MsgPendingCallback (CAN_HandleTypeDef* hcan) {
    CAN_RxFifo0MsgPendingCallback (hcan);
}
