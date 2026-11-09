/*
 * ============================================================
 * Board 灯(272 LED) 测试
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

  FrameDownLight_Clear();

  HAL_Delay(20);
}

void Board_Loop() {
	if (current_end_led > FRAME_DOWN_LIGHT_LED_NUM) {
		current_end_led = 0;
	}
	FrameDownLight_Draw_Strip(0, current_end_led);
	current_end_led += 1;
  HAL_Delay(20);
}