#include "Lime_nrf_video.h"
#include <string.h>

#if 1
	#define DEBUG_LOG(...)	LEprintf(__VA_ARGS__)
#else
	#define DEBUG_LOG(...)
#endif

//-------------------------------BSP PORTS-----------------------------------//
#include "spi.h"

static uint8_t pollingTxBuf[64] __attribute__((section("LIME_RAM_D2"), aligned(32))) = {0};
static uint8_t pollingRxBuf[64] __attribute__((section("LIME_RAM_D2"), aligned(32))) = {0};
static uint8_t txTempBuf[33] = {0};
static uint8_t rxTempBuf[33] = {0};

#define VIDEO_NRF_IRQ_LEVEL_READ()	HAL_GPIO_ReadPin(NRF1_IRQ_GPIO_Port, NRF1_IRQ_Pin)
#define VIDEO_SPI_IS_BUSY()					(hspi4.State != HAL_SPI_STATE_READY)

static void Video_Nrf_CE_Ctrl(bool level)
{
	if(level)
		HAL_GPIO_WritePin(NRF1_CE_GPIO_Port, NRF1_CE_Pin, GPIO_PIN_SET);
	else
		HAL_GPIO_WritePin(NRF1_CE_GPIO_Port, NRF1_CE_Pin, GPIO_PIN_RESET);
}

static void Video_Nrf_SpiTxRxDate(uint8_t* txData, uint8_t *rxDate, uint16_t length, bool isPolling)
{
	if(isPolling)
	{
		while(VIDEO_SPI_IS_BUSY())
			;
		memcpy(pollingTxBuf, txData, length);
		SCB_CleanDCache_by_Addr((uint32_t*)pollingTxBuf, 32);
		SCB_InvalidateDCache_by_Addr((uint32_t*)pollingRxBuf, 32);
		HAL_SPI_TransmitReceive_DMA(&hspi4, pollingTxBuf, pollingRxBuf, length);
		while(VIDEO_SPI_IS_BUSY())
			;
		memcpy(rxDate, pollingRxBuf, length);
	}
	else
	{
		while(VIDEO_SPI_IS_BUSY())
			;
		HAL_SPI_TransmitReceive_DMA(&hspi4, txData, rxDate, length);
	}
}

//-------------------------------SUB PORTS-----------------------------------//

static uint8_t Video_Nrf_write_reg(uint8_t reg, uint8_t value)
{
	uint16_t txBuf, rxBuf = 0;
	txBuf = (value << 8) | reg;
	
	Video_Nrf_SpiTxRxDate((uint8_t*)&txBuf, (uint8_t*)&rxBuf, 2, true);

	return rxBuf & 0xff;
}

static uint8_t Video_Nrf_writeBuf(uint8_t reg, uint8_t *pbuf, uint8_t len)
{
  uint8_t status;
	
	txTempBuf[0] = reg;
	memcpy(txTempBuf + 1, pbuf, len);
		
	Video_Nrf_SpiTxRxDate(txTempBuf, rxTempBuf, len + 1, true);
	
	status = rxTempBuf[0];
    
  return status;
}

static uint8_t Video_Nrf_read_buf(uint8_t reg, uint8_t *pbuf, uint8_t len)
{
	uint8_t status;
	
	memset(txTempBuf, 0xff, len + 1);
	txTempBuf[0] = reg;
	
	
	Video_Nrf_SpiTxRxDate(txTempBuf, rxTempBuf, len + 1, true);
	
	status = rxTempBuf[0];
	
	memcpy(pbuf, rxTempBuf + 1, len);
    
  return status;
}

static uint8_t readAndCleraStatusReg(void)
{
	uint8_t nrfStatus = 0;

	nrfStatus = Video_Nrf_write_reg(NRF_WRITE_REG + STATUS, 0x70);

	return nrfStatus;
}

//--------------------------------LOGIC--------------------------------------//

static uint8_t Video_Nrf_NrfIsOnline(void)
{
	uint8_t bufA[5] = {0XA6, 0XA6, 0XA6, 0XA6, 0XA6};
	uint8_t bufB[5] = {0X32, 0X32, 0X32, 0X32, 0X32};
  uint8_t i;
		
	Video_Nrf_CE_Ctrl(0);
	HAL_Delay(1);
	
	if(VIDEO_NRF_IRQ_LEVEL_READ() == GPIO_PIN_RESET)
	{
		readAndCleraStatusReg();
	}
		
	Video_Nrf_writeBuf(NRF_WRITE_REG + TX_ADDR, bufA, 5);
	memset(bufA, 0, sizeof(bufA));
	Video_Nrf_read_buf(TX_ADDR, bufA, 5);
	for (i = 0; i < 5; i++)
	{
			if (bufA[i] != 0XA6) break;
	}
	if (i != 5) return 0;
	
	Video_Nrf_writeBuf(NRF_WRITE_REG + TX_ADDR, bufB, 5);
	memset(bufB, 0, sizeof(bufB));
	Video_Nrf_read_buf(TX_ADDR, bufB, 5);
	for (i = 0; i < 5; i++)
	{
			if (bufB[i] != 0X32) break;
	}
	if (i != 5) return 0;

	return 1;
}

//uint8_t defaultVideoAddress[5] = {0xE5,0x01,0x02,0x03,0x04};
uint8_t defaultVideoAddress[5] = {'a','b','c','d','e'};

HAL_StatusTypeDef Lime_nrf_video_Init(uint8_t channel, bool isReceiver)
{
	Video_Nrf_CE_Ctrl(0);
	
	if( !Video_Nrf_NrfIsOnline())
	{
		DEBUG_LOG("%s():NRF Offline!!!\n", __FUNCTION__);
		return HAL_ERROR;
	}
	DEBUG_LOG("%s():NRF Online!!!\n", __FUNCTION__);
	
	Video_Nrf_writeBuf( NRF_WRITE_REG + TX_ADDR, defaultVideoAddress, 5);
	Video_Nrf_writeBuf( NRF_WRITE_REG + RX_ADDR_P0, defaultVideoAddress, 5);
	Video_Nrf_write_reg( NRF_WRITE_REG + EN_RXADDR, 0x01);
	Video_Nrf_write_reg( NRF_WRITE_REG + RF_CH, channel);
	Video_Nrf_write_reg( NRF_WRITE_REG + RX_PW_P0, 32);
	Video_Nrf_write_reg( NRF_WRITE_REG + SETUP_RETR, 0x1F);
	Video_Nrf_write_reg( NRF_WRITE_REG + RF_SETUP, 0x0F);
	Video_Nrf_write_reg( NRF_WRITE_REG + EN_AA, 0x01);
	Video_Nrf_write_reg(NRF_WRITE_REG + FEATURE, 0x04);
	Video_Nrf_write_reg(NRF_WRITE_REG + DYNPD, 0x01);
	
	readAndCleraStatusReg();
	
	if(isReceiver)
	{
		Video_Nrf_write_reg( NRF_WRITE_REG + CONFIG, 0x0F);
	}
	else
	{
		Video_Nrf_write_reg( NRF_WRITE_REG + CONFIG, 0x0E);
	}
	
	Video_Nrf_write_reg(FLUSH_RX, 0xff);
	Video_Nrf_write_reg(FLUSH_TX, 0xff);
	
	Video_Nrf_CE_Ctrl(1);
	
	return HAL_OK;
}



static int8_t thisPackIsStartOrEnd(uint8_t packBuf[32])
{
	uint32_t* HeadCheckBuf = (uint32_t * )packBuf;
	
	
//	LEprintfBuf(packBuf, 32);
	
	if(HeadCheckBuf[0] == 0x11223344 && HeadCheckBuf[1] == 0x55667788)
	{
//		LEprintf(">>ST\n");
		return 1;
	}
	if(HeadCheckBuf[0] == 0x88776655 && HeadCheckBuf[1] == 0x44332211)
	{
//		LEprintf(">>ED\n");
		return 2;
	}
//	LEprintf("DF,%#x\n", packBuf[0]);
	return 0;
}

bool is_trans_start_from_exti = false;
bool is_trans_start_from_rx_ds = false;
bool is_trans_start_from_user_tx = false;



#define VIDEO_RX_RES_BUF_SIZE		10*1024
static uint8_t video_rx_res_buf_A[VIDEO_RX_RES_BUF_SIZE] __attribute__((section("LIME_RAM_D2"), aligned(32))) = {0};
static uint8_t video_rx_res_buf_B[VIDEO_RX_RES_BUF_SIZE] __attribute__((section("LIME_RAM_D2"), aligned(32))) = {0};
static uint8_t video_rx_res_buf_C[VIDEO_RX_RES_BUF_SIZE] __attribute__((section("LIME_RAM_D2"), aligned(32))) = {0};

video_pack_data_t video_pack_data_A = 
{
	.id = 1,
	.buf = video_rx_res_buf_A,
	.finish_time = 0,
};
video_pack_data_t video_pack_data_B = 
{
	.id = 2,
	.buf = video_rx_res_buf_B,
	.finish_time = 0,
};
video_pack_data_t video_pack_data_C = 
{
	.id = 3,
	.buf = video_rx_res_buf_C,
	.finish_time = 0,
};

static video_pack_data_t* video_get_avaliable_buf(void)
{
	if(video_pack_data_A.finish_time < video_pack_data_B.finish_time)
	{
		if(video_pack_data_A.finish_time < video_pack_data_C.finish_time)
			return &video_pack_data_A;
	}
	else
	{
		if(video_pack_data_B.finish_time < video_pack_data_C.finish_time)
			return &video_pack_data_B;
	}
	
	return &video_pack_data_C;
}

video_pack_data_t *Lime_nrf_video_GetAvaliableBuf(void)
{
	if(video_pack_data_A.finish_time > video_pack_data_B.finish_time)
	{
		if(video_pack_data_A.finish_time > video_pack_data_C.finish_time)
			return &video_pack_data_A;
	}
	else
	{
		if(video_pack_data_B.finish_time > video_pack_data_C.finish_time)
			return &video_pack_data_B;
	}
	
	return &video_pack_data_C;
}

void Lime_nrf_video_exti_irq_hook(void)
{
//	DEBUG_LOG("IRQ Occurred\n");
	
	is_trans_start_from_exti = 1;
	
	pollingTxBuf[0] = NRF_WRITE_REG + STATUS;
	pollingTxBuf[1] = 0x70;
	
	SCB_CleanDCache_by_Addr((uint32_t*)pollingTxBuf, 2);
	SCB_InvalidateDCache_by_Addr((uint32_t*)pollingRxBuf, 2);
	Video_Nrf_SpiTxRxDate(pollingTxBuf, pollingRxBuf, 2, false);
}

#include "nrfVideo.h"

void Lime_nrf_video_spi_irq_hook(void)
{
	DEBUG_R_PIN_0();
	
	if(is_trans_start_from_exti)
	{
		is_trans_start_from_exti = false;
		
		/* Rx DS */
		if(pollingRxBuf[0] & 0x40)
		{
			is_trans_start_from_rx_ds = true;
			
			/* read rx buff start */
			pollingTxBuf[0] = RD_RX_PLOAD;
			pollingTxBuf[1] = NOP;
			
			SCB_CleanDCache_by_Addr((uint32_t*)pollingTxBuf, 2);
			SCB_InvalidateDCache_by_Addr((uint32_t*)pollingRxBuf, 33);
			Video_Nrf_SpiTxRxDate(pollingTxBuf, pollingRxBuf, 33, false);
		}
	}
	else
	{
		/* read rx buff finish */
		if(is_trans_start_from_rx_ds)
		{
			is_trans_start_from_rx_ds = false;
			
			static uint32_t this_pack_total_length = 0;
			static uint32_t this_pack_recv_offset = 0;
			static video_pack_data_t* avaliable_pack_buf = &video_pack_data_A;
			
//			DEBUG_LOG("%#x,%#x\n", pollingRxBuf[0], pollingRxBuf[1]);
			
			int8_t id = thisPackIsStartOrEnd(pollingRxBuf + 1);
			switch(id)
			{
				/* head */
				case 1:
				{
					memcpy(&this_pack_total_length, pollingRxBuf + 10 + 1, 4);
					avaliable_pack_buf = video_get_avaliable_buf();
//					DEBUG_LOG("S%d,%d\n", this_pack_total_length, avaliable_pack_buf->id);
					this_pack_recv_offset = 0;
					break;
				}
				/* body */
				case 0:
				{
					if(this_pack_recv_offset >= VIDEO_RX_RES_BUF_SIZE)
						break;
					
					memcpy(avaliable_pack_buf->buf + this_pack_recv_offset, pollingRxBuf + 1, 32);
					this_pack_recv_offset += 32;
//					DEBUG_LOG("B\n");
					break;
				}
				/* tail */
				case 2:
				{
					if(this_pack_recv_offset == this_pack_total_length)
					{
						avaliable_pack_buf->finish_time = HAL_GetTick();
						avaliable_pack_buf->length = this_pack_total_length;
						nrfVideo_taskYIELD_exHook(true);
//						DEBUG_LOG("TS\n");
					}
//					else
//						DEBUG_LOG("TE\n");
					break;
				}
			}
		}
	}
	
	is_trans_start_from_user_tx = false;
	
	DEBUG_R_PIN_1();
}

HAL_StatusTypeDef Lime_nrf_video_SendData(uint8_t* srcBuf, uint32_t totalSizeToSend)
{
	is_trans_start_from_user_tx = true;
	
	
	return HAL_OK;
}

