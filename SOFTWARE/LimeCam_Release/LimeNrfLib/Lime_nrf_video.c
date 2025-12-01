#include "Lime_nrf_video.h"
#include "stdio.h"
#include "string.h"
#include "spi.h"

NRF_Fast_t NRF_Fast;

static uint8_t txTempBuf[33] = {0};
static uint8_t rxTempBuf[33] = {0};

static uint8_t FastNrf_writeBuf(uint8_t reg, uint8_t *pbuf, uint8_t len);
static uint8_t FastNrf_write_reg(uint8_t reg, uint8_t value);
static uint8_t FastNrf_read_buf(uint8_t reg, uint8_t *pbuf, uint8_t len);
static uint8_t FastNrf_read_reg(uint8_t reg);
static uint8_t FastNrf_spi_read_write_byte(uint8_t txdata);
static uint8_t readAndCleraStatusReg(void);

//获取可用的接收缓冲区
static NRF_DataBuf_t *getAvaliableRecvBuf(NRF_Fast_t* nrf);

//获取可用的发送缓冲区
static NRF_DataBuf_t *getAvaliableSendBuf(NRF_Fast_t* nrf);

static uint8_t noInitTempBuf[33] = {0};					//for no init rx mode but has rx data to read

__weak void FastNrf_SpiTxRxDate(uint8_t* txData, uint8_t *rxDate, uint16_t length)
{
	HAL_GPIO_WritePin(NRF1_CSN_GPIO_Port, NRF1_CSN_Pin, GPIO_PIN_RESET);
	HAL_SPI_TransmitReceive(&hspi1, txData, rxDate,length, 100);
	HAL_GPIO_WritePin(NRF1_CSN_GPIO_Port, NRF1_CSN_Pin, GPIO_PIN_SET);
}

__weak void FastNrf_CE_Ctrl(uint8_t status)
{
	if(status)
		HAL_GPIO_WritePin(NRF_CE_GPIO_Port, NRF_CE_Pin, GPIO_PIN_SET);
	else
		HAL_GPIO_WritePin(NRF_CE_GPIO_Port, NRF_CE_Pin, GPIO_PIN_RESET);
}

static int8_t thisPackIsStartOrEnd(uint8_t packBuf[32])
{
	uint32_t* HeadCheckBuf = (uint32_t * )packBuf;
	
	
//	printfBuf(packBuf, 32);
	
	if(HeadCheckBuf[0] == 0x11223344 && HeadCheckBuf[1] == 0x55667788)
	{
//		printf(">>ST\n");
		return 1;
	}
	if(HeadCheckBuf[0] == 0x88776655 && HeadCheckBuf[1] == 0x44332211)
	{
//		printf(">>ED\n");
		return 2;
	}
//	printf("DF,%#x\n", packBuf[0]);
	return 0;
}

static void printErrorCode(uint8_t errID)
{
	return;
	switch(errID)
	{
		case 1:
			printf("Err:1, LostPack\n");
			break;
		case 2:
			printf("Err:2, More Pack\n");
			break;
		default:break;
	}
}

void FastNrf_IRQ_Hook(void)
{
	static NRF_DataBuf_t* sendStructPin = NULL;
	static uint8_t errorCount = 0;
	
	uint8_t statusReg = readAndCleraStatusReg();
	
	if(statusReg & 0x20)//TX_DS
	{
		static NRF_Status_e lastStatus = NRF_Status_Init;
		
		errorCount = 0;
		
		if(NRF_Fast.Status == NRF_Status_Busy && lastStatus != NRF_Status_Busy)
		{
			sendStructPin = getAvaliableSendBuf(&NRF_Fast);
			NRF_Fast.txStartTime = HAL_GetTick();
		}
		
		sendStructPin->nowHasSendded += 32;
		if(sendStructPin->nowHasSendded >= sendStructPin->totalToSend)
		{
			sendStructPin->isFull = 0;
			NRF_Fast.Status = NRF_Status_Idle;
			NRF_Fast.nowTxSpeedBytesPerSec = (float)sendStructPin->totalToSend / (float)(HAL_GetTick() - NRF_Fast.txStartTime);
		}
		else
		{
			FastNrf_writeBuf(WR_TX_PLOAD, sendStructPin->buf + sendStructPin->nowHasSendded, 32);
		}
		
		lastStatus = NRF_Fast.Status;
//		NRF_Fast.nowHasSended += 32;
//		if(NRF_Fast.nowHasSended >= NRF_Fast.totalToSend)
//		{
//			NRF_Fast.sendFinishFlag = 1;
//			NRF_Fast.Status = NRF_Status_Idle;
//		}
//		else
//		{
//			FastNrf_writeBuf(WR_TX_PLOAD, NRF_Fast.sendBuf + NRF_Fast.nowHasSended, 32);
//		}
//		printf("IT\n");
	}
	if(statusReg & 0x40)//RX_DS
	{
		FastNrf_read_buf(RD_RX_PLOAD, noInitTempBuf, 32);
		int8_t id = thisPackIsStartOrEnd(noInitTempBuf);
		
		static NRF_DataBuf_t* recvStructPin = NULL;
		static uint8_t thisRecvPackCmdID = 0;
		static uint32_t thisRecvPackTotalLength = 0;

		switch(id)
		{
			case 1:							//数据头
			{
				recvStructPin = getAvaliableRecvBuf(&NRF_Fast);
				if(recvStructPin == NULL)return;
				
				recvStructPin->isFull = 0;
				recvStructPin->nowHasRecved = 0;
				thisRecvPackCmdID = noInitTempBuf[8] << 8 | noInitTempBuf[9];
				memcpy(&thisRecvPackTotalLength, noInitTempBuf + 10, 4);
				
				NRF_Fast.rxStartTime = HAL_GetTick();
			
//					printf("Cmd:%d, Length:%d\n", thisRecvPackCmdID, thisRecvPackTotalLength);
				break;
			}
			case 2:							//数据尾
			{
				if(thisRecvPackCmdID == 1)
				{
					if(recvStructPin == NULL)return;
					
					if(recvStructPin->nowHasRecved == thisRecvPackTotalLength)
					{								//实际收到的数据和头中通知的数据相同，接收完成
						recvStructPin->isFull = 1;
						recvStructPin->finishTime = HAL_GetTick();
						NRF_Fast.nowRxSpeedBytesPerSec = (float)thisRecvPackTotalLength / (float)(HAL_GetTick() - NRF_Fast.rxStartTime);
					}
					else						//数据丢包，直接破坏当前接收进度，等待下一个数据头的到来
					{
						recvStructPin->nowHasRecved = 0;
						printErrorCode(1);
					}
				}
				break;
			}
			case 0:							//正常数据
			{
				if(thisRecvPackCmdID == 1)
				{
					if(recvStructPin == NULL)return;
					
					if((recvStructPin->nowHasRecved < thisRecvPackTotalLength) &&
						(recvStructPin->nowHasRecved < recvStructPin->size))
					{								//防止数据多包
						memcpy(recvStructPin->buf + recvStructPin->nowHasRecved ,noInitTempBuf, 32);
						recvStructPin->nowHasRecved += 32;
					}
					else
					{
						printErrorCode(2);
					}
				}
				break;
			}
			default:
				break;
		}
//		printf("IR\n");
	}
	
	
	if(statusReg & 0x10)//TX ERROR
	{
		errorCount ++;
		if(errorCount > 5)
			NRF_Fast.Status = NRF_Status_Error;
		
		FastNrf_spi_read_write_byte(FLUSH_TX);
		if(sendStructPin == NULL)
			sendStructPin = getAvaliableSendBuf(&NRF_Fast);
		FastNrf_writeBuf(WR_TX_PLOAD, sendStructPin->buf + sendStructPin->nowHasSendded, 32);
		
//		printf("IE\n");
	}
//	printf("FF\n");
	
	
}


static uint8_t FastNrf_spi_read_write_byte(uint8_t txdata)
{
	uint8_t rxdata;
	FastNrf_SpiTxRxDate(&txdata,&rxdata,1);
	return rxdata;
}

static uint8_t FastNrf_write_reg(uint8_t reg, uint8_t value)
{
//  uint8_t status;
	
	txTempBuf[0] = reg;
	txTempBuf[1] = value;
	
	FastNrf_SpiTxRxDate(txTempBuf, rxTempBuf, 2);
	
//	status = FastNrf_spi_read_write_byte(reg);
//	FastNrf_spi_read_write_byte(value);

	return rxTempBuf[0];
}

static uint8_t FastNrf_read_reg(uint8_t reg)
{
	uint8_t reg_val;

	FastNrf_spi_read_write_byte(reg);
	reg_val = FastNrf_spi_read_write_byte(0Xff);

	return reg_val;
}

static uint8_t readAndCleraStatusReg(void)
{
	uint8_t nrfStatus = 0;

	nrfStatus = FastNrf_write_reg(NRF_WRITE_REG + STATUS, 0x70);

	return nrfStatus;
}



static uint8_t FastNrf_writeBuf(uint8_t reg, uint8_t *pbuf, uint8_t len)
{
  uint8_t status;
	
	txTempBuf[0] = reg;
	memcpy(txTempBuf + 1, pbuf, len);
		
	FastNrf_SpiTxRxDate(txTempBuf, rxTempBuf, len + 1);
	
	status = rxTempBuf[0];
    
  return status;
}

static uint8_t FastNrf_read_buf(uint8_t reg, uint8_t *pbuf, uint8_t len)
{
	uint8_t status;
	
	memset(txTempBuf, 0xff, len + 1);
	txTempBuf[0] = reg;
	
	
	FastNrf_SpiTxRxDate(txTempBuf, rxTempBuf, len + 1);
	
	status = rxTempBuf[0];
	
	memcpy(pbuf, rxTempBuf + 1, len);
    
  return status;
}


uint8_t FastNrf_NrfIsOnline(void)
{
	uint8_t bufA[5] = {0XA6, 0XA6, 0XA6, 0XA6, 0XA6};
	uint8_t bufB[5] = {0X32, 0X32, 0X32, 0X32, 0X32};
  uint8_t i;
		
	FastNrf_CE_Ctrl(0);
	HAL_Delay(1);
	
	if(HAL_GPIO_ReadPin(NRF_IRQ_GPIO_Port, NRF_IRQ_Pin) == GPIO_PIN_RESET)
	{
		readAndCleraStatusReg();
	}
		
	FastNrf_writeBuf(NRF_WRITE_REG + TX_ADDR, bufA, 5);
	memset(bufA, 0, sizeof(bufA));
	FastNrf_read_buf(TX_ADDR, bufA, 5);
	for (i = 0; i < 5; i++)
	{
			if (bufA[i] != 0XA6) break;
	}
	if (i != 5) return 0;
	
	FastNrf_writeBuf(NRF_WRITE_REG + TX_ADDR, bufB, 5);
	memset(bufB, 0, sizeof(bufB));
	FastNrf_read_buf(TX_ADDR, bufB, 5);
	for (i = 0; i < 5; i++)
	{
			if (bufB[i] != 0X32) break;
	}
	if (i != 5) return 0;

	return 1;
}




void FastNrf_Config(NRF_Fast_t * configInfo)
{
	uint8_t errorCount = 0;
	
	if(configInfo == NULL)return;
	
	memcpy(&NRF_Fast, configInfo, sizeof(NRF_Fast_t));
	
	uint8_t rfSetUpTemp = 0x07;
	if(configInfo->speed == 0x01)
		rfSetUpTemp |= 0x08;
	else if(configInfo->speed == 0x02)
		rfSetUpTemp |= 0x20;
	
	if(configInfo == NULL)return;
	
	while(errorCount < 5)
	{
		if(HAL_GPIO_ReadPin(NRF_IRQ_GPIO_Port, NRF_IRQ_Pin) == GPIO_PIN_RESET)
		{
			readAndCleraStatusReg();
			errorCount ++;
		}
		else
			break;
	}
	if(errorCount == 5)
	{
		configInfo->Status = NRF_Status_Error;
		return;
	}
	
	FastNrf_CE_Ctrl(0);
	
	switch(configInfo->Mode)
	{
		case NRF_Mode_Tx:
		{
			FastNrf_write_reg(NRF_WRITE_REG + SETUP_AW,		configInfo->addressLength - 2);
			
			FastNrf_writeBuf(NRF_WRITE_REG + TX_ADDR, 		configInfo->address, configInfo->addressLength);
			FastNrf_writeBuf(NRF_WRITE_REG + RX_ADDR_P0, 	configInfo->address, configInfo->addressLength);
			
			FastNrf_write_reg(NRF_WRITE_REG + FEATURE, 0x04);
			FastNrf_write_reg(NRF_WRITE_REG + DYNPD, 0x01);
			FastNrf_write_reg(NRF_WRITE_REG + SETUP_RETR, 0x1f);
			FastNrf_write_reg(NRF_WRITE_REG + RF_CH, 			configInfo->rfChannel);
			FastNrf_write_reg(NRF_WRITE_REG + RF_SETUP, rfSetUpTemp);
			FastNrf_write_reg(NRF_WRITE_REG + CONFIG, 0x0e);
			
			FastNrf_spi_read_write_byte(FLUSH_TX);
			FastNrf_spi_read_write_byte(FLUSH_RX);
			
			FastNrf_CE_Ctrl(1);
			
			break;
		}
		case NRF_Mode_Rx:
		{
			FastNrf_writeBuf(NRF_WRITE_REG + RX_ADDR_P0, 	configInfo->address, configInfo->addressLength);
			FastNrf_write_reg(NRF_WRITE_REG + EN_RXADDR, 	0x01);
			FastNrf_write_reg(NRF_WRITE_REG + RF_CH, 			configInfo->rfChannel);
			FastNrf_write_reg(NRF_WRITE_REG + RX_PW_P0, 	32);
			FastNrf_write_reg(NRF_WRITE_REG + SETUP_AW,		configInfo->addressLength - 2);
			FastNrf_write_reg(NRF_WRITE_REG + FEATURE, 0x04);
			FastNrf_write_reg(NRF_WRITE_REG + DYNPD, 0x01);
			FastNrf_write_reg(NRF_WRITE_REG + RF_SETUP, rfSetUpTemp);
			
			FastNrf_write_reg(NRF_WRITE_REG + CONFIG, 		0x0f);
			
			FastNrf_spi_read_write_byte(FLUSH_TX);
			FastNrf_spi_read_write_byte(FLUSH_RX);
			
			FastNrf_CE_Ctrl(1);
			
			break;
		}
		case NRF_Sleep:
		{
			break;
		}
	}
	
	NRF_Fast.Status = NRF_Status_Idle;
}

static uint8_t sendBufA[10*1024] = {0};
const uint32_t startMark[2] = {0x11223344, 0x55667788}; //{0x11, 0x22, 0x33, 0x44, 0x55, 0x66, 0x77, 0x88};
const uint32_t endMark[2] = {0x88776655, 0x44332211};

void FastNrf_SendPack(NRF_Fast_t * configInfo, uint8_t* data, uint32_t length)
{
	if(data == NULL)return;
	
	uint8_t errorCount = 0;
	uint32_t offset = 0;
	
	while(errorCount < 5)
	{
		if(HAL_GPIO_ReadPin(NRF_IRQ_GPIO_Port, NRF_IRQ_Pin) == GPIO_PIN_RESET)
		{
			readAndCleraStatusReg();
			errorCount ++;
		}
		else
			break;
	}
	if(errorCount == 5)
	{
		configInfo->Status = NRF_Status_Error;
		return;
	}
	
	NRF_Fast.sendBuf = sendBufA;
	NRF_Fast.Status = NRF_Status_Busy;
	NRF_Fast.nowHasSended = 0;
	NRF_Fast.totalToSend = length + 64;
	FastNrf_CE_Ctrl(1);
	
	
	memcpy(sendBufA, startMark, 8);
	sendBufA[8] = 0;
	sendBufA[9] = 1;
	memcpy(sendBufA + 10, &length, 4);
	memcpy(sendBufA + 32, data, length);
	memcpy(sendBufA + 32 + length, endMark, 8);
	
	FastNrf_writeBuf(WR_TX_PLOAD, sendBufA, 32);
}


void FastNrf_RecvPack(NRF_Fast_t * configInfo, uint8_t* data, uint32_t length)
{
	if(data == NULL)return;
	
	uint8_t errorCount = 0;
	uint32_t offset = 0;
	
	while(errorCount < 5)
	{
		if(HAL_GPIO_ReadPin(NRF_IRQ_GPIO_Port, NRF_IRQ_Pin) == GPIO_PIN_RESET)
		{
			readAndCleraStatusReg();
			errorCount ++;
		}
		else
			break;
	}
	if(errorCount == 5)
	{
		configInfo->Status = NRF_Status_Error;
		return;
	}
	
	FastNrf_CE_Ctrl(1);
	
	NRF_Fast.recvBuf = data;
	NRF_Fast.Status = NRF_Status_Busy;
	NRF_Fast.nowHasRecved = 0;
	NRF_Fast.totalToRecv = length;
}

void FastNrf_ErrResumeSend(void) 			//调用它，以便在发送失败后重新发送数据！
{
	NRF_Fast.Status = NRF_Status_Busy;
	FastNrf_writeBuf(WR_TX_PLOAD, NRF_Fast.sendBuf + NRF_Fast.nowHasSended, 32);
}

#define RECV_BUF_SIZE				10*1024
volatile uint8_t recvBufA[RECV_BUF_SIZE] = {0,1,2,3};
volatile uint8_t recvBufB[RECV_BUF_SIZE] = {0,1,2,3};

int8_t FastNrf_InitTemplete(NRF_Mode_e mode, uint8_t channel)
{
	if(mode == NRF_Mode_Tx)
	{
		if(FastNrf_NrfIsOnline())
		{
			printf("Nrf Online, Mode:Tx\n");
			
			NRF_Fast_t NrfInitInfo = 
			{
				.Mode = NRF_Mode_Tx,
				.address = (uint8_t*)"abcde",
				.addressLength = 5,
				.rfChannel = channel,
				.speed = 1
			};
			
			FastNrf_Config(&NrfInitInfo);
			
			FastNrf_InitSendBuff(&NRF_Fast, (uint8_t*)recvBufA, (uint8_t*)recvBufB, RECV_BUF_SIZE);
			return 1;
		}
		else
		{
			printf("Nrf Offline\n");
			return -1;
		}
	}
	else if(mode == NRF_Mode_Rx)
	{
		if(FastNrf_NrfIsOnline())
		{
			printf("Nrf Online, Mode:Rx\n");
			
			NRF_Fast_t NrfInitInfo = 
			{
				.Mode = NRF_Mode_Rx,
				.address = (uint8_t*)"abcde",
				.addressLength = 5,
				.rfChannel = channel,
				.speed = 1
			};
			
			FastNrf_Config(&NrfInitInfo);
			
			FastNrf_InitRecvBuff(&NRF_Fast, (uint8_t*)recvBufA, (uint8_t*)recvBufB, RECV_BUF_SIZE);
			return 1;
		}
		else
		{
			printf("Nrf Offline\n");
			return -1;
		}
	}
	else
	{
		
	}
}

float FastNrf_CalSpeed(uint32_t totalBytes, uint32_t elapsTimeMs)
{
	return (float)totalBytes / elapsTimeMs * 1000.0f / 1024.0f;
}

void printfBuf(uint8_t* buf, uint32_t length)
{
	printf("Buf:\n");
	for(uint32_t i = 0; i < length; i++)
	{
		printf("0x%02x,",buf[i]);
		if(i % 8 == 7)printf("\n");
	}
	printf("end\n");
}

//获取可用的缓冲区
static NRF_DataBuf_t *getAvaliableRecvBuf(NRF_Fast_t* nrf)
{
	if(nrf->recvBufA.buf == NULL || nrf->recvBufB.buf == NULL)
		return NULL;
	
	if( ! nrf->recvBufA.isFull)					//有可用的，返回可用缓冲区
		return &nrf->recvBufA;
	if( ! nrf->recvBufB.isFull)
		return &nrf->recvBufB;
																			//无可用的，返回最旧的缓冲区
	if(nrf->recvBufA.finishTime < nrf->recvBufB.finishTime)
		return &nrf->recvBufA;
	return &nrf->recvBufB;
}

//初始化接收双缓冲区
void FastNrf_InitRecvBuff(NRF_Fast_t* nrf, uint8_t *bufA, uint8_t* bufB, uint32_t oneBufSize)
{
	nrf->recvBufA.buf = bufA;
	nrf->recvBufA.isFull = 0;
	nrf->recvBufA.size = oneBufSize;
	nrf->recvBufB.buf = bufB;
	nrf->recvBufB.isFull = 0;
	nrf->recvBufB.size = oneBufSize;
}

//从初始化的缓冲区内读取数据，数据量不大于oneBufSize
int8_t FastNrf_GetData(NRF_Fast_t* nrf, uint8_t* dstBuf, uint32_t* totalReaddedSize)
{
	if(nrf->recvBufA.isFull && nrf->recvBufB.isFull)
	{																		//二者都满，取最新
		if(nrf->recvBufA.finishTime < nrf->recvBufB.finishTime)
		{
			memcpy(dstBuf, nrf->recvBufB.buf,  nrf->recvBufB.nowHasRecved);
			* totalReaddedSize = nrf->recvBufB.nowHasRecved;
			nrf->recvBufB.isFull = 0;
			return 2;
		}
		else
		{
			memcpy(dstBuf, nrf->recvBufA.buf,  nrf->recvBufA.nowHasRecved);
			* totalReaddedSize = nrf->recvBufA.nowHasRecved;
			nrf->recvBufA.isFull = 0;
			return 1;
		}
	}
	
	if(nrf->recvBufA.isFull)						//A满了
	{
		memcpy(dstBuf, nrf->recvBufA.buf,  nrf->recvBufA.nowHasRecved);
		* totalReaddedSize = nrf->recvBufA.nowHasRecved;
		nrf->recvBufA.isFull = 0;
		return 1;
	}
	if(nrf->recvBufB.isFull)						//B满了
	{
		memcpy(dstBuf, nrf->recvBufB.buf,  nrf->recvBufB.nowHasRecved);
		* totalReaddedSize = nrf->recvBufB.nowHasRecved;
		nrf->recvBufB.isFull = 0;
		return 2;
	}
	
	return 0;														//都没数
}

//初始化发送双缓冲区
void FastNrf_InitSendBuff(NRF_Fast_t* nrf, uint8_t *bufA, uint8_t* bufB, uint32_t oneBufSize)
{
	nrf->sendBufA.buf = bufA;
	nrf->sendBufA.isFull = 0;
	nrf->sendBufA.size = oneBufSize;
	nrf->sendBufB.buf = bufB;
	nrf->sendBufB.isFull = 0;
	nrf->sendBufB.size = oneBufSize;
}

//发送buf内数据
int8_t FastNrf_SendData(NRF_Fast_t* nrf, uint8_t* srcBuf, uint32_t totalSizeToSend)
{
	NRF_DataBuf_t *freeDataBuf = NULL;
	if(nrf->sendBufA.buf == NULL || nrf->sendBufB.buf == NULL)
		return -1;											//未初始化
	
	totalSizeToSend = (totalSizeToSend % 32) ? totalSizeToSend + (32 - (totalSizeToSend % 32)) : totalSizeToSend;
	
	if(!nrf->sendBufA.isFull)
		freeDataBuf = &nrf->sendBufA;
	if(!nrf->sendBufB.isFull)
		freeDataBuf = &nrf->sendBufB;
	if(freeDataBuf == NULL)
	{
		if(nrf->Status == NRF_Status_Busy)
			return 0;											//buf全满且处于正常发送中
		if(nrf->Status == NRF_Status_Error)
		{																//上次发送失败了，准备复位状态机重发
			freeDataBuf = &nrf->sendBufA;
		}
	}
	
	if(totalSizeToSend > freeDataBuf->size - 64)
		return -2;											//要发送的数据过多
	
	
	memcpy(freeDataBuf->buf, startMark, 8);
	freeDataBuf->buf[8] = 0;					//合成包头包尾数据类型等参数
	freeDataBuf->buf[9] = 1;
	memcpy(freeDataBuf->buf + 10, &totalSizeToSend, 4);
	memcpy(freeDataBuf->buf + 32, srcBuf, totalSizeToSend);
	memcpy(freeDataBuf->buf + 32 + totalSizeToSend, endMark, 8);
	
	freeDataBuf->isFull = 1;					//缓冲区被填满了
	freeDataBuf->nowHasSendded = 0;
	freeDataBuf->totalToSend = totalSizeToSend + 64;
	freeDataBuf->finishTime = HAL_GetTick();
	
	
	if(nrf->Status != NRF_Status_Busy)
	{
		nrf->Status = NRF_Status_Busy;
		
																		//写入NRF激活发送
		uint8_t status = FastNrf_writeBuf(WR_TX_PLOAD, freeDataBuf->buf, 32);
		uint8_t tempOffset = 32;
		while(status & 0x01)
		{																//只要FIFO是空的
			status = FastNrf_writeBuf(WR_TX_PLOAD, freeDataBuf->buf + tempOffset, 32);
			tempOffset += 32;
			if(tempOffset >= 32 * 4)			//正常来说肯定不会触发，因为NRF硬件FIFO深度为3*32字节
				break;
		}
	}
	
	return 1;
}

//获取可用的发送缓冲区
static NRF_DataBuf_t *getAvaliableSendBuf(NRF_Fast_t* nrf)
{
	if(nrf->sendBufA.isFull && nrf->sendBufB.isFull)
	{																	//发送缓冲区A和B都有数据，取最新的一包数据发送，同时清空旧数据
		if(nrf->sendBufA.finishTime < nrf->sendBufB.finishTime)
		{
			nrf->sendBufA.isFull = 0;
			return &nrf->sendBufB;
		}
		else
		{
			nrf->sendBufB.isFull = 0;
			return &nrf->sendBufA;
		}
	}
	
	if(nrf->sendBufA.isFull)
		return &nrf->sendBufA;
	
	return &nrf->sendBufB;
}


