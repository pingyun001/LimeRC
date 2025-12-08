#ifndef __SDCARD_TESTDEMO_H
#define __SDCARD_TESTDEMO_H

#include "main.h"



void SD_SmallDataCheck(void);

void SD_ReadSpeedTest(void);

void SD_FullChipCheck(uint32_t startBlock, uint32_t totalSize);

void SD_PrintSDInfo(void);

void SD_AutoRunHandle(void);

#endif	//__SDCARD_TESTDEMO_H
