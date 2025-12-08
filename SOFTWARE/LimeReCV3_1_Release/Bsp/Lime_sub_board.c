#include "Lime_sub_board.h"
#include "Lime_App_Hal.h"
#include "usart.h"
#include "crc.h"

#include <string.h>

#if 0
#define DEBUG_PRINTF(...)		LEprintf(__VA_ARGS__)
#else
#define DEBUG_PRINTF(...)
#endif

uint8_t Lime_sub_board_rawBufA[LIME_SUB_BOARD_BUF_SIZE] __attribute__((section("LIME_RAM_D1"), aligned(32))) = {0};
uint8_t Lime_sub_board_rawBufB[LIME_SUB_BOARD_BUF_SIZE] __attribute__((section("LIME_RAM_D1"), aligned(32))) = {0};

Lime_sub_board_caliInfo_t Lime_sub_board_caliInfo = 
{
	.calEn = {false, false, false, false},
	.LimitMin = {   0,    0,    0,    0},
	.DeathMin = {2000, 2000, 2000, 2000},
	.DeathMax = {2000, 2000, 2000, 2000},
	.LimitMax = {4096, 4096, 4096, 4096},
};

void Lime_sub_board_Init(void)
{
	HAL_UARTEx_ReceiveToIdle_DMA(&huart4, Lime_sub_board_rawBufB, 128);
}

static inline void Lime_sub_buard_copyKeyInfo(uint16_t *buf)
{
	uint16_t *keyInfoPin = (uint16_t*)&LimeHAL_GetInfoPin()->keyInfo;
	
//	LEprintf("cpy%#x<-%#x\n", (uint32_t*)keyInfoPin, (uint32_t*)buf);
	
	for(uint8_t i = 0; i < 14; i++)
		keyInfoPin[i] = buf[i];
	
	/* rocker calibrite */
	Lime_sub_board_rocker_CaluCaliVal(LimeHAL_GetInfoPin()->keyInfo.rocker, &Lime_sub_board_caliInfo, LimeHAL_GetInfoPin()->keyInfo.rocker);
}

void Lime_sub_board_UartHook(uint8_t *buf, uint32_t size)
{
	DEBUG_PRINTF("RS:%d\n", size);
	
	/* check size */
	if(size != 32)
	{
		DEBUG_PRINTF("Size Err:%d\n", size);
		return;
	}
	
	/* check head & tail */
	if((buf[0] != 'L') || (buf[1] != 'R') || buf[31] != '\n')
	{
		DEBUG_PRINTF("Head&Tail Err\n");
		return;
	}
	
	/* check CRC */
	uint8_t crcVal = HAL_CRC_Calculate(&hcrc, (uint32_t*)buf, 30);
	if(buf[30] != crcVal)
	{
		DEBUG_PRINTF("CRC:%#x != %#x\n", buf[30], crcVal);
//		return;
	}
	
	DEBUG_PRINTF("PASS\n");
	
	Lime_sub_buard_copyKeyInfo((uint16_t*)(buf + 2));
}

static inline int32_t rocker_map(int32_t x, int32_t in_min, int32_t in_max, int32_t out_min, int32_t out_max)
{
	int32_t returnVal = (x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
	
	returnVal = returnVal < out_min ? out_min : returnVal > out_max ? out_max : returnVal;
	
  return returnVal;
}

void Lime_sub_board_rocker_CaluCaliVal(uint16_t raw[4], Lime_sub_board_caliInfo_t *calInfo, uint16_t res[4])
{
	if(raw == NULL || calInfo == NULL || res == NULL)
		return;
	
	for(uint8_t i = 0; i < 4; i++)
	{
		if( !calInfo->calEn[i])
			continue;
		  
		/* Limit Min - Death Min */
		if(raw[i] < calInfo->DeathMin[i])
		{
			res[i] = rocker_map(raw[i], calInfo->LimitMin[i], calInfo->DeathMin[i], 0, 2048);
		}
		/* Death Min - Death Max */
		else if(raw[i] < calInfo->DeathMax[i])
		{
			res[i] = 2048;
		}
		/* Death Max - Limit Max */
		else
		{
			res[i] = rocker_map(raw[i], calInfo->DeathMax[i], calInfo->LimitMax[i], 2048, 4095);
		}
	}
}

void Lime_sub_board_rocker_AutoCalHandle(bool isInit, bool* isFinish, Lime_sub_board_caliInfo_t *calInfo, int8_t ui_channelPos[4], uint8_t ui_channelPercent[4][2])
{
	static bool allFinishFlag = false;
	static uint32_t midTime[4] = {0};
	static uint32_t minTime[4] = {0};
	static uint32_t maxTime[4] = {0};
	static bool midFinishFlag[4] = {0};
	static bool minFinishFlag[4] = {0};
	static bool maxFinishFlag[4] = {0};
	
	const int16_t death_gap = 100;
	const int16_t death_gap_offset = 80;
	const int16_t min_max_offset = 30;
	
	static uint16_t midVal[4] = {0};
	
	uint16_t *rocker = (uint16_t *)&LimeHAL_GetInfoPin()->keyInfo.rocker;
	
	if(isInit)
	{
		*isFinish = false;
		allFinishFlag = false;
		
		for(uint8_t i = 0; i < 4; i++)
		{
			calInfo->calEn[i] = false;
			midVal[i] = rocker[i];
			calInfo->DeathMin[i] = midVal[i];
			calInfo->DeathMax[i] = midVal[i];
			midTime[i] = HAL_GetTick();
			midFinishFlag[i] = false;
			
			calInfo->LimitMin[i] = midVal[i] - 400;
			minTime[i] = HAL_GetTick();
			minFinishFlag[i] = false;
			
			calInfo->LimitMax[i] = midVal[i] + 400;
			maxTime[i] = HAL_GetTick();
			maxFinishFlag[i] = false;
		}
		
		return;
	}
	
	for(uint8_t i = 0; i < 4; i++)
	{
		/* detech mid val */
		if((rocker[i] > midVal[i] - death_gap) && (rocker[i] < midVal[i] + death_gap) && (!midFinishFlag[i]))
		{
			if(rocker[i] < calInfo->DeathMin[i])
				calInfo->DeathMin[i] = rocker[i];
			if(rocker[i] > calInfo->DeathMax[i])
				calInfo->DeathMax[i] = rocker[i];
			
			if(HAL_GetTick() - midTime[i] > 1000)
			{
				midFinishFlag[i] = true;
				calInfo->DeathMin[i] -= death_gap_offset;
				calInfo->DeathMax[i] += death_gap_offset;
			}
		}
		else
		{
			midTime[i] = HAL_GetTick();
		}
		
		/* detech min val */
		if(midFinishFlag[i] && (rocker[i] < (midVal[i] - 600)) && ( !minFinishFlag[i]))
		{
			if(rocker[i] < calInfo->LimitMin[i])
				calInfo->LimitMin[i] = rocker[i];
			
			if(HAL_GetTick() - minTime[i] > 2000)
			{
				minFinishFlag[i] = true;
				calInfo->LimitMin[i] += min_max_offset;
			}
		}
		else
		{
			minTime[i] = HAL_GetTick();
		}
		
		/* detech max val */
		if(midFinishFlag[i] && (rocker[i] > (midVal[i] + 600)) && ( !maxFinishFlag[i]))
		{
			if(rocker[i] > calInfo->LimitMax[i])
				calInfo->LimitMax[i] = rocker[i];
			
			if(HAL_GetTick() - maxTime[i] > 2000)
			{
				maxFinishFlag[i] = true;
				calInfo->LimitMax[i] -= min_max_offset;
			}
		}
		else
		{
			maxTime[i] = HAL_GetTick();
		}
		
		/* all finish */
		if(minFinishFlag[i] && midFinishFlag[i] && maxFinishFlag[i])
		{
			calInfo->calEn[i] = true;
		}
	}
	
	if(calInfo->calEn[0] && calInfo->calEn[1] && calInfo->calEn[2] && calInfo->calEn[3])
		allFinishFlag = true;
	
	*isFinish = allFinishFlag;
	
	if(ui_channelPercent != NULL)
	{
		/* channel 0 */
//		LEprintf("DebugA:time:%d, minTime:%d,%d,%d,%d\n", HAL_GetTick(), minTime[0], minTime[1], minTime[2], minTime[3]);
		ui_channelPercent[2][0] = minFinishFlag[0] ? 100 : rocker_map((HAL_GetTick() - minTime[0]), 0, 2000, 0, 100);
		ui_channelPercent[2][1] = maxFinishFlag[0] ? 100 : rocker_map((HAL_GetTick() - maxTime[0]), 0, 2000, 0, 100);
		
		/* channel 1 */
		ui_channelPercent[3][0] = minFinishFlag[1] ? 100 : rocker_map((HAL_GetTick() - minTime[1]), 0, 2000, 0, 100);
		ui_channelPercent[3][1] = maxFinishFlag[1] ? 100 : rocker_map((HAL_GetTick() - maxTime[1]), 0, 2000, 0, 100);
		
		/* channel 2 */
		ui_channelPercent[0][0] = minFinishFlag[2] ? 100 : rocker_map((HAL_GetTick() - minTime[2]), 0, 2000, 0, 100);
		ui_channelPercent[0][1] = maxFinishFlag[2] ? 100 : rocker_map((HAL_GetTick() - maxTime[2]), 0, 2000, 0, 100);
		
		/* channel 3 */
		ui_channelPercent[1][0] = minFinishFlag[3] ? 100 : rocker_map((HAL_GetTick() - minTime[3]), 0, 2000, 0, 100);
		ui_channelPercent[1][1] = maxFinishFlag[3] ? 100 : rocker_map((HAL_GetTick() - maxTime[3]), 0, 2000, 0, 100);
	}
	
	if(ui_channelPos != NULL)
	{
		ui_channelPos[0] = rocker_map(rocker[0], 0, 4096, -100, 100);
		ui_channelPos[1] = rocker_map(rocker[1], 0, 4096, -100, 100);
		ui_channelPos[2] = rocker_map(rocker[2], 0, 4096, -100, 100);
		ui_channelPos[3] = rocker_map(rocker[3], 0, 4096, -100, 100);
	}
	
}

