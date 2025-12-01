#include "Lime_sccb.h"
#define DEBUG      1
#include "stdio.h"


/*
此文件仅仅为底层GPIO配置和软件IIC协议实现，无其它功能
如果读取不到MID和PID，请检查PWDN引脚是否未初始化为推挽输出！！！
*/

void Lime_sccb_gpio_Init(void)
{
  GPIO_InitTypeDef GPIO_InitStruct = {0};
	
	SCCB_SCL_CLK_ENABLE;	                                //开时钟
	SCCB_SDA_CLK_ENABLE;
	
	GPIO_InitStruct.Pin 			= SCCB_SCL_PIN;				      // SCL引脚
	GPIO_InitStruct.Mode 		  = GPIO_MODE_OUTPUT_OD;			// 开漏输出，且必须为开漏！！！
	GPIO_InitStruct.Pull 		  = GPIO_PULLUP;						  // 不带上下拉
	GPIO_InitStruct.Speed 		= GPIO_SPEED_FREQ_HIGH;			// 速度等级 
	HAL_GPIO_Init(SCCB_SCL_PORT, &GPIO_InitStruct);

	GPIO_InitStruct.Pin 			= SCCB_SDA_PIN;				      // SDA引脚
	HAL_GPIO_Init(SCCB_SDA_PORT, &GPIO_InitStruct);		

	GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;      			// 推挽输出
	GPIO_InitStruct.Pull  = GPIO_PULLUP;		 					    // 上拉

	HAL_GPIO_WritePin(SCCB_SCL_PORT, SCCB_SCL_PIN, GPIO_PIN_SET);
	HAL_GPIO_WritePin(SCCB_SDA_PORT, SCCB_SDA_PIN, GPIO_PIN_SET);
}



//-------------------以下用于模仿软件IIC通信格式------------------------------------
static void Lime_sccb_Delay(uint32_t a)                 //简单延时函数
{
	volatile uint16_t i;
	while (a --)				
	{
		for (i = 0; i < 5; i++);
	}
}
void Lime_sccb_Start(void)                              //发送IIC起始位
{
	SCCB_SDA(1);		
	SCCB_SCL(1);
	Lime_sccb_Delay(SCCB_DelayVaule);
	
	SCCB_SDA(0);
	Lime_sccb_Delay(SCCB_DelayVaule);
	SCCB_SCL(0);
	Lime_sccb_Delay(SCCB_DelayVaule);
}

void Lime_sccb_Stop(void)                                    //发送IIC停止位
{
	SCCB_SCL(0);
	Lime_sccb_Delay(SCCB_DelayVaule);
	SCCB_SDA(0);
	Lime_sccb_Delay(SCCB_DelayVaule);
	
	SCCB_SCL(1);
	Lime_sccb_Delay(SCCB_DelayVaule);
	SCCB_SDA(1);
	Lime_sccb_Delay(SCCB_DelayVaule);
}

void Lime_sccb_ACK(void)                                     //发送应答
{
	SCCB_SCL(0);
	Lime_sccb_Delay(SCCB_DelayVaule);
	SCCB_SDA(0);
	Lime_sccb_Delay(SCCB_DelayVaule);	
	SCCB_SCL(1);
	Lime_sccb_Delay(SCCB_DelayVaule);
	
	SCCB_SCL(0);		// SCL输出低时，SDA应立即拉高，释放总线
	SCCB_SDA(1);		
	
	Lime_sccb_Delay(SCCB_DelayVaule);

}
void Lime_sccb_NoACK(void)                                   //发送未应答
{
	SCCB_SCL(0);	
	Lime_sccb_Delay(SCCB_DelayVaule);
	SCCB_SDA(1);
	Lime_sccb_Delay(SCCB_DelayVaule);
	SCCB_SCL(1);
	Lime_sccb_Delay(SCCB_DelayVaule);
	
	SCCB_SCL(0);
	Lime_sccb_Delay(SCCB_DelayVaule);
}

static inline uint8_t Lime_sccb_WaitACK(void)                //等待设备应答
{
	SCCB_SDA(1);
	Lime_sccb_Delay(SCCB_DelayVaule);
	SCCB_SCL(1);
	Lime_sccb_Delay(SCCB_DelayVaule);	
	
	if( HAL_GPIO_ReadPin(SCCB_SDA_PORT,SCCB_SDA_PIN) != 0) //判断设备是否有做出响应		
	{
		SCCB_SCL(0);	
		Lime_sccb_Delay( SCCB_DelayVaule );		
		return LIME_SCCB_ACK_ERR;	//无应答
	}
	else
	{
		SCCB_SCL(0);	
		Lime_sccb_Delay( SCCB_DelayVaule );		
		return LIME_SCCB_ACK_OK;	//应答正常
	}
}
uint8_t Lime_sccb_WriteByte(uint8_t IIC_Data)
{
	uint8_t i;

	for (i = 0; i < 8; i++)
	{
		SCCB_SDA(IIC_Data & 0x80);
		
		Lime_sccb_Delay( SCCB_DelayVaule );
		SCCB_SCL(1);
		Lime_sccb_Delay( SCCB_DelayVaule );
		SCCB_SCL(0);		
		if(i == 7)
		{
			SCCB_SDA(1);			
		}
		IIC_Data <<= 1;
	}

	return Lime_sccb_WaitACK(); //等待设备响应
}
uint8_t Lime_sccb_ReadByte(uint8_t ACK_Mode)
{
	uint8_t IIC_Data = 0;
	uint8_t i = 0;
	
	for (i = 0; i < 8; i++)
	{
		IIC_Data <<= 1;
		
		SCCB_SCL(1);
		Lime_sccb_Delay( SCCB_DelayVaule );
		IIC_Data |= (HAL_GPIO_ReadPin(SCCB_SDA_PORT,SCCB_SDA_PIN) & 0x01);	
		SCCB_SCL(0);
		Lime_sccb_Delay( SCCB_DelayVaule );
	}
	
	if ( ACK_Mode == 1 )//	应答信号
		Lime_sccb_ACK();
	else
		Lime_sccb_NoACK();// 非应答信号
	
	return IIC_Data; 
}

//-----------------------------------------以下为封装--------------------------------------
uint8_t Lime_sccb_WriteRegAddress (uint8_t addr)    //准备要读取的寄存器地址
{
	uint8_t status;		// 状态标志位

	Lime_sccb_Start();	// 启动IIC通信
	if( Lime_sccb_WriteByte(OV2640_DEVICE_ADDRESS) == LIME_SCCB_ACK_OK )
	{
		if( Lime_sccb_WriteByte((uint8_t)(addr)) != LIME_SCCB_ACK_OK )
		{
			status = ERROR;	// 操作失败
		}			
	}
	status = SUCCESS;	// 操作成功
	return status;	
}


uint8_t Lime_sccb_WriteReg (uint8_t addr,uint8_t value)
{
	uint8_t status;
	
	Lime_sccb_Start(); //启动IIC通讯

	if( Lime_sccb_WriteRegAddress(addr) == SUCCESS)	 //准备要读取的寄存器地址
	{
		if (Lime_sccb_WriteByte(value) != LIME_SCCB_ACK_OK) //写数据
		{
			status = ERROR;						
		}
	}	
	Lime_sccb_Stop(); // 停止通讯
	
	status = SUCCESS;	// 写入成功
	return status;
}


uint8_t Lime_sccb_ReadReg (uint8_t addr)
{
   uint8_t value = 0;

	Lime_sccb_Start();		// 启动IIC通信

	if( Lime_sccb_WriteRegAddress(addr) == SUCCESS) //写入要操作的寄存器
	{
    Lime_sccb_Stop();	// 停止IIC通信
		Lime_sccb_Start(); //重新启动IIC通讯

		if (Lime_sccb_WriteByte(OV2640_DEVICE_ADDRESS|0X01) == LIME_SCCB_ACK_OK)
		{	
			value = Lime_sccb_ReadByte(0);	// 读到最后一个数据时发送 非应答信号
		}					
		Lime_sccb_Stop();	// 停止IIC通信
	}
	return value;	
}



