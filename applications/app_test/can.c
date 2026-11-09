/*
 * ============================================================
 * CAN 连通性测试
 *
 * 用法：
 *   Board_Init  : 初始化 CAN1 接收（过滤全部，接收所有帧）
 *   Board_Loop  : 空转
 *   回调         : 收到 StdId=0x101 的帧后，原样回发 StdId=0x201
 *
 *   在 center 板发送 0x101 帧，若 board 回发 0x201
 *   （内容与发送一致），则 CAN 双向连通正常
 * ============================================================
 */
#include "usart/usart_ex.h"
#include <can.h>

void Board_Init() {
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
  USART_Ex_Init(USART_EX_1);
  USART_Ex_SendStr(USART_EX_1, "usart inited over\n");
}

void Board_Loop() { HAL_Delay(1); }

void HAL_CAN_RxFifo0MsgPendingCallback(CAN_HandleTypeDef *hcan) {
  if (hcan->Instance == CAN1) {
    CAN_RxHeaderTypeDef hcan1_rx_header;
    uint8_t can1_rx_data[8];
    HAL_CAN_GetRxMessage(hcan, CAN_RX_FIFO0, &hcan1_rx_header, can1_rx_data);
    if (hcan1_rx_header.StdId == 0x101) {
      USART_Ex_SendStr(USART_EX_1, "can get data\n");
      CAN_TxHeaderTypeDef canTxHeader = {0};
      uint32_t txMailbox = 0;

      canTxHeader.StdId = 0x201;
      canTxHeader.IDE = CAN_ID_STD;
      canTxHeader.RTR = CAN_RTR_DATA;
      canTxHeader.DLC = 8;

      HAL_CAN_AddTxMessage(hcan, &canTxHeader, can1_rx_data, &txMailbox);
    }
  }
}