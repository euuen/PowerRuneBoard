/*
 * ============================================================
 * Running 灯(300 LED) 测试 — strip 点亮
 *
 * 通过遥控器(FSi6x)操作：
 *   swa  : 0 = 全部熄灭 / 1 = 点亮
 *   swd  : 每按一次，点亮 LED 数量 +1
 *   swb  : 每按一次，点亮 LED 数量 -1（最小 0）
 *   右手摇杆 x : 平滑微调点亮数量（范围 0 ~ 300）
 *
 * 效果：从 LED 0 开始往前点亮一条 strip，
 *       用于验证 Running 灯条(8x32 snake 矩阵)编号顺序
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
int8_t fsi6x_right_stick_x = 0; // -10 ~ 10

uint8_t last_fsi6x_swb = 0;
uint8_t last_fsi6x_swd = 0;

typedef enum {
  FSi6x_STICK_STATE_UP,
  FSi6x_STICK_STATE_MID,
  FSi6x_STICK_STATE_DOWN,
} FSi6x_STICK_STATE;

typedef union {
  uint16_t value;
  uint8_t bytes[2];
} U16_Converter;

float current_end_led = 0;

Board board;

void Board_Init() {
  memset(&board, 0, sizeof(board));
  board.board_id = BOARD_ID;
  board.board_color = COLOR_BLUE;
  board.board_state = BOARD_IGNORE;
  board.center_request_code = CENTER_REQUEST_NOTHING;
  board.brightness = BRIGHTNESS;
  CAN_FilterTypeDef sFilterConfig;

  sFilterConfig.FilterIdHigh = 0;                        // 过滤器ID高位
  sFilterConfig.FilterIdLow = 0;                         // 过滤器ID低位
  sFilterConfig.FilterMaskIdHigh = 0;                    // 掩码ID高位
  sFilterConfig.FilterMaskIdLow = 0;                     // 掩码ID低位
  sFilterConfig.FilterFIFOAssignment = CAN_FILTER_FIFO0; // 使用FIFO0接收
  sFilterConfig.FilterBank = 0;                          // 使用滤波器组0
  sFilterConfig.FilterMode = CAN_FILTERMODE_IDMASK;      // 标识符掩码模式
  sFilterConfig.FilterScale = CAN_FILTERSCALE_32BIT;     // 32位滤波器
  sFilterConfig.FilterActivation = ENABLE;               // 启用滤波器
  sFilterConfig.SlaveStartFilterBank = 0;

  HAL_CAN_ConfigFilter(&hcan, &sFilterConfig);
  HAL_CAN_ActivateNotification(&hcan, CAN_IT_RX_FIFO0_MSG_PENDING);
  HAL_CAN_Start(&hcan);


  RunningLight_Clear();

  HAL_Delay(20);
}

void Board_Loop() {
  if (fsi6x_swd != last_fsi6x_swd) {
    last_fsi6x_swd = fsi6x_swd;
    current_end_led += 1;
  }

  if (fsi6x_swb != last_fsi6x_swb) {
    last_fsi6x_swb = fsi6x_swb;
    current_end_led -= 1;
    if (current_end_led < 0)
      current_end_led = 0;
  }

  current_end_led += fsi6x_right_stick_x * 0.0001f;
  if (current_end_led < 0)
    current_end_led = 0;

  uint8_t can1_tx_data[8] = {0};
  CAN_TxHeaderTypeDef canTxHeader = {0};
  uint32_t txMailbox = 0;

  canTxHeader.StdId = 0x201;
  canTxHeader.IDE = CAN_ID_STD;
  canTxHeader.RTR = CAN_RTR_DATA;
  canTxHeader.DLC = 8;

  U16_Converter u16_converter;
  u16_converter.value = (uint16_t)current_end_led;
  can1_tx_data[0] = u16_converter.bytes[0];
  can1_tx_data[1] = u16_converter.bytes[1];
  can1_tx_data[2] = fsi6x_right_stick_x;
  HAL_CAN_AddTxMessage(&hcan, &canTxHeader, can1_tx_data, &txMailbox);

  // 总的控制灯泡开关，防止意外发生。
  if (fsi6x_swa == 0) {
    RunningLight_Clear();
  } else {
    RunningLight_Draw_Strip(0, current_end_led);
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