#include "main.h"
#include <usart/usart_ex.h>
#include <board.h>

void Board_Init() {
    USART_Ex_Init(USART_EX_1);
    USART_Ex_SendStr(USART_EX_1, "usart inited over\n");
}

void Board_Loop() {
    USART_Ex_SendStr(USART_EX_1, "usart is looping now\n");
    HAL_Delay(1000);
}