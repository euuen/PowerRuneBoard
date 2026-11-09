#include "fsm.h"
#include "board.h"
#include "key/rings_key.h"
#include "light/board_light.h"
#include "light/frame_down_light.h"
#include "light/frame_light.h"
#include "light/frame_up_light.h"
#include "light/running_light.h"
#include "main.h"
#include "protocol/protocol.h"
#include <stdint.h>

static uint8_t pressed_key = 0;

static void FSM_EnterState(BoardState _board_state) {
  switch (_board_state) {
  case BOARD_IGNORE:
    board.score = 0;
    break;
  case BOARD_READY:
    break;
  case BOARD_HIT:
    break;
  case BOARD_TIMEOUT:
    break;
  default:
    break;
  }
}

static void FSM_ExitState(BoardState _board_state) {
  switch (_board_state) {
  case BOARD_IGNORE:
    break;
  case BOARD_READY:
    break;
  case BOARD_HIT:
    break;
  case BOARD_TIMEOUT:
    break;
  default:
    break;
  }
}

static void FSM_ChangeState(BoardState new_board_state) {
  FSM_ExitState(board.board_state);
  board.board_state = new_board_state;
  FSM_EnterState(new_board_state);
}

void FSM_Process() {
  switch (board.board_state) {
  case BOARD_IGNORE:
    BoardLight_Clear();
    RunningLight_Clear();
    FrameUpLight_Clear();
    FrameLight_Clear();
    FrameDownLight_Clear();
    if (board.center_request_code == CENTER_REQUEST_READY) {
      FSM_ChangeState(BOARD_READY);
    }
    break;
  case BOARD_READY:
    BoardLight_Draw_Cross();
    RunningLight_Draw_Arrow(HAL_GetTick() / 50);
    FrameUpLight_Draw_All();
    FrameDownLight_Draw_All();
    FrameLight_Draw_All();

    pressed_key = RingsKey_Scan();
    if (pressed_key) {
      board.score = pressed_key;
      FSM_ChangeState(BOARD_HIT);
      break;
    }

    if (board.center_request_code == CENTER_REQUEST_RESET) {
      FSM_ChangeState(BOARD_IGNORE);
      break;
    }
    break;
  case BOARD_HIT:
    FrameLight_Clear();
    FrameUpLight_Clear();
    FrameDownLight_Clear();
    RunningLight_Draw_All();
    BoardLight_Draw_Ring(board.score);

    if (board.center_request_code == CENTER_REQUEST_RESET) {
      FSM_ChangeState(BOARD_IGNORE);
    }
    break;
  default:
    break;
  }
}
