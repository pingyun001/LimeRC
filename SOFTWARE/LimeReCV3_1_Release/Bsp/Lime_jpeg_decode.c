#include "Lime_jpeg_decode.h"

#include "jpeg.h"
#include "dma2d.h"

#if 0
	#define DEBUG_LOG(...)	LEprintf(__VA_ARGS__)
#else
	#define DEBUG_LOG(...)
#endif

/*

fixed w:240, h:240
do not modify

*/

static uint8_t yuvBuf[240 * 240 * 3] __attribute__((section("LIME_RAM_D2"),aligned(32))) = {0};

uint8_t rgb565buff[240 * 240 * 2] __attribute__((section("LIME_RAM_D1"), aligned(32))) = {0};

static uint32_t yuvOffset = 0;

HAL_StatusTypeDef Lime_jpeg_decode(uint8_t *jpegFile, uint32_t size)
{
	yuvOffset = 0;
	
	HAL_JPEG_Decode_DMA(&hjpeg, (uint8_t*)jpegFile, 19800, yuvBuf, 65536);
	while(hjpeg.State != HAL_JPEG_STATE_READY)
		;
	
	HAL_DMA2D_PollForTransfer(&hdma2d, 10);
	HAL_DMA2D_Start(&hdma2d, ((uint32_t)yuvBuf) + 240 * 128 * 2, ((uint32_t)rgb565buff) + 240 * 128 * 2, 240, 240 - 128);
	HAL_DMA2D_PollForTransfer(&hdma2d, 10);

	
	return HAL_OK;
}

uint8_t *Lime_jpeg_getResBuf(void)
{
	SCB_InvalidateDCache_by_Addr(rgb565buff, sizeof(rgb565buff));
	
	return rgb565buff;
}

void HAL_JPEG_InfoReadyCallback(JPEG_HandleTypeDef *hjpeg, JPEG_ConfTypeDef *pInfo)
{
	DEBUG_LOG("w:%d,h:%d\n", pInfo->ImageWidth, pInfo->ImageHeight);
	DEBUG_LOG("ChromaSubsampling:%#x\n", pInfo->ChromaSubsampling);
}

void HAL_JPEG_EncodeCpltCallback(JPEG_HandleTypeDef *hjpeg)
{
	DEBUG_LOG("%s()\n", __FUNCTION__);
}
void HAL_JPEG_DecodeCpltCallback(JPEG_HandleTypeDef *hjpeg)
{
	DEBUG_LOG("%s()\n", __FUNCTION__);
}
void HAL_JPEG_ErrorCallback(JPEG_HandleTypeDef *hjpeg)
{
	DEBUG_LOG("%s()\n", __FUNCTION__);
}
void HAL_JPEG_GetDataCallback(JPEG_HandleTypeDef *hjpeg, uint32_t NbDecodedData)
{
	DEBUG_LOG("%s(%d)\n", __FUNCTION__, NbDecodedData);
}
void HAL_JPEG_DataReadyCallback(JPEG_HandleTypeDef *hjpeg, uint8_t *pDataOut, uint32_t OutDataLength)
{
	DEBUG_LOG("%s(%d)\n", __FUNCTION__, OutDataLength);
	
	yuvOffset += OutDataLength;
	
	HAL_JPEG_ConfigOutputBuffer(hjpeg, yuvBuf + yuvOffset, 65536);
	HAL_DMA2D_Start(&hdma2d, (uint32_t)yuvBuf, (uint32_t)rgb565buff, 240, 128);
}

void rgb565_to_grayscale(uint8_t *rgb565buf, int width, int height) 
{
    // 使用整数运算避免浮点开销
    const uint32_t total_pixels = width * height;
    
    for (uint32_t i = 0; i < total_pixels; i++) {
        const uint32_t idx = i * 2;
        
        // 读取RGB565像素值（小端序）
        const uint16_t pixel = ((uint16_t)rgb565buf[idx + 1] << 8) | rgb565buf[idx];
        
        // 提取RGB分量（5位红，6位绿，5位蓝）
        const uint8_t r5 = (pixel >> 11) & 0x1F;
        const uint8_t g6 = (pixel >> 5) & 0x3F;
        const uint8_t b5 = pixel & 0x1F;
        
        // 快速灰度转换（使用整数近似）
        // 公式：Gray = (R*77 + G*150 + B*29) >> 8
        const uint16_t gray = 
            ((r5 * 77) << 1) +  // 等效 r5 * 154 (补偿5->8位扩展)
            (g6 * 150) +         // g6已接近8位范围
            ((b5 * 29) << 1);    // 等效 b5 * 58 (补偿5->8位扩展)
        
        // 将灰度值映射回RGB565格式
        // 取灰度值的高5位作为红和蓝，高6位作为绿
        const uint8_t gray_val = gray >> 8;  // 取高8位作为灰度值
        const uint16_t gray_pixel = 
            ((gray_val & 0xF8) << 8) |  // 红分量（高5位）
            ((gray_val & 0xFC) << 3) |  // 绿分量（高6位）
            (gray_val >> 3);            // 蓝分量（高5位）
        
        // 以小端序写回缓冲区
        rgb565buf[idx] = gray_pixel & 0xFF;
        rgb565buf[idx + 1] = (gray_pixel >> 8) & 0xFF;
    }
}
