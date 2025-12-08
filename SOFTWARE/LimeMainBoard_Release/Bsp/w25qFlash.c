#include "w25qFlash.h"
#include "quadspi.h"

#define QFLASH_TIMEOUT_VAL		2000	

uint32_t QSPI_W25Qxx_ReadID(void)
{
	QSPI_CommandTypeDef s_command = {0};										
	uint8_t	QSPI_ReceiveBuff[3];														
	uint32_t	W25Qxx_ID;																		

	s_command.InstructionMode   = QSPI_INSTRUCTION_1_LINE;  
	s_command.AddressSize       = QSPI_ADDRESS_24_BITS;     
	s_command.AlternateByteMode = QSPI_ALTERNATE_BYTES_NONE;
	s_command.DdrMode           = QSPI_DDR_MODE_DISABLE;    
	s_command.DdrHoldHalfCycle  = QSPI_DDR_HHC_ANALOG_DELAY;
	s_command.SIOOMode          = QSPI_SIOO_INST_EVERY_CMD;	
	s_command.AddressMode		 		= QSPI_ADDRESS_NONE;   		 	
	s_command.DataMode			 		= QSPI_DATA_1_LINE;       	
	s_command.DummyCycles 		 	= 0;                   		 	
	s_command.NbData 				 		= 3;                        
	s_command.Instruction 		 	= W25QXX_CMD_JEDEC_ID;      

	if (HAL_QSPI_Command(&hqspi, &s_command, QFLASH_TIMEOUT_VAL) != HAL_OK) 
	{
		return 0;
	}
	if (HAL_QSPI_Receive(&hqspi, QSPI_ReceiveBuff, QFLASH_TIMEOUT_VAL) != HAL_OK) 
	{
		return 0;
	}
	W25Qxx_ID = (QSPI_ReceiveBuff[0] << 16) | (QSPI_ReceiveBuff[1] << 8 ) | QSPI_ReceiveBuff[2];
	
	QSPI_W25Qxx_SetQE();

	return W25Qxx_ID;
}

void QSPI_W25Qxx_SetQE(void)
{
	uint8_t reg2 = 0;
	QSPI_W25Qxx_ReadReg(2, &reg2);
	
	/* QE has setted */
	if(reg2 & 0x02)
	{
		return;
	}
	
	/* set QE */
	reg2 |= 0x02;
	QSPI_W25Qxx_WriteReg(2, reg2);
}

HAL_StatusTypeDef QSPI_W25Qxx_ReadReg(uint8_t regID, uint8_t* regVal)
{
	QSPI_CommandTypeDef s_command = {0};					
	uint8_t	QSPI_ReceiveBuff[1];									
	uint8_t instruction = 0;
	
	switch(regID)
	{
		case 1:
			instruction = W25QXX_CMD_READ_STATUS_REG1;
			break;
		case 2:
			instruction = W25QXX_CMD_READ_STATUS_REG2;
			break;
		case 3:
			instruction = W25QXX_CMD_READ_STATUS_REG3;
			break;
		default:
			return HAL_ERROR;
	}

	s_command.InstructionMode   = QSPI_INSTRUCTION_1_LINE;  
	s_command.AddressSize       = QSPI_ADDRESS_24_BITS;     
	s_command.AlternateByteMode = QSPI_ALTERNATE_BYTES_NONE;
	s_command.DdrMode           = QSPI_DDR_MODE_DISABLE;    
	s_command.DdrHoldHalfCycle  = QSPI_DDR_HHC_ANALOG_DELAY;
	s_command.SIOOMode          = QSPI_SIOO_INST_EVERY_CMD;	
	s_command.AddressMode		 		= QSPI_ADDRESS_NONE;   		 	
	s_command.DataMode			 		= QSPI_DATA_1_LINE;       	
	s_command.DummyCycles 		 	= 0;                   		 	
	s_command.NbData 				 		= 1;                        
	s_command.Instruction 		 	= instruction;        			
	if (HAL_QSPI_Command(&hqspi, &s_command, QFLASH_TIMEOUT_VAL) != HAL_OK) 
	{
		return HAL_ERROR;
	}
	if (HAL_QSPI_Receive(&hqspi, QSPI_ReceiveBuff, QFLASH_TIMEOUT_VAL) != HAL_OK) 
	{
		return HAL_ERROR;
	}
	
	*regVal = QSPI_ReceiveBuff[0];

	return HAL_OK;
}
HAL_StatusTypeDef QSPI_W25Qxx_WriteReg(uint8_t regID, uint8_t regVal)
{
	QSPI_CommandTypeDef s_command = {0};						
	uint8_t	QSPI_TransmitBuff[1];										
	uint8_t instruction = 0;
	
	switch(regID)
	{
		case 1:
			instruction = W25Qxx_CMD_WRITE_STATUS_REG1;
			break;
		case 2:
			instruction = W25Qxx_CMD_WRITE_STATUS_REG2;
			break;
		case 3:
			instruction = W25Qxx_CMD_WRITE_STATUS_REG3;
			break;
		default:
			return HAL_ERROR;
	}
	
	QSPI_TransmitBuff[0] = regVal;

	s_command.InstructionMode   = QSPI_INSTRUCTION_1_LINE;   
	s_command.AddressSize       = QSPI_ADDRESS_24_BITS;     	
	s_command.AlternateByteMode = QSPI_ALTERNATE_BYTES_NONE; 
	s_command.DdrMode           = QSPI_DDR_MODE_DISABLE;     
	s_command.DdrHoldHalfCycle  = QSPI_DDR_HHC_ANALOG_DELAY; 
	s_command.SIOOMode          = QSPI_SIOO_INST_EVERY_CMD;	 
	s_command.AddressMode		 		= QSPI_ADDRESS_NONE;   		 		
	s_command.DataMode			 		= QSPI_DATA_1_LINE;       	 
	s_command.DummyCycles 		 	= 0;                   		 		
	s_command.NbData 				 		= 1;                         
	s_command.Instruction 		 	= instruction;        				
	if (HAL_QSPI_Command(&hqspi, &s_command, QFLASH_TIMEOUT_VAL) != HAL_OK) 
	{
		return HAL_ERROR;
	}
	if (HAL_QSPI_Transmit(&hqspi, QSPI_TransmitBuff, QFLASH_TIMEOUT_VAL) != HAL_OK) 
	{
		return HAL_ERROR;
	}

	return HAL_OK;
}

HAL_StatusTypeDef QSPI_W25Qxx_AutoPollingMemReady(void)
{
	QSPI_CommandTypeDef     s_command = {0};	   						
	QSPI_AutoPollingTypeDef s_config = {0};									

	s_command.InstructionMode   = QSPI_INSTRUCTION_1_LINE;	
	s_command.AddressMode       = QSPI_ADDRESS_NONE;				
	s_command.AlternateByteMode = QSPI_ALTERNATE_BYTES_NONE;
	s_command.DdrMode           = QSPI_DDR_MODE_DISABLE;	  
	s_command.DdrHoldHalfCycle  = QSPI_DDR_HHC_ANALOG_DELAY;
	s_command.SIOOMode          = QSPI_SIOO_INST_EVERY_CMD;	
	s_command.DataMode          = QSPI_DATA_1_LINE;			
	s_command.DummyCycles       = 0;	
	s_command.Instruction       = W25QXX_CMD_READ_STATUS_REG1;
	
	s_config.Match           = 0;
	s_config.MatchMode       = QSPI_MATCH_MODE_AND;
	s_config.Interval        = 0x10;
	s_config.AutomaticStop   = QSPI_AUTOMATIC_STOP_ENABLE;
	s_config.StatusBytesSize = 1;
	s_config.Mask            = W25QXX_STATUS_REG1_BUSY;
	
	if (HAL_QSPI_AutoPolling(&hqspi, &s_command, &s_config, QFLASH_TIMEOUT_VAL) != HAL_OK)
	{
		return HAL_TIMEOUT;
	}
	return HAL_OK;

}

HAL_StatusTypeDef QSPI_W25Qxx_Reset(void)	
{
	QSPI_CommandTypeDef s_command = {0};										

	s_command.InstructionMode   = QSPI_INSTRUCTION_1_LINE;   
	s_command.AddressMode 		 = QSPI_ADDRESS_NONE;   			
	s_command.AlternateByteMode = QSPI_ALTERNATE_BYTES_NONE; 
	s_command.DdrMode           = QSPI_DDR_MODE_DISABLE;     
	s_command.DdrHoldHalfCycle  = QSPI_DDR_HHC_ANALOG_DELAY; 
	s_command.SIOOMode          = QSPI_SIOO_INST_EVERY_CMD;	 
	s_command.DataMode 			 = QSPI_DATA_NONE;       				
	s_command.DummyCycles 		 = 0;
	s_command.Instruction 		 = W25QXX_CMD_RESET_EN;


	if (HAL_QSPI_Command(&hqspi, &s_command, QFLASH_TIMEOUT_VAL) != HAL_OK) 
	{
		return HAL_ERROR;
	}

	if (QSPI_W25Qxx_AutoPollingMemReady() != HAL_OK)
	{
		return HAL_TIMEOUT;
	}

	s_command.Instruction  = W25QXX_CMD_RESET_DEVICE; 


	if (HAL_QSPI_Command(&hqspi, &s_command, QFLASH_TIMEOUT_VAL) != HAL_OK) 
	{
		return HAL_TIMEOUT;
	}

	if (QSPI_W25Qxx_AutoPollingMemReady() != HAL_OK)
	{
		return HAL_TIMEOUT;
	}	
	return HAL_OK;
}

HAL_StatusTypeDef QSPI_W25Qxx_WriteEnable(void)
{
	QSPI_CommandTypeDef     s_command;
	QSPI_AutoPollingTypeDef s_config;	

	s_command.InstructionMode   	= QSPI_INSTRUCTION_1_LINE;  
	s_command.AddressMode 			= QSPI_ADDRESS_NONE;   		    
	s_command.AlternateByteMode 	= QSPI_ALTERNATE_BYTES_NONE;
	s_command.DdrMode           	= QSPI_DDR_MODE_DISABLE;    
	s_command.DdrHoldHalfCycle  	= QSPI_DDR_HHC_ANALOG_DELAY;
	s_command.SIOOMode          	= QSPI_SIOO_INST_EVERY_CMD;	
	s_command.DataMode 				= QSPI_DATA_NONE;       	      
	s_command.DummyCycles 			= 0;                   	      
	s_command.Instruction	 		= W25QXX_CMD_WRITE_ENABLE;      

	if (HAL_QSPI_Command(&hqspi, &s_command, QFLASH_TIMEOUT_VAL) != HAL_OK) 
	{
		return HAL_TIMEOUT;
	}
	
	s_config.Match           = 0x02;
	s_config.Mask            = W25QXX_STATYS_REG1_WEL;
	s_config.MatchMode       = QSPI_MATCH_MODE_AND;
	s_config.StatusBytesSize = 1;
	s_config.Interval        = 0x10;
	s_config.AutomaticStop   = QSPI_AUTOMATIC_STOP_ENABLE;

	s_command.Instruction    = W25QXX_CMD_READ_STATUS_REG1;
	s_command.DataMode       = QSPI_DATA_1_LINE;
	s_command.NbData         = 1;								

	if (HAL_QSPI_AutoPolling(&hqspi, &s_command, &s_config, QFLASH_TIMEOUT_VAL) != HAL_OK)
	{
		return HAL_TIMEOUT;
	}	
	return HAL_OK;
}

HAL_StatusTypeDef QSPI_W25Qxx_SectorErase(uint32_t SectorAddress)	
{
	QSPI_CommandTypeDef s_command = {0};
	
	s_command.InstructionMode   	= QSPI_INSTRUCTION_1_LINE;  
	s_command.AddressSize       	= QSPI_ADDRESS_24_BITS;     
	s_command.AlternateByteMode 	= QSPI_ALTERNATE_BYTES_NONE;
	s_command.DdrMode           	= QSPI_DDR_MODE_DISABLE;    
	s_command.DdrHoldHalfCycle  	= QSPI_DDR_HHC_ANALOG_DELAY;
	s_command.SIOOMode          	= QSPI_SIOO_INST_EVERY_CMD;	
	s_command.AddressMode 				= QSPI_ADDRESS_1_LINE;      
	s_command.DataMode 						= QSPI_DATA_NONE;           
	s_command.DummyCycles 				= 0;                        
	s_command.Address           	= SectorAddress;            
	s_command.Instruction	 				= W25QXX_CMD_SECTOR_ERASE;  


	if (QSPI_W25Qxx_WriteEnable() != HAL_OK)
	{
		return HAL_TIMEOUT;
	}
	if (HAL_QSPI_Command(&hqspi, &s_command, QFLASH_TIMEOUT_VAL) != HAL_OK)
	{
		return HAL_TIMEOUT;
	}

	if (QSPI_W25Qxx_AutoPollingMemReady() != HAL_OK)
	{
		return HAL_TIMEOUT;
	}
	return HAL_OK;
}

HAL_StatusTypeDef QSPI_W25Qxx_MemoryMappedMode(void)
{
	QSPI_CommandTypeDef      s_command = {0};				 				
	QSPI_MemoryMappedTypeDef s_mem_mapped_cfg = {0};	 			

	s_command.InstructionMode   = QSPI_INSTRUCTION_1_LINE;  
	s_command.AddressSize       = QSPI_ADDRESS_24_BITS;     
	s_command.AlternateByteMode = QSPI_ALTERNATE_BYTES_NONE;
	s_command.DdrMode           = QSPI_DDR_MODE_DISABLE;    
	s_command.DdrHoldHalfCycle  = QSPI_DDR_HHC_ANALOG_DELAY;
	s_command.SIOOMode          = QSPI_SIOO_INST_EVERY_CMD;	
	s_command.AddressMode 		 = QSPI_ADDRESS_4_LINES; 			
	s_command.DataMode    		 = QSPI_DATA_4_LINES;    			
	s_command.DummyCycles 		 = 6;                    			
	s_command.Instruction 		 = W25QXX_CMD_QUAD_FAST_READ; 
	
	s_mem_mapped_cfg.TimeOutActivation = QSPI_TIMEOUT_COUNTER_DISABLE;
	s_mem_mapped_cfg.TimeOutPeriod     = 0;

	QSPI_W25Qxx_Reset();
	
	if (HAL_QSPI_MemoryMapped(&hqspi, &s_command, &s_mem_mapped_cfg) != HAL_OK)
	{
		return HAL_TIMEOUT;
	}

	return HAL_OK;
}

HAL_StatusTypeDef QSPI_W25Qxx_WritePage(uint8_t* pBuffer, uint32_t WriteAddr, uint16_t NumByteToWrite)
{
	QSPI_CommandTypeDef s_command = {0};
	
	s_command.InstructionMode   = QSPI_INSTRUCTION_1_LINE;
	s_command.AddressSize       = QSPI_ADDRESS_24_BITS;
	s_command.AlternateByteMode = QSPI_ALTERNATE_BYTES_NONE;
	s_command.DdrMode           = QSPI_DDR_MODE_DISABLE;
	s_command.DdrHoldHalfCycle  = QSPI_DDR_HHC_ANALOG_DELAY;
	s_command.SIOOMode          = QSPI_SIOO_INST_EVERY_CMD;
	s_command.AddressMode 		 = QSPI_ADDRESS_1_LINE;
	s_command.DataMode    		 = QSPI_DATA_4_LINES;
	s_command.DummyCycles 		 = 0;
	s_command.NbData      		 = NumByteToWrite;
	s_command.Address     		 = WriteAddr;
	s_command.Instruction 		 = W25QXX_CMD_QUAD_PAGE_PROG;
	

	if (QSPI_W25Qxx_WriteEnable() != HAL_OK)
	{
		return HAL_TIMEOUT;
	}

	if (HAL_QSPI_Command(&hqspi, &s_command, HAL_QPSI_TIMEOUT_DEFAULT_VALUE) != HAL_OK)
	{
		return HAL_TIMEOUT;
	}
	
#if 1
	if (HAL_QSPI_Transmit(&hqspi, pBuffer, HAL_QPSI_TIMEOUT_DEFAULT_VALUE) != HAL_OK)
	{
		return HAL_TIMEOUT;
	}
#else
	SCB_CleanDCache_by_Addr((uint32_t*)pBuffer, NumByteToWrite);
	
	if (HAL_QSPI_Transmit_DMA(&hqspi, pBuffer) != HAL_OK)
	{
		return HAL_TIMEOUT;
	}
	
	while(hqspi.State != HAL_QSPI_STATE_READY)
		;
#endif
	
	if (QSPI_W25Qxx_AutoPollingMemReady() != HAL_OK)
	{
		return HAL_TIMEOUT;
	}
	return HAL_OK;
}

HAL_StatusTypeDef QSPI_W25Qxx_WriteBuffer(uint8_t* pBuffer, uint32_t WriteAddr, uint32_t Size)
{	
	uint32_t end_addr, current_size, current_addr;
	uint8_t *write_data;
	
//	LEprintf("Write:buf:%#x, add:%#x, Size:%#x\n", (uint32_t)pBuffer, WriteAddr, Size);

	current_size = W25QXX_PAGE_SIZE - (WriteAddr % W25QXX_PAGE_SIZE);

	if (current_size > Size)
	{
		current_size = Size;
	}

	current_addr = WriteAddr;
	end_addr = WriteAddr + Size;
	write_data = pBuffer;

	do
	{
		if(QSPI_W25Qxx_WritePage(write_data, current_addr, current_size) != HAL_OK)
		{
			return HAL_TIMEOUT;
		}

		current_addr += current_size;
		write_data += current_size;

		current_size = ((current_addr + W25QXX_PAGE_SIZE) > end_addr) ? (end_addr - current_addr) : W25QXX_PAGE_SIZE;
	}
	while (current_addr < end_addr) ;
	
	return HAL_OK;
}



#include <string.h>
uint32_t catRxBuf[4096 / 4 + 8] __attribute__((section("LIME_RAM_D2"), aligned(32))) = {0};

HAL_StatusTypeDef QSPI_W25Qxx_ReadBuffer(uint8_t* pBuffer, uint32_t ReadAddr, uint32_t NumByteToRead)
{
//	LEprintf(">>Read:buf:%#x, add:%#x, Size:%d\n", (uint32_t)pBuffer, ReadAddr, NumByteToRead);
	
	QSPI_CommandTypeDef s_command = {0};
	
	s_command.InstructionMode   = QSPI_INSTRUCTION_1_LINE;  
	s_command.AddressSize       = QSPI_ADDRESS_24_BITS;       
	s_command.AlternateByteMode = QSPI_ALTERNATE_BYTES_NONE;
	s_command.DdrMode           = QSPI_DDR_MODE_DISABLE;    
	s_command.DdrHoldHalfCycle  = QSPI_DDR_HHC_ANALOG_DELAY;
	s_command.SIOOMode          = QSPI_SIOO_INST_EVERY_CMD;	
	s_command.AddressMode 		 = QSPI_ADDRESS_4_LINES; 			
	s_command.DataMode    		 = QSPI_DATA_4_LINES;    			
	s_command.DummyCycles 		 = 6;                    			
	s_command.NbData      		 = NumByteToRead;      			  
	s_command.Address     		 = ReadAddr;         					
	s_command.Instruction 		 = W25QXX_CMD_QUAD_FAST_READ; 
	
	/* Send CMD */
	if (HAL_QSPI_Command(&hqspi, &s_command, HAL_QPSI_TIMEOUT_DEFAULT_VALUE) != HAL_OK)
	{
		return HAL_TIMEOUT;
	}

#if 0
	if (HAL_QSPI_Receive(&hqspi, pBuffer, HAL_QPSI_TIMEOUT_DEFAULT_VALUE) != HAL_OK)
	{
		return HAL_TIMEOUT;
	}
#else
	
	/*It seem like FatFs Lib has some bugs, need to catRxBuf as temp data buffer.*/
	SCB_InvalidateDCache_by_Addr((uint32_t*)catRxBuf, NumByteToRead);
	
	if (HAL_QSPI_Receive_DMA(&hqspi, (uint8_t*)catRxBuf) != HAL_OK)
	{
		return HAL_TIMEOUT;
	}
	while(hqspi.State != HAL_QSPI_STATE_READY)
		;
	
	memcpy(pBuffer, catRxBuf, NumByteToRead);
#endif
	
	return HAL_OK;	// 读取数据成功
}


