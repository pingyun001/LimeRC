#ifndef __LIME_NRF_MASTER_H
#define __LIME_NRF_MASTER_H

#include "main.h"
#include "Lime_App_Hal.h"

#define LIME_NRF_COMM_HEAD				0x70
#define LIME_NRF_COMM_HEAD_MASTER	0xAA
#define LIME_NRF_COMM_HEAD_SLAVE	0xBB
#define LIME_NRF_COMM_TAIL				'\n'

#define LIME_NRF_SLAVE_INFO_NUM		4

/******************************************************************************************/
/* NRF24L01寄存器操作命令 */
#define NRF_READ_REG    0x00    /* 读配置寄存器,低5位为寄存器地址 */
#define NRF_WRITE_REG   0x20    /* 写配置寄存器,低5位为寄存器地址 */
#define RD_RX_PLOAD     0x61    /* 读RX有效数据,1~32字节 */
#define WR_TX_PLOAD     0xA0    /* 写TX有效数据,1~32字节 */
#define FLUSH_TX        0xE1    /* 清除TX FIFO寄存器.发射模式下用 */
#define FLUSH_RX        0xE2    /* 清除RX FIFO寄存器.接收模式下用 */
#define REUSE_TX_PL     0xE3    /* 重新使用上一包数据,CE为高,数据包被不断发送. */
#define NOP             0xFF    /* 空操作,可以用来读状态寄存器 */

/* SPI(NRF24L01)寄存器地址 */
#define CONFIG          0x00    /* 配置寄存器地址;bit0:1接收模式,0发射模式;bit1:电选择;bit2:CRC模式;bit3:CRC使能; */
                                /* bit4:中断MAX_RT(达到最大重发次数中断)使能;bit5:中断TX_DS使能;bit6:中断RX_DR使能 */
#define EN_AA           0x01    /* 使能自动应答功能  bit0~5,对应通道0~5 */
#define EN_RXADDR       0x02    /* 接收地址允许,bit0~5,对应通道0~5 */
#define SETUP_AW        0x03    /* 设置地址宽度(所有数据通道):bit1,0:00,3字节;01,4字节;02,5字节; */
#define SETUP_RETR      0x04    /* 建立自动重发;bit3:0,自动重发计数器;bit7:4,自动重发延时 250*x+86us */
#define RF_CH           0x05    /* RF通道,bit6:0,工作通道频率; */
#define RF_SETUP        0x06    /* RF寄存器;bit3:传输速率(0:1Mbps,1:2Mbps);bit2:1,发射功率;bit0:低噪声放大器增益 */
#define STATUS          0x07    /* 状态寄存器;bit0:TX FIFO满标志;bit3:1,接收数据通道号(最大:6);bit4,达到最多次重发 */
                                /* bit5:数据发送完成中断;bit6:接收数据中断; */
#define MAX_TX          0x10    /* 达到最大发送次数中断 */
#define TX_OK           0x20    /* TX发送完成中断 */
#define RX_OK           0x40    /* 接收到数据中断 */

#define OBSERVE_TX      0x08    /* 发送检测寄存器,bit7:4,数据包丢失计数器;bit3:0,重发计数器 */
#define CD              0x09    /* 载波检测寄存器,bit0,载波检测; */
#define RX_ADDR_P0      0x0A    /* 数据通道0接收地址,最大长度5个字节,低字节在前 */
#define RX_ADDR_P1      0x0B    /* 数据通道1接收地址,最大长度5个字节,低字节在前 */
#define RX_ADDR_P2      0x0C    /* 数据通道2接收地址,最低字节可设置,高字节,必须同RX_ADDR_P1[39:8]相等; */
#define RX_ADDR_P3      0x0D    /* 数据通道3接收地址,最低字节可设置,高字节,必须同RX_ADDR_P1[39:8]相等; */
#define RX_ADDR_P4      0x0E    /* 数据通道4接收地址,最低字节可设置,高字节,必须同RX_ADDR_P1[39:8]相等; */
#define RX_ADDR_P5      0x0F    /* 数据通道5接收地址,最低字节可设置,高字节,必须同RX_ADDR_P1[39:8]相等; */
#define TX_ADDR         0x10    /* 发送地址(低字节在前),ShockBurstTM模式下,RX_ADDR_P0与此地址相等 */
#define RX_PW_P0        0x11    /* 接收数据通道0有效数据宽度(1~32字节),设置为0则非法 */
#define RX_PW_P1        0x12    /* 接收数据通道1有效数据宽度(1~32字节),设置为0则非法 */
#define RX_PW_P2        0x13    /* 接收数据通道2有效数据宽度(1~32字节),设置为0则非法 */
#define RX_PW_P3        0x14    /* 接收数据通道3有效数据宽度(1~32字节),设置为0则非法 */
#define RX_PW_P4        0x15    /* 接收数据通道4有效数据宽度(1~32字节),设置为0则非法 */
#define RX_PW_P5        0x16    /* 接收数据通道5有效数据宽度(1~32字节),设置为0则非法 */
#define NRF_FIFO_STATUS 0x17    /* FIFO状态寄存器;bit0,RX FIFO寄存器空标志;bit1,RX FIFO满标志;bit2,3,保留 */
                                /* bit4,TX FIFO空标志;bit5,TX FIFO满标志;bit6,1,循环发送上一数据包.0,不循环; */
#define DYNPD						0x1C		/* 特征寄存器*/
#define FEATURE					0x1D		/* 特征寄存器*/
/******************************************************************************************/

typedef enum
{
	Info_Nrf_Mode_Transmit,
	Info_Nrf_Mode_Receive,
}LimeNrfMaster_BspMode_e;

typedef enum
{
	LimeNrfMaster_Status_Init = 0,
	LimeNrfMaster_Status_WaitingPairing,
	LimeNrfMaster_Status_Paired,
	LimeNrfMaster_Status_NoPaired,
}LimeNrfMaster_Status_e;

typedef enum
{
	LimeNrfMaster_Role_debug = 0,
	LimeNrfMaster_Role_CamerV3_1 = 1,
	LimeNrfMaster_Role_BasicV3_1 = 2,
	//...
}LimeNrfMaster_Role_e;

typedef enum
{
	LimeNrfCatAgreement_None = 0,
	LimeNrfCatAgreement_DBUS = 1,
	LimeNrfCatAgreement_SBUS = 2,
	LimeNrfCatAgreement_PP = 3,
	//...
	LimeNrfCatAgreement_MaxAndErr,
}LimeNrfCatAgreement_e;

typedef struct
{
	/* public use */
	uint8_t NowChannel;
	uint32_t lastSuccRunTime;
	LimeNrfCatAgreement_e catAgreement;
	bool catOutputTogg;
	
	LimeNrfMaster_Status_e myStatus;
	LimeNrfMaster_Role_e myRole;
	uint32_t LimeChipUID[3];
	
	uint32_t sendBufFreeSize;
	uint32_t readBufUsedSize;
	
	/* private use */
	uint32_t succeCount;
	uint32_t errorCount;
	uint8_t magicCodeSlave;
	uint8_t magicCodeMaster;
	uint32_t sendBufrdPin;		//USED for tx FIFO
	uint32_t sendBufwrPin;		//USED for tx FIFO
	
	/* extern malloc (need be init extern)*/
	uint8_t *sendBuff;				//USED for tx FIFO
	uint32_t sendBufTotalSize;//USED for tx FIFO
	
	uint8_t *readBuff;
	uint32_t readBufTotalSize;
	
	/* API (in blocking mode) (need be init extern)*/
	HAL_StatusTypeDef (*nrf_Init)(uint8_t channel);
	
	HAL_StatusTypeDef (*nrf_ChangeBspModeTo)(LimeNrfMaster_BspMode_e mode);
	
	HAL_StatusTypeDef (*nrf_SendPack32)(uint8_t buf[32]);
	
	uint8_t (*nrf_NewPackAvaliable)(void);
	HAL_StatusTypeDef (*nrf_ReadPack32)(uint8_t buf[32]);
	
	HAL_StatusTypeDef (*nrf_PowerCtrl)(bool isPowerOn);
	
}LimeNrfMaster_t;


HAL_StatusTypeDef LimeNrfMaster_Init(LimeNrfMaster_t *nrf_Master);

typedef struct
{
	bool isEnable;
	bool isConnected;
	uint32_t ackTime;
	
	uint8_t slClass;				//0:debug, 1:Adv, 2:Std(not support cam)
	uint32_t UID[3];
	char name[14];					//only used 13byte
	
}LimeNrfSlaveInfo_t;
extern LimeNrfSlaveInfo_t LimeNrfSlaveInfo[LIME_NRF_SLAVE_INFO_NUM];

HAL_StatusTypeDef LimeNrfMaster_ScanSlave(LimeNrfMaster_t *nrf_Master, LimeNrfSlaveInfo_t* info);

HAL_StatusTypeDef LimeNrfMaster_JumpToPrivateChannel(LimeNrfMaster_t *nrf_Master, LimeNrfSlaveInfo_t* info, uint8_t channel);

HAL_StatusTypeDef LimeNrfMaster_SendNoAckOutput(LimeNrfMaster_t *nrf_Master, uint8_t payload[27]);
HAL_StatusTypeDef LimeNrfMaster_SendRockerInfo(LimeNrfMaster_t *nrf_Master, LimeHal_KeyInfo_t *keyInfo);

HAL_StatusTypeDef LimeNrfMaster_SetReceiverOutputMode(LimeNrfMaster_t *nrf_Master, uint8_t agreement, uint8_t outLevel, bool openVideo, uint8_t videoChannel);

HAL_StatusTypeDef LimeNrfMaster_CheckChannelIsClear(LimeNrfMaster_t *nrf_Master, uint8_t channel);

/* extern API */
HAL_StatusTypeDef LimeNrfMaster_PowerCtrl(LimeNrfMaster_t *nrf_Master, bool isPowerOn);
uint32_t LimeNrfMaster_GetLastSuccessRunTime(LimeNrfMaster_t *nrf_Master);


#endif	//__LIME_NRF_MASTER_H
