/*
 * ============================================================
 * Board 灯(272 LED) 测试 — Draw_Ring 环形灯
 *
 * 通过遥控器(FSi6x)操作：
 *   swa  : 0 = 全部熄灭 / 1 = 点亮当前选中的环
 *   swd  : 切换到下一个环 (1 -> 2 -> ... -> 10 -> 1)
 *   swb  : 切换到上一个环 (10 -> 9 -> ... -> 1 -> 10)
 *
 * 环编号 LED 范围：
 *   ring1 : 0-50    ring2 : 50-95    ring3 : 95-135
 *   ring4 : 135-170 ring5 : 170-200  ring6 : 200-225
 *   ring7 : 225-245 ring8 : 245-260  ring9 : 260-268
 *   ring10: 268-272
 *
 * 用于验证 BoardLight_Draw_Ring 各环的 LED 区间是否正确，
 * 并把当前环编号通过 CAN 0x201 发回 center 板
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

#define RING_MAX 9
#define RING_MIN 1

uint8_t fsi6x_swa = 0;
uint8_t fsi6x_swb = 0;
uint8_t fsi6x_swd = 0;
int8_t fsi6x_right_stick_x = 0; // -10 ~ 10

uint8_t last_fsi6x_swb = 0;
uint8_t last_fsi6x_swd = 0;

uint8_t current_ring = RING_MIN;

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
  if (fsi6x_swb != last_fsi6x_swb) {
    last_fsi6x_swb = fsi6x_swb;
    current_ring--;
    if (current_ring < RING_MIN)
      current_ring = RING_MAX;
  }

  if (fsi6x_swd != last_fsi6x_swd) {
    last_fsi6x_swd = fsi6x_swd;
    current_ring++;
    if (current_ring > RING_MAX)
      current_ring = RING_MIN;
  }

  uint8_t can1_tx_data[8] = {0};
  CAN_TxHeaderTypeDef canTxHeader = {0};
  uint32_t txMailbox = 0;

  canTxHeader.StdId = 0x201;
  canTxHeader.IDE = CAN_ID_STD;
  canTxHeader.RTR = CAN_RTR_DATA;
  canTxHeader.DLC = 8;

  can1_tx_data[0] = current_ring;
  can1_tx_data[1] = fsi6x_right_stick_x;
  HAL_CAN_AddTxMessage(&hcan, &canTxHeader, can1_tx_data, &txMailbox);

  if (fsi6x_swa == 0) {
    BoardLight_Clear();
  } else {
    BoardLight_Draw_Ring(current_ring);
  }

  HAL_Delay(5);
}

void HAL_CAN_RxFifo0MsgPendingCallback(CAN_HandleTypeDef *hcan) {
  uint8_t can1_rx_data[8] = {0};
  if (hcan->Instance == CAN1) {
    CAN_RxHeaderTypeDef hcan1_rx_header;
    HAL_CAN_GetRxMessage(hcan, CAN_RX_FIFO0, &hcan1_rx_header, can1_rx_data);
    fsi6x_swa = can1_rx_data[0];
    fsi6x_swb = can1_rx_data[1];
    fsi6x_swd = can1_rx_data[2];
    fsi6x_right_stick_x = (int8_t)can1_rx_data[3];
  }
}