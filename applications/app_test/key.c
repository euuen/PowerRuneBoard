/*
 * ============================================================
 * 按键(key)测试
 *
 * 用法：
 *   Board_Init  : 初始化 CAN1 接收（全部过滤，接收所有消息）
 *   Board_Loop  : 读取 9 路按键(rings_key)电平，
 *                 打包成 16bit 位(byte0+byte1 bit0) + RingsKey_Scan()结果，
 *                 通过 CAN 0x201 发送给 center 板
 *
 *   用 center 板查看收到的按键数据是否正确即可
 * ============================================================
 */
#include <can.h>
#include <stdio.h>
#include <string.h>
#include <usart/usart_ex.h>
#include <key/rings_key.h>
#include "board.h"
#include "config.h"
#include "light/board_light.h"
#include "light/frame_down_light.h"
#include "light/frame_light.h"
#include "light/frame_up_light.h"
#include "light/running_light.h"

Board board;


void Board_Init() {
  memset(&board, 0, sizeof(board));
  board.board_id = BOARD_ID;
  board.board_color = COLOR_BLUE;
  board.board_state = BOARD_IGNORE;
  board.center_request_code = CENTER_REQUEST_NOTHING;
  board.brightness = BRIGHTNESS;

  HAL_CAN_Start(&hcan);

  BoardLight_Clear();
  FrameLight_Clear();
  FrameDownLight_Clear();
  FrameUpLight_Clear();
  RunningLight_Clear();

  USART_Ex_Init(USART_EX_1);
}

void Board_Loop() {
  uint8_t can1_tx_data[8];
  CAN_TxHeaderTypeDef canTxHeader = {0};
  uint32_t txMailbox = 0;

  canTxHeader.StdId = 0x201;
  canTxHeader.IDE = CAN_ID_STD;
  canTxHeader.RTR = CAN_RTR_DATA;
  canTxHeader.DLC = 8;

  // for (uint8_t i = 0; i < 9; i++) {
  //   can1_tx_data[i] = HAL_GPIO_ReadPin((GPIO_TypeDef *)RingsKey_Port[i], RingsKey_Pin[i]);
  // }

  uint16_t key_bits = 0;
  for (uint8_t i = 0; i < 9; i++) {
    if (HAL_GPIO_ReadPin((GPIO_TypeDef *)RingsKey_Port[i], RingsKey_Pin[i]))
    {
      key_bits |= (1U << i); // 置位对应位
    }
  }
  can1_tx_data[0] = (uint8_t)(key_bits & 0xFF); // 低8位
  can1_tx_data[1] =
      (uint8_t)((key_bits >> 8) & 0xFF); // 第9位（bit8）放入第二字节bit0

  can1_tx_data[2] = RingsKey_Scan();
  HAL_CAN_AddTxMessage(&hcan, &canTxHeader, can1_tx_data, &txMailbox);
  char out_str[64];
  int len = 0;

  // 打印9个按键状态（bit0~bit8）
  for (int i = 0; i < 9; i++) {
    int state = (key_bits >> i) & 0x01;
    len += snprintf(out_str + len, sizeof(out_str) - len, "%d ", state);
  }

  // 追加第三个字节的数据（以十进制打印）
  len += snprintf(out_str + len, sizeof(out_str) - len, "%d\n", can1_tx_data[2]);
  USART_Ex_SendStr(USART_EX_1, out_str);
  HAL_Delay(100);
}
