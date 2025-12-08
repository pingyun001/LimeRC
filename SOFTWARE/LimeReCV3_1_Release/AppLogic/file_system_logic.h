#ifndef __FILE_SYSTEM_LOGIC
#define __FILE_SYSTEM_LOGIC

#include "main.h"
#include "ff.h"
#include "cmsis_os.h"

#define LIME_COPY_BUF_SIZE		64
#define LIME_FATFS_DYNAMIC_MEM_MELLOC		0

#if LIME_FATFS_DYNAMIC_MEM_MELLOC
#include "cmsis_os.h"
#define LIME_FATFS_MELLOC(size)		pvPortMalloc(size)
#define LIME_FATFS_FREE(pin)			vPortFree(pin)
#endif


void file_system_Init(void);
bool file_system_sd_card_detech_handle(void);

FRESULT Lime_file_system_get_free(char *pdrv, uint32_t *total, uint32_t *free);

FRESULT Lime_file_copy(const char* src_path, const char* dst_path);

FRESULT Lime_file_path_copy(const char* src_path, const char* dst_path);

FRESULT Lime_file_create(const char* filename, const uint8_t* buffer, uint32_t buffer_size);

FRESULT Lime_file_system_mkfs(const char *root_path);

FRESULT Lime_file_path_clean(const char *dst_path);

FRESULT Lime_file_system_get_folder_size(const char *dst_path, float* size_kB);

bool Lime_file_exists(const char* filename);

FRESULT Lime_file_delete(const char *dst_path);

#endif		//__FILE_SYSTEM_LOGIC
