#include "file_system_logic.h"
#include "ff.h"
#include "sdCard.h"
#include <string.h>
#include <stdio.h>

#if 1
#define DEBUG_LOG(...)	LEprintf(__VA_ARGS__)
#else
#define DEBUG_LOG(...)
#endif

static FATFS fs_flash = {0};
static FATFS fs_sdcard = {0};

uint8_t *wrBuf = (uint8_t*)"HelloYQY,just test file create & save";

void file_system_Init(void)
{
	/* try to mount flash */
	if(f_mount(&fs_flash, "D:", 1) != FR_OK)
	{
		DEBUG_LOG("FatFs Flash mount Failed\n");
		if(Lime_file_system_mkfs("D:") != FR_OK)
		{
			DEBUG_LOG("Create FileSystem Failed\n");
		}
		if(f_mount(&fs_flash, "D:", 1) != FR_OK)
		{
			DEBUG_LOG("FatFs Flash mount Failed 2\n");
		}
	}
	else
	{
		DEBUG_LOG("FatFs Flash mount Success\n");
	}
	
	/* try to mount SD card */
	file_system_sd_card_detech_handle();
	
	/* just for test */
	float sizeKb = 0;
	Lime_file_system_get_folder_size("D:/LimeLvResources", &sizeKb);
	DEBUG_LOG("floader size:%.3fkb\n", sizeKb);
#if 0
	
//	Lime_file_path_copy("E:/LimeLvResources", "D:/LimeLvResources");
//	Lime_file_path_copy("E:/LimeLvResources", "E:/LimeLvResources2");
	
	Lime_file_system_mkfs("E:");
	
	Lime_file_path_copy("D:/LimeLvResources", "E:/LimePicOut2");
	
	uint32_t total, free;
	Lime_file_system_get_free("D:", &total, &free);
	DEBUG_LOG("Flash:total:%d kb, free:%d kb\n", total, free);
	Lime_file_system_get_free("E:", &total, &free);
	DEBUG_LOG("SDCard:total:%d kb, free:%d kb\n", total, free);
	
//	Lime_file_create("E:/LimeLvResources/testN3.txt", wrBuf, 8);
	
#endif
	
	
}

bool file_system_sd_card_detech_handle(void)
{
	bool value_change = false;
	static bool sd_card_is_inserted_last = false;
	bool sd_card_is_inserted = IS_TF_CARD_INSERTED();
	
	/* sd card inserted */
	if(sd_card_is_inserted && (!sd_card_is_inserted_last))
	{
		DEBUG_LOG("SD Card inserted\n");
		osDelay(200);
		if(f_mount(&fs_sdcard, "E:", 1)  != FR_OK)
		{
			DEBUG_LOG("Mount SD Card Failed\n");
		}
		else
		{
			DEBUG_LOG("Mount SD Card Success\n");
			value_change = true;
		}
	}
	
	/* sd card removed */
	if((!sd_card_is_inserted) && sd_card_is_inserted_last)
	{
		//no operation
		DEBUG_LOG("SD Card Removed\n");
		value_change = true;
	}
	
	sd_card_is_inserted_last = sd_card_is_inserted;
	
	return value_change;
}

FRESULT Lime_file_system_get_free(char *pdrv, uint32_t *total, uint32_t *free)
{
    FATFS *fs1;
    uint8_t res;
    uint32_t fre_clust = 0, fre_sect = 0, tot_sect = 0;
    
    /* 得到磁盘信息及空闲簇数量 */
    res = (uint32_t)f_getfree((const TCHAR *)pdrv, (DWORD *)&fre_clust, &fs1);

    if (res == 0)
    {
        tot_sect = (fs1->n_fatent - 2) * fs1->csize;    /* 得到总扇区数 */
        fre_sect = fre_clust * fs1->csize;              /* 得到空闲扇区数 */
#if FF_MAX_SS!=512  /* 扇区大小不是512字节,则转换为512字节 */
        tot_sect *= fs1->ssize / 512;
        fre_sect *= fs1->ssize / 512;
#endif
        *total = tot_sect >> 1;     /* 单位为KB */
        *free = fre_sect >> 1;      /* 单位为KB */
    }
		else
		{
			*total = 0;
			*free = 0;
		}

    return res;
}

#if !LIME_FATFS_DYNAMIC_MEM_MELLOC
static FIL ssrc_file __attribute__((section("LIME_RAM_D3"), aligned(4))) = {0};
static FIL ddst_file __attribute__((section("LIME_RAM_D3"), aligned(4))) = {0};
static BYTE cpyBuf[LIME_COPY_BUF_SIZE] __attribute__((section("LIME_RAM_D3"), aligned(4)));
#endif

FRESULT Lime_file_copy(const char* src_path, const char* dst_path) 
{
    FIL *src_file, *dst_file;
    FRESULT res;
    UINT br, bw;
    BYTE *buf;
	
#if LIME_FATFS_DYNAMIC_MEM_MELLOC
	src_file = LIME_FATFS_MELLOC(sizeof(FIL));
	dst_file = LIME_FATFS_MELLOC(sizeof(FIL));
	buf = LIME_FATFS_MELLOC(LIME_COPY_BUF_SIZE);
#else
	src_file = &ssrc_file;
	dst_file = &ddst_file;
	buf = cpyBuf;
#endif

    // 打开源文件
    res = f_open(src_file, src_path, FA_READ);
    if (res != FR_OK) 
		{
			DEBUG_LOG("file_copy--open src_file Fail:%d\n", res);
			return res;
		}

    // 创建[目标]文件
    res = f_open(dst_file, dst_path, FA_CREATE_ALWAYS | FA_WRITE);
    if (res != FR_OK) 
		{
				DEBUG_LOG("file_copy--open dst_file Fail:%d\n", res);
        f_close(src_file);
        return res;
    }

    // 逐块读取源文件并写入[目标]文件
    while ((f_read(src_file, buf, LIME_COPY_BUF_SIZE, &br) == FR_OK) && (br > 0)) 
		{
        res = f_write(dst_file, buf, br, &bw);
        if (res != FR_OK || bw < br) break;
    }

    // 关闭文件
    f_close(src_file);
    f_close(dst_file);

    if (res == FR_OK)
			DEBUG_LOG("file_copy %s -> %s --CopyFinish\n", src_path, dst_path);
		else
			DEBUG_LOG("file_copy--ErrOccurred,res:%d\n", res);
	
	
#if LIME_FATFS_DYNAMIC_MEM_MELLOC
	LIME_FATFS_FREE(src_file);
	LIME_FATFS_FREE(dst_file);
	LIME_FATFS_FREE(buf);
#endif
	
	return res;
}

#define DYNAMIC_FILE_PATH_LEN		128
#if !LIME_FATFS_DYNAMIC_MEM_MELLOC
static DIR ddir = {0};
static FILINFO ffno = {0};
static char ssrc_path_buf[DYNAMIC_FILE_PATH_LEN] = {0};
static char ddst_path_buf[DYNAMIC_FILE_PATH_LEN] = {0};
#endif
FRESULT Lime_file_path_copy(const char* src_path, const char* dst_path)
{
	FRESULT res = 0;
	
#if LIME_FATFS_DYNAMIC_MEM_MELLOC
	DIR *dir = LIME_FATFS_MELLOC(sizeof(DIR));
	FILINFO *fno = LIME_FATFS_MELLOC(sizeof(FILINFO));
	char *src_file_path = LIME_FATFS_MELLOC(DYNAMIC_FILE_PATH_LEN);
	char *dst_file_path = LIME_FATFS_MELLOC(DYNAMIC_FILE_PATH_LEN);
#else
	DIR *dir = &ddir;
	FILINFO *fno = &ffno;
	char *src_file_path = ssrc_path_buf;
	char *dst_file_path = ddst_path_buf;
#endif
	
	/* check dst path is exist */
	res = f_mkdir(dst_path);
	switch (res)
	{
		case FR_OK:
				DEBUG_LOG("CreatePath:%s Success\n", dst_path);
				break;
		case FR_EXIST:
				DEBUG_LOG("Path exist\n");
				break;
		default:
			DEBUG_LOG("Create Failed, errCode: %d\n", res);
			goto finish;
	}
	
	/* scan full path */
	res = f_opendir(dir, src_path);
	if (res == FR_OK) 
	{
		// 读取目录中的每个条目
		while ((res = f_readdir(dir, fno)) == FR_OK && fno->fname[0]) 
		{
			// 如果是文件或文件夹，则打印名称
			if (fno->fattrib & AM_DIR)
			{
				DEBUG_LOG("DIR: %s, no need to copy\n", fno->fname);
			} 
			else 
			{
				/* generate file path */
				memset(src_file_path, 0, DYNAMIC_FILE_PATH_LEN);
				memset(dst_file_path, 0, DYNAMIC_FILE_PATH_LEN);
				snprintf(src_file_path, DYNAMIC_FILE_PATH_LEN, "%s/%s", src_path, fno->fname);
				snprintf(dst_file_path, DYNAMIC_FILE_PATH_LEN, "%s/%s", dst_path, fno->fname);
				
				DEBUG_LOG("generated path: copy:%s --> %s\n", src_file_path, dst_file_path);
				DEBUG_LOG("FILE: %s, size:%d B\n", fno->fname, (int)fno->fsize);
				Lime_file_copy(src_file_path, dst_file_path);
			}
		}
		f_closedir(dir);
	} 
	else 
	{
		DEBUG_LOG("Failed to open root directory!\n");
	}
	
finish:
#if LIME_FATFS_DYNAMIC_MEM_MELLOC
	LIME_FATFS_FREE(dir);
	LIME_FATFS_FREE(fno);
	LIME_FATFS_FREE(src_file_path);
	LIME_FATFS_FREE(dst_file_path);
#endif
	return res;
}

#if !LIME_FATFS_DYNAMIC_MEM_MELLOC
static FIL ccreate_file __attribute__((section("LIME_RAM_D3"), aligned(4))) = {0};
#endif
FRESULT Lime_file_create(const char* filename, const uint8_t* buffer, uint32_t buffer_size)
{
	FRESULT res;
	UINT bw;
#if LIME_FATFS_DYNAMIC_MEM_MELLOC
	FIL *create_file = LIME_FATFS_MELLOC(sizeof(FIL));
#else
	FIL *create_file = &ccreate_file;
#endif
	
	DEBUG_LOG("%s(),start\n", __FUNCTION__);
	
	/* create target file */
	res = f_open(create_file, filename, FA_CREATE_ALWAYS | FA_WRITE);
	if (res != FR_OK) 
	{
		DEBUG_LOG("create failed:%d\n", res);
			f_close(create_file);
			return res;
	}
//	DEBUG_LOG("create/open success\n");
	
	/* write to file */
	res = f_write(create_file, buffer, buffer_size, &bw);
	if (res != FR_OK || bw < buffer_size)
	{
		f_close(create_file);
		DEBUG_LOG("write failed,%d\n", res);
#if LIME_FATFS_DYNAMIC_MEM_MELLOC
	LIME_FATFS_FREE(create_file);
#endif
		return res;
	}
	
	f_close(create_file);
//	DEBUG_LOG("write success\n");
#if LIME_FATFS_DYNAMIC_MEM_MELLOC
	LIME_FATFS_FREE(create_file);
#endif
	return res;
}

extern char mkfsBuffer[4096];
FRESULT Lime_file_system_mkfs(const char *root_path)
{
	FRESULT res;
	res = f_mkfs(root_path, 0, mkfsBuffer, FF_MAX_SS);  // 格式化为 FAT32 文件系统
	if (res != FR_OK) 
	{
		DEBUG_LOG("File system creation failed with error code: %d\n", res);
	}
	else
	{
		DEBUG_LOG("File system created successfully.\n");
	}
	
	return res;
}

FRESULT Lime_file_path_clean(const char *dst_path)
{
	FRESULT res = 0;
	
#if LIME_FATFS_DYNAMIC_MEM_MELLOC
	DIR *dir = LIME_FATFS_MELLOC(sizeof(DIR));
	FILINFO *fno = LIME_FATFS_MELLOC(sizeof(FILINFO));
	char *dst_file_path = LIME_FATFS_MELLOC(DYNAMIC_FILE_PATH_LEN);
#else
	DIR *dir = &ddir;
	FILINFO *fno = &ffno;
	char *dst_file_path = ddst_path_buf;
#endif
	
	/* scan full path */
	res = f_opendir(dir, dst_path);
	if (res == FR_OK) 
	{
		// 读取目录中的每个条目
		while ((res = f_readdir(dir, fno)) == FR_OK && fno->fname[0]) 
		{
			// 如果是文件或文件夹，则打印名称
			if (fno->fattrib & AM_DIR)
			{
				DEBUG_LOG("DIR: %s, no need to del\n", fno->fname);
			} 
			else 
			{
				/* generate file path */
				memset(dst_file_path, 0, DYNAMIC_FILE_PATH_LEN);
				snprintf(dst_file_path, DYNAMIC_FILE_PATH_LEN, "%s/%s", dst_path, fno->fname);
				
				DEBUG_LOG("del file:%s, size:%d B\n", dst_file_path, (int)fno->fsize);
				res = f_unlink(dst_file_path);
				if(res != FR_OK)
				{
					DEBUG_LOG("del failed\n");
				}
			}
		}
		f_closedir(dir);
	} 
	else 
	{
		DEBUG_LOG("Failed to open root directory!\n");
	}
	
finish:
#if LIME_FATFS_DYNAMIC_MEM_MELLOC
	LIME_FATFS_FREE(dir);
	LIME_FATFS_FREE(fno);
	LIME_FATFS_FREE(dst_file_path);
#endif
	return res;
}
FRESULT Lime_file_system_get_folder_size(const char *dst_path, float* size_kB)
{
	FRESULT res = 0;
	
#if LIME_FATFS_DYNAMIC_MEM_MELLOC
	DIR *dir = LIME_FATFS_MELLOC(sizeof(DIR));
	FILINFO *fno = LIME_FATFS_MELLOC(sizeof(FILINFO));
#else
	DIR *dir = &ddir;
	FILINFO *fno = &ffno;
#endif
	
	* size_kB = 0;
	
	/* scan full path */
	res = f_opendir(dir, dst_path);
	if (res == FR_OK)
	{
		// 读取目录中的每个条目
		while ((res = f_readdir(dir, fno)) == FR_OK && fno->fname[0]) 
		{
			// 如果是文件或文件夹，则打印名称
			if (fno->fattrib & AM_DIR)
			{
				DEBUG_LOG("DIR: %s, no need to scan\n", fno->fname);
			} 
			else 
			{
				/* get file size */
				* size_kB += ((float)fno->fsize) / 1024.0f;
			}
		}
		f_closedir(dir);
	} 
	else 
	{
		DEBUG_LOG("Failed to open root directory!\n");
		* size_kB = 0;
	}
	
	finish:
#if LIME_FATFS_DYNAMIC_MEM_MELLOC
	LIME_FATFS_FREE(dir);
	LIME_FATFS_FREE(fno);
#endif
	
	return res;
}

bool Lime_file_exists(const char* filename) 
{
#if LIME_FATFS_DYNAMIC_MEM_MELLOC
	FILINFO *fno = LIME_FATFS_MELLOC(sizeof(FILINFO));
#else
	FILINFO *fno = &ffno;
#endif
	
	FRESULT fr = f_stat(filename, fno);
	
#if LIME_FATFS_DYNAMIC_MEM_MELLOC
	LIME_FATFS_FREE(fno);
#endif
	
	return (fr == FR_OK);
}

FRESULT Lime_file_delete(const char *dst_path)
{
	if(f_stat(dst_path, NULL) != FR_OK) 
	{
		return FR_NO_FILE;
	}
	
	return f_unlink(dst_path);
}

