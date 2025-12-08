#ifndef __FATFSSELFTEST_H
#define __FATFSSELFTEST_H

#include "main.h"
#include "ff.h"

void FatFs_test(uint8_t diskID);

//扫描根目录下所有文件并打印出来
void FatFs_TraverseSDCardRoot(char *path);

//通过新建文件 填数据并回读的方式测试文件系统环境是否可靠
int8_t FatFs_TestHardware(uint8_t diskID, uint32_t fileSize);

//Lime复制
FRESULT Lime_mountDualFatFs(void);

#endif	//__FATFSSELFTEST_H
