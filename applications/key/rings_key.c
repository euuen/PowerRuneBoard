#include "rings_key.h"
#include "config.h"
#include <stdint.h>

// 第1个是第1环，以此递推到第9个是第9环
const GPIO_TypeDef *RingsKey_Port[9] = {
    KEY_1_PORT, KEY_2_PORT, KEY_3_PORT, KEY_4_PORT, KEY_5_PORT,
    KEY_6_PORT, KEY_7_PORT, KEY_8_PORT, KEY_9_PORT,
};

const uint16_t RingsKey_Pin[9] = {
    KEY_1_PIN, KEY_2_PIN, KEY_3_PIN, KEY_4_PIN, KEY_5_PIN,
    KEY_6_PIN, KEY_7_PIN, KEY_8_PIN, KEY_9_PIN,
};


/* 扫描全部靶环开关，返回第一个被压下的按键编号(1~9)，如果同时按下两个按键返回环数较高的按键编号，无按下返回
 * 0 */
uint8_t RingsKey_Scan(void) {
  uint8_t pressed_key = 0;
  for (int8_t i = 8; i >= 0; i--) {
    if (HAL_GPIO_ReadPin((GPIO_TypeDef *)RingsKey_Port[i], RingsKey_Pin[i]) ==
        GPIO_PIN_RESET) {
      pressed_key = i + 1; /* 随着环数增加，只记录环数较高的编号 */
    }
  }
  return pressed_key;
}