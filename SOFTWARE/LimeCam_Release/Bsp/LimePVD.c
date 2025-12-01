#include "LimePVD.h"
#include <stdio.h>


void Lime_PVD_Init(void)
{
	PWR_PVDTypeDef sConfigPVD = 
	{
		.PVDLevel = PWR_PVDLEVEL_0,
		.Mode = PWR_PVD_MODE_IT_RISING,
	};
	HAL_PWR_ConfigPVD(&sConfigPVD);
	
	HAL_PWR_EnablePVD();
}

void Lime_PVD_Hook(void)
{
	HAL_GPIO_WritePin(NRF1_CE_GPIO_Port, NRF1_CE_Pin, GPIO_PIN_RESET);
	HAL_GPIO_WritePin(NRF2_CE_GPIO_Port, NRF2_CE_Pin, GPIO_PIN_RESET);
	
	uint32_t startTime = 0;
	while(HAL_GetTick() - startTime < 100)
	{
		HAL_GPIO_TogglePin(DEBUGB_GPIO_Port, DEBUGB_Pin);
	}
	
	printf("PVD\n");
}


