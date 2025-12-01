#include "Lime_nrf_slave_ports.h"
#include "Lime_nrf_video.h"
#include "spi.h"

#include <string.h>
#include <stdio.h>

static uint8_t txTempBuf[33] = {0};
static uint8_t rxTempBuf[33] = {0};

static uint8_t txApiBuf[256] = {0};
static uint8_t rxApiBuf[256] = {0};

LimeNrfSlave_t LimeInfoNrf = 
{
	.sendBuff = txApiBuf,
	.sendBufTotalSize = 256,
	.readBuff = rxApiBuf,
	.readBufTotalSize = 256,
	
	.nrf_Init = Info_Nrf_Slave_Init,
	.nrf_ChangeBspModeTo = Info_Nrf_ChangeModeTo,
	.nrf_SendPack32 = Info_Nrf_SendPack32,
	.nrf_NewPackAvaliable = Info_Nrf_NewPackAvaliable,
	.nrf_ReadPack32 = Info_Nrf_ReceivePack32,
	.nrf_PowerCtrl = Info_Nrf_PowerCtrl,
};

#if ((1) && GLOBAL_DEBUG_LOG_EN)
	#define DEBUG_LOG(...)	printf(__VA_ARGS__)
#else
	#define DEBUG_LOG(...)
#endif

#define INFO_NRF_IRQ_LEVEL_READ()		HAL_GPIO_ReadPin(NRF2_IRQ_GPIO_Port, NRF2_IRQ_Pin)

static HAL_StatusTypeDef Info_Nrf_PowerCtrl(bool isPowerOn)
{
	if(isPowerOn)
		HAL_GPIO_WritePin(NRF2_EN_GPIO_Port, NRF2_EN_Pin, GPIO_PIN_RESET);
	else
		HAL_GPIO_WritePin(NRF2_EN_GPIO_Port, NRF2_EN_Pin, GPIO_PIN_SET);
	
	return HAL_OK;
}

static void Info_Nrf_CE_Ctrl(uint8_t status)
{
	if(status)
		HAL_GPIO_WritePin(NRF2_CE_GPIO_Port, NRF2_CE_Pin, GPIO_PIN_SET);
	else
		HAL_GPIO_WritePin(NRF2_CE_GPIO_Port, NRF2_CE_Pin, GPIO_PIN_RESET);
}

static void Info_Nrf_SpiTxRxDate(uint8_t* txData, uint8_t *rxDate, uint16_t length)
{
	HAL_GPIO_WritePin(NRF2_CSN_GPIO_Port, NRF2_CSN_Pin, GPIO_PIN_RESET);
	HAL_SPI_TransmitReceive(&hspi2, txData, rxDate,length, 100);
	HAL_GPIO_WritePin(NRF2_CSN_GPIO_Port, NRF2_CSN_Pin, GPIO_PIN_SET);
}

static uint8_t Info_Nrf_spi_read_write_byte(uint8_t txdata)
{
	uint8_t rxdata;
	Info_Nrf_SpiTxRxDate(&txdata,&rxdata,1);
	return rxdata;
}

static uint8_t Info_Nrf_write_reg(uint8_t reg, uint8_t value)
{	
	txTempBuf[0] = reg;
	txTempBuf[1] = value;
	
	Info_Nrf_SpiTxRxDate(txTempBuf, rxTempBuf, 2);

	return rxTempBuf[0];
}

static uint8_t Info_Nrf_read_reg(uint8_t reg)
{
	uint8_t reg_val;

	Info_Nrf_spi_read_write_byte(reg);
	reg_val = Info_Nrf_spi_read_write_byte(0Xff);

	return reg_val;
}

static uint8_t Info_Nrf_writeBuf(uint8_t reg, uint8_t *pbuf, uint8_t len)
{
  uint8_t status;
	
	txTempBuf[0] = reg;
	memcpy(txTempBuf + 1, pbuf, len);
		
	Info_Nrf_SpiTxRxDate(txTempBuf, rxTempBuf, len + 1);
	
	status = rxTempBuf[0];
    
  return status;
}

static uint8_t Info_Nrf_read_buf(uint8_t reg, uint8_t *pbuf, uint8_t len)
{
	uint8_t status;
	
	memset(txTempBuf, 0xff, len + 1);
	txTempBuf[0] = reg;
	
	
	Info_Nrf_SpiTxRxDate(txTempBuf, rxTempBuf, len + 1);
	
	status = rxTempBuf[0];
	
	memcpy(pbuf, rxTempBuf + 1, len);
    
  return status;
}

static uint8_t readAndCleraStatusReg(void)
{
	uint8_t nrfStatus = 0;

	nrfStatus = Info_Nrf_write_reg(NRF_WRITE_REG + STATUS, 0x70);

	return nrfStatus;
}

static uint8_t Info_Nrf_NrfIsOnline(void)
{
	uint8_t bufA[5] = {0XA6, 0XA6, 0XA6, 0XA6, 0XA6};
	uint8_t bufB[5] = {0X32, 0X32, 0X32, 0X32, 0X32};
  uint8_t i;
		
	Info_Nrf_CE_Ctrl(0);
	HAL_Delay(1);
	
	if(INFO_NRF_IRQ_LEVEL_READ() == GPIO_PIN_RESET)
	{
		readAndCleraStatusReg();
	}
		
	Info_Nrf_writeBuf(NRF_WRITE_REG + TX_ADDR, bufA, 5);
	memset(bufA, 0, sizeof(bufA));
	Info_Nrf_read_buf(TX_ADDR, bufA, 5);
	for (i = 0; i < 5; i++)
	{
			if (bufA[i] != 0XA6) break;
	}
	if (i != 5) return 0;
	
	Info_Nrf_writeBuf(NRF_WRITE_REG + TX_ADDR, bufB, 5);
	memset(bufB, 0, sizeof(bufB));
	Info_Nrf_read_buf(TX_ADDR, bufB, 5);
	for (i = 0; i < 5; i++)
	{
			if (bufB[i] != 0X32) break;
	}
	if (i != 5) return 0;

	return 1;
}

uint8_t defaultInfoAddress[5] = {0xE1,0xE2,0xE3,0xE4,0XE5};

HAL_StatusTypeDef Info_Nrf_Slave_Init(uint8_t channel)
{
	if(Info_Nrf_NrfIsOnline())
	{
		DEBUG_LOG("Bsp:Info Nrf is Online\n");
	}
	else
	{
		DEBUG_LOG("Bsp:Info Nrf is offline!!!!!!!!!!!!!\n");
		return HAL_TIMEOUT;
	}
	
	Info_Nrf_CE_Ctrl(0);
	
	Info_Nrf_write_reg(FLUSH_RX, 0xff);
	Info_Nrf_write_reg(FLUSH_TX, 0xff);
	
	Info_Nrf_writeBuf( NRF_WRITE_REG + TX_ADDR, defaultInfoAddress, 5);
	Info_Nrf_writeBuf( NRF_WRITE_REG + RX_ADDR_P0, defaultInfoAddress, 5);
	Info_Nrf_write_reg( NRF_WRITE_REG + EN_RXADDR, 0x01);
	Info_Nrf_write_reg( NRF_WRITE_REG + RF_CH, channel);
	Info_Nrf_write_reg( NRF_WRITE_REG + RX_PW_P0, 32);
	Info_Nrf_write_reg( NRF_WRITE_REG + SETUP_RETR, 0x1F);
	Info_Nrf_write_reg( NRF_WRITE_REG + RF_SETUP, 0x27);
	Info_Nrf_write_reg( NRF_WRITE_REG + EN_AA, 0x01);
	
	readAndCleraStatusReg();
	
	return HAL_OK;
}

//已测试非常正确
HAL_StatusTypeDef Info_Nrf_ChangeModeTo(LimeNrfSlave_BspMode_e mode)
{
	Info_Nrf_CE_Ctrl(0);
	
	if(mode == Info_Nrf_Mode_Transmit)
	{
		Info_Nrf_write_reg( NRF_WRITE_REG + CONFIG, 0x0E);
	}
	else if(mode == Info_Nrf_Mode_Receive)
	{
		Info_Nrf_write_reg( NRF_WRITE_REG + CONFIG, 0x0F);
	}
	else
		return HAL_ERROR;
	
	Info_Nrf_CE_Ctrl(1);
	
	return HAL_OK;
}

HAL_StatusTypeDef Info_Nrf_SendPack32(uint8_t buf[32])
{
	Info_Nrf_writeBuf(WR_TX_PLOAD, buf, 32);
	
	uint32_t startTime = HAL_GetTick();
	while(INFO_NRF_IRQ_LEVEL_READ() == GPIO_PIN_SET)
	{
		if(HAL_GetTick() - startTime > 10)
		{
			DEBUG_LOG("SendTimeOut\n");
			return HAL_TIMEOUT;
		}
	}
	
	uint8_t res = readAndCleraStatusReg();
	
	if(res & (1 << 4))
	{
		return HAL_TIMEOUT;
	}
	else if(res & (1 << 5))
	{
		return HAL_OK;
	}
	
	return HAL_ERROR;
}

uint8_t Info_Nrf_NewPackAvaliable(void)
{
	if(INFO_NRF_IRQ_LEVEL_READ() == GPIO_PIN_RESET)
	{
		uint8_t res = readAndCleraStatusReg();
		if(res & (1 << 6))
		{
			return 1;
		}
	}
	
	return 0;
}

HAL_StatusTypeDef Info_Nrf_ReceivePack32(uint8_t buf[32])
{
	Info_Nrf_read_buf(RD_RX_PLOAD, buf, 32);
	Info_Nrf_write_reg(FLUSH_RX, 0xff);
	
	return HAL_OK;
}

