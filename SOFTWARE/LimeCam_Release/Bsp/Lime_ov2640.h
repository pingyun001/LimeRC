#ifndef __LIME_OV2640_H
#define __LIME_OV2640_H

#include "main.h"


//----------------------------------寄存器组---------------------------------------------
#define 	OV2640_SENSOR_COM7        0x12	// 公共控制,系统复位、摄像头分辨率选择、缩放模式、颜色彩条设置 
#define 	OV2640_SENSOR_REG04       0x04	// 寄存器组4,可设置摄像头扫描方向等
#define  OV2640_SENSOR_PIDH         0x0a	// ID高字节
#define  OV2640_SENSOR_PIDL         0x0b	// ID低字节

#define  OV2640_SEL_Registers       0xFF	// 寄存器组选择寄存器
#define  LIME_OV2640_SEL_DSP        0x00	// 设置为0x00时，选择  DSP    寄存器组
#define  LIME_OV2640_SEL_SENSOR     0x01	// 设置为0x01时，选择  SENSOR 寄存器组

//----------------------------------枚举--------------------------------------------------
/* Lime_ov2640输出图像格式枚举 */
typedef enum
{
    LIME_OV2640_FORMAT_RGB565 = 0x00,     /* RGB565 */
    LIME_OV2640_FORMAT_JPEG,              /* JPEG */
} Lime_ov2640_output_format_e;

/* ATK-MC2640模块灯光模式枚举 */
typedef enum
{
    LIME_OV2640_LIGHT_MODE_AUTO = 0x00,          /* Auto */
    LIME_OV2640_LIGHT_MODE_SUNNY,                /* Sunny */
    LIME_OV2640_LIGHT_MODE_CLOUDY,               /* Cloudy */
    LIME_OV2640_LIGHT_MODE_OFFICE,               /* Office */
    LIME_OV2640_LIGHT_MODE_HOME,                 /* Home */
} Lime_ov2640_light_mode_e;

/* ATK-MC2640模块色彩饱和度枚举 */
typedef enum
{
    LIME_OV2640_COLOR_SATURATION_0 = 0x00,       /* +2 */
    LIME_OV2640_COLOR_SATURATION_1,              /* +1 */
    LIME_OV2640_COLOR_SATURATION_2,              /* 0 */
    LIME_OV2640_COLOR_SATURATION_3,              /* -1 */
    LIME_OV2640_COLOR_SATURATION_4,              /* -2 */
} Lime_ov2640_color_saturation_t;

/* ATK-MC2640模块亮度枚举 */
typedef enum
{
    LIME_OV2640_BRIGHTNESS_0 = 0x00,             /* +2 */
    LIME_OV2640_BRIGHTNESS_1,                    /* +1 */
    LIME_OV2640_BRIGHTNESS_2,                    /* 0 */
    LIME_OV2640_BRIGHTNESS_3,                    /* -1 */
    LIME_OV2640_BRIGHTNESS_4,                    /* -2 */
} Lime_ov2640_brightness_t;

/* ATK-MC2640模块对比度枚举 */
typedef enum
{
    LIME_OV2640_CONTRAST_0 = 0x00,               /* +2 */
    LIME_OV2640_CONTRAST_1,                      /* +1 */
    LIME_OV2640_CONTRAST_2,                      /* 0 */
    LIME_OV2640_CONTRAST_3,                      /* -1 */
    LIME_OV2640_CONTRAST_4,                      /* -2 */
} Lime_ov2640_contrast_t;

/* ATK-MC2640模块特殊效果枚举 */
typedef enum
{
    LIME_OV2640_SPECIAL_EFFECT_ANTIQUE = 0x00,   /* Antique */
    LIME_OV2640_SPECIAL_EFFECT_BLUISH,           /* Bluish */
    LIME_OV2640_SPECIAL_EFFECT_GREENISH,         /* Greenish */
    LIME_OV2640_SPECIAL_EFFECT_REDISH,           /* Redish */
    LIME_OV2640_SPECIAL_EFFECT_BW,               /* B&W */
    LIME_OV2640_SPECIAL_EFFECT_NEGATIVE,         /* Negative */
    LIME_OV2640_SPECIAL_EFFECT_BW_NEGATIVE,      /* B&W Negative */
    LIME_OV2640_SPECIAL_EFFECT_NORMAL,           /* Normal */
} Lime_ov2640_special_effect_t;



//----------------------------------移植--------------------------------------------------
#define OV2640_DELAY        HAL_Delay                           //这里使用HAL库的延时函数

#define OV2640_PWDN_PIN            			 GPIO_PIN_13        		// PWDN 引脚      
#define OV2640_PWDN_PORT           			 GPIOD                 	// PWDN GPIO端口     
//#define GPIO_OV2640_PWDN_CLK_ENABLE    	__HAL_RCC_GPIOD_CLK_ENABLE() 		// PWDN GPIO端口时钟

                                                                // 低电平，不开启掉电模式，摄像头正常工作
#define	OV2640_PWDN_OFF	HAL_GPIO_WritePin(OV2640_PWDN_PORT, OV2640_PWDN_PIN, GPIO_PIN_RESET)	

                                                                // 高电平，进入掉电模式，摄像头停止工作，此时功耗降到最低
#define 	OV2640_PWDN_ON		HAL_GPIO_WritePin(OV2640_PWDN_PORT, OV2640_PWDN_PIN, GPIO_PIN_SET)	



//----------------------------------对外接口--------------------------------------------------
uint8_t Lime_ov2640_Init(void);                                 //OV2640初始化
void Lime_ov2640_Read_Picture_Size(void);                       //读取OV2640内部DSP输出图像尺寸
void Lime_ov2640_ParaInit(const uint8_t (*ConfigData)[2]);      //向OV2640填入默认参数
uint8_t Lime_ov2640_set_output_format(Lime_ov2640_output_format_e format);
uint8_t Lime_ov2640_set_output_size(uint16_t width, uint16_t height);
uint8_t Lime_ov2640_set_light_mode(Lime_ov2640_light_mode_e mode);
uint8_t Lime_ov2640_set_color_saturation(Lime_ov2640_color_saturation_t saturation);
uint8_t Lime_ov2640_set_brightness(Lime_ov2640_brightness_t brightness);
uint8_t Lime_ov2640_set_brightness(Lime_ov2640_brightness_t brightness);
uint8_t Lime_ov2640_set_contrast(Lime_ov2640_contrast_t contrast);
uint8_t Lime_ov2640_set_special_effect(Lime_ov2640_special_effect_t effect);



#endif    //__LIME_OV2640_H
