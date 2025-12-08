#ifndef __WS2812_H
#define __WS2812_H

#include "main.h"

void ws2812_Init(void);

void WS2812_SetRGB(uint8_t id, uint8_t r, uint8_t g, uint8_t b);

void WS2812_SetBatteryPercent(uint8_t color[3], float batPercent);

void WS2812_SetBatteryInfo(int8_t info);

void WS2812_SetPowerOnSequence(uint8_t sequence);

void WS2812_SetRockerSInfo(uint8_t s_l, uint8_t s_r);

#endif	//__WS2812_H
