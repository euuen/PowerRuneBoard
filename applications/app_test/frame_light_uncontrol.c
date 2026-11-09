/*
 * ============================================================
 * Frame 灯(28 LED) 无控制测试
 *
 * 上电后不需要遥控器/CAN 控制，自动从 LED 0 开始逐颗点亮，
 * 点亮到末尾后回到 0 重新开始，用于验证 Frame 灯条
 * 的 LED 编号顺序和整条灯带是否正常。
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
uint32_t current_end_led = 0;

void Board_Init() {
  memset(&board, 0, sizeof(board));
  board.board_id = BOARD_ID;
  board.board_color = COLOR_BLUE;
  board.board_state = BOARD_IGNORE;
  board.center_request_code = CENTER_REQUEST_NOTHING;
  board.brightness = BRIGHTNESS;

  FrameLight_Clear();

  HAL_Delay(20);
}

void Board_Loop() {
  if (current_end_led > FRAME_LIGHT_LED_NUM) {
    current_end_led = 0;
  }
  FrameLight_Draw_Strip(0, current_end_led);
  current_end_led += 1;
  HAL_Delay(20);
}
