#include "board_light.h"
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
  FRAME_RING,
  FRAME_CROSS,
  FRAME_STRIP,
} FrameType;

typedef struct {
  FrameType type;
  BoardColor color;
  uint8_t brightness;
  uint8_t ring_id;
  uint16_t start;
  uint16_t end;
} LightFrame;

static uint8_t Frame_Equals(const LightFrame *a, const LightFrame *b) {
  return a->type == b->type &&
         a->color == b->color &&
         a->brightness == b->brightness &&
         a->ring_id == b->ring_id &&
         a->start == b->start &&
         a->end == b->end;
}

uint16_t board_light_buffer[BOARD_LIGHT_FRAME_LED_NUM * 24];
LightFrame board_light_last;

uint8_t BoardLight_IsReady(void) {
  return HAL_DMA_GetState(&BOARD_LIGHT_DMA) == HAL_DMA_STATE_READY;
}

/*
 * 判断这一帧是否需要更新：
 *   1. 帧描述和上一次输出的完全一致 -> 不需要
 *   2. 这一路上一帧还在 DMA 发送 -> 本次先不更新，下次调用再试
 * 返回 1 时 frame 里的颜色已经填好，可以开始写显存。
 */
static uint8_t BoardLight_Begin(LightFrame *frame) {
  frame->color = board.board_color;
  frame->brightness = board.brightness;

  if (Frame_Equals(&board_light_last, frame))
    return 0;

  if (!BoardLight_IsReady())
    return 0;

  return 1;
}

static void BoardLight_Commit(const LightFrame *frame) {
  board_light_last = *frame;
  WS281x_Update(board_light_buffer, &BOARD_LIGHT_TIM, BOARD_LIGHT_CHANNEL,
                BOARD_LIGHT_FRAME_LED_NUM);
}

static void BoardLight_GetColor(BoardColor color, uint8_t brightness,
                                uint8_t *r, uint8_t *g, uint8_t *b) {
  switch (color) {
  case COLOR_RED:  *r = brightness; *g = 0;          *b = 0;          break;
  case COLOR_BLUE: *r = 0;          *g = 0;          *b = brightness; break;
  default:         *r = 0;          *g = 0;          *b = 0;          break;
  }
}

void BoardLight_Clear(void) {
  LightFrame frame = {.type = FRAME_CLEAR};
  if (!BoardLight_Begin(&frame))
    return;

  for (uint16_t i = 0; i < BOARD_LIGHT_LED_NUM; i++)
    WS281x_SetPixel(board_light_buffer, i, 0, 0, 0);
  for (uint16_t i = BOARD_LIGHT_LED_NUM; i < BOARD_LIGHT_FRAME_LED_NUM; i++)
    WS281x_SetResetSignal(board_light_buffer, i);

  BoardLight_Commit(&frame);
}

void BoardLight_Draw_Strip(uint16_t start, uint16_t end) {
  LightFrame frame = {.type = FRAME_STRIP, .start = start, .end = end};
  if (!BoardLight_Begin(&frame))
    return;

  uint8_t R, G, B;
  BoardLight_GetColor(frame.color, frame.brightness, &R, &G, &B);

  for (uint16_t i = 0; i < BOARD_LIGHT_LED_NUM; i++) {
    if (i >= start && i < end)
      WS281x_SetPixel(board_light_buffer, i, R, G, B);
    else
      WS281x_SetPixel(board_light_buffer, i, 0, 0, 0);
  }
  for (uint16_t i = BOARD_LIGHT_LED_NUM; i < BOARD_LIGHT_FRAME_LED_NUM; i++)
    WS281x_SetResetSignal(board_light_buffer, i);

  BoardLight_Commit(&frame);
}

void BoardLight_Draw_Ring(uint8_t ring_id) {
  uint16_t ring_start = 0, ring_end = 0;
  switch (ring_id) {
  case 9:  ring_start = RING9_START;  ring_end = RING9_END;  break;
  case 8:  ring_start = RING8_START;  ring_end = RING8_END;  break;
  case 7:  ring_start = RING7_START;  ring_end = RING7_END;  break;
  case 6:  ring_start = RING6_START;  ring_end = RING6_END;  break;
  case 5:  ring_start = RING5_START;  ring_end = RING5_END;  break;
  case 4:  ring_start = RING4_START;  ring_end = RING4_END;  break;
  case 3:  ring_start = RING3_START;  ring_end = RING3_END;  break;
  case 2:  ring_start = RING2_START;  ring_end = RING2_END;  break;
  case 1:  ring_start = RING1_START;  ring_end = RING1_END;  break;
  default: return;
  }

  LightFrame frame = {.type = FRAME_RING, .ring_id = ring_id};
  if (!BoardLight_Begin(&frame))
    return;

  uint8_t R, G, B;
  BoardLight_GetColor(frame.color, frame.brightness, &R, &G, &B);

  for (uint16_t i = 0; i < BOARD_LIGHT_LED_NUM; i++) {
    if (i >= ring_start && i < ring_end)
      WS281x_SetPixel(board_light_buffer, i, R, G, B);
    else
      WS281x_SetPixel(board_light_buffer, i, 0, 0, 0);
  }
  for (uint16_t i = BOARD_LIGHT_LED_NUM; i < BOARD_LIGHT_FRAME_LED_NUM; i++)
    WS281x_SetResetSignal(board_light_buffer, i);

  BoardLight_Commit(&frame);
}

void BoardLight_Draw_All(void) {
  LightFrame frame = {.type = FRAME_ALL};
  if (!BoardLight_Begin(&frame))
    return;

  uint8_t R, G, B;
  BoardLight_GetColor(frame.color, frame.brightness, &R, &G, &B);

  for (uint16_t i = 0; i < BOARD_LIGHT_LED_NUM; i++)
    WS281x_SetPixel(board_light_buffer, i, R, G, B);
  for (uint16_t i = BOARD_LIGHT_LED_NUM; i < BOARD_LIGHT_FRAME_LED_NUM; i++)
    WS281x_SetResetSignal(board_light_buffer, i);

  BoardLight_Commit(&frame);
}

void BoardLight_Draw_Cross(void) {
  static const uint16_t cross_indices[] = {
    // 环1
    1, 2, 3,
    11, 12, 13, 14, 15,
    23, 24, 25, 26, 27, 28,
    35, 36, 37, 38,39,
    47, 48,

    // 环2
    49, 50, 
    59, 60, 61,
    69, 70, 71, 72,
    80, 81, 82,
    90,

    // 环3
    91, 92, 93,
    101, 102, 103, 104,
    112, 113, 114, 115,
    123, 124, 125, 126,
    134,

    // 环4
    135, 136, 137,
    144, 145, 146, 147,
    154, 155, 156, 157,
    164, 165, 166, 167,
    174,

    // 环5
    175, 176,
    183, 184, 185,
    192, 193, 194,
    201, 202, 203,
    210,

    // 环6
    211, 212,
    215, 216, 217,
    220, 221, 222,
    225, 226, 227,
    230,

    // 环9
    263,
    268,
    270,
  };
  const uint8_t num_cross = sizeof(cross_indices) / sizeof(cross_indices[0]);

  LightFrame frame = {.type = FRAME_CROSS};
  if (!BoardLight_Begin(&frame))
    return;

  uint8_t R, G, B;
  BoardLight_GetColor(frame.color, frame.brightness, &R, &G, &B);

  for (uint16_t i = 0; i < BOARD_LIGHT_LED_NUM; i++) {
    uint8_t lit = 0;

    if (!lit) {
      for (uint8_t k = 0; k < num_cross; k++) {
        if (cross_indices[k] - 1 == i) { lit = 1; break; }
      }
    }

    if (!lit && i >= RING2_START && i < RING2_END) lit = 1;
    if (!lit && i >= RING6_START && i < RING6_END) lit = 1;
    if (!lit && i >= RING9_START && i < RING9_END) lit = 1;

    if (lit)
      WS281x_SetPixel(board_light_buffer, i, R, G, B);
    else
      WS281x_SetPixel(board_light_buffer, i, 0, 0, 0);
  }
  for (uint16_t i = BOARD_LIGHT_LED_NUM; i < BOARD_LIGHT_FRAME_LED_NUM; i++)
    WS281x_SetResetSignal(board_light_buffer, i);

  BoardLight_Commit(&frame);
}
