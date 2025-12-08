#include "Lime_eeprom.h"
#include <stdio.h>
#include <string.h>

#if 0
  #define DEBUG_LOG(...)  LEprintf(__VA_ARGS__)
#else
  #define DEBUG_LOG(...)
#endif

#if 0
#define DELAY_MS(ms)	HAL_Delay(ms)
#else
#include "cmsis_os.h"
#define DELAY_MS(ms)	osDelay(ms)
#endif

#define EEPFUNC_CHECK_RETURN_RES(fun)	\
	do\
	{\
		HAL_StatusTypeDef ret = fun;\
		if(ret != HAL_OK)\
		{\
			DEBUG_LOG("%s(),line:%d, detech error! \n", __FUNCTION__, __LINE__);\
			return ret;\
		}\
	}while(0)

static HAL_StatusTypeDef Lime_eep_WriteData(Lime_eep_t *eepInfo, uint32_t address, uint8_t* buf, uint32_t length)
{
	if(eepInfo == NULL)
	{
		EEPFUNC_CHECK_RETURN_RES(HAL_ERROR);
		return HAL_ERROR;
	}
	
	uint32_t offsetPin = 0;
	uint32_t stepLength = 0;
	
	while(offsetPin < length)
	{
		stepLength = ((length - offsetPin) > eepInfo->writeWrapSize) ? eepInfo->writeWrapSize : (length - offsetPin);
		
//		DEBUG_LOG("%s(),address:%d, stepLength:%d, offset:%d, totalLength:%d\n", __FUNCTION__, address, stepLength, offsetPin, length);
		EEPFUNC_CHECK_RETURN_RES(eepInfo->write(address + offsetPin, buf + offsetPin, stepLength));
		DELAY_MS(eepInfo->latency);
		
		offsetPin += stepLength;
	}
	
//	DEBUG_LOG("%s(), successend\n", __FUNCTION__);
	
	return HAL_OK;
}

static HAL_StatusTypeDef Lime_eep_ReadData(Lime_eep_t *eepInfo, uint32_t address, uint8_t* buf, uint32_t length)
{
	if(eepInfo == NULL)
	{
		EEPFUNC_CHECK_RETURN_RES(HAL_ERROR);
		return HAL_ERROR;
	}
	
	uint32_t offsetPin = 0;
	uint32_t stepLength = 0;
	
	while(offsetPin < length)
	{
		stepLength = ((length - offsetPin) > eepInfo->readWrapSize) ? eepInfo->readWrapSize : (length - offsetPin);
		
//		DEBUG_LOG("%s(), stepLength:%d, offset:%d, totalLength:%d\n", __FUNCTION__, stepLength, offsetPin, length);
		EEPFUNC_CHECK_RETURN_RES(eepInfo->read(address + offsetPin, buf + offsetPin, stepLength));
		DELAY_MS(eepInfo->latency);
		
		offsetPin += stepLength;
	}
	
//	DEBUG_LOG("%s(), successend\n", __FUNCTION__);
	
	return HAL_OK;
}

HAL_StatusTypeDef Lime_eep_Init(Lime_eep_t *info, Lime_eep_data_t *defaultData, Lime_eep_data_t *syncData)
{
	DEBUG_LOG("%s()\n", __FUNCTION__);
	
	if(info == NULL)
	{
		DEBUG_LOG("Warning: PARA is NULL\n");
		return HAL_ERROR;
	}
	
	if(info->isDeviceOnline == NULL)
	{
		DEBUG_LOG("Warning: isDeviceOnline() is NULL\n");
		info->status = Lime_eep_status_ApiErr;
		return HAL_ERROR;
	}
	if(info->write == NULL)
	{
		DEBUG_LOG("Warning: write() is NULL\n");
		info->status = Lime_eep_status_ApiErr;
		return HAL_ERROR;
	}
	if(info->read == NULL)
	{
		DEBUG_LOG("Warning: read() is NULL\n");
		info->status = Lime_eep_status_ApiErr;
		return HAL_ERROR;
	}
	
	DELAY_MS(2);
	
	if(info->isDeviceOnline() != HAL_OK)
	{
		DEBUG_LOG("Warning: eeprom offline\n");
		info->status = Lime_eep_status_Offline;
		return HAL_ERROR;
	}
	
	info->status = Lime_eep_status_Initing;
	
	DELAY_MS(5);
	
	/* read all datas */
	EEPFUNC_CHECK_RETURN_RES(Lime_eep_ReadData(info, 0, info->eep_raw_data, EEP_TOTAL_SIZE));
	
//	Lime_eep_printBufAdv(info->eep_raw_data, EEP_TOTAL_SIZE, 0);
	
	/* check head & tail */
	if(info->eep_raw_data[0] != EEP_HEAD || info->eep_raw_data[EEP_TOTAL_SIZE - 1] != EEP_TAIL)
	{
		DEBUG_LOG("new eeprom, fill default para\n");
		
		/* new flash, check hardware environment */
		EEPFUNC_CHECK_RETURN_RES(Lime_eep_HardwareTest(info));
		
		/* fill default data */
		if(defaultData == NULL)
			memset(info->eep_raw_data, 0, EEP_TOTAL_SIZE);
		else
		{
			if(sizeof(Lime_eep_data_t) + 2 > EEP_TOTAL_SIZE)
				EEPFUNC_CHECK_RETURN_RES(HAL_ERROR);
			
			memcpy(info->eep_raw_data + 1, defaultData, sizeof(Lime_eep_data_t));
		}
		
		/* fill head & tail */
		info->eep_raw_data[0] = EEP_HEAD;
		info->eep_raw_data[EEP_TOTAL_SIZE - 1] = EEP_TAIL;
		
		/* write eeprom */
		EEPFUNC_CHECK_RETURN_RES(Lime_eep_WriteData(info, 0, info->eep_raw_data, EEP_TOTAL_SIZE));
		
		/* reread & check */
		memset(info->eep_raw_data, 0, EEP_TOTAL_SIZE);
		EEPFUNC_CHECK_RETURN_RES(Lime_eep_ReadData(info, 0, info->eep_raw_data, EEP_TOTAL_SIZE));
		if(info->eep_raw_data[0] != EEP_HEAD || info->eep_raw_data[EEP_TOTAL_SIZE - 1] != EEP_TAIL)
		{
			DEBUG_LOG("reWrite Failed\n");
			return HAL_ERROR;
		}
	}
	else
	{
		DEBUG_LOG("old eeprom, read para\n");
	}
	
	memcpy(&info->eep_saved_data, info->eep_raw_data + 1, sizeof(Lime_eep_data_t));
	memcpy(syncData, info->eep_raw_data + 1, sizeof(Lime_eep_data_t));
	
//	uint8_t buf[256] = {0};
//	EEPFUNC_CHECK_RETURN_RES(Lime_eep_ReadData(info, 0, buf, 256));
//	Lime_eep_printBufAdv(buf, 256, 0);
	
	info->status = Lime_eep_status_Idle;
	
	DEBUG_LOG("%s(),Success\n", __FUNCTION__);
	
	return HAL_OK;
}

HAL_StatusTypeDef Lime_eep_HardwareTest(Lime_eep_t *info)
{
	DEBUG_LOG("%s(),start\n", __FUNCTION__);
	
	if(info->status <= Lime_eep_status_Offline)
		return HAL_TIMEOUT;
	
	/* step 1: write 0xff */
	for(uint32_t i = 0; i < EEP_TOTAL_SIZE; i++)
		info->eep_raw_data[i] = 0xff;
	EEPFUNC_CHECK_RETURN_RES(Lime_eep_WriteData(info, 0, info->eep_raw_data, EEP_TOTAL_SIZE));
	
	/* rear & check */
	memset(info->eep_raw_data, 0xff, EEP_TOTAL_SIZE);
	EEPFUNC_CHECK_RETURN_RES(Lime_eep_ReadData(info, 0, info->eep_raw_data, EEP_TOTAL_SIZE));
	for(uint32_t i = 0; i < EEP_TOTAL_SIZE; i++)
		if(info->eep_raw_data[i] != 0xff)
		{
			EEPFUNC_CHECK_RETURN_RES(HAL_ERROR);
		}
		
	/* step 2: write Incremental numbers */
	for(uint32_t i = 0; i < EEP_TOTAL_SIZE; i++)
		info->eep_raw_data[i] = i;
	EEPFUNC_CHECK_RETURN_RES(Lime_eep_WriteData(info, 0, info->eep_raw_data, EEP_TOTAL_SIZE));
		
	/* rear & check */
	memset(info->eep_raw_data, 0xff, EEP_TOTAL_SIZE);
	EEPFUNC_CHECK_RETURN_RES(Lime_eep_ReadData(info, 0, info->eep_raw_data, EEP_TOTAL_SIZE));
	for(uint32_t i = 0; i < EEP_TOTAL_SIZE; i++)
		if(info->eep_raw_data[i] != i)
		{
			EEPFUNC_CHECK_RETURN_RES(HAL_ERROR);
		}
	
	DEBUG_LOG("%s(),pass\n", __FUNCTION__);
	
	return HAL_OK;
}

HAL_StatusTypeDef Lime_eep_SyncData(Lime_eep_t *info, Lime_eep_data_t *syncData)
{
	if(info == NULL || syncData == NULL)
	{
		return HAL_ERROR;
	}
	
	if(info->status != Lime_eep_status_Idle)
	{
		return HAL_TIMEOUT;
	}
	
	memcpy(&info->eep_cache_data, syncData, sizeof(Lime_eep_data_t));
	
	int16_t sPos = -1, ePos = -1;
	uint8_t *bufA = NULL, *bufB = NULL;
	bufA = (uint8_t*)&info->eep_cache_data;
	bufB = (uint8_t*)&info->eep_saved_data;
	for(uint16_t i = 0; i < sizeof(Lime_eep_data_t); i++)
	{
		if(bufA[i] != bufB[i])
		{
			if(sPos == -1)
				sPos = i;
			
			ePos = i;
		}
	}
	
	/* some different data */
	if(sPos != -1)
	{
		DEBUG_LOG("Find Change:%d - %d\n", sPos, ePos);
		
		memcpy(info->eep_raw_data + 1, &info->eep_cache_data, sizeof(Lime_eep_data_t));
		
		/* fill head & tail */
		info->eep_raw_data[0] = EEP_HEAD;
		info->eep_raw_data[EEP_TOTAL_SIZE - 1] = EEP_TAIL;
		
		/* write eeprom */
		EEPFUNC_CHECK_RETURN_RES(Lime_eep_WriteData(info, sPos + 1, info->eep_raw_data + sPos + 1, ePos - sPos + 1));
		
		/* refresh saved data */
		memcpy(&info->eep_saved_data, &info->eep_cache_data, sizeof(Lime_eep_data_t));
		
		DEBUG_LOG("Finish\n");
	}
	
	
	return HAL_OK;
}

Lime_eep_status_e Lime_eep_GetStatus(Lime_eep_t *info)
{
	if(info != NULL)
		return info->status;
	
	return Lime_eep_status_ApiErr;
}

void Lime_eep_printBufAdv(uint8_t* buf, uint32_t length, uint32_t showAddr)
{
	uint32_t addr = showAddr;
	size_t size = length;
	uint8_t *data = buf;
	uint32_t i = 0;
	DEBUG_LOG("Offset (h) 00 01 02 03 04 05 06 07 08 09 0A 0B 0C 0D 0E 0F\r\n");
	for (i = 0; i < size; i++)
	{
		if (i % 16 == 0)
		{
			LEprintf("[%08X] ", addr + i);
		}
		if(data[i] != 0)
			LEprintf("%02X ", data[i]);
		else
			LEprintf("-- ");
		
		if (((i + 1) % 16 == 0) || i == size - 1)
		{
			LEprintf("\r\n");
		}
	}
}


/*----------------------------------------ports & users----------------------------------------*/
#include "i2c.h"
HAL_StatusTypeDef isDeviceOnline(void)
{
	return HAL_I2C_IsDeviceReady(&hi2c1, 0xA0, 5, 100);
}

HAL_StatusTypeDef eepWrite(uint32_t address, uint8_t* buf, uint32_t length)
{
	return HAL_I2C_Mem_Write(&hi2c1, 0xA0, address, 1, buf, length, 100);
}

HAL_StatusTypeDef eepRead(uint32_t address, uint8_t* buf, uint32_t length)
{
	return HAL_I2C_Mem_Read(&hi2c1, 0xA0, address, 1, buf, length, 100);
}

Lime_eep_t LimeEEP = 
{
	.writeWrapSize = 1,
	.readWrapSize = 8,
	.latency = 3,
	.isDeviceOnline = isDeviceOnline,
	.write = eepWrite,
	.read = eepRead,
};

Lime_eep_data_t defaultData = 
{
	/* screen */
	.screen_auto_reduce_light_time = 1,
	.screen_auto_off_time = 2,
	
	/* rocker cali */
	.LimitMin = {   0,    0,    0,    0},
	.DeathMin = {2000, 2000, 2000, 2000},
	.DeathMax = {2000, 2000, 2000, 2000},
	.LimitMax = {4096, 4096, 4096, 4096},
	.calEn = {false, false, false, false},
	
	/* last saved receiver info */
	.autoConnEn = false,
	.autoConnUID = {0},
	
	/* receiver agreement */
	.receiver_Agreement = 3,		//0: DBUS, 1:SBUS, 2:PP, 3:DEBUG
	.receiver_OutToggle = 0,		//0: silent, 1: normal, 2: toggle
	
	//...
};
Lime_eep_data_t hotData = {0};

