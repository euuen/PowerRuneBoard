#pragma once
#include "can.h"
#include <stdint.h>

void Protocol_Init ();
void UploadBoardState ();
void CAN_RxFifo0MsgPendingCallback (CAN_HandleTypeDef* hcan);