/*
 * ============================================================
 * Running 灯(300 LED) 无控制测试 — 箭头动画
 *
 * 上电后不需要遥控器/CAN 控制，直接持续运行箭头动画：
 *   8x32 snake 矩阵上，多个左箭头紧密平铺(间距 6 列)，
 *   以 offset = HAL_GetTick() / 50 的速度持续向左移动。
 *
 * 用于验证 rc_to_led snake 映射以及箭头方向/移动逻辑。
 * ============================================================
 */
#include "board.h"
#include "config.h"
#include "light/board_light.h"
#include "light/frame_down_light.h"
#include "light/frame_light.h"
#include "light/frame_up_light.h"
#include "light/running_light.h"
#include "main.h"
#include "stm32f1xx_hal.h"
#include <can.h>
#include <stdint.h>
#include <string.h>

Board board;

void Board_Init() {
  memset(&board, 0, sizeof(board));
  board.board_id = BOARD_ID;
  board.board_color = COLOR_BLUE;
  board.board_state = BOARD_IGNORE;
  board.center_request_code = CENTER_REQUEST_NOTHING;
  board.brightness = BRIGHTNESS;

  RunningLight_Clear();

  HAL_Delay(20);
}

void Board_Loop() {
  uint16_t offset = HAL_GetTick() / 50;
  RunningLight_Draw_Arrow(offset);

  HAL_Delay(20);
}
