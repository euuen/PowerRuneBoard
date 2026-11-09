#pragma once

#include "tim.h"
#include <stdint.h>
#include <stddef.h>

#define WS1 30  //随着自动重装载值而变化 2:1
#define WS0 15

uint32_t WS281x_Color(uint8_t red, uint8_t green, uint8_t blue);
void WS281x_SetPixel(uint16_t *buffer, size_t p, uint8_t R, uint8_t G, uint8_t B);
void WS281x_SetResetSignal (uint16_t* buffer, size_t p);
void WS281x_Update(uint16_t *buffer, TIM_HandleTypeDef *tim, uint32_t channel, uint16_t length);

