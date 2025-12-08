#include "key.h"


KeyInfo_t KeyInfo = {0};

const uint16_t offsetBook[] = OFFSET_BOOK_LIST;

static uint8_t buf[4] = {0};

static uint16_t ScanHC165(void);

uint8_t key_HC165Scan(uint16_t* data)
{
#if 1
	*data = ScanHC165();
#else
	HAL_GPIO_WritePin(HC165_NLOAD_GPIO_Port, HC165_NLOAD_Pin, GPIO_PIN_RESET);
	HAL_GPIO_WritePin(HC165_NLOAD_GPIO_Port, HC165_NLOAD_Pin, GPIO_PIN_SET);
	
	
	if(HAL_SPI_Receive(&hspi1, (uint8_t*)data, 2, 10) != HAL_OK)
		return 0;
#endif
	return 1;
}

void key_ScanHandle(void)
{
	static uint16_t lastRawData = 0xffff;
	static uint8_t lastKRData = 0;
	uint8_t KRData = 0, KRNData = 0;
	uint16_t nowRawData = 0xffff;
	uint16_t changeVal = 0;
	
	if(key_HC165Scan(&nowRawData))
	{
		changeVal = lastRawData ^ nowRawData;
		
		uint8_t* keyIndexPin = &KeyInfo.sw_up;
		
		for(uint16_t i = 0; i < sizeof(offsetBook) / sizeof(uint16_t); i++)
		{
			if(changeVal & offsetBook[i])
			{
				(*keyIndexPin) ++;
			}
			keyIndexPin ++;
		}
		
		KeyInfo.refreshTime = HAL_GetTick();
		
		lastRawData = nowRawData;
	}
	
	KRData = (HAL_GPIO_ReadPin(KR_GPIO_Port, KR_Pin) == GPIO_PIN_RESET) ? 1 : 0;
	KRNData = KRData ^ lastKRData;
	
	if(KRNData)
		KeyInfo.kr ++;
	
	lastKRData = KRData;
}

uint8_t key_GetHardwareReversion(KeyInfo_t * info, uint8_t* reversion)
{
	uint16_t data16 = 0;
	if(key_HC165Scan(&data16))
	{
		*reversion = KEY_GET_HARDWARE_REV(data16) + 1;
		info->reversion = *reversion;
		
		return 1;
	}
	
	return 0;
}

static uint16_t ScanHC165(void)
{
	uint16_t data = 0, i = 0;
	
	/* LOD 0-> 1		Lock Data */
	HAL_GPIO_WritePin(HC165_NLOAD_GPIO_Port, HC165_NLOAD_Pin,GPIO_PIN_RESET);
	HAL_GPIO_WritePin(HC165_NLOAD_GPIO_Port, HC165_NLOAD_Pin,GPIO_PIN_SET);
	
	/* Read Data */
	for(i = 0;i < 16;i++)
	{
		if(HAL_GPIO_ReadPin(HC165_QOUT_GPIO_Port, HC165_QOUT_Pin) == GPIO_PIN_SET)data |= 0x01;
		HAL_GPIO_WritePin(HC165_CLK_GPIO_Port, HC165_CLK_Pin,GPIO_PIN_SET);
		HAL_GPIO_WritePin(HC165_CLK_GPIO_Port, HC165_CLK_Pin,GPIO_PIN_RESET);
		data <<= 1;
	}
	if(HAL_GPIO_ReadPin(HC165_QOUT_GPIO_Port, HC165_QOUT_Pin) == GPIO_PIN_SET)data |= 0x01;
	HAL_GPIO_WritePin(HC165_NLOAD_GPIO_Port, HC165_NLOAD_Pin,GPIO_PIN_RESET);
	HAL_GPIO_WritePin(HC165_CLK_GPIO_Port, HC165_CLK_Pin,GPIO_PIN_SET);
	return data;
}

