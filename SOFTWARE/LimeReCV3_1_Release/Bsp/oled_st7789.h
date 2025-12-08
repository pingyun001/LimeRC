#ifndef __OLED_ST7789_H
#define __OLED_ST7789_H

#include "main.h"
#include "spi.h"

#define	LCD_PWR(n)		(n?\
						__HAL_TIM_SET_COMPARE(&htim15, TIM_CHANNEL_1, 1000):\
						__HAL_TIM_SET_COMPARE(&htim15, TIM_CHANNEL_1, 0))
#define	LCD_WR_RS(n)	(n?\
						HAL_GPIO_WritePin(OLED_DC_GPIO_Port,OLED_DC_Pin,GPIO_PIN_SET):\
						HAL_GPIO_WritePin(OLED_DC_GPIO_Port,OLED_DC_Pin,GPIO_PIN_RESET))
#define	LCD_RST(n)		(n?\
						HAL_GPIO_WritePin(OLED_RES_GPIO_Port,OLED_RES_Pin,GPIO_PIN_SET):\
						HAL_GPIO_WritePin(OLED_RES_GPIO_Port,OLED_RES_Pin,GPIO_PIN_RESET))


//LCD屏幕分辨率定义
#define LCD_Width   240
#define LCD_Height  240
//颜色定义
#define WHITE   0xFFFF	//白色
#define YELLOW  0xFFE0	//黄色
#define BRRED   0XFC07  //棕红色
#define PINK    0XF81F	//粉色
#define RED     0xF800	//红色
#define BROWN   0XBC40  //棕色
#define GRAY    0X8430  //灰色
#define GBLUE   0X07FF	//兰色
#define GREEN   0x07E0	//绿色
#define BLUE    0x001F  //蓝色
#define BLACK   0x0000	//黑色


void oled_st7789_Init(void);
void oled_st7789_Clear(uint16_t color);

//对接lvgl
void oled_st7789_ColorFill(uint16_t x1,uint16_t y1,uint16_t x2,uint16_t y2,uint16_t* color_p);

void oled_st7789_AddressSet(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2);

void oled_st7789_DmaFinishHook(void);

void oled_st7789_setBackLight(uint8_t percent);

#endif  //__OLED_ST7789_H
