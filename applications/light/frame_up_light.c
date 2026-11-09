#include "frame_up_light.h"
#include "board.h"
#include "config.h"
#include "ws2812.h"
#include <stdint.h>
#include <tim.h>

/* 本路自己的帧描述：只有类型/颜色/参数变化时才重新生成画面 */
typedef enum {
  FRAME_NONE = 0,
  FRAME_CLEAR,
  FRAME_ALL,
  FRAME_STRIP,
} FrameType;

typedef struct {
  FrameType type;
  BoardColor color;
  uint8_t brightness;
  uint16_t start;
  uint16_t end;
} LightFrame;

static uint8_t Frame_Equals(const LightFrame *a, const LightFrame *b) {
  return a->type == b->type &&
         a->color == b->color &&
         a->brightness == b->brightness &&
         a->start == b->start &&
         a->end == b->end;
}

static uint16_t frame_up_light_buffer[FRAME_UP_LIGHT_FRAME_LED_NUM * 24];
static LightFrame frame_up_light_last;

uint8_t FrameUpLight_IsReady(void) {
  return HAL_DMA_GetState(&FRAME_UP_LIGHT_DMA) == HAL_DMA_STATE_READY;
}

static uint8_t FrameUpLight_Begin(LightFrame *frame) {
  frame->color = board.board_color;
  frame->brightness = board.brightness;

  if (Frame_Equals(&frame_up_light_last, frame))
    return 0;

  if (!FrameUpLight_IsReady())
    return 0;

  return 1;
}

static void FrameUpLight_Commit(const LightFrame *frame) {
  frame_up_light_last = *frame;
  WS281x_Update(frame_up_light_buffer, &FRAME_UP_LIGHT_TIM, FRAME_UP_LIGHT_CHANNEL,
                FRAME_UP_LIGHT_FRAME_LED_NUM);
}

static void FrameUpLight_GetColor(BoardColor color, uint8_t brightness,
                                  uint8_t *r, uint8_t *g, uint8_t *b) {
  switch (color) {
  case COLOR_RED:  *r = brightness; *g = 0;          *b = 0;          break;
  case COLOR_BLUE: *r = 0;          *g = 0;          *b = brightness; break;
  default:         *r = 0;          *g = 0;          *b = 0;          break;
  }
}

void FrameUpLight_Clear(void) {
  LightFrame frame = {.type = FRAME_CLEAR};
  if (!FrameUpLight_Begin(&frame))
    return;

  for (uint16_t i = 0; i < FRAME_UP_LIGHT_LED_NUM; i++)
    WS281x_SetPixel(frame_up_light_buffer, i, 0, 0, 0);
  for (uint16_t i = FRAME_UP_LIGHT_LED_NUM; i < FRAME_UP_LIGHT_FRAME_LED_NUM; i++)
    WS281x_SetResetSignal(frame_up_light_buffer, i);

  FrameUpLight_Commit(&frame);
}

void FrameUpLight_Draw_All(void) {
  LightFrame frame = {.type = FRAME_ALL};
  if (!FrameUpLight_Begin(&frame))
    return;

  uint8_t R, G, B;
  FrameUpLight_GetColor(frame.color, frame.brightness, &R, &G, &B);

  for (uint16_t i = 0; i < FRAME_UP_LIGHT_LED_NUM; i++)
    WS281x_SetPixel(frame_up_light_buffer, i, R, G, B);
  for (uint16_t i = FRAME_UP_LIGHT_LED_NUM; i < FRAME_UP_LIGHT_FRAME_LED_NUM; i++)
    WS281x_SetResetSignal(frame_up_light_buffer, i);

  FrameUpLight_Commit(&frame);
}

void FrameUpLight_Draw_Strip(uint16_t start, uint16_t end) {
  LightFrame frame = {.type = FRAME_STRIP, .start = start, .end = end};
  if (!FrameUpLight_Begin(&frame))
    return;

  uint8_t R, G, B;
  FrameUpLight_GetColor(frame.color, frame.brightness, &R, &G, &B);

  for (uint16_t i = 0; i < FRAME_UP_LIGHT_LED_NUM; i++) {
    if (i >= start && i < end)
      WS281x_SetPixel(frame_up_light_buffer, i, R, G, B);
    else
      WS281x_SetPixel(frame_up_light_buffer, i, 0, 0, 0);
  }
  for (uint16_t i = FRAME_UP_LIGHT_LED_NUM; i < FRAME_UP_LIGHT_FRAME_LED_NUM; i++)
    WS281x_SetResetSignal(frame_up_light_buffer, i);

  FrameUpLight_Commit(&frame);
}
