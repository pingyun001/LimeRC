#include "ws2812.h"
#include "tim.h"

static uint32_t ws2812RawBuf[340 + 24 * 10] __attribute__((section("LIME_RAM_D4"), aligned(32))) = {0};
#define WS2812TL 		15
#define WS2812TS 		5

void WS2812_SetRGB(uint8_t id, uint8_t r, uint8_t g, uint8_t b)
{
	uint32_t index = 0;
	for(index = 0; index < 8; index ++)
	{
		ws2812RawBuf[id * 24 + index   ] = (g & (0x01 << (7 - index))) ? WS2812TL : WS2812TS;
		ws2812RawBuf[id * 24 + index +8] = (r & (0x01 << (7 - index))) ? WS2812TL : WS2812TS;
		ws2812RawBuf[id * 24 + index+16] = (b & (0x01 << (7 - index))) ? WS2812TL : WS2812TS;
	}
}

void ws2812_Init(void)
{
	WS2812_SetRGB(0, 0x00, 0x00, 0x07);
	WS2812_SetRGB(1, 0x00, 0x00, 0x07);
	WS2812_SetRGB(2, 0x00, 0x00, 0x07);
	WS2812_SetRGB(3, 0x00, 0x00, 0x07);
	WS2812_SetRGB(4, 0x00, 0x00, 0x07);
	WS2812_SetRGB(5, 0x00, 0x00, 0x07);
	WS2812_SetRGB(6, 0x00, 0x04, 0x00);
	WS2812_SetRGB(7, 0x00, 0x04, 0x00);
	WS2812_SetRGB(8, 0x00, 0x04, 0x00);
	WS2812_SetRGB(9, 0x00, 0x04, 0x00);
	
	HAL_TIM_PWM_Start_DMA(&htim5, TIM_CHANNEL_4, ws2812RawBuf, sizeof(ws2812RawBuf) / 4);
	DMA1_Stream0->CR &= ~(0x0000001E);
}

void WS2812_SetBatteryPercent(uint8_t color[3], float batPercent)
{
	if(batPercent < 1.0f)
	{
		WS2812_SetRGB(0, 0x00, 0x00, 0x00);
		WS2812_SetRGB(1, 0x00, 0x00, 0x00);
		WS2812_SetRGB(2, 0x00, 0x00, 0x00);
		WS2812_SetRGB(3, 0x00, 0x00, 0x00);
	}
	else if(batPercent < 25.0f)
	{
		WS2812_SetRGB(0, color[0], color[1], color[2]);
		WS2812_SetRGB(1, 0x00, 0x00, 0x00);
		WS2812_SetRGB(2, 0x00, 0x00, 0x00);
		WS2812_SetRGB(3, 0x00, 0x00, 0x00);
	}
	else if(batPercent < 50.0f)
	{
		WS2812_SetRGB(0, color[0], color[1], color[2]);
		WS2812_SetRGB(1, color[0], color[1], color[2]);
		WS2812_SetRGB(2, 0x00, 0x00, 0x00);
		WS2812_SetRGB(3, 0x00, 0x00, 0x00);
	}
	else if(batPercent < 75.0f)
	{
		WS2812_SetRGB(0, color[0], color[1], color[2]);
		WS2812_SetRGB(1, color[0], color[1], color[2]);
		WS2812_SetRGB(2, color[0], color[1], color[2]);
		WS2812_SetRGB(3, 0x00, 0x00, 0x00);
	}
	else
	{
		WS2812_SetRGB(0, color[0], color[1], color[2]);
		WS2812_SetRGB(1, color[0], color[1], color[2]);
		WS2812_SetRGB(2, color[0], color[1], color[2]);
		WS2812_SetRGB(3, color[0], color[1], color[2]);
	}
}

void WS2812_SetBatteryInfo(int8_t info)
{
	switch(info)
	{
		case -1:
		{
			WS2812_SetRGB(6, 0x00, 0x00, 0x03);
			WS2812_SetRGB(7, 0x00, 0x00, 0x03);
			WS2812_SetRGB(8, 0x00, 0x00, 0x03);
			WS2812_SetRGB(9, 0x00, 0x00, 0x03);
			break;
		}
		case 0:
		{
			WS2812_SetRGB(6, 0x00, 0x00, 0x00);
			WS2812_SetRGB(7, 0x00, 0x00, 0x00);
			WS2812_SetRGB(8, 0x00, 0x00, 0x00);
			WS2812_SetRGB(9, 0x03, 0x03, 0x00);
			break;
		}
		case 1:
		{
			WS2812_SetRGB(6, 0x00, 0x00, 0x00);
			WS2812_SetRGB(7, 0x00, 0x00, 0x00);
			WS2812_SetRGB(8, 0x00, 0x03, 0x00);
			WS2812_SetRGB(9, 0x00, 0x03, 0x00);
			break;
		}
		case 2:
		{
			WS2812_SetRGB(6, 0x00, 0x00, 0x00);
			WS2812_SetRGB(7, 0x00, 0x03, 0x00);
			WS2812_SetRGB(8, 0x00, 0x03, 0x00);
			WS2812_SetRGB(9, 0x00, 0x03, 0x00);
			break;
		}
		case 3:
		{
			WS2812_SetRGB(6, 0x00, 0x03, 0x00);
			WS2812_SetRGB(7, 0x00, 0x03, 0x00);
			WS2812_SetRGB(8, 0x00, 0x03, 0x00);
			WS2812_SetRGB(9, 0x00, 0x03, 0x00);
			break;
		}
	}
}
void WS2812_SetPowerOnSequence(uint8_t sequence)
{
	WS2812_SetRGB(0, 0x00, 0x00, 0x00);
	WS2812_SetRGB(1, 0x00, 0x00, 0x00);
	WS2812_SetRGB(2, 0x00, 0x00, 0x00);
	WS2812_SetRGB(3, 0x00, 0x00, 0x00);
	WS2812_SetRGB(4, 0x00, 0x00, 0x00);
	WS2812_SetRGB(5, 0x00, 0x00, 0x00);
	
	switch(sequence)
	{
		case 0:
		{
			WS2812_SetRGB(6, 0x00, 0x00, 0x00);
			WS2812_SetRGB(7, 0x00, 0x00, 0x00);
			WS2812_SetRGB(8, 0x00, 0x00, 0x00);
			WS2812_SetRGB(9, 0x00, 0x00, 0x00);
			break;
		}
		case 1:
		{
			WS2812_SetRGB(6, 0x00, 0x00, 0x00);
			WS2812_SetRGB(7, 0x00, 0x00, 0x00);
			WS2812_SetRGB(8, 0x00, 0x00, 0x00);
			WS2812_SetRGB(9, 0x00, 0x04, 0x02);
			break;
		}
		case 2:
		{
			WS2812_SetRGB(6, 0x00, 0x00, 0x00);
			WS2812_SetRGB(7, 0x00, 0x00, 0x00);
			WS2812_SetRGB(8, 0x00, 0x04, 0x02);
			WS2812_SetRGB(9, 0x00, 0x04, 0x02);
			break;
		}
		case 3:
		{
			WS2812_SetRGB(6, 0x00, 0x00, 0x00);
			WS2812_SetRGB(7, 0x00, 0x04, 0x02);
			WS2812_SetRGB(8, 0x00, 0x04, 0x02);
			WS2812_SetRGB(9, 0x00, 0x04, 0x02);
			break;
		}
		case 4:
		{
			WS2812_SetRGB(6, 0x00, 0x04, 0x02);
			WS2812_SetRGB(7, 0x00, 0x04, 0x02);
			WS2812_SetRGB(8, 0x00, 0x04, 0x02);
			WS2812_SetRGB(9, 0x00, 0x04, 0x02);
			break;
		}
	}
}
void WS2812_SetRockerSInfo(uint8_t s_l, uint8_t s_r)
{
	switch(s_l)
	{
		case 1:
		{
			WS2812_SetRGB(3, 0x00, 0x04, 0x02);
			WS2812_SetRGB(4, 0x00, 0x00, 0x00);
			WS2812_SetRGB(5, 0x00, 0x00, 0x00);
			break;
		}
		case 3:
		{
			WS2812_SetRGB(3, 0x00, 0x00, 0x00);
			WS2812_SetRGB(4, 0x00, 0x04, 0x02);
			WS2812_SetRGB(5, 0x00, 0x00, 0x00);
			break;
		}
		case 2:
		{
			WS2812_SetRGB(3, 0x00, 0x00, 0x00);
			WS2812_SetRGB(4, 0x00, 0x00, 0x00);
			WS2812_SetRGB(5, 0x00, 0x04, 0x02);
			break;
		}
		default:
		{
			WS2812_SetRGB(3, 0x08, 0x00, 0x00);
			WS2812_SetRGB(4, 0x08, 0x00, 0x00);
			WS2812_SetRGB(5, 0x08, 0x00, 0x00);
		}
	}
	switch(s_r)
	{
		case 1:
		{
			WS2812_SetRGB(2, 0x00, 0x04, 0x02);
			WS2812_SetRGB(1, 0x00, 0x00, 0x00);
			WS2812_SetRGB(0, 0x00, 0x00, 0x00);
			break;
		}
		case 3:
		{
			WS2812_SetRGB(2, 0x00, 0x00, 0x00);
			WS2812_SetRGB(1, 0x00, 0x04, 0x02);
			WS2812_SetRGB(0, 0x00, 0x00, 0x00);
			break;
		}
		case 2:
		{
			WS2812_SetRGB(2, 0x00, 0x00, 0x00);
			WS2812_SetRGB(1, 0x00, 0x00, 0x00);
			WS2812_SetRGB(0, 0x00, 0x04, 0x02);
			break;
		}
		default:
		{
			WS2812_SetRGB(2, 0x08, 0x00, 0x00);
			WS2812_SetRGB(1, 0x08, 0x00, 0x00);
			WS2812_SetRGB(0, 0x08, 0x00, 0x00);
		}
	}
}
