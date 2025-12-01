#ifndef __LIME_NRF_VIDEO_H
#define __LIME_NRF_VIDEO_H

#include "main.h"

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
	NRF_Mode_Rx = 0,
	NRF_Mode_Tx = 1,
	NRF_Sleep = 2,
}NRF_Mode_e;

typedef enum
{
	NRF_Status_Busy = 0,
	NRF_Status_Idle = 1,
	NRF_Status_Error = 2,
	NRF_Status_Init = 3,
}NRF_Status_e;

typedef struct
{
	uint8_t *buf;												//(user)指向缓冲区
	uint32_t nowHasRecved;							//(auto)目前接收完成的字节数（仅用于接收）（是32字节的倍数）
	uint32_t nowHasSendded;							//(auto)目前已发送完成的字节数（仅用于发送）（是32字节的倍数）
	uint32_t totalToSend;								//(auto)一共要发送的字节数（仅用于发送）（是32字节的倍数）
	
	uint8_t isFull;											//(auto)该接收缓冲区已满
	uint32_t size;											//(user)接收缓冲区总大小
	uint32_t finishTime;								//(auto)接收完成缓冲区
	
}NRF_DataBuf_t;

typedef struct
{
	NRF_Status_e Status;
	
	uint8_t sendFinishFlag;							//(auto)同步是否发送完成
	uint8_t recvFinishFlag;							//(auto)同步是否收到新数据
	
	uint8_t* sendBuf;										//(auto)发送缓冲区指针，注意！一旦开始发送，该缓冲区内数据不应被更改，除非Status变为NRF_Status_Idle
	uint32_t totalToSend;								//(auto)一共需要发送的数据量
	uint32_t nowHasSended;							//(auto)当前已发送
	
	uint8_t* recvBuf;										//(auto)发送缓冲区指针，注意！一旦开始发送，该缓冲区内数据不应被更改，除非Status变为NRF_Status_Idle
	uint32_t totalToRecv;								//(auto)一共需要发送的数据量
	uint32_t nowHasRecved;							//(auto)当前已发送
	
	
	NRF_DataBuf_t recvBufA;							//(user)接收双缓冲
	NRF_DataBuf_t recvBufB;							//(user)接收双缓冲
	
	NRF_DataBuf_t sendBufA;							//(user)发送双缓冲
	NRF_DataBuf_t sendBufB;							//(user)发送双缓冲
	
	uint32_t txStartTime;								//(auto)发送开始的时间
	uint32_t rxStartTime;								//(auto)接收开始的时间
	float nowTxSpeedBytesPerSec;				//(auto)发送速率
	float nowRxSpeedBytesPerSec;				//(auto)接收速率
	
	NRF_Mode_e Mode;										//(init)需要被配置成的模式
	uint8_t *address;										//(init)收发地址
	uint8_t addressLength;							//(init)地址长度
	uint8_t rfChannel;									//(init)通讯频道
	uint8_t speed;											//(init)射频速率 0:1Mbps 1:2Mbps 2:250Kbps
	
}NRF_Fast_t;

extern NRF_Fast_t NRF_Fast;

void FastNrf_SpiTxRxDate(uint8_t* txData, uint8_t *rxDate, uint16_t length);
void FastNrf_IRQ_Hook(void);

void FastNrf_Config(NRF_Fast_t * configInfo);

uint8_t FastNrf_NrfIsOnline(void);

void FastNrf_SendPack(NRF_Fast_t * configInfo, uint8_t* data, uint32_t length);
void FastNrf_RecvPack(NRF_Fast_t * configInfo, uint8_t* data, uint32_t length);
void FastNrf_ErrResumeSend(void);			//调用它，以便在发送失败后重新发送数据！

int8_t FastNrf_InitTemplete(NRF_Mode_e mode, uint8_t channel);

float FastNrf_CalSpeed(uint32_t totalBytes, uint32_t elapsTimeMs);

void printfBuf(uint8_t* buf, uint32_t length);

//初始化接收双缓冲区
void FastNrf_InitRecvBuff(NRF_Fast_t* nrf, uint8_t *bufA, uint8_t* bufB, uint32_t oneBufSize);

//从初始化的缓冲区内读取数据，数据量不大于oneBufSize
int8_t FastNrf_GetData(NRF_Fast_t* nrf, uint8_t* dstBuf, uint32_t* totalReaddedSize);

//初始化发送双缓冲区
void FastNrf_InitSendBuff(NRF_Fast_t* nrf, uint8_t *bufA, uint8_t* bufB, uint32_t oneBufSize);

//发送buf内数据
int8_t FastNrf_SendData(NRF_Fast_t* nrf, uint8_t* srcBuf, uint32_t totalSizeToSend);

#define TOTAL_TEST_SIZE  4096

#endif	//__LIME_NRF_VIDEO_H
