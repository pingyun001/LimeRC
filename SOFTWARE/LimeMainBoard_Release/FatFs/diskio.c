/*-----------------------------------------------------------------------*/
/* Low level disk I/O module SKELETON for FatFs     (C)ChaN, 2019        */
/*-----------------------------------------------------------------------*/
/* If a working storage control module is available, it should be        */
/* attached to the FatFs via a glue function rather than modifying it.   */
/* This is an example of glue functions to attach various exsisting      */
/* storage control modules to the FatFs module with a defined API.       */
/*-----------------------------------------------------------------------*/

#include "ff.h"				/* Obtains integer types */
#include "diskio.h"		/* Declarations of disk functions */

/* Definitions of physical drive number for each drive */

#define QSPI_FLASH	0	/* Example: Map SPI Flash to physical drive 0 */
#define SD_CARD			1	/* Example: Map Ramdisk to physical drive 1 */


#include <string.h>

#include "w25qFlash.h"
#include "sdCard.h"
#include "Lime_rtc_time.h"

uint8_t diskIOBuffer[10];

#define SPI_FLASH_SECTOR_SIZE		4096
#define SPI_FLASH_SCETOR_COUNT	4096
#define SPI_FLASH_BLOCK_SIZE		1
#define SPI_FLASH_FATFS_BASE		(0)		//必须SECTOR_SIZE整数倍



#if 0
#define DEBUG_PRINTF(...)		LEprintf(__VA_ARGS__)
#else
#define DEBUG_PRINTF(...)
#endif

/*-----------------------------------------------------------------------*/
/* Get Drive Status                                                      */
/*-----------------------------------------------------------------------*/

DSTATUS disk_status (
	BYTE pdrv		/* Physical drive nmuber to identify the drive */
)
{
	DSTATUS stat = 0;
	int result;
	
	DEBUG_PRINTF(">>%s\n", __FUNCTION__);

	switch (pdrv) 
	{
		case QSPI_FLASH:
		{
			stat &= ~STA_NOINIT;
			
			return stat;
		}
		case SD_CARD :
		{
			stat &= ~STA_NOINIT;
			
			return stat;
		}
	}
	
	return stat;
}



/*-----------------------------------------------------------------------*/
/* Inidialize a Drive                                                    */
/*-----------------------------------------------------------------------*/

DSTATUS disk_initialize (
	BYTE pdrv				/* Physical drive nmuber to identify the drive */
)
{
	DSTATUS stat = 0;
	
	DEBUG_PRINTF(">>%s\n", __FUNCTION__);

	switch (pdrv) 
	{
		case QSPI_FLASH:
		{
			if(QSPI_W25Qxx_ReadID() == 0x00FFFFFF)
			{
				stat |= STA_NOINIT;
			}
			return stat;
		}
		
		case SD_CARD :
		{
			if(Lime_SDCard_Init() != HAL_OK)
			{
				stat |= STA_NOINIT;
			}
			
			return stat;
		}
	}

	return STA_NOINIT;
}



/*-----------------------------------------------------------------------*/
/* Read Sector(s)                                                        */
/*-----------------------------------------------------------------------*/

DRESULT disk_read (
	BYTE pdrv,		/* Physical drive nmuber to identify the drive */
	BYTE *buff,		/* Data buffer to store read data */
	LBA_t sector,	/* Start sector in LBA */
	UINT count		/* Number of sectors to read */
)
{
	DEBUG_PRINTF(">>%s, sector: %d, count: %d, pdrv:%d\n", __FUNCTION__, sector, count, pdrv);
	
	switch (pdrv) 
	{
		case QSPI_FLASH:
		{
			if(QSPI_W25Qxx_ReadBuffer(buff, SPI_FLASH_FATFS_BASE + sector * 4096, count * 4096) != HAL_OK)
			{
				DEBUG_PRINTF("Line:%d, Sector%#xRead Err\n", __LINE__, (sector) * SPI_FLASH_SECTOR_SIZE);
				return RES_ERROR;
			}
			
			return RES_OK;
		}
		
		case SD_CARD :
		{
			if(Lime_SDCard_Read(buff, sector, count) != HAL_OK)
			{
				return RES_ERROR;
			}
			
			return RES_OK;
		}
	}
	
	return RES_PARERR;
}



/*-----------------------------------------------------------------------*/
/* Write Sector(s)                                                       */
/*-----------------------------------------------------------------------*/

#if FF_FS_READONLY == 0

DRESULT disk_write (
	BYTE pdrv,			/* Physical drive nmuber to identify the drive */
	const BYTE *buff,	/* Data to be written */
	LBA_t sector,		/* Start sector in LBA */
	UINT count			/* Number of sectors to write */
)
{	
	DEBUG_PRINTF(">>%s, sector: %d, count: %d, pdrv:%d\n", __FUNCTION__, sector, count, pdrv);

	switch (pdrv) 
	{
		case QSPI_FLASH:
		{
			for(uint32_t countOffset = 0; countOffset < count; countOffset++)
			{
				if(QSPI_W25Qxx_SectorErase(SPI_FLASH_FATFS_BASE + (sector + countOffset) * SPI_FLASH_SECTOR_SIZE) != HAL_OK)
				{
					DEBUG_PRINTF("Line:%d, Sector%#xErase Err\n", __LINE__, (sector + countOffset) * SPI_FLASH_SECTOR_SIZE);
					return RES_ERROR;
				}
			}
			
			if(QSPI_W25Qxx_WriteBuffer((uint8_t*)buff, SPI_FLASH_FATFS_BASE + sector * SPI_FLASH_SECTOR_SIZE, count * SPI_FLASH_SECTOR_SIZE) != HAL_OK)
			{
				DEBUG_PRINTF("Line:%d, Sector%#xWrite Err\n", __LINE__, (sector) * SPI_FLASH_SECTOR_SIZE);
				return RES_ERROR;
			}
			
			return RES_OK;
		}
		case SD_CARD :
		{
			if(Lime_SDCard_Write(buff, sector, count) != HAL_OK)
			{
				DEBUG_PRINTF("Line:%d, Sector%#xWrite Err\n", __LINE__, (sector) * SPI_FLASH_SECTOR_SIZE);
				return RES_ERROR;
			}
			
			return RES_OK;
		}
	}
	
	return RES_PARERR;
}

#endif


/*-----------------------------------------------------------------------*/
/* Miscellaneous Functions                                               */
/*-----------------------------------------------------------------------*/

DRESULT disk_ioctl (
	BYTE pdrv,		/* Physical drive nmuber (0..) */
	BYTE cmd,		/* Control code */
	void *buff		/* Buffer to send/receive control data */
)
{
	DRESULT res = 0;
	int result;
	
	DEBUG_PRINTF(">>%s, cmd: %d, pdrv:%d\n", __FUNCTION__, cmd, pdrv);

	switch (pdrv) 
	{
		case QSPI_FLASH:
		{
			switch (cmd) 
			{
				case GET_SECTOR_SIZE :    
					*(WORD * )buff = SPI_FLASH_SECTOR_SIZE;
				break;
				case GET_BLOCK_SIZE :
					*(DWORD * )buff = SPI_FLASH_BLOCK_SIZE;
				break;

				case GET_SECTOR_COUNT:
					*(DWORD * )buff = SPI_FLASH_SCETOR_COUNT;
					break;
				case CTRL_SYNC :
					break;
				break;
			}
			return RES_OK;
		}
		case SD_CARD :
		{
			switch (cmd) 
			{
				// Get R/W sector size (WORD) 
				case GET_SECTOR_SIZE :    
					*(WORD * )buff = 512;
					//printf("disk_ioctl: GET_SECTOR_SIZE:\n");
				break;
				// Get erase block size in unit of sector (DWORD)
				case GET_BLOCK_SIZE :
					*(DWORD * )buff = 1;
				break;
				case GET_SECTOR_COUNT:
				{
					HAL_SD_CardInfoTypeDef info = {0};
					if(Lime_SDCard_GetCardInfo(&info) != HAL_OK)
					{
						DEBUG_PRINTF("err\n");
						return RES_ERROR;
					}
					
					*(DWORD * )buff = info.BlockNbr;
					
					break;
				}
				case CTRL_SYNC :
				break;
			}
			return RES_OK;
		}
	}

	return RES_PARERR;
}

 DWORD get_fattime(void) 
{
//	LEprintf(">>%s\n", __FUNCTION__);
	
	uint16_t year;
	uint8_t month, day, hour, minute, second;
	
	LimeRtc_GetNowDate(&year, &month, &day);
	LimeRtc_GetNowTime(&hour, &minute, &second);
	
//	LEprintf("%d,%d,%d,%d,%d,%d\n", year, month, day ,hour, minute, second);
	
	/* 返回当前时间戳 */
	return	  ((DWORD)(year - 1980) << 25)	/* Year 2015 */
			| ((DWORD)month << 21)				/* Month 1 */
			| ((DWORD)day << 16)				/* Mday 1 */
			| ((DWORD)hour << 11)				/* Hour 0 */
			| ((DWORD)minute << 5)				  /* Min 0 */
			| ((DWORD)second >> 1);				/* Sec 0 */
}
