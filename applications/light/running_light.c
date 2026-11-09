#include "running_light.h"
#include "board.h"
#include "config.h"
#include "ws2812.h"
#include <stdint.h>
#include <tim.h>

#define MATRIX_COLS  32
#define MATRIX_ROWS  8
#define ARROW_COLS   6
#define ARROW_SPACING 6

/* 本路自己的帧描述：只有类型/颜色/参数变化时才重新生成画面 */
typedef enum {
  FRAME_NONE = 0,
  FRAME_CLEAR,
  FRAME_ALL,
  FRAME_ARROW,
  FRAME_STRIP,
} FrameType;

typedef struct {
  FrameType type;
  BoardColor color;
  uint8_t brightness;
  uint16_t offset;
  uint16_t start;
  uint16_t end;
} LightFrame;

static uint8_t Frame_Equals(const LightFrame *a, const LightFrame *b) {
  return a->type == b->type &&
         a->color == b->color &&
         a->brightness == b->brightness &&
         a->offset == b->offset &&
         a->start == b->start &&
         a->end == b->end;
}

static uint16_t running_light_buffer[RUNNING_LIGHT_FRAME_LED_NUM * 24];
static LightFrame running_light_last;

static uint16_t rc_to_led(uint8_t row, uint8_t col) {
  if (col % 2 == 0)
    return col * 8 + (8 - row);
  else
    return col * 8 + (row + 1);
}

static const uint8_t arrow_pattern[8][6] = {
  {0, 0, 0, 1, 1, 1},
  {0, 0, 1, 1, 1, 0},
  {0, 1, 1, 1, 0, 0},
  {1, 1, 1, 0, 0, 0},
  {1, 1, 1, 0, 0, 0},
  {0, 1, 1, 1, 0, 0},
  {0, 0, 1, 1, 1, 0},
  {0, 0, 0, 1, 1, 1},
};

uint8_t RunningLight_IsReady(void) {
  return HAL_DMA_GetState(&RUNNING_LIGHT_DMA) == HAL_DMA_STATE_READY;
}

static uint8_t RunningLight_Begin(LightFrame *frame) {
  frame->color = board.board_color;
  frame->brightness = board.brightness;

  if (Frame_Equals(&running_light_last, frame))
    return 0;

  if (!RunningLight_IsReady())
    return 0;

  return 1;
}

static void RunningLight_Commit(const LightFrame *frame) {
  running_light_last = *frame;
  WS281x_Update(running_light_buffer, &RUNNING_LIGHT_TIM, RUNNING_LIGHT_CHANNEL,
                RUNNING_LIGHT_FRAME_LED_NUM);
}

static void RunningLight_GetColor(BoardColor color, uint8_t brightness,
                                  uint8_t *r, uint8_t *g, uint8_t *b) {
  switch (color) {
  case COLOR_RED:  *r = brightness; *g = 0;          *b = 0;          break;
  case COLOR_BLUE: *r = 0;          *g = 0;          *b = brightness; break;
  default:         *r = 0;          *g = 0;          *b = 0;          break;
  }
}

void RunningLight_Clear(void) {
  LightFrame frame = {.type = FRAME_CLEAR};
  if (!RunningLight_Begin(&frame))
    return;

  for (uint16_t i = 0; i < RUNNING_LIGHT_LED_NUM; i++)
    WS281x_SetPixel(running_light_buffer, i, 0, 0, 0);
  for (uint16_t i = RUNNING_LIGHT_LED_NUM; i < RUNNING_LIGHT_FRAME_LED_NUM; i++)
    WS281x_SetResetSignal(running_light_buffer, i);

  RunningLight_Commit(&frame);
}

void RunningLight_Draw_All(void) {
  LightFrame frame = {.type = FRAME_ALL};
  if (!RunningLight_Begin(&frame))
    return;

  uint8_t R, G, B;
  RunningLight_GetColor(frame.color, frame.brightness, &R, &G, &B);

  for (uint16_t i = 0; i < RUNNING_LIGHT_LED_NUM; i++)
    WS281x_SetPixel(running_light_buffer, i, R, G, B);
  for (uint16_t i = RUNNING_LIGHT_LED_NUM; i < RUNNING_LIGHT_FRAME_LED_NUM; i++)
    WS281x_SetResetSignal(running_light_buffer, i);

  RunningLight_Commit(&frame);
}

void RunningLight_Draw_Arrow(uint16_t offset) {
  LightFrame frame = {.type = FRAME_ARROW, .offset = offset};
  if (!RunningLight_Begin(&frame))
    return;

  uint8_t R, G, B;
  RunningLight_GetColor(frame.color, frame.brightness, &R, &G, &B);

  for (uint8_t row = 0; row < MATRIX_ROWS; row++) {
    for (uint8_t col = 0; col < MATRIX_COLS; col++) {
      uint16_t led = rc_to_led(row, col) - 1;
      uint8_t adj = (col + offset) % ARROW_SPACING;
      if (adj < ARROW_COLS && arrow_pattern[row][adj])
        WS281x_SetPixel(running_light_buffer, led, R, G, B);
      else
        WS281x_SetPixel(running_light_buffer, led, 0, 0, 0);
    }
  }

  for (uint16_t i = RUNNING_LIGHT_LED_NUM; i < RUNNING_LIGHT_FRAME_LED_NUM; i++)
    WS281x_SetResetSignal(running_light_buffer, i);

  RunningLight_Commit(&frame);
}

void RunningLight_Draw_Strip(uint16_t start, uint16_t end) {
  LightFrame frame = {.type = FRAME_STRIP, .start = start, .end = end};
  if (!RunningLight_Begin(&frame))
    return;

  uint8_t R, G, B;
  RunningLight_GetColor(frame.color, frame.brightness, &R, &G, &B);

  for (uint16_t i = 0; i < RUNNING_LIGHT_LED_NUM; i++) {
    if (i >= start && i < end)
      WS281x_SetPixel(running_light_buffer, i, R, G, B);
    else
      WS281x_SetPixel(running_light_buffer, i, 0, 0, 0);
  }
  for (uint16_t i = RUNNING_LIGHT_LED_NUM; i < RUNNING_LIGHT_FRAME_LED_NUM; i++)
    WS281x_SetResetSignal(running_light_buffer, i);

  RunningLight_Commit(&frame);
}
