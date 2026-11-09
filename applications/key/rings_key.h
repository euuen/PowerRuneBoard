#include <stdint.h>
#include "main.h"

extern const GPIO_TypeDef *RingsKey_Port[9];
extern const uint16_t RingsKey_Pin[9];

/* 扫描全部靶环开关，返回第一个被压下的编号(1~10)，无按下返回 0 */
uint8_t RingsKey_Scan(void);
