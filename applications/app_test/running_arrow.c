/*
 * ============================================================
 * Running 灯(300 LED) 测试 — 箭头动画
 *
 * 通过遥控器(FSi6x)操作：
 *   swa  : 0 = 熄灭 / 1 = 显示箭头动画
 *
 * 效果：8x32 snake 矩阵上，多个左箭头紧密平铺(间距6列)，
 *       以 offset = HAL_GetTick()/50 的速度持续向左移动
 *
 * 用于验证 rc_to_led snake 映射与箭头方向/移动逻辑
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

uint8_t fsi6x_swa = 0;

Board board;

void Board_Init() {
  memset(&board, 0, sizeof(board));
  board.board_id = BOARD_ID;
  board.board_color = COLOR_BLUE;
  board.board_state = BOARD_IGNORE;
  board.center_request_code = CENTER_REQUEST_NOTHING;
  board.brightness = BRIGHTNESS;
  CAN_FilterTypeDef sFilterConfig;

  sFilterConfig.FilterIdHigh = 0;
  sFilterConfig.FilterIdLow = 0;
  sFilterConfig.FilterMaskIdHigh = 0;
  sFilterConfig.FilterMaskIdLow = 0;
  sFilterConfig.FilterFIFOAssignment = CAN_FILTER_FIFO0;
  sFilterConfig.FilterBank = 0;
  sFilterConfig.FilterMode = CAN_FILTERMODE_IDMASK;
  sFilterConfig.FilterScale = CAN_FILTERSCALE_32BIT;
  sFilterConfig.FilterActivation = ENABLE;
  sFilterConfig.SlaveStartFilterBank = 0;

  HAL_CAN_ConfigFilter(&hcan, &sFilterConfig);
  HAL_CAN_ActivateNotification(&hcan, CAN_IT_RX_FIFO0_MSG_PENDING);
  HAL_CAN_Start(&hcan);

  RunningLight_Clear();

  HAL_Delay(20);
}

void Board_Loop() {
  if (fsi6x_swa == 0) {
    RunningLight_Clear();
  } else {
    uint16_t offset = HAL_GetTick() / 50;
    RunningLight_Draw_Arrow(offset);
  }

  HAL_Delay(5);
}

void HAL_CAN_RxFifo0MsgPendingCallback(CAN_HandleTypeDef *hcan) {
  uint8_t can1_rx_data[8] = {0};
  if (hcan->Instance == CAN1) {
    CAN_RxHeaderTypeDef hcan1_rx_header;
    HAL_CAN_GetRxMessage(hcan, CAN_RX_FIFO0, &hcan1_rx_header, can1_rx_data);
    fsi6x_swa = can1_rx_data[0];
  }
}
