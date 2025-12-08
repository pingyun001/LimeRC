#include "FatFsSelfTest.h"

#include "ff.h"
#include "diskio.h"
#include <stdio.h>
#include <string.h>

#if 1
#define DEBUG_LOG(...)	LEprintf(__VA_ARGS__)
#else
#define DEBUG_LOG(...)
#endif

/* 公用(防止爆栈) */
FATFS fs = {0};                  	// FatFs 文件系统对象
FIL fil = {0};                      // 文件对象

char mkfsBuffer[4096] = {0};

void FatFs_test(uint8_t diskID)
{
	DEBUG_LOG("%s(%d)\n", __FUNCTION__, diskID);
// 挂载磁盘编号 0 的文件系统
//	fr = f_mkfs("0:", 0, mkfsBuffer, FF_MAX_SS);  // 格式化为 FAT32 文件系统
    
	FRESULT fr = {0};                   // 文件操作返回结果
	char buffer[64] = {0};              // 用于读取文件的缓冲区

	char diskRootPath[4] = {0};
	snprintf(diskRootPath, 4, "%d:", diskID);

	fr = f_mount(&fs, diskRootPath, 1);
	if (fr != FR_OK) 
	{
		DEBUG_LOG("Mount failed with error code: %d\n", fr);
		DEBUG_LOG("Attempting to create new file system...\n");

		// 格式化磁盘 0，创建新的文件系统
		fr = f_mkfs(diskRootPath, 0, mkfsBuffer, FF_MAX_SS);  // 格式化为 FAT32 文件系统
		if (fr != FR_OK) 
		{
			DEBUG_LOG("File system creation failed with error code: %d\n", fr);
			return;
		}
		DEBUG_LOG("File system created successfully.\n");

		// 重新挂载文件系统
		fr = f_mount(&fs, "diskRootPath", 1);  
		if (fr != FR_OK) 
		{
			DEBUG_LOG("Mount failed twice with error code: %d\n", fr);
			return;
		}
	} 
	else 
	{
			DEBUG_LOG("File system mounted successfully.\n");
	}

	// 尝试打开文件 "test.txt"
	char textTxtPath[15] = {0};
	snprintf(textTxtPath, 15, "%stest.txt", diskRootPath);
	fr = f_open(&fil, textTxtPath, FA_READ);  
	if (fr == FR_OK) 
	{
		// 如果文件存在，则读取文件内容
		DEBUG_LOG("File opened for reading.\n");

		// 读取文件内容
		unsigned int bytesRead;
		fr = f_read(&fil, buffer, sizeof(buffer) - 1, &bytesRead);  
		if (fr != FR_OK) 
		{
				DEBUG_LOG("File read failed with error code: %d\n", fr);
				f_close(&fil);
				f_unmount(diskRootPath);  // 卸载文件系统
				return;
		}
		buffer[bytesRead] = '\0';  // 确保读取的数据以'\0'结尾
		DEBUG_LOG("Read data: %s\n", buffer);

		f_close(&fil);
	} 
	else if (fr == FR_NO_FILE) 
	{
		// 如果文件不存在，则创建并写入内容
		DEBUG_LOG("File not found. Creating new file...\n");

		// 创建文件并打开
		fr = f_open(&fil, textTxtPath, FA_CREATE_ALWAYS | FA_WRITE);
		if (fr != FR_OK) 
		{
			DEBUG_LOG("File creation failed with error code: %d\n", fr);
			f_unmount(diskRootPath);  // 卸载文件系统
			return;
		}
		DEBUG_LOG("File created successfully.\n");

		// 向文件写入数据
		const char *data = "Hello, FatFs! This is a test file.\n";
		unsigned int bytesWritten;
		fr = f_write(&fil, data, strlen(data), &bytesWritten);  
		if (fr != FR_OK || bytesWritten != strlen(data)) 
		{
			DEBUG_LOG("File write failed with error code: %d\n", fr);
			f_close(&fil);
			f_unmount(diskRootPath);  // 卸载文件系统
			return;
		}
		DEBUG_LOG("Data written to file successfully.\n");

		// 关闭文件
		f_close(&fil);
	}
	else 
	{
		DEBUG_LOG("File open failed with error code: %d\n", fr);
		f_unmount(diskRootPath);  // 卸载文件系统
		return;
	}

	// 卸载文件系统
	f_unmount(diskRootPath);
	DEBUG_LOG("File system unmounted.\n");
}

//扫描根目录下所有文件并打印出来
void FatFs_TraverseSDCardRoot(char *path)
{
	DIR dir = {0};              // 目录对象
	FILINFO fno = {0};          // 文件信息对象
	FRESULT res = 0;            // FatFs函数结果

	DEBUG_LOG("%s(%s),start\n", __FUNCTION__, path);

	char diskRootPath[4] = {0};
	snprintf(diskRootPath, 3, "%s", path);

	res = f_mount(&fs, diskRootPath, 1);
	if (res == FR_OK) 
	{
		res = f_opendir(&dir, path);
		if (res == FR_OK) 
		{
			// 读取目录中的每个条目
			while ((res = f_readdir(&dir, &fno)) == FR_OK && fno.fname[0]) 
			{
				// 如果是文件或文件夹，则打印名称
				if (fno.fattrib & AM_DIR) 
				{
					DEBUG_LOG("DIR: %s\n", fno.fname);
				} 
				else 
				{
					DEBUG_LOG("FILE: %s, size:%d B\n", fno.fname, (int)fno.fsize);
				}
			}
			f_closedir(&dir);
		} 
		else 
		{
			DEBUG_LOG("Failed to open root directory!\n");
		}
		f_unmount(diskRootPath);
		DEBUG_LOG("ScanFinish\n");
	}
	else 
	{
		DEBUG_LOG("Failed to mount SD card!\n");
	}
}

//FatFs硬件环境测试
int8_t FatFs_TestHardware(uint8_t diskID, uint32_t fileSize)
{
	FRESULT res;
	UINT bytes_written, bytes_read;
	unsigned char write_byte = 0;
	unsigned char read_byte;
	unsigned int counter;

	char diskRootPath[4] = {0};
	snprintf(diskRootPath, 4, "%d:", diskID);
	
	DEBUG_LOG("FatFs Hardware Environment Test...\n");
    
	// 1. 挂载文件系统
	res = f_mount(&fs, diskRootPath, 0);
	if (res != FR_OK) 
	{
		DEBUG_LOG("f_mount Failed\n");
		return -1;  // 挂载失败
	}

	// 2. 创建并写入测试文件
	char textTxtPath[15] = {0};
	snprintf(textTxtPath, 15, "%stestHD.bin", diskRootPath);
	res = f_open(&fil, textTxtPath, FA_CREATE_ALWAYS | FA_WRITE);
	if (res != FR_OK) 
	{
		f_unmount(diskRootPath);  // 尝试卸载
		DEBUG_LOG("file creat Failed\n");
		return -2;      // 创建文件失败
	}
	
	DEBUG_LOG("File Creat Successfully, writing...\n");

	// 写入递增数据
	for (counter = 0; counter < fileSize; counter++) 
	{
		write_byte = (unsigned char)(counter & 0xFF);  // 生成0-255循环字节
		res = f_write(&fil, &write_byte, 1, &bytes_written);

		if(((counter) % (100 * 1024)) == (100 * 1024 - 4))
		{
			DEBUG_LOG("write random num %d kB, %.3f MB\n", (counter + 1) / 1024, (float)(counter + 1) / 1024.0f / 1024.0f);
		}

		// 检查写入状态
		if (res != FR_OK || bytes_written != 1) 
		{
			f_close(&fil);
			f_unmount(diskRootPath);
			DEBUG_LOG("file write Failed\n");
			return -3;  // 写入失败
		}
	}
	
	DEBUG_LOG("write random num %d kB, %.3f MB\n", (counter + 1) / 1024, (float)(counter + 1) / 1024.0f / 1024.0f);
	DEBUG_LOG("Finish\n");

	// 3. 关闭文件
	res = f_close(&fil);
	if (res != FR_OK) 
	{
		f_unmount(diskRootPath);
		DEBUG_LOG("file close Failed\n");
		return -4;  // 关闭失败
	}

	// 4. 重新打开文件验证数据
	res = f_open(&fil, textTxtPath, FA_READ);
	if (res != FR_OK) 
	{
		f_unmount(diskRootPath);
		DEBUG_LOG("file reopen Failed\n");
		return -5;  // 重新打开失败
	}
	
	DEBUG_LOG("ReOpen Sucess, ReReading&Checking...\n");
	
	// 读取并校验数据
	for (counter = 0; counter < fileSize; counter++) 
	{
		res = f_read(&fil, &read_byte, 1, &bytes_read);

		if(((counter) % (100 * 1024)) == (100 * 1024 - 4))
		{
			DEBUG_LOG("read random num %d kB, %.3f MB\n", (counter + 1) / 1024, (float)(counter + 1) / 1024.0f / 1024.0f);
		}

		// 检查读取状态
		if (res != FR_OK || bytes_read != 1) 
		{
			f_close(&fil);
			f_unmount(diskRootPath);
			DEBUG_LOG("file reRead Failed\n");
			return -6;  // 读取失败
		}

		// 校验数据
		if (read_byte != (unsigned char)(counter & 0xFF)) 
		{
			f_close(&fil);
			f_unmount(diskRootPath);
			DEBUG_LOG("file reRead Data Failed:offset:%#x, %#x != %#x\n", counter, read_byte, (unsigned char)(counter & 0xFF));
			return -7;  // 数据校验失败
		}
	}
	
	DEBUG_LOG("read random num %d kB, %.3f MB\n", (counter + 1) / 1024, (float)(counter + 1) / 1024.0f / 1024.0f);
    
	DEBUG_LOG("Finish\n");

	// 5. 最终清理
	res = f_close(&fil);
	if (res != FR_OK) 
	{
		f_unmount(diskRootPath);
		DEBUG_LOG("file close Failed\n");
		return -8;  // 最终关闭失败
	}

	// 6. 删除测试文件
	if(f_unlink(textTxtPath) != FR_OK)
	{
		DEBUG_LOG("file Del Failed\n");
	}

	// 卸载文件系统（这里不检查卸载结果）
	f_unmount(diskRootPath);

	DEBUG_LOG("Environment Test Success <^-^>\n");
	return 0;  // 所有测试通过
}

FATFS fs2 = {0};

#define DYNAMIC_MEM_MELLOC		1
#if DYNAMIC_MEM_MELLOC
#include "cmsis_os.h"
#define EXFUNS_MELLOC(size)		pvPortMalloc(size)
#define EXFUNS_FREE(pin)		vPortFree(pin)
#endif

#define COPY_BUF_SIZE		4096
#if !DYNAMIC_MEM_MELLOC
FIL ssrc_file, ddst_file;
uint8_t cpyBuf[COPY_BUF_SIZE];
#endif

FRESULT Lime_mountDualFatFs(void)
{
	FRESULT res;
	
	res += f_mount(&fs, "0:", 1);
	res += f_mount(&fs2, "1:", 1);
	
	return res;
}


