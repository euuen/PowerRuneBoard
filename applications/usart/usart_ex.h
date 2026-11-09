#pragma once

#include "main.h"
#include <stddef.h>

typedef struct {
    UART_HandleTypeDef* huart;
} USART_Ex_t;

typedef enum {
    USART_EX_1
} USART_EX_Handle;

void USART_Ex_Init(USART_EX_Handle handle);
void USART_Ex_SendData(USART_Ex_t* usart_ex, const uint8_t* data, size_t size);
void USART_Ex_SendStr(USART_EX_Handle handle, const char* str);
void USART_Ex_SendJustFloat(USART_EX_Handle handle, const float* data, size_t size);
