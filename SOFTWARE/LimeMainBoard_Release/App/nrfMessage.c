#include "nrfMessage.h"
#include "cmsis_os.h"
#include <stdio.h>

#include "Lime_nrf_master_ports.h"
#include "Lime_App_Hal.h"
#include <string.h>

#include "nrfVideo.h"

static TaskHandle_t nrfMessageTask_handler = NULL;

typedef enum
{
	nrf_message_status_noConnect_Init,
	nrf_message_status_noConnect_Scaning,
	nrf_message_status_Connected,
	
}nrf_message_status_e;

typedef struct
{
	nrf_message_status_e nowStatus;
	nrf_message_status_e lastStatus;
	uint8_t sendErrCount;
	
	bool autoConnEn;
	uint32_t autoConnUID[3];
	
	uint8_t target_free_MessageChannel;
	uint8_t target_free_VideoChannel;
}nrf_message_t;

static nrf_message_t nrf_message = 
{
	.nowStatus = nrf_message_status_noConnect_Init,
	.sendErrCount = 0,
	.autoConnEn = false,
};


bool* nrfMessage_getAutoConnEnPin(void)
{
	return &nrf_message.autoConnEn;
}
uint32_t* nrfMessage_getAutoConnUIDPin(void)
{
	return nrf_message.autoConnUID;
}

static void nrfMessage_AppRunHandle(void)
{
	LimeHAL_NRF_Info_t *LimeHalNrfInfo = &LimeHAL_GetInfoPin()->nrfInfo;
	
	/* en auto conn, when UID same, auto conn */
	if(nrf_message.autoConnEn && (nrf_message.nowStatus == nrf_message_status_noConnect_Scaning))
	{
		for(uint8_t i = 0; i < LIME_NRF_SLAVE_INFO_NUM; i++)
		{
			if(LimeNrfSlaveInfo[i].isEnable && (LimeNrfSlaveInfo[i].UID[0] == nrf_message.autoConnUID[0])
																			&& (LimeNrfSlaveInfo[i].UID[1] == nrf_message.autoConnUID[1])
																			&& (LimeNrfSlaveInfo[i].UID[2] == nrf_message.autoConnUID[2]))
			{
				LimeHalNrfInfo->w_startConn = true;
				LimeHalNrfInfo->w_wantUsedIndex = i;
				break;
			}
		}
	}
	
	/* try connect from GUI */
	if(LimeHalNrfInfo->w_startConn)
	{
		if(nrf_message.nowStatus == nrf_message_status_Connected)
		{
			LEprintf("Try Disconn!!!");
			
			/* disable auto conn */
			LimeHalNrfInfo->w_startConn = 0;
			nrf_message.autoConnEn = false;
			
			/* disable video trans */
			nrfVideo_Stop();
			
			/* jump to scan mode */
			nrf_message.nowStatus = nrf_message_status_noConnect_Scaning;
				
			/* clean last scan res */
			LimeHalNrfInfo->totalNum = 0;
			for(uint8_t i = 0; i < LIME_NRF_SLAVE_INFO_NUM; i++)
			{
				LimeNrfSlaveInfo[i].isEnable = false;
				LimeHalNrfInfo->itemInfo[i].status = 0;
			}
			
			/* return to channel 0 */
			LimeNrfMaster_Init(&LimeInfoNrf);
		}
		else
		{
			/* find free channel */
			for(uint8_t i = 4; i < 64; i += 4)
			{
				if(LimeNrfMaster_CheckChannelIsClear(&LimeInfoNrf, i) == HAL_OK)
				{
					LEprintf("Channel%d, free\n", i);
					
					nrf_message.target_free_MessageChannel = i;
					nrf_message.target_free_VideoChannel = (i - 4) * 2 + 64;
					
					nrfVideo_Start(nrf_message.target_free_VideoChannel);
					osDelay(100);
					
					break;
				}
				else
				{
					LEprintf("Channel%d, busy\n", i);
				}
			}
			
			if(LimeNrfMaster_JumpToPrivateChannel(&LimeInfoNrf, &LimeNrfSlaveInfo[LimeHalNrfInfo->w_wantUsedIndex], nrf_message.target_free_MessageChannel) == HAL_OK)
			{
				LimeHalNrfInfo->itemInfo[LimeHalNrfInfo->w_wantUsedIndex].status |= LimeHAL_NRF_Status_Connected;
				LimeHalNrfInfo->w_startConn = 0;
				nrf_message.nowStatus = nrf_message_status_Connected;
				
				/* sync receiver name to LimeRC APP */
				LimeHAL_SetReceiverName(LimeNrfSlaveInfo[LimeHalNrfInfo->w_wantUsedIndex].name);
				LimeHAL_SetReceiverChannel(nrf_message.target_free_MessageChannel);
				
				/* cache connected UID */
				nrf_message.autoConnUID[0] = LimeNrfSlaveInfo[LimeHalNrfInfo->w_wantUsedIndex].UID[0];
				nrf_message.autoConnUID[1] = LimeNrfSlaveInfo[LimeHalNrfInfo->w_wantUsedIndex].UID[1];
				nrf_message.autoConnUID[2] = LimeNrfSlaveInfo[LimeHalNrfInfo->w_wantUsedIndex].UID[2];
			}
		}
		
		
		osDelay(50);
	}
	
	/* first connect to slave */
	if(nrf_message.nowStatus == nrf_message_status_Connected && 
		nrf_message.lastStatus != nrf_message_status_Connected)
	{
		/* get now app, if limeRC app, enable video */
		int16_t lv_now_running_index =  LimeHAL_GetAppRunningIndex();
		bool video_en = (lv_now_running_index == 0);
		
		LimeHAL_SettingInfo_t *setInfo = &LimeHAL_GetInfoPin()->settingInfo;
		if(LimeNrfMaster_SetReceiverOutputMode(&LimeInfoNrf, setInfo->receiver_Agreement, setInfo->receiver_OutToggle, video_en, nrf_message.target_free_VideoChannel) != HAL_OK)
		{
			LEprintf("Sync output mode failed!,line:%d\n", __LINE__);
			return;
		}
		
		osDelay(50);
	}
	
	/* (normal)send rocker info to receiver */
	if(nrf_message.nowStatus == nrf_message_status_Connected)
	{
		/* send rocker info */
		if(LimeNrfMaster_SendRockerInfo(&LimeInfoNrf, &LimeHAL_GetInfoPin()->keyInfo) != HAL_OK)
		{
			/* send failed(receiver offline) */
			nrf_message.sendErrCount ++;
			LEprintf("Err Occurred:%d!\n", nrf_message.sendErrCount);
			if(nrf_message.sendErrCount > 20)
			{
				nrf_message.nowStatus = nrf_message_status_noConnect_Scaning;
				
				/* clean last scan res */
				LimeHalNrfInfo->totalNum = 0;
				for(uint8_t i = 0; i < LIME_NRF_SLAVE_INFO_NUM; i++)
				{
					LimeNrfSlaveInfo[i].isEnable = false;
					LimeHalNrfInfo->itemInfo[i].status = 0;
				}
				
				/* return to channel 0 */
				LimeNrfMaster_Init(&LimeInfoNrf);
				
				/* disable nrf Video */
				nrfVideo_Stop();
				
			}
			
			/* en auto conn */
			nrf_message.autoConnEn = true;
			
			return;
		}
		else
		{
			/* send success */
			nrf_message.sendErrCount = 0;
		}
		osDelay(2);
		
		/* en/disable video transmit when enter/exit LimeRC App */
		int16_t lv_now_running_index =  LimeHAL_GetAppRunningIndex();
		static int16_t lv_last_running_index = -1;
		
		/* enter LimeRC App */
		if(lv_now_running_index == 0 && lv_last_running_index == -1)
		{
			LEprintf("EnableVideo\n");
			
			LimeHAL_SettingInfo_t *setInfo = &LimeHAL_GetInfoPin()->settingInfo;
			if(LimeNrfMaster_SetReceiverOutputMode(&LimeInfoNrf, setInfo->receiver_Agreement, setInfo->receiver_OutToggle, true, 64) != HAL_OK)
			{
				LEprintf("Sync output mode failed!,line:%d\n", __LINE__);
				return;
			}
		}
		
		/* exit LimeRC App */
		if(lv_now_running_index == -1 && lv_last_running_index == 0)
		{
			LEprintf("DisableVideo\n");
			
			LimeHAL_SettingInfo_t *setInfo = &LimeHAL_GetInfoPin()->settingInfo;
			if(LimeNrfMaster_SetReceiverOutputMode(&LimeInfoNrf, setInfo->receiver_Agreement, setInfo->receiver_OutToggle, false, 64) != HAL_OK)
			{
				LEprintf("Sync output mode failed!,line:%d\n", __LINE__);
				return;
			}
		}
		
		lv_last_running_index = lv_now_running_index;
		osDelay(2);
	}
	
	/* default: try to scan slave info */
	if(nrf_message.nowStatus == nrf_message_status_noConnect_Scaning)
	{
		/* try to scan slave info */
		if(LimeNrfMaster_ScanSlave(&LimeInfoNrf, LimeNrfSlaveInfo) == HAL_OK)
		{
			/* sync info to HAL */
			static uint8_t totalNum = 0;
			totalNum = 0;
			for(uint8_t i = 0; i < LIME_NRF_SLAVE_INFO_NUM; i++)
			{
				if(LimeNrfSlaveInfo[i].isEnable)
				{
					LimeHalNrfInfo->itemInfo[i].status = 0;
					LimeHalNrfInfo->itemInfo[i].status |= LimeHAL_NRF_Status_Enabled;
					LimeHalNrfInfo->itemInfo[i].status |= LimeHAL_NRF_Status_IsOnline;
					LimeHalNrfInfo->itemInfo[i].rssi = 2;
					memset(LimeHalNrfInfo->itemInfo[i].name, 0, 32);
					memcpy(LimeHalNrfInfo->itemInfo[i].name, LimeNrfSlaveInfo[i].name, 13);
					
					totalNum ++;
				}
			}
			LimeHalNrfInfo->totalNum = totalNum;
			
			osDelay(200);
		}
		else
			osDelay(500);
	}
	
	/* sync last Status */
	nrf_message.lastStatus = nrf_message.nowStatus;
}

void nrfMessage_main(void const * argument)
{
	/* get the handle of task */
	nrfMessageTask_handler = xTaskGetHandle(pcTaskGetName(NULL));
	
	/* waiting bspInfo task */
	if(ulTaskNotifyTake(pdTRUE,pdMS_TO_TICKS(3000))  != pdPASS)
	{
		LEprintf(">>>Task:%s, ulTaskNotifyTake Timeout!\n", __FUNCTION__);
		
		while(1)
		{
			osDelay(1000);
		}
	}
	else
	{
		LEprintf(">>>Task:%s, start!\n", __FUNCTION__);
	}
	
//	while(1)
//	{
//		osDelay(10);
//	}
	
	/* power on and init Line Info Nrf Hardware */
	LimeNrfMaster_PowerCtrl(&LimeInfoNrf, false);
	osDelay(200);
	LimeNrfMaster_PowerCtrl(&LimeInfoNrf, true);
	osDelay(600);
	for(uint8_t i = 0; i < 5; i++)
	{
		if(LimeNrfMaster_Init(&LimeInfoNrf) == HAL_OK)
		{
			nrf_message.nowStatus = nrf_message_status_noConnect_Scaning;
			break;
		}
	}
	
	/* hardware offline check */
	if(nrf_message.nowStatus == nrf_message_status_noConnect_Init)
	{
		LEprintf("Nrf Message Offline!!!!!\n");
		
		while(1)
		{
			osDelay(1000);
		}
	}
	
	while(1)
	{
		nrfMessage_AppRunHandle();
		osDelay(5);
	}
}

void nrfMessage_taskYIELD_exHook(bool isISR)
{
	if(nrfMessageTask_handler == NULL)
		return;
	
	if (xTaskGetSchedulerState() != taskSCHEDULER_NOT_STARTED)
	{
		static BaseType_t xHigherPriorityTaskWoken;
		
		if(isISR)
		{
			vTaskNotifyGiveFromISR(nrfMessageTask_handler, &xHigherPriorityTaskWoken);
			portYIELD_FROM_ISR(xHigherPriorityTaskWoken);
		}
		else
		{
			xTaskNotifyGive(nrfMessageTask_handler);
			taskYIELD();
		}
	}
}
