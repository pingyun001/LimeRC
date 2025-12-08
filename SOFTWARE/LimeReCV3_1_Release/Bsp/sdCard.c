#include "sdCard.h"
#include "sdmmc.h"
#include <string.h>

#if 0
#define DEBUG_LOG(...)	LEprintf(__VA_ARGS__)
#else
#define DEBUG_LOG(...)
#endif

HAL_StatusTypeDef Lime_SDCard_Init(void)
{
	/* Check SD Card is inserted */
	if( !IS_TF_CARD_INSERTED())
		return HAL_ERROR;
	
	hsd1.Instance = SDMMC1;
  hsd1.Init.ClockEdge = SDMMC_CLOCK_EDGE_RISING;
  hsd1.Init.ClockPowerSave = SDMMC_CLOCK_POWER_SAVE_DISABLE;
  hsd1.Init.BusWide = SDMMC_BUS_WIDE_4B;
  hsd1.Init.HardwareFlowControl = SDMMC_HARDWARE_FLOW_CONTROL_DISABLE;
  hsd1.Init.ClockDiv = 4-1;
  
	return HAL_SD_Init(&hsd1);
}

static uint8_t sdCard_Is_Busy(void)
{
	if( !IS_TF_CARD_INSERTED())
		return 1;
	
	if(hsd1.State != HAL_SD_STATE_READY)
		return 1;
	
	if(HAL_SD_GetCardState(&hsd1) != HAL_SD_CARD_TRANSFER)
		return 1;
	
	return 0;
}

HAL_StatusTypeDef Lime_SDCard_GetCardInfo(HAL_SD_CardInfoTypeDef *cardInfo)
{
	/* Check SD Card is inserted */
	if( !IS_TF_CARD_INSERTED())
		return HAL_ERROR;
	
	return HAL_SD_GetCardInfo(&hsd1, cardInfo);
}

HAL_StatusTypeDef Lime_SDCard_Erase(uint32_t BlockStartAdd, uint32_t BlockEndAdd)
{
	/* Check SD Card is inserted */
	if( !IS_TF_CARD_INSERTED())
		return HAL_ERROR;
	
	HAL_StatusTypeDef ret = HAL_OK;
	uint32_t startTime = 0;
	
	ret = HAL_SD_Erase(&hsd1, BlockStartAdd, BlockEndAdd);
	if(ret != HAL_OK)
		return ret;
	
	startTime = HAL_GetTick();
	while(sdCard_Is_Busy())
	{
		if(HAL_GetTick() - startTime > 1000)
			return HAL_TIMEOUT;
	}
	
	return HAL_OK;
}


uint32_t catSdTxBuf[4096 / 4 + 8] __attribute__((section("LIME_RAM_D1"), aligned(32))) = {0};
//be careful: pData must not in DTCM when using DMA!
HAL_StatusTypeDef Lime_SDCard_Write(const uint8_t *pData, uint32_t BlockAdd, uint32_t NumberOfBlocks)
{
	DEBUG_LOG("%s(%#x, add:%d, nb:%d)\n", __FUNCTION__, pData, BlockAdd, NumberOfBlocks);
//	DEBUG_LOG("SD State:%#x\n", HAL_SD_GetCardState(&hsd1));
	
	/* Check SD Card is inserted */
	if( !IS_TF_CARD_INSERTED())
		return HAL_ERROR;
	
	HAL_StatusTypeDef ret = HAL_OK;
	uint32_t startTime = 0;
	
	/* Waiting SD Card Ready */
	startTime = HAL_GetTick();
	while(sdCard_Is_Busy())
	{
		if(HAL_GetTick() - startTime > 1000)
		{
			DEBUG_LOG("Timeout1\n");
			return HAL_TIMEOUT;
		}
	}

#if 0
	
	/* Write Data in Polling Mode */
	ret = HAL_SD_WriteBlocks(&hsd1, pData, BlockAdd, NumberOfBlocks, 1000);
#else
	
	/* Write Data in DMA Mode */
	memcpy(catSdTxBuf, pData, 4096);
	SCB_CleanDCache_by_Addr((uint32_t*)catSdTxBuf, NumberOfBlocks * 512);
	ret = HAL_SD_WriteBlocks_DMA(&hsd1, (const uint8_t*)catSdTxBuf, BlockAdd, NumberOfBlocks);
#endif
	if(ret != HAL_OK)
	{
		DEBUG_LOG("Err Return:%d, SD Driver:%d\n", ret, hsd1.ErrorCode);
		return ret;
	}
	
	/* Waiting SD Card Ready */
	startTime = HAL_GetTick();
	while(sdCard_Is_Busy())
	{
		if(HAL_GetTick() - startTime > 1000)
		{
			DEBUG_LOG("Timeout2\n");
			return HAL_TIMEOUT;
		}
	}
	
	return HAL_OK;
}



uint32_t catSdRxBuf[4096 / 4 + 8] __attribute__((section("LIME_RAM_D1"), aligned(32))) = {0};
//do not change LIME_RAM_D1!!!!!!!!!!

HAL_StatusTypeDef Lime_SDCard_Read(uint8_t *pData, uint32_t BlockAdd, uint32_t NumberOfBlocks)
{
	DEBUG_LOG("%s(%#x, add:%d, nb:%d)\n", __FUNCTION__, pData, BlockAdd, NumberOfBlocks);
//	DEBUG_LOG("SD State:%#x\n", HAL_SD_GetCardState(&hsd1));
	
	/* Check SD Card is inserted */
	if( !IS_TF_CARD_INSERTED())
		return HAL_ERROR;
	
	HAL_StatusTypeDef ret = HAL_OK;
	uint32_t startTime = 0;

	/* Waiting SD Card Ready */
	startTime = HAL_GetTick();
	while(sdCard_Is_Busy())
	{
		if(HAL_GetTick() - startTime > 2000)
		{
			DEBUG_LOG("Timeout3\n");
			return HAL_TIMEOUT;
		}
	}

#if 0
	
	/* Read Data in Polling Mode */
	ret = HAL_SD_ReadBlocks(&hsd1, pData, BlockAdd, NumberOfBlocks, 1000);
	if(ret != HAL_OK)
	{
		DEBUG_LOG("Err Return:%d\n", ret);
		return ret;
	}
#else
	
	/*It seem like FatFs Lib has some bugs, need to catRxBuf as temp data buffer.*/
	
	/* Read Data in DMA Mode */
	SCB_InvalidateDCache_by_Addr((uint32_t*)catSdRxBuf, NumberOfBlocks * 512);
	ret = HAL_SD_ReadBlocks_DMA(&hsd1, (uint8_t*)catSdRxBuf, BlockAdd, NumberOfBlocks);
	if(ret != HAL_OK)
	{
		DEBUG_LOG("Err Return:%d, SD Driver:%d\n", ret, hsd1.ErrorCode);
		return ret;
	}
	
	/* Waiting SD Driver Ready */
	startTime = HAL_GetTick();
	while(hsd1.State != HAL_SD_STATE_READY)
	{
		if(HAL_GetTick() - startTime > 2000)
		{
			DEBUG_LOG("Timeout4\n");
			return HAL_TIMEOUT;
		}
	}
	
	memcpy(pData, catSdRxBuf, NumberOfBlocks * 512);
#endif
	
	/* Waiting SD Card Ready */
	startTime = HAL_GetTick();
	while(sdCard_Is_Busy())
	{
		if(HAL_GetTick() - startTime > 2000)
		{
			DEBUG_LOG("Timeout5\n");
			return HAL_TIMEOUT;
		}
	}
	
	return HAL_OK;
}



