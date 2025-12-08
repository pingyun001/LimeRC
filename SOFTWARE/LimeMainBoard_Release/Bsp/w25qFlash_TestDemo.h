#ifndef __W25QFLASH_TEST_DEMO_H
#define __W25QFLASH_TEST_DEMO_H

#include "main.h"

void W25QFlash_SmallDataCheck(void);

void W25QFlash_FullChipCheck(uint32_t totalSize);


void W25QFlash_ReadSpeedTest(void);
void W25QFlash_AIPReadSpeedTest(void);
void STM32InternalFlash_ReadSpeedTest(void);


#endif	//__W25QFLASH_TEST_DEMO_H
