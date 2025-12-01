#include "nrfPhoto.h"
#include "cmsis_os.h"
#include <stdio.h>
#include <string.h>


#include "dcmi.h"
#include "Lime_sccb.h"
#include "Lime_ov2640.h"
#include "Lime_vofaConn.h"
#include "Lime_nrf_video.h"

#if ((1) && GLOBAL_DEBUG_LOG_EN)
	#define DEBUG_LOG(...)	printf(__VA_ARGS__)
#else
	#define DEBUG_LOG(...)
#endif

uint8_t CameraBuffer[10 * 1024] = {0};

static TaskHandle_t nrfPhotoTask_handler = NULL;

volatile bool nrfPhoto_task_restart_flag = false;
volatile uint8_t nrfPhoto_target_channel = 0;

void nrfPhoto_main(void const * argument)
{
	/* get the handle of task */
	nrfPhotoTask_handler = xTaskGetHandle(pcTaskGetName(NULL));
	
	/* waiting bsp UI task */
	if(ulTaskNotifyTake(pdTRUE,pdMS_TO_TICKS(3000))  != pdPASS)
	{
		DEBUG_LOG(">>>Task:%s, ulTaskNotifyTake Timeout!\n", __FUNCTION__);
		
		while(1)
		{
			osDelay(1000);
		}
	}
	else
	{
		DEBUG_LOG(">>>Task:%s, start!\n", __FUNCTION__);
	}
	
	/* stop this task, waiting nrf message call */
	nrfPhotoTask_Suspend();

nrfPhoto_Restart_pos:
	if(nrfPhoto_task_restart_flag)
	{
		nrfPhoto_task_restart_flag = false;
		DEBUG_LOG(">>>Task:%s, restart!,channel:%d\n", __FUNCTION__, nrfPhoto_target_channel);
	}
	
	HAL_GPIO_WritePin(NRF1_EN_GPIO_Port, NRF1_EN_Pin, GPIO_PIN_RESET);
	osDelay(100);
	
	for(uint8_t i = 0; i < 5; i++)
	{
		if(FastNrf_InitTemplete(NRF_Mode_Tx, nrfPhoto_target_channel) == 1)
			break;
	}
	
#if 1
	/* release Camera RESET */
	HAL_GPIO_WritePin(PWDN_GPIO_Port, PWDN_Pin, GPIO_PIN_RESET);
	
	/* Init Camera */
	Lime_ov2640_Init();
	HAL_DCMI_Start_DMA(&hdcmi, DCMI_MODE_CONTINUOUS, (uint32_t)CameraBuffer, sizeof(CameraBuffer));
#else
	/* force Camera RESET */
	HAL_GPIO_WritePin(PWDN_GPIO_Port, PWDN_Pin, GPIO_PIN_SET);
#endif
	
	uint32_t taskStartTime = HAL_GetTick();
	uint32_t lastTxStartTime = 0;
	
	while(1)
	{
		/* wait task notify */
		if (ulTaskNotifyTake(pdTRUE, 2000) != pdPASS)
		{
			goto nrfPhoto_Restart_pos;
		}
		else
		{
			if(nrfPhoto_task_restart_flag)
			{
				goto nrfPhoto_Restart_pos;
			}
		}
		
		
		/* stop dcmi */
		HAL_DCMI_Stop(&hdcmi);
		
		/* find total size */
		uint32_t totalJpegSize = (sizeof(CameraBuffer) - DMA2_Stream7->NDTR) * 4;
		uint32_t endPos = 0;
		bool hasEndFlag = false;
		for(uint32_t i = totalJpegSize + 4; i > totalJpegSize / 2; i--)
		{
			if(CameraBuffer[i] == 0xff && CameraBuffer[i + 1] == 0xd9)
			{
				hasEndFlag = true;
				endPos = i + 1;
			}
		}
		
		/* check jpeg format ok! */
		if(hasEndFlag)
		{
			endPos += ((32 - endPos % 32)) + 32;
#if 0
			/* send jpeg to Vofa+ */
			Vofa_sendOneImegeOfJpeg((uint8_t*)CameraBuffer, endPos);
#else
//			DEBUG_LOG("jpeg ok!, len:%d, raw:%d B\n", endPos, totalJpegSize);
		
		/* send jpeg to LimeRC */
		int8_t ret = FastNrf_SendData(&NRF_Fast, CameraBuffer, endPos);
			
		/* test FPS */
#if 0
		static uint32_t lastFpsTickTime = 0;
		static uint8_t totalSuccessCount = 0;
		if(ret)
		{
			/* count 100 frame, calculate fps */
			if(totalSuccessCount == 0)
			{
				float oneSnapTimeMs = (HAL_GetTick() - lastFpsTickTime) / 100.0f;
				DEBUG_LOG("fps:%.2f\n", 1000.0f / oneSnapTimeMs);
				lastFpsTickTime = HAL_GetTick();
			}
			
			totalSuccessCount = (totalSuccessCount >= 100) ? 0 : totalSuccessCount + 1;
		}
#endif
			
			
		/* error occurred, retry */
		if(((HAL_GetTick() - NRF_Fast.txStartTime) > 1000) && (lastTxStartTime == NRF_Fast.txStartTime) && ((HAL_GetTick() - taskStartTime) > 500))
		{
			DEBUG_LOG("Nrf Video Err! restart Task!");
			osDelay(2);
			
			goto nrfPhoto_Restart_pos;
		}
		lastTxStartTime = NRF_Fast.txStartTime;
#endif
		}
		
		/* clear camera buffer */
		memset(CameraBuffer, 0, endPos + 16);
		
		/* restart dcmi */
		HAL_DCMI_Start_DMA(&hdcmi, DCMI_MODE_SNAPSHOT, (uint32_t)CameraBuffer,sizeof(CameraBuffer));
	}
}

void nrfPhoto_taskYIELD_exHook(bool isISR)
{
	if (xTaskGetSchedulerState() != taskSCHEDULER_NOT_STARTED)
	{
		static BaseType_t xHigherPriorityTaskWoken;
		
		if(isISR)
		{
			vTaskNotifyGiveFromISR(nrfPhotoTask_handler, &xHigherPriorityTaskWoken);
			portYIELD_FROM_ISR(xHigherPriorityTaskWoken);
		}
		else
		{
			xTaskNotifyGive(nrfPhotoTask_handler);
			taskYIELD();
		}
	}
}

static bool nrfPhotoTask_suspended = true;

void nrfPhotoTask_ReStart(uint8_t channel)
{
	if(nrfPhotoTask_handler == NULL)
		return;
	
	DEBUG_LOG("%s(%d)\n", __FUNCTION__, channel);
	
	if( !nrfPhotoTask_suspended)
		return;
	
	/* target channel */
	nrfPhoto_target_channel = channel;
	
	/* set restart flag */
	nrfPhoto_task_restart_flag = true;
	
	/* resume task */
	vTaskResume(nrfPhotoTask_handler);
	
	nrfPhotoTask_suspended = false;
}

void nrfPhotoTask_Suspend(void)
{
	if(nrfPhotoTask_handler == NULL)
		return;
	
	DEBUG_LOG("%s()\n", __FUNCTION__);
	
	/* suspend task */
	vTaskSuspend(nrfPhotoTask_handler);
	
	/* force NRF Power off */
//	HAL_GPIO_WritePin(NRF1_EN_GPIO_Port, NRF1_EN_Pin, GPIO_PIN_SET);
	
	/* force Camera RESET */
//	HAL_GPIO_WritePin(PWDN_GPIO_Port, PWDN_Pin, GPIO_PIN_SET);
	
	/* stop dcmi */
	HAL_DCMI_Stop(&hdcmi);
	
	nrfPhotoTask_suspended = true;
}

bool nrfPhotoTask_IsSuspend(void)
{
	return nrfPhotoTask_suspended;
}

