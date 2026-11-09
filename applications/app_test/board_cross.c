/*
 * ============================================================
 * Board 灯(272 LED) 测试 — Draw_Cross 十字图案
 *
 * 通过遥控器(FSi6x)操作：
 *   swa  : 0 = 全部熄灭 / 1 = 点亮十字图案
 *
 * 效果：一次性点亮十字(LED编号数组) + ring2 + ring6 + ring9，
 *       其余熄灭。用于验证 BoardLight_Draw_Cross 图案位置
 *
 * 并把当前 swa 状态通过 CAN 0x201 发回 center 板
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


  BoardLight_Clear();

  HAL_Delay(20);
}

void Board_Loop() {
  uint8_t can1_tx_data[8] = {0};
  CAN_TxHeaderTypeDef canTxHeader = {0};
  uint32_t txMailbox = 0;

  canTxHeader.StdId = 0x201;
  canTxHeader.IDE = CAN_ID_STD;
  canTxHeader.RTR = CAN_RTR_DATA;
  canTxHeader.DLC = 8;

  can1_tx_data[0] = fsi6x_swa;
  HAL_CAN_AddTxMessage(&hcan, &canTxHeader, can1_tx_data, &txMailbox);

  if (fsi6x_swa == 0) {
    BoardLight_Clear();
  } else {
    BoardLight_Draw_Cross();
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