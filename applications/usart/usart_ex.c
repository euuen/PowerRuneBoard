#include "usart_ex.h"
#include "usart.h"
#include <stdint.h>
#include <string.h>

// 单次 HAL 发送的超时时间，按分块发送，每块都重新计时
#define USART_EX_TX_TIMEOUT_MS 1000
// HAL_UART_Transmit 的长度参数为 uint16_t，分块发送以支持任意长度
#define USART_EX_TX_CHUNK_SIZE 1024

// VOFA+ JustFloat 帧尾：0x00 0x00 0x80 0x7f
static const uint8_t just_float_tail[4] = {0x00, 0x00, 0x80, 0x7f};

static USART_Ex_t usart_ex_handles[] = {
    [USART_EX_1] = {.huart = &huart1},
};

static USART_Ex_t* USART_Ex_Get(USART_EX_Handle handle) {
    if ((size_t)handle >= sizeof (usart_ex_handles) / sizeof (usart_ex_handles[0])) {
        return NULL;
    }
    return &usart_ex_handles[handle];
}

void USART_Ex_SendData(USART_Ex_t* usart_ex, const uint8_t* data, size_t size) {
    while (size > 0) {
        uint16_t chunk_size =
            size > USART_EX_TX_CHUNK_SIZE ? USART_EX_TX_CHUNK_SIZE : (uint16_t)size;

        if (HAL_UART_Transmit (usart_ex->huart, data, chunk_size,
                               USART_EX_TX_TIMEOUT_MS) != HAL_OK) {
            return;
        }

        data += chunk_size;
        size -= chunk_size;
    }
}

void USART_Ex_Init (USART_EX_Handle handle) {
    USART_Ex_t* usart_ex = USART_Ex_Get (handle);
    if (usart_ex == NULL) {
        return;
    }

    // 已由 MX_USARTx_UART_Init 初始化过则不重复初始化
    if (usart_ex->huart->gState == HAL_UART_STATE_RESET) {
        switch (handle) {
        case USART_EX_1:
            MX_USART1_UART_Init ();
            break;
        default:
            break;
        }
    }
}

void USART_Ex_SendStr (USART_EX_Handle handle, const char* str) {
    USART_Ex_t* usart_ex = USART_Ex_Get (handle);
    if (usart_ex == NULL || str == NULL) {
        return;
    }

    USART_Ex_SendData (usart_ex, (const uint8_t*)str, strlen (str));
}

void USART_Ex_SendJustFloat (USART_EX_Handle handle, const float* data, size_t size) {
    USART_Ex_t* usart_ex = USART_Ex_Get (handle);
    if (usart_ex == NULL || (data == NULL && size > 0)) {
        return;
    }

    // STM32 为小端，float 可直接按字节发送，JustFloat 同样要求小端
    USART_Ex_SendData (usart_ex, (const uint8_t*)data, size * sizeof (float));
    USART_Ex_SendData (usart_ex, just_float_tail, sizeof (just_float_tail));
}
