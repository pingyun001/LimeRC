#include "w25qFlash.h"
#include "quadspi.h"

#define QFLASH_TIMEOUT_VAL		2000													//超时时间，单位ms

uint32_t QSPI_W25Qxx_ReadID(void)
{
	QSPI_CommandTypeDef s_command = {0};											// QSPI传输配置
	uint8_t	QSPI_ReceiveBuff[3];															// 存储QSPI读到的数据
	uint32_t	W25Qxx_ID;																			// 器件的ID

	s_command.InstructionMode   = QSPI_INSTRUCTION_1_LINE;    // 1线指令模式
	s_command.AddressSize       = QSPI_ADDRESS_24_BITS;     	// 24位地址
	s_command.AlternateByteMode = QSPI_ALTERNATE_BYTES_NONE;  // 无交替字节 
	s_command.DdrMode           = QSPI_DDR_MODE_DISABLE;      // 禁止DDR模式
	s_command.DdrHoldHalfCycle  = QSPI_DDR_HHC_ANALOG_DELAY;  // DDR模式中数据延迟，这里用不到
	s_command.SIOOMode          = QSPI_SIOO_INST_EVERY_CMD;	 	// 每次传输数据都发送指令
	s_command.AddressMode		 		= QSPI_ADDRESS_NONE;   		 		// 无地址模式
	s_command.DataMode			 		= QSPI_DATA_1_LINE;       	 	// 1线数据模式
	s_command.DummyCycles 		 	= 0;                   		 		// 空周期个数
	s_command.NbData 				 		= 3;                          // 传输数据的长度
	s_command.Instruction 		 	= W25QXX_CMD_JEDEC_ID;        // 执行读器件ID命令

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
	QSPI_CommandTypeDef s_command = {0};											// QSPI传输配置
	uint8_t	QSPI_ReceiveBuff[1];															// 存储QSPI读到的数据
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

	s_command.InstructionMode   = QSPI_INSTRUCTION_1_LINE;    // 1线指令模式
	s_command.AddressSize       = QSPI_ADDRESS_24_BITS;     	// 24位地址
	s_command.AlternateByteMode = QSPI_ALTERNATE_BYTES_NONE;  // 无交替字节 
	s_command.DdrMode           = QSPI_DDR_MODE_DISABLE;      // 禁止DDR模式
	s_command.DdrHoldHalfCycle  = QSPI_DDR_HHC_ANALOG_DELAY;  // DDR模式中数据延迟，这里用不到
	s_command.SIOOMode          = QSPI_SIOO_INST_EVERY_CMD;	 	// 每次传输数据都发送指令
	s_command.AddressMode		 		= QSPI_ADDRESS_NONE;   		 		// 无地址模式
	s_command.DataMode			 		= QSPI_DATA_1_LINE;       	 	// 1线数据模式
	s_command.DummyCycles 		 	= 0;                   		 		// 空周期个数
	s_command.NbData 				 		= 1;                          // 传输数据的长度
	s_command.Instruction 		 	= instruction;        				// 执行写寄存器命令
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
	QSPI_CommandTypeDef s_command = {0};											// QSPI传输配置
	uint8_t	QSPI_TransmitBuff[1];															// 存储QSPI读到的数据
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

	s_command.InstructionMode   = QSPI_INSTRUCTION_1_LINE;    // 1线指令模式
	s_command.AddressSize       = QSPI_ADDRESS_24_BITS;     	// 24位地址
	s_command.AlternateByteMode = QSPI_ALTERNATE_BYTES_NONE;  // 无交替字节 
	s_command.DdrMode           = QSPI_DDR_MODE_DISABLE;      // 禁止DDR模式
	s_command.DdrHoldHalfCycle  = QSPI_DDR_HHC_ANALOG_DELAY;  // DDR模式中数据延迟，这里用不到
	s_command.SIOOMode          = QSPI_SIOO_INST_EVERY_CMD;	 	// 每次传输数据都发送指令
	s_command.AddressMode		 		= QSPI_ADDRESS_NONE;   		 		// 无地址模式
	s_command.DataMode			 		= QSPI_DATA_1_LINE;       	 	// 1线数据模式
	s_command.DummyCycles 		 	= 0;                   		 		// 空周期个数
	s_command.NbData 				 		= 1;                          // 传输数据的长度
	s_command.Instruction 		 	= instruction;        				// 执行写寄存器命令
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
	QSPI_CommandTypeDef     s_command = {0};	   							// QSPI传输配置
	QSPI_AutoPollingTypeDef s_config = {0};										// 轮询比较相关配置参数

	s_command.InstructionMode   = QSPI_INSTRUCTION_1_LINE;		// 1线指令模式
	s_command.AddressMode       = QSPI_ADDRESS_NONE;					// 无地址模式
	s_command.AlternateByteMode = QSPI_ALTERNATE_BYTES_NONE;	//	无交替字节 
	s_command.DdrMode           = QSPI_DDR_MODE_DISABLE;	    // 禁止DDR模式
	s_command.DdrHoldHalfCycle  = QSPI_DDR_HHC_ANALOG_DELAY;	   	// DDR模式中数据延迟，这里用不到
	s_command.SIOOMode          = QSPI_SIOO_INST_EVERY_CMD;	   	//	每次传输数据都发送指令	
	s_command.DataMode          = QSPI_DATA_1_LINE;						// 1线数据模式
	s_command.DummyCycles       = 0;											//	空周期个数
	s_command.Instruction       = W25QXX_CMD_READ_STATUS_REG1;	   // 读状态信息寄存器
																					
// 不停的查询 W25QXX_CMD_READ_STATUS_REG1 寄存器，将读取到的状态字节中的 W25QXX_STATUS_REG1_BUSY 不停的与0作比较
// 读状态寄存器1的第0位（只读），Busy标志位，当正在擦除/写入数据/写命令时会被置1，空闲或通信结束为0
	
	s_config.Match           = 0;   									//	匹配值
	s_config.MatchMode       = QSPI_MATCH_MODE_AND;	      	//	与运算
	s_config.Interval        = 0x10;	                     	//	轮询间隔
	s_config.AutomaticStop   = QSPI_AUTOMATIC_STOP_ENABLE;	// 自动停止模式
	s_config.StatusBytesSize = 1;	                        	//	状态字节数
	s_config.Mask            = W25QXX_STATUS_REG1_BUSY;	   // 对在轮询模式下接收的状态字节进行屏蔽，只比较需要用到的位
		
	// 发送轮询等待命令
	if (HAL_QSPI_AutoPolling(&hqspi, &s_command, &s_config, QFLASH_TIMEOUT_VAL) != HAL_OK)
	{
		return HAL_TIMEOUT; // 轮询等待无响应
	}
	return HAL_OK; // 通信正常结束

}

HAL_StatusTypeDef QSPI_W25Qxx_Reset(void)	
{
	QSPI_CommandTypeDef s_command = {0};											// QSPI传输配置

	s_command.InstructionMode   = QSPI_INSTRUCTION_1_LINE;   	// 1线指令模式
	s_command.AddressMode 		 = QSPI_ADDRESS_NONE;   				// 无地址模式
	s_command.AlternateByteMode = QSPI_ALTERNATE_BYTES_NONE; 	// 无交替字节 
	s_command.DdrMode           = QSPI_DDR_MODE_DISABLE;     	// 禁止DDR模式
	s_command.DdrHoldHalfCycle  = QSPI_DDR_HHC_ANALOG_DELAY; 	// DDR模式中数据延迟，这里用不到
	s_command.SIOOMode          = QSPI_SIOO_INST_EVERY_CMD;	 	// 每次传输数据都发送指令
	s_command.DataMode 			 = QSPI_DATA_NONE;       					// 无数据模式	
	s_command.DummyCycles 		 = 0;                     			// 空周期个数
	s_command.Instruction 		 = W25QXX_CMD_RESET_EN;      // 执行复位使能命令

	// 发送复位使能命令
	if (HAL_QSPI_Command(&hqspi, &s_command, QFLASH_TIMEOUT_VAL) != HAL_OK) 
	{
		return HAL_ERROR;			// 如果发送失败，返回错误信息
	}
	// 使用自动轮询标志位，等待通信结束
	if (QSPI_W25Qxx_AutoPollingMemReady() != HAL_OK)
	{
		return HAL_TIMEOUT;	// 轮询等待无响应
	}

	s_command.Instruction  = W25QXX_CMD_RESET_DEVICE;     // 复位器件命令    

	//发送复位器件命令
	if (HAL_QSPI_Command(&hqspi, &s_command, QFLASH_TIMEOUT_VAL) != HAL_OK) 
	{
		return HAL_TIMEOUT;		  // 如果发送失败，返回错误信息
	}
	// 使用自动轮询标志位，等待通信结束
	if (QSPI_W25Qxx_AutoPollingMemReady() != HAL_OK)
	{
		return HAL_TIMEOUT;	// 轮询等待无响应
	}	
	return HAL_OK;
}

HAL_StatusTypeDef QSPI_W25Qxx_WriteEnable(void)
{
	QSPI_CommandTypeDef     s_command;	   // QSPI传输配置
	QSPI_AutoPollingTypeDef s_config;		// 轮询比较相关配置参数

	s_command.InstructionMode   	= QSPI_INSTRUCTION_1_LINE;    	// 1线指令模式
	s_command.AddressMode 			= QSPI_ADDRESS_NONE;   		      	// 无地址模式
	s_command.AlternateByteMode 	= QSPI_ALTERNATE_BYTES_NONE;  	// 无交替字节 
	s_command.DdrMode           	= QSPI_DDR_MODE_DISABLE;      	// 禁止DDR模式
	s_command.DdrHoldHalfCycle  	= QSPI_DDR_HHC_ANALOG_DELAY;  	// DDR模式中数据延迟，这里用不到
	s_command.SIOOMode          	= QSPI_SIOO_INST_EVERY_CMD;			// 每次传输数据都发送指令	
	s_command.DataMode 				= QSPI_DATA_NONE;       	      		// 无数据模式
	s_command.DummyCycles 			= 0;                   	         	// 空周期个数
	s_command.Instruction	 		= W25QXX_CMD_WRITE_ENABLE;      		// 发送写使能命令

	// 发送写使能命令
	if (HAL_QSPI_Command(&hqspi, &s_command, QFLASH_TIMEOUT_VAL) != HAL_OK) 
	{
		return HAL_TIMEOUT;
	}
	
// 不停的查询 W25QXX_CMD_READ_STATUS_REG1 寄存器，将读取到的状态字节中的 W25QXX_STATYS_REG1_WEL 不停的与 0x02 作比较
// 读状态寄存器1的第1位（只读），WEL写使能标志位，该标志位为1时，代表可以进行写操作
	
	s_config.Match           = 0x02;  								// 匹配值
	s_config.Mask            = W25QXX_STATYS_REG1_WEL;	 		// 读状态寄存器1的第1位（只读），WEL写使能标志位，该标志位为1时，代表可以进行写操作
	s_config.MatchMode       = QSPI_MATCH_MODE_AND;			 	// 与运算
	s_config.StatusBytesSize = 1;									 	// 状态字节数
	s_config.Interval        = 0x10;							 		// 轮询间隔
	s_config.AutomaticStop   = QSPI_AUTOMATIC_STOP_ENABLE;	// 自动停止模式

	s_command.Instruction    = W25QXX_CMD_READ_STATUS_REG1;	// 读状态信息寄存器
	s_command.DataMode       = QSPI_DATA_1_LINE;					// 1线数据模式
	s_command.NbData         = 1;										// 数据长度

	// 发送轮询等待命令	
	if (HAL_QSPI_AutoPolling(&hqspi, &s_command, &s_config, QFLASH_TIMEOUT_VAL) != HAL_OK)
	{
		return HAL_TIMEOUT; 	// 轮询等待无响应
	}	
	return HAL_OK;  // 通信正常结束
}

HAL_StatusTypeDef QSPI_W25Qxx_SectorErase(uint32_t SectorAddress)	
{
	QSPI_CommandTypeDef s_command = {0};	// QSPI传输配置
	
	s_command.InstructionMode   	= QSPI_INSTRUCTION_1_LINE;    // 1线指令模式
	s_command.AddressSize       	= QSPI_ADDRESS_24_BITS;       // 24位地址模式
	s_command.AlternateByteMode 	= QSPI_ALTERNATE_BYTES_NONE;  // 无交替字节 
	s_command.DdrMode           	= QSPI_DDR_MODE_DISABLE;      // 禁止DDR模式
	s_command.DdrHoldHalfCycle  	= QSPI_DDR_HHC_ANALOG_DELAY;  // DDR模式中数据延迟，这里用不到
	s_command.SIOOMode          	= QSPI_SIOO_INST_EVERY_CMD;		// 每次传输数据都发送指令
	s_command.AddressMode 				= QSPI_ADDRESS_1_LINE;        // 1线地址模式
	s_command.DataMode 						= QSPI_DATA_NONE;             // 无数据
	s_command.DummyCycles 				= 0;                          // 空周期个数
	s_command.Address           	= SectorAddress;              // 要擦除的地址
	s_command.Instruction	 				= W25QXX_CMD_SECTOR_ERASE;     // 扇区擦除命令


	if (QSPI_W25Qxx_WriteEnable() != HAL_OK)
	{
		return HAL_TIMEOUT;
	}
	// 发出擦除命令
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
	QSPI_CommandTypeDef      s_command = {0};				 							// QSPI传输配置
	QSPI_MemoryMappedTypeDef s_mem_mapped_cfg = {0};	 						// 内存映射访问参数

	s_command.InstructionMode   = QSPI_INSTRUCTION_1_LINE;    		// 1线指令模式
	s_command.AddressSize       = QSPI_ADDRESS_24_BITS;           // 24位地址
	s_command.AlternateByteMode = QSPI_ALTERNATE_BYTES_NONE;  		// 无交替字节 
	s_command.DdrMode           = QSPI_DDR_MODE_DISABLE;     			// 禁止DDR模式
	s_command.DdrHoldHalfCycle  = QSPI_DDR_HHC_ANALOG_DELAY; 			// DDR模式中数据延迟，这里用不到
	s_command.SIOOMode          = QSPI_SIOO_INST_EVERY_CMD;				// 每次传输数据都发送指令	
	s_command.AddressMode 		 = QSPI_ADDRESS_4_LINES; 						// 4线地址模式
	s_command.DataMode    		 = QSPI_DATA_4_LINES;    						// 4线数据模式
	s_command.DummyCycles 		 = 6;                    						// 空周期个数
	s_command.Instruction 		 = W25QXX_CMD_QUAD_FAST_READ; 			// 1-4-4模式下(1线指令4线地址4线数据)，快速读取指令
	
	s_mem_mapped_cfg.TimeOutActivation = QSPI_TIMEOUT_COUNTER_DISABLE; // 禁用超时计数器, nCS 保持激活状态
	s_mem_mapped_cfg.TimeOutPeriod     = 0;									 			// 超时判断周期

	QSPI_W25Qxx_Reset();																					// 复位W25Qxx
	
	if (HAL_QSPI_MemoryMapped(&hqspi, &s_command, &s_mem_mapped_cfg) != HAL_OK)	// 进行配置
	{
		return HAL_TIMEOUT; 	// 设置内存映射模式错误
	}

	return HAL_OK; // 配置成功
}

HAL_StatusTypeDef QSPI_W25Qxx_WritePage(uint8_t* pBuffer, uint32_t WriteAddr, uint16_t NumByteToWrite)
{
	QSPI_CommandTypeDef s_command = {0};													// QSPI传输配置	
	
	s_command.InstructionMode   = QSPI_INSTRUCTION_1_LINE;    		// 1线指令模式
	s_command.AddressSize       = QSPI_ADDRESS_24_BITS;           // 24位地址
	s_command.AlternateByteMode = QSPI_ALTERNATE_BYTES_NONE;  		// 无交替字节 
	s_command.DdrMode           = QSPI_DDR_MODE_DISABLE;     			// 禁止DDR模式
	s_command.DdrHoldHalfCycle  = QSPI_DDR_HHC_ANALOG_DELAY; 			// DDR模式中数据延迟，这里用不到
	s_command.SIOOMode          = QSPI_SIOO_INST_EVERY_CMD;				// 每次传输数据都发送指令	
	s_command.AddressMode 		 = QSPI_ADDRESS_1_LINE; 						// 1线地址模式
	s_command.DataMode    		 = QSPI_DATA_4_LINES;    						// 4线数据模式
	s_command.DummyCycles 		 = 0;                    						// 空周期个数
	s_command.NbData      		 = NumByteToWrite;      			   		// 数据长度，最大只能256字节
	s_command.Address     		 = WriteAddr;         							// 要写入 W25Qxx 的地址
	s_command.Instruction 		 = W25QXX_CMD_QUAD_PAGE_PROG; // 1-1-4模式下(1线指令1线地址4线数据)，页编程指令
	
	// 写使能
	if (QSPI_W25Qxx_WriteEnable() != HAL_OK)
	{
		return HAL_TIMEOUT;	// 写使能失败
	}
	// 写命令
	if (HAL_QSPI_Command(&hqspi, &s_command, HAL_QPSI_TIMEOUT_DEFAULT_VALUE) != HAL_OK)
	{
		return HAL_TIMEOUT;		// 传输数据错误
	}
	
	// 开始传输数据
#if 1
	if (HAL_QSPI_Transmit(&hqspi, pBuffer, HAL_QPSI_TIMEOUT_DEFAULT_VALUE) != HAL_OK)
	{
		return HAL_TIMEOUT;		// 传输数据错误
	}
#else
	SCB_CleanDCache_by_Addr((uint32_t*)pBuffer, NumByteToWrite);
	
	if (HAL_QSPI_Transmit_DMA(&hqspi, pBuffer) != HAL_OK)
	{
		return HAL_TIMEOUT;		// 传输数据错误
	}
	
	while(hqspi.State != HAL_QSPI_STATE_READY)
		;
#endif
	
	// 使用自动轮询标志位，等待写入的结束 
	if (QSPI_W25Qxx_AutoPollingMemReady() != HAL_OK)
	{
		return HAL_TIMEOUT; // 轮询等待无响应
	}
	return HAL_OK;	// 写数据成功
}

HAL_StatusTypeDef QSPI_W25Qxx_WriteBuffer(uint8_t* pBuffer, uint32_t WriteAddr, uint32_t Size)
{	
	uint32_t end_addr, current_size, current_addr;
	uint8_t *write_data;  // 要写入的数据
	
//	LEprintf("Write:buf:%#x, add:%#x, Size:%#x\n", (uint32_t)pBuffer, WriteAddr, Size);

	current_size = W25QXX_PAGE_SIZE - (WriteAddr % W25QXX_PAGE_SIZE); // 计算当前页还剩余的空间

	if (current_size > Size)	// 判断当前页剩余的空间是否足够写入所有数据
	{
		current_size = Size;		// 如果足够，则直接获取当前长度
	}

	current_addr = WriteAddr;		// 获取要写入的地址
	end_addr = WriteAddr + Size;	// 计算结束地址
	write_data = pBuffer;			// 获取要写入的数据

	do
	{
		// 按页写入数据
		if(QSPI_W25Qxx_WritePage(write_data, current_addr, current_size) != HAL_OK)
		{
			return HAL_TIMEOUT;
		}

		current_addr += current_size;	// 计算下一次要写入的地址
		write_data += current_size;	// 获取下一次要写入的数据存储区地址
		// 计算下一次写数据的长度
		current_size = ((current_addr + W25QXX_PAGE_SIZE) > end_addr) ? (end_addr - current_addr) : W25QXX_PAGE_SIZE;
	}
	while (current_addr < end_addr) ; // 判断数据是否全部写入完毕
	
	return HAL_OK;
}



#include <string.h>
uint32_t catRxBuf[4096 / 4 + 8] __attribute__((section("LIME_RAM_D2"), aligned(32))) = {0};

HAL_StatusTypeDef QSPI_W25Qxx_ReadBuffer(uint8_t* pBuffer, uint32_t ReadAddr, uint32_t NumByteToRead)
{
//	LEprintf(">>Read:buf:%#x, add:%#x, Size:%d\n", (uint32_t)pBuffer, ReadAddr, NumByteToRead);
	
	QSPI_CommandTypeDef s_command = {0};													// QSPI传输配置
	
	s_command.InstructionMode   = QSPI_INSTRUCTION_1_LINE;    		// 1线指令模式
	s_command.AddressSize       = QSPI_ADDRESS_24_BITS;           // 24位地址
	s_command.AlternateByteMode = QSPI_ALTERNATE_BYTES_NONE;  		// 无交替字节 
	s_command.DdrMode           = QSPI_DDR_MODE_DISABLE;     			// 禁止DDR模式
	s_command.DdrHoldHalfCycle  = QSPI_DDR_HHC_ANALOG_DELAY; 			// DDR模式中数据延迟，这里用不到
	s_command.SIOOMode          = QSPI_SIOO_INST_EVERY_CMD;				// 每次传输数据都发送指令	
	s_command.AddressMode 		 = QSPI_ADDRESS_4_LINES; 						// 4线地址模式
	s_command.DataMode    		 = QSPI_DATA_4_LINES;    						// 4线数据模式
	s_command.DummyCycles 		 = 6;                    						// 空周期个数
	s_command.NbData      		 = NumByteToRead;      			   			// 数据长度，最大不能超过flash芯片的大小
	s_command.Address     		 = ReadAddr;         								// 要读取 W25Qxx 的地址
	s_command.Instruction 		 = W25QXX_CMD_QUAD_FAST_READ; 			// 1-4-4模式下(1线指令4线地址4线数据)，快速读取指令
	
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


