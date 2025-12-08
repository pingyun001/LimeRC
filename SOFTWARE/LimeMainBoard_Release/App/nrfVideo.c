#include "nrfVideo.h"
#include "cmsis_os.h"
#include <stdio.h>
#include <string.h>

#include "Lime_jpeg_decode.h"
#include "oled_st7789.h"
#include "Lime_nrf_video.h"

#include "FAST_Nrf.h"
#include "Lime_App_Hal.h"

static TaskHandle_t nrfVideoTask_handler = NULL;


extern const uint8_t ayakaJpeg[19690];
extern unsigned char acApp2[2848];

static bool nrfVideoReStartFlag = false;
static uint8_t targetChannel = 64;

void nrfVideo_main(void const * argument)
{
	/* get the handle of task */
	nrfVideoTask_handler = xTaskGetHandle(pcTaskGetName(NULL));
	
//	/* waiting bspInfo task */
//	if(ulTaskNotifyTake(pdTRUE,pdMS_TO_TICKS(3000))  != pdPASS)
//	{
//		LEprintf(">>>Task:%s, ulTaskNotifyTake Timeout!\n", __FUNCTION__);
//		
//		while(1)
//		{	
//			osDelay(1000);
//		}
//	}
//	else
	{
		LEprintf(">>>Task:%s, start!\n", __FUNCTION__);
	}
	
	nrfVideo_Stop();
	
restart:
	
	HAL_GPIO_WritePin(NRF1_EN_GPIO_Port, NRF1_EN_Pin, GPIO_PIN_RESET);
	osDelay(50);
	Lime_nrf_video_Init(targetChannel, true);
	
	
	while(1)
	{
		while(ulTaskNotifyTake(pdTRUE,pdMS_TO_TICKS(3000))  != pdPASS)
		{
			osDelay(10);
		}
		
		if(nrfVideoReStartFlag)
		{
			nrfVideoReStartFlag = false;
			goto restart;
		}
		
		video_pack_data_t *video_pack = Lime_nrf_video_GetAvaliableBuf();
		
		uint8_t* rgb565buf = Lime_jpeg_getResBuf();
		
		SCB_CleanDCache_by_Addr((uint32_t*)video_pack->buf, video_pack->length + 32);
		Lime_jpeg_decode((uint8_t*)video_pack->buf, video_pack->length);
		
#if 0
		oled_st7789_ColorFill(0, 0, 239, 239, (uint16_t*)rgb565buf);
#else
		LimeHAL_VideoAddedNewFrame();
#endif
	}
}

void nrfVideo_taskYIELD_exHook(bool isISR)
{
	if(nrfVideoTask_handler == NULL)
		return;
	
	if (xTaskGetSchedulerState() != taskSCHEDULER_NOT_STARTED)
	{
		static BaseType_t xHigherPriorityTaskWoken;
		
		if(isISR)
		{
			vTaskNotifyGiveFromISR(nrfVideoTask_handler, &xHigherPriorityTaskWoken);
			portYIELD_FROM_ISR(xHigherPriorityTaskWoken);
		}
		else
		{
			xTaskNotifyGive(nrfVideoTask_handler);
			taskYIELD();
		}
	}
}

void nrfVideo_Stop(void)
{
	/* disable NRF POWER */
	HAL_GPIO_WritePin(NRF1_EN_GPIO_Port, NRF1_EN_Pin, GPIO_PIN_SET);
	
	/* suspend task */
	if(nrfVideoTask_handler != NULL)
	{
		vTaskSuspend(nrfVideoTask_handler);
	}
	
	/* let pic become gray color */
	uint8_t* rgb565buf = Lime_jpeg_getResBuf();
	rgb565_to_grayscale(rgb565buf, 240, 240);
	LimeHAL_VideoAddedNewFrame();
	
}
void nrfVideo_Start(uint8_t channel)
{
	targetChannel = channel;
	nrfVideoReStartFlag = true;
	
	if(nrfVideoTask_handler != NULL)
	{
		vTaskResume(nrfVideoTask_handler);
		nrfVideo_taskYIELD_exHook(false);
	}
}


