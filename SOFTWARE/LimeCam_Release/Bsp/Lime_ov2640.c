#include "Lime_ov2640.h"
#include "Lime_sccb.h"
#include "Lime_ov2640map.h"
#include "dcmi.h"
#include "dma.h"
#include "stdio.h"

#if ((1) && GLOBAL_DEBUG_LOG_EN)
	#define DEBUG_LOG(...)	printf(__VA_ARGS__)
#else
	#define DEBUG_LOG(...)
#endif

void Lime_ov2640_Reset(void)
{
	OV2640_DELAY(5);  // 等待模块上电稳定，最少5ms，然后拉低PWDN  	
	
	OV2640_PWDN_OFF;  // PWDN 引脚输出低电平，不开启掉电模式，摄像头正常工作，此时摄像头模块的白色LED会点亮
  
// 根据OV2640的上电时序，硬件复位的持续时间要>=3ms，反客的OV2640采用硬件RC复位，持续时间大概在6ms左右
// 因此加入延时，等待硬件复位完成并稳定下来
	OV2640_DELAY(5);    
	
	Lime_sccb_WriteReg( LIME_OV2640_REG_BANK_SEL, LIME_OV2640_SEL_SENSOR);   // 选择 SENSOR 寄存器组
	Lime_sccb_WriteReg( OV2640_SENSOR_COM7, 0x80);                  // 启动软件复位

// 根据OV2640的软件复位时序，软件复位执行后，要>=2ms方可执行SCCB配置，此处采用保守一点的参数，延时10ms
	OV2640_DELAY(10);    
}

static uint16_t Lime_ov2640_get_MID(void)
{
  uint16_t mid;

  Lime_sccb_WriteReg(LIME_OV2640_REG_BANK_SEL, LIME_OV2640_SEL_SENSOR);                   //选择SENSOR
  mid = Lime_sccb_ReadReg(LIME_OV2640_REG_SENSOR_MIDH) << 8;
  mid |= Lime_sccb_ReadReg(LIME_OV2640_REG_SENSOR_MIDL);
  return mid;
}

static uint16_t Lime_ov2640_ReadID(void)
{
  uint8_t PID_H,PID_L;
  
  Lime_sccb_WriteReg( OV2640_SEL_Registers, LIME_OV2640_SEL_SENSOR);                      // 选择 SENSOR 寄存器组

  PID_H = Lime_sccb_ReadReg(OV2640_SENSOR_PIDH); // 读取ID高字节
  PID_L = Lime_sccb_ReadReg(OV2640_SENSOR_PIDL); // 读取ID低字节
	
	return(PID_H<<8)|PID_L; // 返回完整的器件ID
}



void Lime_ov2640_ParaInit(const uint8_t (*ConfigData)[2])
{
   uint32_t i; // 计数变量

	for( i=0;ConfigData[i][0] ; i++)
	{
		Lime_sccb_WriteReg( ConfigData[i][0], ConfigData[i][1]);  // 进行参数配置   	
	} 
  
//  Lime_ov2640_Read_Picture_Size();              //读取并显示图像尺寸，确认初始化成功
}

uint8_t Lime_ov2640_set_output_format(Lime_ov2640_output_format_e format)
{
  uint32_t cfg_index;
  
  switch (format)
  {
    case LIME_OV2640_FORMAT_RGB565:
    {
      for (cfg_index=0; cfg_index<(sizeof(atk_mc2640_set_rgb565_cfg)/sizeof(atk_mc2640_set_rgb565_cfg[0])); cfg_index++)
      {
        Lime_sccb_WriteReg(atk_mc2640_set_rgb565_cfg[cfg_index][0], atk_mc2640_set_rgb565_cfg[cfg_index][1]);
      }
      break;
    }
    case LIME_OV2640_FORMAT_JPEG:
    {
      for (cfg_index=0; cfg_index<(sizeof(atk_mc2640_set_yuv422_cfg)/sizeof(atk_mc2640_set_yuv422_cfg[0])); cfg_index++)
      {
        Lime_sccb_WriteReg(atk_mc2640_set_yuv422_cfg[cfg_index][0], atk_mc2640_set_yuv422_cfg[cfg_index][1]);
      }
      for (cfg_index=0; cfg_index<(sizeof(atk_mc2640_set_jpeg_cfg)/sizeof(atk_mc2640_set_jpeg_cfg[0])); cfg_index++)
      {
        Lime_sccb_WriteReg(atk_mc2640_set_jpeg_cfg[cfg_index][0], atk_mc2640_set_jpeg_cfg[cfg_index][1]);
      }
      break;
    }
    default:
    {
      return 1;
    }
  }
  
  return 0;
}

uint8_t Lime_ov2640_set_output_size(uint16_t width, uint16_t height)
{
  uint16_t output_width;
  uint16_t output_height;
  
  if (((width & (4 - 1)) != 0) || ((height & (4 - 1)) != 0))
  {
    DEBUG_LOG("error para in File:%s line:%d",__FILE__, __LINE__);
    return 1;
  }
  
  output_width = width >> 2;
  output_height = height >> 2;
  
  Lime_sccb_WriteReg( OV2640_SEL_Registers, LIME_OV2640_SEL_DSP);                      // 选择 DSP 寄存器组
  Lime_sccb_WriteReg(LIME_OV2640_REG_DSP_RESET, 0x04);
  Lime_sccb_WriteReg(LIME_OV2640_REG_DSP_ZMOW, (uint8_t)(output_width & 0x00FF));
  Lime_sccb_WriteReg(LIME_OV2640_REG_DSP_ZMOH, (uint8_t)(output_height & 0x00FF));
  Lime_sccb_WriteReg(LIME_OV2640_REG_DSP_ZMHH, ((uint8_t)(output_width >> 8) & 0x03) | ((uint8_t)(output_height >> 6) & 0x04));
  Lime_sccb_WriteReg(LIME_OV2640_REG_DSP_RESET, 0x00);
  
  return 0;
}

void Lime_ov2640_Read_Picture_Size(void)
{
  uint8_t zmow;
  uint8_t zmoh;
  uint8_t zmhh;
  uint16_t width;
  uint16_t height;
  
  Lime_sccb_WriteReg( OV2640_SEL_Registers, LIME_OV2640_SEL_DSP);
  zmow = Lime_sccb_ReadReg(LIME_OV2640_REG_DSP_ZMOW);
  zmoh = Lime_sccb_ReadReg(LIME_OV2640_REG_DSP_ZMOH);
  zmhh = Lime_sccb_ReadReg(LIME_OV2640_REG_DSP_ZMHH);
  
  width = ((uint16_t)zmow | ((zmhh & 0x03) << 8)) << 2;
  height = ((uint16_t)zmoh | ((zmhh & 0x04) << 6)) << 2;
  
  DEBUG_LOG("%s(): width:%d,height:%d\r\n",__func__, width,height);
}

uint8_t Lime_ov2640_set_light_mode(Lime_ov2640_light_mode_e mode)
{
    switch (mode)
    {
        case LIME_OV2640_LIGHT_MODE_AUTO:
        {
            Lime_sccb_WriteReg( OV2640_SEL_Registers, LIME_OV2640_SEL_DSP);
            Lime_sccb_WriteReg(0xC7, 0x00);
            break;
        }
        case LIME_OV2640_LIGHT_MODE_SUNNY:
        {
            Lime_sccb_WriteReg( OV2640_SEL_Registers, LIME_OV2640_SEL_DSP);
            Lime_sccb_WriteReg(0xC7, 0x40);
            Lime_sccb_WriteReg(0xCC, 0x5E);
            Lime_sccb_WriteReg(0xCD, 0x41);
            Lime_sccb_WriteReg(0xCE, 0x54);
            break;
        }
        case LIME_OV2640_LIGHT_MODE_CLOUDY:
        {
            Lime_sccb_WriteReg( OV2640_SEL_Registers, LIME_OV2640_SEL_DSP);
            Lime_sccb_WriteReg(0xC7, 0x40);
            Lime_sccb_WriteReg(0xCC, 0x65);
            Lime_sccb_WriteReg(0xCD, 0x41);
            Lime_sccb_WriteReg(0xCE, 0x4F);
            break;
        }
        case LIME_OV2640_LIGHT_MODE_OFFICE:
        {
            Lime_sccb_WriteReg( OV2640_SEL_Registers, LIME_OV2640_SEL_DSP);
            Lime_sccb_WriteReg(0xC7, 0x40);
            Lime_sccb_WriteReg(0xCC, 0x52);
            Lime_sccb_WriteReg(0xCD, 0x41);
            Lime_sccb_WriteReg(0xCE, 0x66);
            break;
        }
        case LIME_OV2640_LIGHT_MODE_HOME:
        {
            Lime_sccb_WriteReg( OV2640_SEL_Registers, LIME_OV2640_SEL_DSP);
            Lime_sccb_WriteReg(0xC7, 0x40);
            Lime_sccb_WriteReg(0xCC, 0x42);
            Lime_sccb_WriteReg(0xCD, 0x3F);
            Lime_sccb_WriteReg(0xCE, 0x71);
            break;
        }
        default:
        {
            return 2;
        }
    }
    
    return 0;
}

uint8_t Lime_ov2640_set_color_saturation(Lime_ov2640_color_saturation_t saturation)
{
    switch (saturation)
    {
        case LIME_OV2640_COLOR_SATURATION_0:
        {
            Lime_sccb_WriteReg( OV2640_SEL_Registers, LIME_OV2640_SEL_DSP);
            Lime_sccb_WriteReg(0x7C, 0x00);
            Lime_sccb_WriteReg(0x7D, 0x02);
            Lime_sccb_WriteReg(0x7C, 0x03);
            Lime_sccb_WriteReg(0x7D, 0x68);
            Lime_sccb_WriteReg(0x7D, 0x68);
            break;
        }
        case LIME_OV2640_COLOR_SATURATION_1:
        {
            Lime_sccb_WriteReg( OV2640_SEL_Registers, LIME_OV2640_SEL_DSP);
            Lime_sccb_WriteReg(0x7C, 0x00);
            Lime_sccb_WriteReg(0x7D, 0x02);
            Lime_sccb_WriteReg(0x7C, 0x03);
            Lime_sccb_WriteReg(0x7D, 0x58);
            Lime_sccb_WriteReg(0x7D, 0x58);
            break;
        }
        case LIME_OV2640_COLOR_SATURATION_2:
        {
            Lime_sccb_WriteReg( OV2640_SEL_Registers, LIME_OV2640_SEL_DSP);
            Lime_sccb_WriteReg(0x7C, 0x00);
            Lime_sccb_WriteReg(0x7D, 0x02);
            Lime_sccb_WriteReg(0x7C, 0x03);
            Lime_sccb_WriteReg(0x7D, 0x48);
            Lime_sccb_WriteReg(0x7D, 0x48);
            break;
        }
        case LIME_OV2640_COLOR_SATURATION_3:
        {
            Lime_sccb_WriteReg( OV2640_SEL_Registers, LIME_OV2640_SEL_DSP);
            Lime_sccb_WriteReg(0x7C, 0x00);
            Lime_sccb_WriteReg(0x7D, 0x02);
            Lime_sccb_WriteReg(0x7C, 0x03);
            Lime_sccb_WriteReg(0x7D, 0x38);
            Lime_sccb_WriteReg(0x7D, 0x38);
            break;
        }
        case LIME_OV2640_COLOR_SATURATION_4:
        {
            Lime_sccb_WriteReg( OV2640_SEL_Registers, LIME_OV2640_SEL_DSP);
            Lime_sccb_WriteReg(0x7C, 0x00);
            Lime_sccb_WriteReg(0x7D, 0x02);
            Lime_sccb_WriteReg(0x7C, 0x03);
            Lime_sccb_WriteReg(0x7D, 0x28);
            Lime_sccb_WriteReg(0x7D, 0x28);
            break;
        }
        default:
        {
            return 2;
        }
    }
    
    return 0;
}

/**
 * @brief       设置ATK-MC2640模块亮度
 * @param       brightness: ATK_MC2640_BRIGHTNESS_0: +2
 *                          ATK_MC2640_BRIGHTNESS_1: +1
 *                          ATK_MC2640_BRIGHTNESS_2: 0
 *                          ATK_MC2640_BRIGHTNESS_3: -1
 *                          ATK_MC2640_BRIGHTNESS_4: -2
 * @retval      ATK_MC2640_EOK   : 设置ATK-MC2640模块亮度成功
 *              ATK_MC2640_EINVAL: 传入参数错误
 */
uint8_t Lime_ov2640_set_brightness(Lime_ov2640_brightness_t brightness)
{
    switch (brightness)
    {
        case LIME_OV2640_BRIGHTNESS_0:
        {
            Lime_sccb_WriteReg( OV2640_SEL_Registers, LIME_OV2640_SEL_DSP);
            Lime_sccb_WriteReg(0x7C, 0x00);
            Lime_sccb_WriteReg(0x7D, 0x04);
            Lime_sccb_WriteReg(0x7C, 0x09);
            Lime_sccb_WriteReg(0x7D, 0x40);
            Lime_sccb_WriteReg(0x7D, 0x00);
            break;
        }
        case LIME_OV2640_BRIGHTNESS_1:
        {
            Lime_sccb_WriteReg( OV2640_SEL_Registers, LIME_OV2640_SEL_DSP);
            Lime_sccb_WriteReg(0x7C, 0x00);
            Lime_sccb_WriteReg(0x7D, 0x04);
            Lime_sccb_WriteReg(0x7C, 0x09);
            Lime_sccb_WriteReg(0x7D, 0x30);
            Lime_sccb_WriteReg(0x7D, 0x00);
            break;
        }
        case LIME_OV2640_BRIGHTNESS_2:
        {
            Lime_sccb_WriteReg( OV2640_SEL_Registers, LIME_OV2640_SEL_DSP);
            Lime_sccb_WriteReg(0x7C, 0x00);
            Lime_sccb_WriteReg(0x7D, 0x04);
            Lime_sccb_WriteReg(0x7C, 0x09);
            Lime_sccb_WriteReg(0x7D, 0x20);
            Lime_sccb_WriteReg(0x7D, 0x00);
            break;
        }
        case LIME_OV2640_BRIGHTNESS_3:
        {
            Lime_sccb_WriteReg( OV2640_SEL_Registers, LIME_OV2640_SEL_DSP);
            Lime_sccb_WriteReg(0x7C, 0x00);
            Lime_sccb_WriteReg(0x7D, 0x04);
            Lime_sccb_WriteReg(0x7C, 0x09);
            Lime_sccb_WriteReg(0x7D, 0x10);
            Lime_sccb_WriteReg(0x7D, 0x00);
            break;
        }
        case LIME_OV2640_BRIGHTNESS_4:
        {
            Lime_sccb_WriteReg( OV2640_SEL_Registers, LIME_OV2640_SEL_DSP);
            Lime_sccb_WriteReg(0x7C, 0x00);
            Lime_sccb_WriteReg(0x7D, 0x04);
            Lime_sccb_WriteReg(0x7C, 0x09);
            Lime_sccb_WriteReg(0x7D, 0x00);
            Lime_sccb_WriteReg(0x7D, 0x00);
            break;
        }
        default:
        {
            return 2;
        }
    }
    
    return 0;
}

/**
 * @brief       设置ATK-MC2640模块对比度
 * @param       contrast: ATK_MC2640_CONTRAST_0: +2
 *                        ATK_MC2640_CONTRAST_1: +1
 *                        ATK_MC2640_CONTRAST_2: 0
 *                        ATK_MC2640_CONTRAST_3: -1
 *                        ATK_MC2640_CONTRAST_4: -2
 * @retval      ATK_MC2640_EOK   : 设置ATK-MC2640模块对比度成功
 *              ATK_MC2640_EINVAL: 传入参数错误
 */
uint8_t Lime_ov2640_set_contrast(Lime_ov2640_contrast_t contrast)
{
    switch (contrast)
    {
        case LIME_OV2640_CONTRAST_0:
        {
            Lime_sccb_WriteReg( OV2640_SEL_Registers, LIME_OV2640_SEL_DSP);
            Lime_sccb_WriteReg(0x7C, 0x00);
            Lime_sccb_WriteReg(0x7D, 0x04);
            Lime_sccb_WriteReg(0x7C, 0x07);
            Lime_sccb_WriteReg(0x7D, 0x20);
            Lime_sccb_WriteReg(0x7D, 0x28);
            Lime_sccb_WriteReg(0x7D, 0x0C);
            Lime_sccb_WriteReg(0x7D, 0x06);
            break;
        }
        case LIME_OV2640_CONTRAST_1:
        {
            Lime_sccb_WriteReg( OV2640_SEL_Registers, LIME_OV2640_SEL_DSP);
            Lime_sccb_WriteReg(0x7C, 0x00);
            Lime_sccb_WriteReg(0x7D, 0x04);
            Lime_sccb_WriteReg(0x7C, 0x07);
            Lime_sccb_WriteReg(0x7D, 0x20);
            Lime_sccb_WriteReg(0x7D, 0x24);
            Lime_sccb_WriteReg(0x7D, 0x16);
            Lime_sccb_WriteReg(0x7D, 0x06);
            break;
        }
        case LIME_OV2640_CONTRAST_2:
        {
            Lime_sccb_WriteReg( OV2640_SEL_Registers, LIME_OV2640_SEL_DSP);
            Lime_sccb_WriteReg(0x7C, 0x00);
            Lime_sccb_WriteReg(0x7D, 0x04);
            Lime_sccb_WriteReg(0x7C, 0x07);
            Lime_sccb_WriteReg(0x7D, 0x20);
            Lime_sccb_WriteReg(0x7D, 0x20);
            Lime_sccb_WriteReg(0x7D, 0x20);
            Lime_sccb_WriteReg(0x7D, 0x06);
            break;
        }
        case LIME_OV2640_CONTRAST_3:
        {
            Lime_sccb_WriteReg( OV2640_SEL_Registers, LIME_OV2640_SEL_DSP);
            Lime_sccb_WriteReg(0x7C, 0x00);
            Lime_sccb_WriteReg(0x7D, 0x04);
            Lime_sccb_WriteReg(0x7C, 0x07);
            Lime_sccb_WriteReg(0x7D, 0x20);
            Lime_sccb_WriteReg(0x7D, 0x1C);
            Lime_sccb_WriteReg(0x7D, 0x2A);
            Lime_sccb_WriteReg(0x7D, 0x06);
            break;
        }
        case LIME_OV2640_CONTRAST_4:
        {
            Lime_sccb_WriteReg( OV2640_SEL_Registers, LIME_OV2640_SEL_DSP);
            Lime_sccb_WriteReg(0x7C, 0x00);
            Lime_sccb_WriteReg(0x7D, 0x04);
            Lime_sccb_WriteReg(0x7C, 0x07);
            Lime_sccb_WriteReg(0x7D, 0x20);
            Lime_sccb_WriteReg(0x7D, 0x18);
            Lime_sccb_WriteReg(0x7D, 0x34);
            Lime_sccb_WriteReg(0x7D, 0x06);
            break;
        }
        default:
        {
            return 2;
        }
    }
    
    return 0;
}


/**
 * @brief       设置ATK-MC2640模块特殊效果
 * @param       contrast: ATK_MC2640_SPECIAL_EFFECT_ANTIQUE    : Antique
 *                        ATK_MC2640_SPECIAL_EFFECT_BLUISH     : Bluish
 *                        ATK_MC2640_SPECIAL_EFFECT_GREENISH   : Greenish
 *                        ATK_MC2640_SPECIAL_EFFECT_REDISH     : Redish
 *                        ATK_MC2640_SPECIAL_EFFECT_BW         : B&W
 *                        ATK_MC2640_SPECIAL_EFFECT_NEGATIVE   : Negative
 *                        ATK_MC2640_SPECIAL_EFFECT_BW_NEGATIVE: B&W Negative
 *                        ATK_MC2640_SPECIAL_EFFECT_NORMAL     : Normal
 * @retval      ATK_MC2640_EOK   : 设置ATK-MC2640模块特殊效果成功
 *              ATK_MC2640_EINVAL: 传入参数错误
 */
uint8_t Lime_ov2640_set_special_effect(Lime_ov2640_special_effect_t effect)
{
    switch (effect)
    {
        case LIME_OV2640_SPECIAL_EFFECT_ANTIQUE:
        {
            Lime_sccb_WriteReg( OV2640_SEL_Registers, LIME_OV2640_SEL_DSP);
            Lime_sccb_WriteReg(0x7C, 0x00);
            Lime_sccb_WriteReg(0x7D, 0x18);
            Lime_sccb_WriteReg(0x7C, 0x05);
            Lime_sccb_WriteReg(0x7D, 0x40);
            Lime_sccb_WriteReg(0x7D, 0xA6);
            break;
        }
        case LIME_OV2640_SPECIAL_EFFECT_BLUISH:
        {
            Lime_sccb_WriteReg( OV2640_SEL_Registers, LIME_OV2640_SEL_DSP);
            Lime_sccb_WriteReg(0x7C, 0x00);
            Lime_sccb_WriteReg(0x7D, 0x18);
            Lime_sccb_WriteReg(0x7C, 0x05);
            Lime_sccb_WriteReg(0x7D, 0xA0);
            Lime_sccb_WriteReg(0x7D, 0x40);
            break;
        }
        case LIME_OV2640_SPECIAL_EFFECT_GREENISH:
        {
            Lime_sccb_WriteReg( OV2640_SEL_Registers, LIME_OV2640_SEL_DSP);
            Lime_sccb_WriteReg(0x7C, 0x00);
            Lime_sccb_WriteReg(0x7D, 0x18);
            Lime_sccb_WriteReg(0x7C, 0x05);
            Lime_sccb_WriteReg(0x7D, 0x40);
            Lime_sccb_WriteReg(0x7D, 0x40);
            break;
        }
        case LIME_OV2640_SPECIAL_EFFECT_REDISH:
        {
            Lime_sccb_WriteReg( OV2640_SEL_Registers, LIME_OV2640_SEL_DSP);
            Lime_sccb_WriteReg(0x7C, 0x00);
            Lime_sccb_WriteReg(0x7D, 0x18);
            Lime_sccb_WriteReg(0x7C, 0x05);
            Lime_sccb_WriteReg(0x7D, 0x40);
            Lime_sccb_WriteReg(0x7D, 0xC0);
            break;
        }
        case LIME_OV2640_SPECIAL_EFFECT_BW:
        {
            Lime_sccb_WriteReg( OV2640_SEL_Registers, LIME_OV2640_SEL_DSP);
            Lime_sccb_WriteReg(0x7C, 0x00);
            Lime_sccb_WriteReg(0x7D, 0x18);
            Lime_sccb_WriteReg(0x7C, 0x05);
            Lime_sccb_WriteReg(0x7D, 0x80);
            Lime_sccb_WriteReg(0x7D, 0x80);
            break;
        }
        case LIME_OV2640_SPECIAL_EFFECT_NEGATIVE:
        {
            Lime_sccb_WriteReg( OV2640_SEL_Registers, LIME_OV2640_SEL_DSP);
            Lime_sccb_WriteReg(0x7C, 0x00);
            Lime_sccb_WriteReg(0x7D, 0x40);
            Lime_sccb_WriteReg(0x7C, 0x05);
            Lime_sccb_WriteReg(0x7D, 0x80);
            Lime_sccb_WriteReg(0x7D, 0x80);
            break;
        }
        case LIME_OV2640_SPECIAL_EFFECT_BW_NEGATIVE:
        {
            Lime_sccb_WriteReg( OV2640_SEL_Registers, LIME_OV2640_SEL_DSP);
            Lime_sccb_WriteReg(0x7C, 0x00);
            Lime_sccb_WriteReg(0x7D, 0x58);
            Lime_sccb_WriteReg(0x7C, 0x05);
            Lime_sccb_WriteReg(0x7D, 0x80);
            Lime_sccb_WriteReg(0x7D, 0x80);
            break;
        }
        case LIME_OV2640_SPECIAL_EFFECT_NORMAL:
        {
            Lime_sccb_WriteReg( OV2640_SEL_Registers, LIME_OV2640_SEL_DSP);
            Lime_sccb_WriteReg(0x7C, 0x00);
            Lime_sccb_WriteReg(0x7D, 0x00);
            Lime_sccb_WriteReg(0x7C, 0x05);
            Lime_sccb_WriteReg(0x7D, 0x80);
            Lime_sccb_WriteReg(0x7D, 0x80);
            break;
        }
        default:
        {
            return 2;
        }
    }
    
    return 0;
}



int8_t Lime_ov2640_DCMI_Crop(uint16_t Displey_XSize,uint16_t Displey_YSize,uint16_t Sensor_XSize,uint16_t Sensor_YSize )
{
	uint16_t DCMI_X_Offset,DCMI_Y_Offset;	// 水平和垂直偏移，垂直代表的是行数，水平代表的是像素时钟数（PCLK周期数）
	uint16_t DCMI_CAPCNT;		// 水平有效像素，代表的是像素时钟数（PCLK周期数）
	uint16_t DCMI_VLINE;			// 垂直有效行数

	if( (Displey_XSize>=Sensor_XSize)|| (Displey_YSize>=Sensor_YSize) )
	{
		DEBUG_LOG("实际显示的尺寸大于或等于摄像头输出的尺寸，退出DCMI裁剪\r\n");
		return 0;  //如果实际显示的尺寸大于或等于摄像头输出的尺寸，则退出当前函数，不进行裁剪
	}
	
// 在设置为RGB565格式时，水平偏移，必须是奇数，否则画面色彩不正确，
// 因为一个有效像素是2个字节，需要2个PCLK周期，所以必须从奇数位开始，不然数据会错乱，
// 需要注意的是，寄存器值是从0开始算起的	！
	DCMI_X_Offset = Sensor_XSize - Displey_XSize; // 实际计算过程为（Sensor_XSize - LCD_XSize）/2*2

// 计算垂直偏移，尽量让画面居中裁剪，该值代表的是行数，	
	DCMI_Y_Offset = (Sensor_YSize - Displey_YSize)/2-1; // 寄存器值是从0开始算起的，所以要-1

// 因为一个有效像素是2个字节，需要2个PCLK周期，所以要乘2
// 最终得到的寄存器值，必须要能被4整除！
	DCMI_CAPCNT = Displey_XSize*2-1;	// 寄存器值是从0开始算起的，所以要-1
	
	DCMI_VLINE = Displey_YSize-1;		// 垂直有效行数
	
	DEBUG_LOG("%d  %d  %d  %d\r\n",DCMI_X_Offset,DCMI_Y_Offset,DCMI_CAPCNT,DCMI_VLINE);
	
	HAL_DCMI_ConfigCrop (&hdcmi,DCMI_X_Offset,DCMI_Y_Offset,DCMI_CAPCNT,DCMI_VLINE);// 设置裁剪窗口
	HAL_DCMI_EnableCrop(&hdcmi);		// 使能裁剪

	return 1;	
}


uint8_t Lime_ov2640_Init(void)
{
  uint16_t  Device_ID;		                      //定义变量存储器件ID
  uint16_t  Device_MID;                         //制造商
	
  Lime_sccb_gpio_Init();		                    //SCCB引脚初始化
	Lime_ov2640_Reset();	                        //执行软件复位
	Device_ID = Lime_ov2640_ReadID();		          //读取器件ID
  Device_MID = Lime_ov2640_get_MID();           //读制造商
  DEBUG_LOG ("MID:0x%X\r\n",Device_MID);		        //匹配通过
  DEBUG_LOG ("PID:0x%X\r\n",Device_ID);
  
//  if(Device_MID == 0x7FA2)                      //PID检查通过
//    DEBUG_LOG("MID Check Pass!\r\n");
	
	if( (Device_ID == 0x2640) || (Device_ID == 0x2642) )		                      // 进行匹配，实际的器件ID可能是 0x2640 或者 0x2642
	{
		DEBUG_LOG ("OV2640 OnLine,ID:0x%X\r\n",Device_ID);		                          // 匹配通过
    
    Lime_ov2640_ParaInit(Lime_ov2640_SVGA_Config);                              //初始化寄存器
    Lime_ov2640_set_output_format(LIME_OV2640_FORMAT_JPEG);                   	//设置输出JPEG格式
    Lime_ov2640_set_output_size(240, 240);                                      //由OV2640内DSP裁剪图像尺寸
    Lime_ov2640_Read_Picture_Size();                                            //读取尺寸，确认修改成功
    
    Lime_ov2640_set_light_mode(LIME_OV2640_LIGHT_MODE_SUNNY);                    //设置灯光模式 
    Lime_ov2640_set_color_saturation(LIME_OV2640_COLOR_SATURATION_2);           //设置色彩饱和度 
    Lime_ov2640_set_brightness(LIME_OV2640_BRIGHTNESS_1);                       //设置亮度 
    Lime_ov2640_set_contrast(LIME_OV2640_CONTRAST_1);                           //设置对比度 
    Lime_ov2640_set_special_effect(LIME_OV2640_SPECIAL_EFFECT_NORMAL);          //设置特殊效果 
    	
//		Lime_ov2640_DCMI_Crop( 128, 64, 400, 300 );	// 将OV2640输出图像裁剪成适应屏幕的大小
    
		return 0;	                                  // 返回成功标志		
	}
	else
	{
		DEBUG_LOG ("OV2640 ERROR!!!\r\n");	            // 读取ID错误
	}
  return 1;
}

void OV2640_DMA_Transmit_Continuous(uint32_t DMA_Buffer,uint32_t DMA_BufferSize)
{
  // 使能DCMI采集数据,连续采集模式
   HAL_DCMI_Start_DMA(&hdcmi, DCMI_MODE_CONTINUOUS, (uint32_t)DMA_Buffer,DMA_BufferSize);
}




