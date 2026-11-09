#include "protocol.h"
#include "board.h"
#include "config.h"
#include "can.h"
#include "protocol_type.h"
#include <string.h>

void Protocol_Init() {
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
}

void UploadBoardState() {
  BoardStateFrame boardStateFrame;
  boardStateFrame.board_state = board.board_state;
  boardStateFrame.score = board.score;

  CAN_TxHeaderTypeDef hcan1_tx_header;
  uint32_t CAN_Tx_Mailbox;

  hcan1_tx_header.IDE = CAN_ID_STD;
  hcan1_tx_header.RTR = CAN_RTR_DATA;
  hcan1_tx_header.DLC = sizeof(boardStateFrame);

#if BOARD_ID == 1
  hcan1_tx_header.StdId = BOARD_1_STATE_CAN_ID;
#elif BOARD_ID == 2
  hcan1_tx_header.StdId = BOARD_2_STATE_CAN_ID;
#elif BOARD_ID == 3
  hcan1_tx_header.StdId = BOARD_3_STATE_CAN_ID;
#elif BOARD_ID == 4
  hcan1_tx_header.StdId = BOARD_4_STATE_CAN_ID;
#elif BOARD_ID == 5
  hcan1_tx_header.StdId = BOARD_5_STATE_CAN_ID;
#endif

  HAL_CAN_AddTxMessage(&hcan, &hcan1_tx_header, (uint8_t *)&boardStateFrame,
                       &CAN_Tx_Mailbox);
}

static void Protocol_ApplyCenterRequest(const uint8_t *data, uint8_t len) {
  CenterRequestFrame frame;
  uint8_t copy_len = len < sizeof(frame) ? len : (uint8_t)sizeof(frame);

  memset(&frame, 0, sizeof(frame));
  memcpy(&frame, data, copy_len);

  board.board_color = frame.target_color;
  board.center_request_code = frame.request_code;
  if (len >= sizeof(frame)) {
    board.brightness = frame.brightness;
  }
}

void CAN_RxFifo0MsgPendingCallback(CAN_HandleTypeDef *hcan) {
  if (hcan->Instance == CAN1) {
    CAN_RxHeaderTypeDef hcan1_rx_header;
    uint8_t can1_rx_data[8];
    HAL_CAN_GetRxMessage(hcan, CAN_RX_FIFO0, &hcan1_rx_header, can1_rx_data);

    if (hcan1_rx_header.StdId ==
        (CENTER_REQUEST_1_CAN_ID + (BOARD_ID - 1))) {
      Protocol_ApplyCenterRequest(can1_rx_data, hcan1_rx_header.DLC);
    }
  }
}