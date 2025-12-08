#ifndef __W25QFLASH_H
#define __W25QFLASH_H

#include "main.h"

#define W25QXX_CMD_RESET_EN  					0x66		// enable reset
#define W25QXX_CMD_RESET_DEVICE   		0x99		// reset device
#define W25QXX_CMD_JEDEC_ID 					0x9F		// read jedec id

#define W25QXX_CMD_WRITE_ENABLE				0X06		// write enable

#define W25QXX_CMD_SECTOR_ERASE 			0x20		// sector erase(4kB)  45ms
#define W25QXX_CMD_BLOCK_ERASE_32K 		0x52		// block  erase(32kB) 120ms
#define W25QXX_CMD_BLOCK_ERASE_64K 		0xD8		// block  erase(64kB) 150ms

#define W25QXX_CMD_QUAD_PAGE_PROG  		0x32  	// 1-1-4 page program 0.4ms
#define W25QXX_CMD_QUAD_FAST_READ     0xEB  	// 1-4-4 fast read

#define W25QXX_CMD_READ_STATUS_REG1		0X05		// read status register 1
#define W25QXX_STATUS_REG1_BUSY  			0x01		// read status register 1  bit 0
#define W25QXX_STATYS_REG1_WEL  			0x02		// read status register 1  bit 1 (WEL)

#define W25QXX_CMD_READ_STATUS_REG2		0x35		//LimeRC Added
#define W25QXX_CMD_READ_STATUS_REG3		0x15		//LimeRC Added
#define W25Qxx_CMD_WRITE_STATUS_REG1 	0x01		//LimeRC Added
#define W25Qxx_CMD_WRITE_STATUS_REG2 	0x31		//LimeRC Added
#define W25Qxx_CMD_WRITE_STATUS_REG3 	0x11		//LimeRC Added

#define W25QXX_PAGE_SIZE       				256			// page size

uint32_t QSPI_W25Qxx_ReadID(void);
void QSPI_W25Qxx_SetQE(void);
HAL_StatusTypeDef QSPI_W25Qxx_Reset(void);

HAL_StatusTypeDef QSPI_W25Qxx_SectorErase(uint32_t SectorAddress);

HAL_StatusTypeDef	QSPI_W25Qxx_WritePage(uint8_t* pBuffer, uint32_t WriteAddr, uint16_t NumByteToWrite);	// 按页写入，最大256字节
HAL_StatusTypeDef	QSPI_W25Qxx_WriteBuffer(uint8_t* pData, uint32_t WriteAddr, uint32_t Size);						// 写入数据，最大不能超过flash芯片的大小
HAL_StatusTypeDef QSPI_W25Qxx_ReadBuffer(uint8_t* pBuffer, uint32_t ReadAddr, uint32_t NumByteToRead);	// 读取数据，最大不能超过flash芯片的大小
HAL_StatusTypeDef QSPI_W25Qxx_ReadReg(uint8_t regID, uint8_t* regVal);
HAL_StatusTypeDef QSPI_W25Qxx_WriteReg(uint8_t regID, uint8_t regVal);

HAL_StatusTypeDef QSPI_W25Qxx_MemoryMappedMode(void);	//Map to:0x90000000

#endif	//__W25QFLASH_H

