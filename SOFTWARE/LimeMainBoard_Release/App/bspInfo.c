#include "bspInfo.h"
#include "cmsis_os.h"
#include <stdio.h>
#include <string.h>

#include "Lime_eeprom.h"
#include "nrfMessage.h"
#include "Lime_App_Hal.h"
#include "adc.h"
#include "Lime_rtc_time.h"
#include "w25qFlash.h"
#include "w25qFlash_TestDemo.h"
#include "sdCard_TestDemo.h"
#include "FatFsSelfTest.h"
#include "ws2812.h"
#include "oled_st7789.h"
#include "file_system_logic.h"
#include "nrfVideo.h"
#include "Lime_jpeg_decode.h"
#include "Lime_nrf_video.h"
#include "Lime_nrf_master.h"
#include "Lime_nrf_master_ports.h"
#include "Lime_sub_board.h"

static void bspInfo_FatFsRunHandle(void);
static void bspInfo_FatFs_SyncInfo(void);
static void bspInfo_SyncEEP(bool isInit);
static void bspInfo_PowerCtrl(void);
static void bspInfo_RockerCali(void);

static const float battery_level_book[3] = {3.55f, 3.75f, 3.95f};
void bspInfo_SyncLvData(void)
{
	/* sync RTC Time */
	uint8_t time[3] = {0};
	LimeRtc_GetNowTime(&time[0], &time[1], &time[2]);
	LimeHAL_SetTime(time[0], time[1], time[2]);
	uint16_t year = 0;
	uint8_t month = 0, day = 0;
	LimeRtc_GetNowDate(&year, &month, &day);
	LimeHAL_SetDate(year, month, day);
	if(LimeHAL_IsRtcShouldSave())
	{
		LimeHAL_GetSettedTime(&time[0], &time[1], &time[2]);
		LimeRtc_SetNowTime(time[0], time[1], time[2]);
		LimeHAL_GetSettedDate(&year, &month, &day);
		LimeRtc_SetNowDate(year, month, day);
	}
	
	/* sync BAT Info */
	static int8_t batInfo = -1;
	float usbVol = HAL_ADC_GetValue(&hadc1) / 65536.0f * 3.3f * 2;
	float batVol = HAL_ADC_GetValue(&hadc2) / 65536.0f * 3.3f * (15.1f / 10.0f);
	batVol = fmap(batVol, 3.560f, 4.000f, 3.423f, 3.870f);
	if(usbVol > 4.5)
		batInfo = -1;
	else
	{
		if(batInfo == -1)
		{
			if(batVol > battery_level_book[2] + 0.02f)
				batInfo = 3;
			else
				batInfo = 2;
		}
		if((batInfo == 3) && (batVol < battery_level_book[2]))
			batInfo = 2;
		if((batInfo == 2) && (batVol < battery_level_book[1]))
			batInfo = 1;
		if((batInfo == 1) && (batVol < battery_level_book[0]))
			batInfo = 0;
		
		if((batInfo == 0) && (batVol > (battery_level_book[0] + 0.02f)))
			batInfo = 1;
		if((batInfo == 1) && (batVol > (battery_level_book[1] + 0.02f)))
			batInfo = 2;
		if((batInfo == 2) && (batVol > (battery_level_book[2] + 0.02f)))
			batInfo = 3;
	}
	LimeHAL_SetBatteryInfo(batInfo);
	WS2812_SetBatteryInfo(batInfo);
//	LEprintf("usbVol:%.3fv, batVol:%.3fv, batInfo:%d\n", usbVol, batVol, batInfo);
	
	/* sync NRF Info */
	video_pack_data_t *video_data = Lime_nrf_video_GetAvaliableBuf();
	LimeHal_LRCFace_t *lrfaceInfo = &LimeHAL_GetInfoPin()->lrfaceInfo;
	LimeHAL_MainFaceInfo_t *mainFaceInfo = &LimeHAL_GetInfoPin()->mainFaceInfo;
	
	if(video_data != NULL)
	{
		uint32_t gap_time = HAL_GetTick() - video_data->finish_time;
		
		if(gap_time < 1000)
			lrfaceInfo->videoRssi = 3;
		else if(gap_time < 1500)
			lrfaceInfo->videoRssi = 2;
		else if(gap_time < 2000)
			lrfaceInfo->videoRssi = 1;
		else
			lrfaceInfo->videoRssi = 0;
		
		gap_time = HAL_GetTick() - LimeNrfMaster_GetLastSuccessRunTime(&LimeInfoNrf);
		if(gap_time < 1000)
		{
			lrfaceInfo->messaRssi = 3;
			mainFaceInfo->messaRssi = 3;
		}
		else if(gap_time < 1500)
		{
			lrfaceInfo->messaRssi = 2;
			mainFaceInfo->messaRssi = 2;
		}
		else if(gap_time < 2000)
		{
			lrfaceInfo->messaRssi = 1;
			mainFaceInfo->messaRssi = 1;
		}
		else
		{
			lrfaceInfo->messaRssi = 0;
			mainFaceInfo->messaRssi = 0;
		}
	}
}

#include "Lime_nrf_video.h"
static uint8_t cache_jpeg_buf[10 * 1024] __attribute__((section("LIME_RAM_D3"), aligned(4))) = {0};
static void bspInfo_FatFsRunHandle(void)
{
	bool storage_info_need_sync = false;
	
	/* setting face */
	LimeHAL_SettingInfo_t *settingInfo = &LimeHAL_GetInfoPin()->settingInfo;
	if(settingInfo->storage_IsFlashDataNeedOutput)
	{
		Lime_file_path_copy("D:/LimeLvResources", "E:/LimeRC_PicOut");
		settingInfo->storage_IsFlashDataNeedOutput = false;
		settingInfo->storage_FlashOutputPercent = 100;
		storage_info_need_sync = true;
	}
	if(settingInfo->storage_IsInternalStorageNeedClean)
	{
		Lime_file_path_clean("D:/LimeLvResources");
		Lime_file_path_copy("E:/LimeLvResources", "D:/LimeLvResources");
		settingInfo->storage_IsInternalStorageNeedClean = false;
		settingInfo->storage_InternalStorageCleanPercent = 100;
		storage_info_need_sync = true;
	}
	if(settingInfo->storage_IsExternalStorageNeedClean)
	{
		Lime_file_system_mkfs("E:");
		settingInfo->storage_IsExternalStorageNeedClean = false;
		settingInfo->storage_ExternalStorageCleanPercent = 100;
		storage_info_need_sync = true;
	}
	
	/* app lrc face */
	LimeHal_LRCFace_t *lrcInfo = &LimeHAL_GetInfoPin()->lrfaceInfo;
	if(lrcInfo->needTakePicture)
	{
		video_pack_data_t *video_pack = Lime_nrf_video_GetAvaliableBuf();
		if(video_pack != NULL)
		{
			/* cache data */
			uint32_t length = video_pack->length;
			memcpy(cache_jpeg_buf, video_pack->buf, length);
			
			/* find useable name */
			char name[128] = {0};
			static uint16_t cnt = 0;
			while(1)
			{
				memset(name, 0, sizeof(name));
				snprintf(name, sizeof(name), "D:/LimeLvResources/LimeSnapShot%03d.jpg", cnt);
				if( !Lime_file_exists(name))
					break;
				
				cnt ++;
			}
			
			/* save pic data to filesystem */
			Lime_file_create(name, cache_jpeg_buf, length);
			
			lrcInfo->needTakePicture = false;
			lrcInfo->takePicturePercent = 100;
		}
		storage_info_need_sync = true;
	}
	
	/* app photo face */
	LimeHal_PhotoFaceInfo_t *photoFaceInfo = &LimeHAL_GetInfoPin()->photoFaceInfo;
	if(photoFaceInfo->isPhotoNeedDelete)
	{
		Lime_file_delete(photoFaceInfo->deletePhotoPath);
		photoFaceInfo->isPhotoNeedDelete = false;
		storage_info_need_sync = true;
	}
	
	/* scan fatfs info & get free size */
	if(storage_info_need_sync)
	{
		bspInfo_FatFs_SyncInfo();
	}
}

static void bspInfo_FatFs_SyncInfo(void)
{
	LimeHAL_SettingInfo_t *settingInfo = &LimeHAL_GetInfoPin()->settingInfo;
	uint32_t total_kb, free_kb;
	float pic_kb;
	Lime_file_system_get_free("D:", &total_kb, &free_kb);
	Lime_file_system_get_folder_size("D:/LimeLvResources", &pic_kb);
	settingInfo->storage_FlashTotalMB = ((float)total_kb) / 1024.0f;
	settingInfo->storage_FlashUsedPicMB = ((float)pic_kb) / 1024.0f;
	settingInfo->storage_FlashUsedLimeOSMB = ((float)0) / 1024.0f;
	settingInfo->storage_FlashUsedOthersMB = ((float)0) / 1024.0f;
	
	if(IS_TF_CARD_INSERTED())
	{
		Lime_file_system_get_free("E:", &total_kb, &free_kb);
		Lime_file_system_get_folder_size("E:/LimeRC_PicOut", &pic_kb);
		settingInfo->storage_SDCardTotalMB = ((float)total_kb) / 1024.0f;
		settingInfo->storage_SDCardUsedPicMB = ((float)pic_kb) / 1024.0f;
		settingInfo->storage_SDCardUsedOthersMB = ((float)((total_kb - free_kb) - pic_kb)) / 1024.0f;
		if(settingInfo->storage_SDCardUsedOthersMB < 0)
			settingInfo->storage_SDCardUsedOthersMB = 0;
	}
	else
	{
		settingInfo->storage_SDCardTotalMB = 0;
		settingInfo->storage_SDCardUsedPicMB = 0;
		settingInfo->storage_SDCardUsedOthersMB = 0;
	}
}

static void bspInfo_autoScreenOffDetechHandle(void)
{
	static uint32_t lastMoveTime = 0;
	static LimeHal_KeyInfo_t cacheLastKeyInfo = {0};
	LimeHal_KeyInfo_t *nowKeyInfo = &LimeHAL_GetInfoPin()->keyInfo;
	
	/* key moved */
	if(memcmp(&cacheLastKeyInfo, nowKeyInfo, 14) != 0)
	{
		memcpy(&cacheLastKeyInfo, nowKeyInfo, 14);
		lastMoveTime = HAL_GetTick();
		
		/* resume screen back light */
		oled_st7789_setBackLight(100);
		
		return;
	}
	
	/* rocker moved */
	for(uint8_t i = 0; i < 4; i++)
	{
		int32_t gap = absX(cacheLastKeyInfo.rocker[i] - nowKeyInfo->rocker[i]);
		
		if(gap > 30)
		{
			lastMoveTime = HAL_GetTick();
			
			/* resume screen back light */
			oled_st7789_setBackLight(100);
			
			break;
		}
	}
	for(uint8_t i = 0; i < 4; i++)
		cacheLastKeyInfo.rocker[i] = nowKeyInfo->rocker[i];
	
	/* get setted time */
	static const uint32_t timeBook[4] = {0, 30*1000, 60*1000, 5*60*1000};
	uint32_t autoReduceLightTime = 0;
	uint32_t autoOffTime = 0;
	uint16_t autoReduceIndex = 0, autoOffIndex = 0;
	LimeHAL_GetSettedAutoReduceLightTimeAndAutoOffTime(&autoReduceIndex, &autoOffIndex);
	autoReduceLightTime = timeBook[autoReduceIndex];
	autoOffTime = timeBook[autoOffIndex];
	autoReduceLightTime = autoReduceLightTime > autoOffTime ? autoOffTime : autoReduceLightTime;
	
	/* auto reduce screen back light */
	if((autoReduceLightTime != 0) && (HAL_GetTick() - lastMoveTime > autoReduceLightTime))
	{
		/* reduce screen back light */
		oled_st7789_setBackLight(15);
	}
	
	/* auto turn off screen */
	if((autoOffTime != 0) && (HAL_GetTick() - lastMoveTime > autoOffTime))
	{
		/* turn off screen */
		oled_st7789_setBackLight(0);
	}
}

static void bspInfo_SyncEEP(bool isInit)
{
	Lime_eep_data_t *syncData = &hotData;
	LimeHAL_SettingInfo_t *settingInfo = &LimeHAL_GetInfoPin()->settingInfo;
	if(isInit)
	{
		/* setting screen */
		settingInfo->receiver_Agreement = syncData->receiver_Agreement;
		settingInfo->receiver_OutToggle = syncData->receiver_OutToggle;
		settingInfo->screen_AutoReduceLightTime = syncData->screen_auto_reduce_light_time;
		settingInfo->screen_AutoOffTime = syncData->screen_auto_off_time;
		
		/* rocker calibration */
		for(uint8_t i = 0; i < 4; i++)
		{
			Lime_sub_board_caliInfo.calEn[i]		= syncData->calEn[i]   ;
			Lime_sub_board_caliInfo.LimitMin[i]	= syncData->LimitMin[i];
			Lime_sub_board_caliInfo.DeathMin[i]	= syncData->DeathMin[i];
			Lime_sub_board_caliInfo.DeathMax[i]	= syncData->DeathMax[i];
			Lime_sub_board_caliInfo.LimitMax[i]	= syncData->LimitMax[i];
		}
		
		/* nrf receiver */
		bool* autoConnEn = nrfMessage_getAutoConnEnPin();
		uint32_t* autoConnUID = nrfMessage_getAutoConnUIDPin();
		
		*autoConnEn = syncData->autoConnEn;
		autoConnUID[0] = syncData->autoConnUID[0];
		autoConnUID[1] = syncData->autoConnUID[1];
		autoConnUID[2] = syncData->autoConnUID[2];
	}
	else
	{
		/* setting screen */
		syncData->receiver_Agreement = settingInfo->receiver_Agreement;
		syncData->receiver_OutToggle = settingInfo->receiver_OutToggle;
		syncData->screen_auto_reduce_light_time = settingInfo->screen_AutoReduceLightTime;
		syncData->screen_auto_off_time = settingInfo->screen_AutoOffTime;
		
		/* nrf receiver */
		bool* autoConnEn = nrfMessage_getAutoConnEnPin();
		uint32_t* autoConnUID = nrfMessage_getAutoConnUIDPin();
		
		/* rocker calibration */
		//synced by other function
		
		syncData->autoConnEn = *autoConnEn;
		syncData->autoConnUID[0] = autoConnUID[0]; 
		syncData->autoConnUID[1] = autoConnUID[1]; 
		syncData->autoConnUID[2] = autoConnUID[2]; 
	}
}

static void bspInfo_PowerCtrl(void)
{
	static uint32_t kr_startPressTime = 0;
	static bool lastKRPressed = false;
	static bool isKRReleasedAfterAPPRun = false;
	
	/* app running */
	if(LimeHAL_GetAppRunningIndex() != -1)
	{
		kr_startPressTime = HAL_GetTick();
		lastKRPressed = false;
		isKRReleasedAfterAPPRun = false;
		return;
	}
	
	if(IS_KR_PRESSED() && (!lastKRPressed))
	{
		kr_startPressTime = HAL_GetTick();
	}
	
	if(!IS_KR_PRESSED())
	{
		isKRReleasedAfterAPPRun = true;
	}
	
	if(IS_KR_PRESSED() && (lastKRPressed) && ((HAL_GetTick() - kr_startPressTime) > 1500) && (isKRReleasedAfterAPPRun))
	{
		/* disable main power	*/
		HAL_GPIO_WritePin(MAIN_PWR_GPIO_Port, MAIN_PWR_Pin, GPIO_PIN_RESET);
		
		NVIC_SystemReset();
	}
	
	lastKRPressed = IS_KR_PRESSED();
}

static void bspInfo_RockerCali(void)
{
	bool res = false;
	static bool caliNeedRun = false;
	LimeHAL_CaliAppInfo_t *caliAppInfo = &LimeHAL_GetInfoPin()->caliAppInfo;
	
	/* exit when not finish cali work */
	if(LimeHAL_GetAppRunningIndex() == -1)
	{
		caliNeedRun = false;
	}
	
	/* get the cali cmd */
	if(caliAppInfo->isCaliNeedRun)
	{
		caliNeedRun = true;
		caliAppInfo->isCaliNeedRun = false;
		caliAppInfo->isCaliFinished = false;
		Lime_sub_board_rocker_AutoCalHandle(true, &res, &Lime_sub_board_caliInfo, (int8_t*)caliAppInfo->channelPos, caliAppInfo->channelPercent);
		osDelay(300);
		Lime_sub_board_rocker_AutoCalHandle(true, &res, &Lime_sub_board_caliInfo, (int8_t*)caliAppInfo->channelPos, caliAppInfo->channelPercent);
	}
	
	/* run cali work */
	if(caliNeedRun)
	{
		Lime_sub_board_rocker_AutoCalHandle(false, &res, &Lime_sub_board_caliInfo, (int8_t*)caliAppInfo->channelPos, caliAppInfo->channelPercent);
		caliAppInfo->isCaliFinished = res;
		if(res == true)
		{
			/* save data to eeprom */
			Lime_eep_data_t *syncData = &hotData;
			for(uint8_t i = 0; i < 4; i++)
			{
				syncData->calEn[i] = Lime_sub_board_caliInfo.calEn[i];
				syncData->LimitMin[i] = Lime_sub_board_caliInfo.LimitMin[i];
				syncData->DeathMin[i] = Lime_sub_board_caliInfo.DeathMin[i];
				syncData->DeathMax[i] = Lime_sub_board_caliInfo.DeathMax[i];
				syncData->LimitMax[i] = Lime_sub_board_caliInfo.LimitMax[i];
			}
		}
	}
}

static void bspInfo_SwitchCalcu(void)
{
	LimeHal_KeyInfo_t *keyInfoPin = &LimeHAL_GetInfoPin()->keyInfo;
	static LimeHal_KeyInfo_t Last_KeyInfo = {0};
	
	if((keyInfoPin->l_down != Last_KeyInfo.l_down) && (keyInfoPin->l_down % 2))
		keyInfoPin->s_l = 2;
	if((keyInfoPin->l_mid != Last_KeyInfo.l_mid) && (keyInfoPin->l_mid % 2))
		keyInfoPin->s_l = 3;
	if((keyInfoPin->l_up != Last_KeyInfo.l_up) && (keyInfoPin->l_up % 2))
		keyInfoPin->s_l = 1;
	
	if((keyInfoPin->r_down != Last_KeyInfo.r_down) && (keyInfoPin->r_down % 2))
		keyInfoPin->s_r = 1;
	if((keyInfoPin->r_mid != Last_KeyInfo.r_mid) && (keyInfoPin->r_mid % 2))
		keyInfoPin->s_r = 3;
	if((keyInfoPin->r_up != Last_KeyInfo.r_up) && (keyInfoPin->r_up % 2))
		keyInfoPin->s_r = 2;
	
	WS2812_SetRockerSInfo(keyInfoPin->s_l, keyInfoPin->s_r);
	
	Last_KeyInfo.l_down = keyInfoPin->l_down;
	Last_KeyInfo.l_mid = keyInfoPin->l_mid;
	Last_KeyInfo.l_up = keyInfoPin->l_up;
	Last_KeyInfo.r_down = keyInfoPin->r_down;
	Last_KeyInfo.r_mid = keyInfoPin->r_mid;
	Last_KeyInfo.r_up = keyInfoPin->r_up;
}

void bspInfo_main(void const * argument)
{
	LEprintf(">>>Task:%s, start!\n", __FUNCTION__);
	
	osDelay(100);
	
	/* check eeprom info */
	LimeHAL_SetInitStep(5, "EEPROM");
	Lime_eep_Init(&LimeEEP, &defaultData, &hotData);
	bspInfo_SyncEEP(true);
	osDelay(100);
	
	/* start nrf init sequence */
	LimeHAL_SetInitStep(25, "NRF-Message");
	nrfMessage_taskYIELD_exHook(false);
	osDelay(100);
	
	/* start nrf Video task */
	LimeHAL_SetInitStep(40, "NRF-Video");
	LimeHAL_VideoSetBuffer(Lime_jpeg_getResBuf());
	nrfVideo_taskYIELD_exHook(false);
	osDelay(100);
	
	/* start ADC */
	LimeHAL_SetInitStep(50, "ADC");
	HAL_ADC_Start(&hadc1);
	HAL_ADC_Start(&hadc2);
	osDelay(100);
	
	/* Init FatFs */
	LimeHAL_SetInitStep(75, "FATFS");
	file_system_Init();
	bspInfo_FatFs_SyncInfo();
	osDelay(100);
	
	/* Init RTC */
	LimeHAL_SetInitStep(80, "RTC");
	LimeRtc_CheckAndInitDefaultTime();
	osDelay(100);
	
	LimeHAL_SetInitStep(99, "ALL Finish");
	osDelay(200);
	LimeHAL_SetInitStep(100, "ALL Finish");
	
	
	while(1)
	{
		/* scan hot data & auto save to eeprom */
		bspInfo_SyncEEP(false);
		Lime_eep_SyncData(&LimeEEP, &hotData);
		
		static uint32_t autoScreenDetechTime = 0;
		if(HAL_GetTick() - autoScreenDetechTime > 100)
		{
			autoScreenDetechTime = HAL_GetTick();
			bspInfo_autoScreenOffDetechHandle();
		}
		
		static uint32_t autoSDcardDetechTime = 10;
		if(HAL_GetTick() - autoSDcardDetechTime > 100)
		{
			autoSDcardDetechTime = HAL_GetTick();
			bool is_mount_sd = file_system_sd_card_detech_handle();
			if(is_mount_sd)
			{
				bspInfo_FatFs_SyncInfo();
			}
		}
		
		static uint32_t autoBspInfoSyncTime = 20;
		if(HAL_GetTick() - autoBspInfoSyncTime > 100)
		{
			autoBspInfoSyncTime = HAL_GetTick();
			bspInfo_SyncLvData();
		}
		
		bspInfo_FatFsRunHandle();
		
		bspInfo_PowerCtrl();
		
		bspInfo_RockerCali();
		
		bspInfo_SwitchCalcu();
		
		/* just test key info */
		LimeHal_KeyInfo_t *keyInfoPin = &LimeHAL_GetInfoPin()->keyInfo;
//		LEprintf("ch0:%d,1:%d,2:%d,3:%d\n", keyInfoPin->rocker[0], keyInfoPin->rocker[1], keyInfoPin->rocker[2], keyInfoPin->rocker[3]);
		
		
//		LimeRtc_PrintNowTime();
//		osDelay(390);
		
		osDelay(5);
	}
	
//	FatFs_test(0);
//	FatFs_test(1);
//	
//	FatFs_TestHardware(0, 6 * 1024);
//	FatFs_TestHardware(1, 128 * 1024);
}
