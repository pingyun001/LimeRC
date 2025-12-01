#include "Lime_ws2812.h"
#include "tim.h"
#include "stdio.h"
#include "string.h"

static uint32_t ws2812RawBuf[24+300] = {0};
static volatile uint8_t syncHasFinished = 1;

static void WS2812_SetRGB(uint8_t r, uint8_t g, uint8_t b);

#define WS2812TL 		63
#define WS2812TS 		21
#define WS2812TH 		1

void ws2812SyncFinishedHook(void)
{
    syncHasFinished = 1;
}


static void WS2812_SetRGB(uint8_t r, uint8_t g, uint8_t b)
{
	uint32_t index = 0;
	for(index = 0; index < 8; index ++)
	{
		ws2812RawBuf[index +1] = (g & (0x01 << (7 - index))) ? WS2812TL : WS2812TS;
		ws2812RawBuf[index +9] = (r & (0x01 << (7 - index))) ? WS2812TL : WS2812TS;
		ws2812RawBuf[index+17] = (b & (0x01 << (7 - index))) ? WS2812TL : WS2812TS;
	}
    ws2812RawBuf[25] = WS2812TS;
}

void ws2812_Init(void)
{
	HAL_TIM_PWM_Start_DMA(&htim2, TIM_CHANNEL_2, ws2812RawBuf, sizeof(ws2812RawBuf) / 4);
}

uint8_t ws2812_SetColor(uint8_t r, uint8_t g, uint8_t b)
{
    WS2812_SetRGB(r, g, b);

    if(syncHasFinished)
    {
        syncHasFinished = 0;
        
        return 1;
    }
    else
    {
        return 0;           //sync failed, DMA is busy.
    }
    
}








