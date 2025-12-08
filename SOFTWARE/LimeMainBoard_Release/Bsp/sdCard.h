#ifndef __SDCARD_H
#define __SDCARD_H

#include "main.h"

HAL_StatusTypeDef Lime_SDCard_Init(void);

HAL_StatusTypeDef Lime_SDCard_GetCardInfo(HAL_SD_CardInfoTypeDef *cardInfo);

HAL_StatusTypeDef Lime_SDCard_Erase(uint32_t BlockStartAdd, uint32_t BlockEndAdd);

HAL_StatusTypeDef Lime_SDCard_Write(const uint8_t *pData, uint32_t BlockAdd, uint32_t NumberOfBlocks);

HAL_StatusTypeDef Lime_SDCard_Read(uint8_t *pData, uint32_t BlockAdd, uint32_t NumberOfBlocks);


#endif	//__SDCARD_H
