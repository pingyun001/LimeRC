#include "sdCard_TestDemo.h"

#include "sdCard.h"

#include <string.h>


uint8_t sdTestTxBuf[4096] __attribute__((section("LIME_RAM_D1"))) = {0};
uint8_t sdTestRxBuf[4096] __attribute__((section("LIME_RAM_D1"))) = {0};

static void printfBuf(uint8_t* buf, uint32_t length)
{
	LEprintf("Buf:\n");
	for(uint32_t i = 0; i < length; i++)
	{
		LEprintf("0x%02x,",buf[i]);
		if(i % 16 == 15)
			LEprintf("\n");
	}
	LEprintf("end\n");
}

void SD_SmallDataCheck(void)
{
	LEprintf("%s()\n", __func__);
	
	memset(sdTestTxBuf, 0, sizeof(sdTestTxBuf));
	memset(sdTestRxBuf, 0, sizeof(sdTestRxBuf));
	
	for(uint32_t i = 0; i < sizeof(sdTestTxBuf); i++)
		sdTestTxBuf[i] = i;
	
//	if(Lime_SDCard_Erase(0, 0) != HAL_OK)
//		LEprintf("Erase Err\n");
//	else
//		LEprintf("Erase Success!\n");
	
	if(Lime_SDCard_Write(sdTestTxBuf, 1, 1) != HAL_OK)
		LEprintf("Write Err\n");
	else
		LEprintf("Write Success!\n");
	
	if(Lime_SDCard_Read(sdTestRxBuf, 1, 1) != HAL_OK)
		LEprintf("Read Err\n");
	else
		LEprintf("Read Success!\n");
	
	printfBuf(sdTestRxBuf, 512);
}


static uint32_t getRandomData(uint32_t* seed) 
{
  uint32_t x = *seed;
  x ^= x << 13;
  x ^= x >> 17;
  x ^= x << 5;
  *seed = x;
  return x;
}

static uint32_t temp4KBuf[1024];				//4K字节 BUF

void SD_FullChipCheck(uint32_t startBlock, uint32_t totalSize)
{
	uint32_t seedA = 0x12345678;
  uint32_t i = 0;
  
  LEprintf("func:%s(totalSize:%.6f MB), start!\n", __func__, totalSize / 1024.0f / 1024.0f);
  
  
  for(i = 0; i < totalSize / 4; i++)
  {
    temp4KBuf[i % 1024] = getRandomData(&seedA);
		
		if(i % 1024 == 1023)		//到达4K边界
		{
//			QSPI_W25Qxx_SectorErase((i - 1023) * 4);
			Lime_SDCard_Write((uint8_t*)temp4KBuf,  ((i - 1023) * 4) / 512 + startBlock, 4096 / 512);
		}
    
		if(((i * 4) % (100 * 1024)) == (100 * 1024 - 4))
    {
      LEprintf("write random num %d kB, %.3f MB\n", (i + 1) * 4 / 1024, (float)(i + 1) * 4.0f / 1024.0f / 1024.0f);
    }
  }
  
  LEprintf("write random num %d kB, %.3f MB\n", (i + 1) * 4 / 1024, (float)(i + 1) * 4.0f / 1024.0f / 1024.0f);
  LEprintf("Write random Finish\n");
  
  seedA = 0x12345678;
  for(i = 0; i < totalSize / 4; i++)
  {
		if(i % 1024 == 0)
		{
			Lime_SDCard_Read((uint8_t*)temp4KBuf, (i * 4) / 512 + startBlock, 4096 / 512);
		}
		
    uint32_t randomNum = getRandomData(&seedA);
    if(temp4KBuf[i % 1024] != randomNum)
    {
      LEprintf("Block:%#x,offset:%#x, failed!,read:%#x, target:%#x\n", i * 4 / 512, (i * 4) % 512, temp4KBuf[i], randomNum);
    }
    
    if(((i * 4) % (100 * 1024)) == (100 * 1024 - 4))
    {
      LEprintf("check memory %d kB, %.3f MB\n", (i + 1) * 4 / 1024, (float)(i + 1) * 4.0f / 1024.0f / 1024.0f);
    }
  }
  LEprintf("check memory %d kB, %.3f MB\n", (i + 1) * 4 / 1024, (float)(i + 1) * 4.0f / 1024.0f / 1024.0f);
	
	LEprintf("%s()finish\n", __func__);
}

static float dwtSpeedTester(uint8_t cmd)
{
#define TOTAL_TRANS_DATA_BYTES	(512 * 4096)
    static uint32_t start_time = 0;
    static uint32_t end_time = 0;
    static uint8_t is_timing = 0;
    uint32_t elapsed_time = 0;

    if ((CoreDebug->DEMCR & CoreDebug_DEMCR_TRCENA_Msk) == 0) {
        CoreDebug->DEMCR |= CoreDebug_DEMCR_TRCENA_Msk;
    }
    DWT->CTRL |= DWT_CTRL_CYCCNTENA_Msk;

    switch (cmd) {
        case 1:
            DWT->CYCCNT = 0;
            start_time = DWT->CYCCNT;
            is_timing = 1;
            break;

        case 2:
            if (is_timing) {
                end_time = DWT->CYCCNT;
                is_timing = 0;
            }
            return end_time - start_time;

        case 3:
            if (!is_timing && end_time >= start_time) {
                elapsed_time = end_time - start_time;
                float time_seconds = (float)elapsed_time / SystemCoreClock;
#if	(!TOTAL_TRANS_DATA_BYTES)
                LEprintf("Time elapsed: %.4f ms\n", time_seconds * 1000.0f);
#else
								float speed = (float)TOTAL_TRANS_DATA_BYTES / (time_seconds * 1000000.0f);  // 字节数转为MB，并计算每秒传输的速度
                LEprintf("Time elapsed: %.4f ms, totalSpeed: %.4f MBytes/s\n", time_seconds * 1000.0f, speed);
#endif
                return time_seconds * 1000.0f;
            }
            break;

        default:
            elapsed_time = 0;
            break;
    }

    return 0;
}

void SD_ReadSpeedTest(void)
{
	dwtSpeedTester(1);
	for(uint32_t i = 0; i < 512; i++)
	{
		Lime_SDCard_Read(sdTestRxBuf, 1, 8);
	}
	dwtSpeedTester(2);
	dwtSpeedTester(3);
}


void SD_PrintSDInfo(void)
{
	LEprintf("%s()\n", __func__);
	
	HAL_SD_CardInfoTypeDef info = {0};
	if(Lime_SDCard_GetCardInfo(&info) != HAL_OK)
	{
		LEprintf("err\n");
		return;
	}
	
	LEprintf("CardType:%#x\n", info.CardType);
	LEprintf("CardVersion:%#x\n", info.CardVersion);
	LEprintf("Class:%#x\n", info.Class);
	LEprintf("RelCardAdd:%#x\n", info.RelCardAdd);
	LEprintf("BlockNbr:%#x\n", info.BlockNbr);
	LEprintf("BlockSize:%#x\n", info.BlockSize);
	LEprintf("LogBlockNbr:%#x\n", info.LogBlockNbr);
	LEprintf("LogBlockSize:%#x\n", info.LogBlockSize);
	LEprintf("CardSpeed:%#x\n", info.CardSpeed);
	
	LEprintf("Capacity:%.4f GB\n", info.BlockNbr / 2.0f / 1024.0f / 1024.0f);
}
