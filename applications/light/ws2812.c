#include "ws2812.h"
#include "main.h"
#include <stdint.h>

/* 把 HAL 的通道号映射成定时器对应的 CCx DMA 请求位 */
static uint32_t WS281x_DmaRequest(uint32_t channel) {
    switch (channel) {
    case TIM_CHANNEL_1: return TIM_DMA_CC1;
    case TIM_CHANNEL_2: return TIM_DMA_CC2;
    case TIM_CHANNEL_3: return TIM_DMA_CC3;
    case TIM_CHANNEL_4: return TIM_DMA_CC4;
    default:            return 0U;
    }
}

/* 把 HAL 的通道号映射成对应的 CCx 标志位 */
static uint32_t WS281x_CcFlag(uint32_t channel) {
    switch (channel) {
    case TIM_CHANNEL_1: return TIM_FLAG_CC1;
    case TIM_CHANNEL_2: return TIM_FLAG_CC2;
    case TIM_CHANNEL_3: return TIM_FLAG_CC3;
    case TIM_CHANNEL_4: return TIM_FLAG_CC4;
    default:            return 0U;
    }
}

/*
 * 判断该定时器上是否只有当前通道在用。
 * TIM5(CH1 板灯 / CH2 下边框) 和 TIM1(CH1 流水 / CH2 外框) 都存在
 * 一个定时器驱动两条灯带的情况，此时不能停定时器，否则会把另一条
 * 正在发送的灯带打断。
 */
static uint8_t WS281x_TimerOwnerOnly(TIM_HandleTypeDef *tim, uint32_t channel) {
    uint32_t ccer = tim->Instance->CCER;

    if (channel != TIM_CHANNEL_1 && (ccer & TIM_CCER_CC1E) != 0U) return 0U;
    if (channel != TIM_CHANNEL_2 && (ccer & TIM_CCER_CC2E) != 0U) return 0U;
    if (channel != TIM_CHANNEL_3 && (ccer & TIM_CCER_CC3E) != 0U) return 0U;
    if (channel != TIM_CHANNEL_4 && (ccer & TIM_CCER_CC4E) != 0U) return 0U;

    return 1U;
}

uint32_t WS281x_Color (uint8_t red, uint8_t green, uint8_t blue) {
    return green << 16 | red << 8 | blue;
}

void WS281x_SetPixel (uint16_t* buffer, size_t p, uint8_t R, uint8_t G, uint8_t B) {
    uint8_t i;
    for (i = 0; i < 24; ++i) {
        buffer[24 * p + i] = (((WS281x_Color (R, G, B) << i) & 0X800000) ? WS1 : WS0);
    }
}

void WS281x_SetResetSignal (uint16_t* buffer, size_t p) {
    uint8_t i;
    for (i = 0; i < 24; ++i) {
        buffer[24 * p + i] = 0;
    }
}

void WS281x_Update (uint16_t* buffer, TIM_HandleTypeDef* tim, uint32_t channel, uint16_t length) {
    uint32_t dma_request = WS281x_DmaRequest(channel);
    uint32_t cc_flag = WS281x_CcFlag(channel);

    if (dma_request == 0U || cc_flag == 0U) {
        return;
    }

    /*
     * 上一帧还没发完就直接返回，不能在这时去关 CCxDE，否则会把正在发送的
     * 那一帧打断（例如测试代码里连续调用两次 WS281x_Update 的情况）。
     */
    if (TIM_CHANNEL_STATE_GET(tim, channel) != HAL_TIM_CHANNEL_STATE_READY) {
        return;
    }

    /*
     * 上一帧发送完成后，这个 HAL 版本不会自动清掉定时器的 CCxDE，而定时器
     * 一直在自由运行。若直接调用 HAL_TIM_PWM_Start_DMA()，帧间产生的残留
     * CCx DMA 请求可能在 DMA 通道刚使能时被立刻响应，使新一帧的第一位和
     * PWM 周期错开；一旦错位，整帧数据都会跟着移一位，看起来就是递增时
     * 灯带闪烁。
     *
     * 所以先把本通道的 DMA 请求关掉，并清掉挂起的 CCx 标志，再重新启动。
     */
    __HAL_TIM_DISABLE_DMA(tim, dma_request);
    __HAL_TIM_CLEAR_FLAG(tim, cc_flag);

    /*
     * 如果这个定时器只有当前通道在用，就从 CNT=0 重新开始，保证每帧第一
     * 位的相位固定；如果还有其它通道在用，则不能动共享的定时器。
     */
    if (WS281x_TimerOwnerOnly(tim, channel)) {
        __HAL_TIM_DISABLE(tim);
        __HAL_TIM_SET_COUNTER(tim, 0U);
        __HAL_TIM_CLEAR_FLAG(tim, TIM_FLAG_UPDATE);
    }

    HAL_TIM_PWM_Start_DMA (tim, channel, (const uint32_t*)buffer,
                           (uint16_t)(length * 24U));
}

/*
 * 一帧 DMA 发完就立刻关掉该通道的 CCx DMA 请求，避免帧间空闲期间定时器
 * 持续产生请求，下一帧重启时被误响应。
 */
void HAL_TIM_PWM_PulseFinishedCallback(TIM_HandleTypeDef *htim) {
    switch (htim->Channel) {
    case HAL_TIM_ACTIVE_CHANNEL_1:
        __HAL_TIM_DISABLE_DMA(htim, TIM_DMA_CC1);
        break;
    case HAL_TIM_ACTIVE_CHANNEL_2:
        __HAL_TIM_DISABLE_DMA(htim, TIM_DMA_CC2);
        break;
    case HAL_TIM_ACTIVE_CHANNEL_3:
        __HAL_TIM_DISABLE_DMA(htim, TIM_DMA_CC3);
        break;
    case HAL_TIM_ACTIVE_CHANNEL_4:
        __HAL_TIM_DISABLE_DMA(htim, TIM_DMA_CC4);
        break;
    default:
        break;
    }
}
