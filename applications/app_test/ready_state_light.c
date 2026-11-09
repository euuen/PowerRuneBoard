/*
 * ============================================================
 * Board 灯 + Running 灯 联合测试
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
#include <stdint.h>
#include <string.h>


Board board;

void Board_Init() {
  board.brightness = BRIGHTNESS;

  FrameUpLight_Clear();
  FrameDownLight_Clear();
  FrameLight_Clear();
  BoardLight_Clear();
  RunningLight_Clear();

  HAL_Delay(100);
}

void Board_Loop() {
  FrameUpLight_Draw_All();
  FrameDownLight_Draw_All();
  FrameLight_Draw_All();
  BoardLight_Draw_Cross();
  RunningLight_Draw_Arrow(HAL_GetTick() / 50);


  HAL_Delay(10);
}