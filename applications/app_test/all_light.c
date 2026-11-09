/*
 * ============================================================
 * 全部灯(5 种)切换测试
 *
 * 通过遥控器(FSi6x)操作：
 *   swa  : 0 = 全部熄灭 / 1 = 点亮当前选中的灯
 *   swb  : 切换到上一种灯
 *   swd  : 切换到下一种灯
 *
 * 灯类型循环顺序：
 *   Board(272) → Frame(28) → Frame_Down(92) → Frame_Up(86) → Running(300)
 *
 * 每帧会把非当前类型全部熄灭，只点亮当前选中类型，
 * 并把当前类型编号通过 CAN 0x201 发回 center 板
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
uint8_t fsi6x_swb = 0;
uint8_t fsi6x_swd = 0;
int8_t fsi6x_right_stick_x = 0;

uint8_t last_fsi6x_swb = 0;
uint8_t last_fsi6x_swd = 0;

typedef union {
  uint16_t value;
  uint8_t bytes[2];
} U16_Converter;

typedef enum {
  LIGHT_BOARD = 0,
  LIGHT_FRAME,
  LIGHT_FRAME_DOWN,
  LIGHT_FRAME_UP,
  LIGHT_RUNNING,
} LightType;

uint8_t current_light = LIGHT_BOARD;

Board board;

static void Clear_Light_By_Type(uint8_t type) {
  switch (type) {
    case LIGHT_BOARD:
      BoardLight_Clear();
      break;
    case LIGHT_FRAME:
      FrameLight_Clear();
      break;
    case LIGHT_FRAME_DOWN:
      FrameDownLight_Clear();
      break;
    case LIGHT_FRAME_UP:
      FrameUpLight_Clear();
      break;
    case LIGHT_RUNNING:
      RunningLight_Clear();
      break;
  }
}

static void Draw_Light_By_Type(uint8_t type) {
  switch (type) {
    case LIGHT_BOARD:
      BoardLight_Draw_All();
      break;
    case LIGHT_FRAME:
      FrameLight_Draw_All();
      break;
    case LIGHT_FRAME_DOWN:
      FrameDownLight_Draw_All();
      break;
    case LIGHT_FRAME_UP:
      FrameUpLight_Draw_All();
      break;
    case LIGHT_RUNNING:
      RunningLight_Draw_All();
      break;
  }
}

static void Clear_All_Lights(void) {
  BoardLight_Clear();
  FrameLight_Clear();
  FrameDownLight_Clear();
  FrameUpLight_Clear();
  RunningLight_Clear();
}

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


  Clear_All_Lights();

  HAL_Delay(20);
}

void Board_Loop() {
  if (fsi6x_swb != last_fsi6x_swb) {
    last_fsi6x_swb = fsi6x_swb;
    if (current_light == LIGHT_BOARD)
      current_light = LIGHT_RUNNING;
    else
      current_light--;
  }

  if (fsi6x_swd != last_fsi6x_swd) {
    last_fsi6x_swd = fsi6x_swd;
    current_light++;
    if (current_light > LIGHT_RUNNING)
      current_light = LIGHT_BOARD;
  }

  for (uint8_t i = 0; i <= LIGHT_RUNNING; i++) {
    if (i != current_light)
      Clear_Light_By_Type(i);
  }

  if (fsi6x_swa == 0) {
    Clear_All_Lights();
  } else {
    Draw_Light_By_Type(current_light);
  }

  uint8_t can1_tx_data[8] = {0};
  CAN_TxHeaderTypeDef canTxHeader = {0};
  uint32_t txMailbox = 0;

  canTxHeader.StdId = 0x201;
  canTxHeader.IDE = CAN_ID_STD;
  canTxHeader.RTR = CAN_RTR_DATA;
  canTxHeader.DLC = 8;

  can1_tx_data[0] = current_light;
  can1_tx_data[1] = fsi6x_right_stick_x;
  HAL_CAN_AddTxMessage(&hcan, &canTxHeader, can1_tx_data, &txMailbox);

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
