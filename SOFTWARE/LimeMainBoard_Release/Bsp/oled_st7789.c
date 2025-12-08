#include "oled_st7789.h"
#include "spi.h"
#include "tim.h"


/**
 *@brief    LCD控制引脚和通信接口初始化
 *@param    none
 *@retval   none
*/
static void LCD_GPIO_Init(void)
{
    LCD_PWR(0);
    LCD_RST(0);
    HAL_Delay(50);
    LCD_RST(1);
}

/**
 * @brief    SPI 发送字节函数
 * @param    TxData	要发送的数据
 * @param    size	发送数据的字节大小
 * @return  0:写入成功,其他:写入失败
 */

uint8_t SPI_WriteByte(uint8_t *TxData,uint16_t size)
{
	while(HAL_SPI_GetState(&hspi1) != HAL_SPI_STATE_READY)
		;
	
  return HAL_SPI_Transmit(&hspi1,TxData,size, 1000);
}
/**
 * @brief   写命令到LCD
 * @param   cmd —— 需要发送的命令
 * @return  none
 */
static void LCD_Write_Cmd(uint8_t cmd)
{
    LCD_WR_RS(0);
    SPI_WriteByte(&cmd, 1);
}

/**
 * @brief   写数据到LCD
 * @param   dat —— 需要发送的数据
 * @return  none
 */
static void LCD_Write_Data(uint8_t dat)
{
    LCD_WR_RS(1);
    SPI_WriteByte(&dat, 1);
}

/**
 * @breif   打开LCD显示背光
 * @param   none
 * @return  none
 */
void LCD_DisplayOn(void)
{
    LCD_PWR(1);
}
/**
 * @brief   关闭LCD显示背光
 * @param   none
 * @return  none
 */
void LCD_DisplayOff(void)
{
    LCD_PWR(0);
}

/**
 * @brief   设置数据写入LCD显存区域
 * @param   x1,y1	—— 起点坐标
 * @param   x2,y2	—— 终点坐标
 * @return  none
 */
void oled_st7789_AddressSet(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2)
{
    /* 指定X方向操作区域 */
	uint8_t temp = 0;
	uint8_t addBuffer[4] = {0};
	
	LCD_WR_RS(0);
	temp = 0x2a;
	HAL_SPI_Transmit(&hspi1,&temp,1,0xff);
	
	LCD_WR_RS(1);
	addBuffer[0] = x1 >> 8;
	addBuffer[1] = x1;
	addBuffer[2] = x2 >> 8;
	addBuffer[3] = x2;
	while(HAL_SPI_Transmit(&hspi1,addBuffer,4,0xff)!= HAL_OK){};
	
  /* 指定Y方向操作区域 */
	LCD_WR_RS(0);
	temp = 0x2b;
	HAL_SPI_Transmit(&hspi1,&temp,1,0xff);
	
	LCD_WR_RS(1);
	addBuffer[0] = y1 >> 8;
	addBuffer[1] = y1;
	addBuffer[2] = y2 >> 8;
	addBuffer[3] = y2;
	while(HAL_SPI_Transmit(&hspi1,addBuffer,4,0xff)!= HAL_OK){};
	
	/* 发送该命令，LCD开始等待接收显存数据 */
	LCD_WR_RS(0);
	temp = 0x2C;
	HAL_SPI_Transmit(&hspi1,&temp,1,0xff);
}

#define LCD_TOTAL_BUF_SIZE	(240*240*2)
#define LCD_Buf_Size 480		//1152
static uint8_t lcd_buf[LCD_Buf_Size] __attribute__((section("LIME_RAM_D3"), aligned(32)));
/**
 * @brief   以一种颜色清空LCD屏
 * @param   color —— 清屏颜色(16bit)
 * @return  none
 */
void oled_st7789_Clear(uint16_t color)
{
    uint16_t i, j;
    uint8_t data[2] = {0};  //color是16bit的，每个像素点需要两个字节的显存
		
    /* 将16bit的color值分开为两个单独的字节 */
    data[0] = color >> 8;
    data[1] = color;
    
    /* 显存的值需要逐字节写入 */
    for(j = 0; j < LCD_Buf_Size / 2; j++)
    {
        lcd_buf[j * 2] =  data[0];
        lcd_buf[j * 2 + 1] =  data[1];
    }
		
    /* 指定显存操作地址为全屏幕 */
    oled_st7789_AddressSet(0, 0, LCD_Width - 1, LCD_Height - 1);
    /* 指定接下来的数据为数据 */
    LCD_WR_RS(1);
    /* 将显存缓冲区的数据全部写入缓冲区 */
		
    for(i = 0; i < (LCD_TOTAL_BUF_SIZE / LCD_Buf_Size); i++)
    {
        while(SPI_WriteByte(lcd_buf, (uint16_t)LCD_Buf_Size) != HAL_OK);	//注意这里更改为了阻塞式
    }
}

/**
 * @brief   对接lvgl
 * @param   坐标，颜色
 * @return  无
 */
//void oled_st7789_ColorFill(uint16_t x1,uint16_t y1,uint16_t x2,uint16_t y2,uint16_t* color_p)
//{
//	oled_st7789_AddressSet(x1, y1, x2, y2);
//	LCD_WR_RS(1);
//	SPI_WriteByte((uint8_t*)color_p, (x2-x1+1)*(y2-y1+1));
//}



#include "string.h"

void oled_st7789_ColorFill(uint16_t x1,uint16_t y1,uint16_t x2,uint16_t y2,uint16_t* color_p)
{
	while(HAL_SPI_GetState(&hspi1) != HAL_SPI_STATE_READY);
	
	hspi1.Init.DataSize = SPI_DATASIZE_8BIT;
	if (HAL_SPI_Init(&hspi1) != HAL_OK)
		Error_Handler();
	oled_st7789_AddressSet(x1, y1, x2, y2);
	
	LCD_WR_RS(1);
	hspi1.Init.DataSize = SPI_DATASIZE_16BIT;
	if (HAL_SPI_Init(&hspi1) != HAL_OK)
		Error_Handler();
	while(HAL_SPI_Transmit_DMA(&hspi1, (const uint8_t*)color_p, (x2-x1+1) * (y2-y1+1)) != HAL_OK);
}

/**
 * @brief   LCD初始化
 * @param   none
 * @return  none
 */
void oled_st7789_Init(void)
{
    /* 初始化和LCD通信的引脚 */
		HAL_TIM_PWM_Start(&htim15, TIM_CHANNEL_1);
		__HAL_TIM_SET_COMPARE(&htim15, TIM_CHANNEL_1, 1000);
		oled_st7789_setBackLight(100);
    LCD_GPIO_Init();
    HAL_Delay(10);
	
    /* 关闭睡眠模式 */
    LCD_Write_Cmd(0x11);
    HAL_Delay(120);

    /* 开始设置显存扫描模式，数据格式等 */
    LCD_Write_Cmd(0x36);
    LCD_Write_Data(0x00);
    /* RGB 5-6-5-bit格式  */
    LCD_Write_Cmd(0x3A);
    LCD_Write_Data(0x05);		//0x65
    /* porch 设置 */
    LCD_Write_Cmd(0xB2);
    LCD_Write_Data(0x0C);
    LCD_Write_Data(0x0C);
    LCD_Write_Data(0x00);
    LCD_Write_Data(0x33);
    LCD_Write_Data(0x33);
    /* VGH设置 */
    LCD_Write_Cmd(0xB7);
    LCD_Write_Data(0x35);		//0x72
    /* VCOM 设置 */
    LCD_Write_Cmd(0xBB);
    LCD_Write_Data(0x2B);		//0x3D
    /* LCM 设置 */
    LCD_Write_Cmd(0xC0);
    LCD_Write_Data(0x2C);
    /* VDV and VRH 设置 */
    LCD_Write_Cmd(0xC2);
    LCD_Write_Data(0x01);
    /* VRH 设置 */
    LCD_Write_Cmd(0xC3);
    LCD_Write_Data(0x0F);		//0x19
    /* VDV 设置 */
    LCD_Write_Cmd(0xC4);
    LCD_Write_Data(0x20);
    /* 普通模式下显存速率设置 60Mhz */
    LCD_Write_Cmd(0xC6);
    LCD_Write_Data(0x13);		//0x0F
    /* 电源控制 */
    LCD_Write_Cmd(0xD0);
    LCD_Write_Data(0xA4);
    LCD_Write_Data(0xA1);
    /* 电压设置 */
    LCD_Write_Cmd(0xE0);
    LCD_Write_Data(0xD0);
    LCD_Write_Data(0x04);
    LCD_Write_Data(0x0D);
    LCD_Write_Data(0x11);
    LCD_Write_Data(0x13);
    LCD_Write_Data(0x2B);
    LCD_Write_Data(0x3F);
    LCD_Write_Data(0x54);
    LCD_Write_Data(0x4C);
    LCD_Write_Data(0x18);
    LCD_Write_Data(0x0D);
    LCD_Write_Data(0x0B);
    LCD_Write_Data(0x1F);
    LCD_Write_Data(0x23);
    /* 电压设置 */
    LCD_Write_Cmd(0xE1);
    LCD_Write_Data(0xD0);
    LCD_Write_Data(0x04);
    LCD_Write_Data(0x0C);
    LCD_Write_Data(0x11);
    LCD_Write_Data(0x13);
    LCD_Write_Data(0x2C);
    LCD_Write_Data(0x3F);
    LCD_Write_Data(0x44);
    LCD_Write_Data(0x51);
    LCD_Write_Data(0x2F);
    LCD_Write_Data(0x1F);
    LCD_Write_Data(0x1F);
    LCD_Write_Data(0x20);
    LCD_Write_Data(0x23);
    /* 显示开 */
    LCD_Write_Cmd(0x21);
    LCD_Write_Cmd(0x29);

    /*打开显示*/
    LCD_PWR(1);
}

void oled_st7789_setBackLight(uint8_t percent)
{
	if(percent > 50)
		percent -= 10;
	__HAL_TIM_SET_COMPARE(&htim15, TIM_CHANNEL_1, percent * 10);
}




