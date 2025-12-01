#ifndef __LIME_WS2812_H_
#define __LIME_WS2812_H_

#include "main.h"


void ws2812SyncFinishedHook(void);

void ws2812_Init(void);

uint8_t ws2812_SetColor(uint8_t r, uint8_t g, uint8_t b);


#endif /* __LIME_WS2812_H_ */