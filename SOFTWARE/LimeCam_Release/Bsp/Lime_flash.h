#ifndef __LIME_FLASH_H
#define __LIME_FLASH_H

#include "main.h"

typedef struct
{
	/* Software Version */
	uint32_t head;
	uint32_t count;
	uint32_t version;
	
	/* nrf Info */
	char myName[13];//no used
	uint32_t LimePairedUID[3];
	bool isPaired;
	
	/* end check */
	uint32_t crcVal;
	
	uint32_t tail;
	
}Lime_GlobalData_t;

extern Lime_GlobalData_t Lime_GlobalData;


//void Lime_FlashSelfTest(void);

HAL_StatusTypeDef Lime_FlashInit(void);
HAL_StatusTypeDef Lime_FlashReadGlobalData(void);
HAL_StatusTypeDef Lime_FlashWriteGlobalData(void);

#endif		//__LIME_FLASH_H
