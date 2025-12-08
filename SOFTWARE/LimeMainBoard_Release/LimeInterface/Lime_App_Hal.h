#ifndef __Lime_App_Hal_H__
#define __Lime_App_Hal_H__


#include <stdint.h>
#if USING_LIME_HARDWARE
#include "main.h"
#include "lvgl.h"
#else
#include "../main/src/main.h"
#endif

typedef struct
{
	char hardwareName[32];
	uint8_t initPercent;

	/* if allHardwarePass==1, start interface will exit*/
	uint8_t allHardwarePass;

}LimeHAL_Hardware_Online_Info_t;


/*
BIT0: this item is enabled
BIT1: history used
BIT2: is online
BIT3: connected
*/
typedef enum
{
	LimeHAL_NRF_Status_Enabled = 0x01,
	LimeHAL_NRF_Status_HistoryUsed = 0x02,
	LimeHAL_NRF_Status_IsOnline = 0x04,
	LimeHAL_NRF_Status_Connecting = 0x08,
	LimeHAL_NRF_Status_Connected = 0x10,
	LimeHAL_NRF_Status_ConnFailed = 0x20,
}LimeHAL_NRF_Status_e;
typedef struct
{
	char name[32];
	LimeHAL_NRF_Status_e status;
	uint8_t rssi;			//range: 0, 1, 2
}LimeHAL_NRF_Item_Info_t;

typedef struct
{
	/*lvgl read only*/
	LimeHAL_NRF_Item_Info_t itemInfo[10];
	uint16_t nowUsedIndex;		//judge itemInfo[nowUsedIndex] | LimeHAL_NRF_Status_Connected to judge success connect.
	uint16_t totalNum;

	/*lvgl write*/
	uint16_t w_wantUsedIndex;	//must smaller than totalNum.
	uint16_t w_startConn;		//set 1 to start connect, if conn success or fail, set 0 by low-level driver. judge conn status by BIT3 of itemInfo[?]->status.
}LimeHAL_NRF_Info_t;

typedef struct
{
	uint8_t sw_up;
	uint8_t sw_set;
	uint8_t sw_left;
	uint8_t sw_right;
	uint8_t sw_down;

	uint8_t l_down;
	uint8_t l_mid;
	uint8_t l_up;
	uint8_t r_down;
	uint8_t r_mid;
	uint8_t r_up;

	uint8_t joy_l;
	uint8_t joy_r;

	uint8_t kr;

	uint8_t version;      //hardware version

	uint32_t refreshTime;
	uint16_t rocker[4];
	
	/* rc self calculated, RAW:l_down, l_mid, l_up, r_down, r_mid, r_up, mapped to l_down, l_mid, l_up, r_up, r_mid, r_down */
	uint8_t s_l;					//1:up, 2:down, 3:mid
	uint8_t s_r;					//1:up, 2:down, 3:mid
}LimeHal_KeyInfo_t;

typedef struct
{
	uint8_t time[3];			//[0]hour, [1]minute, [2]second
	uint8_t messaRssi;		//range: 0(no connect), 1, 2, 3(full strength)
	uint8_t videoRssi;		//range: 0(no connect), 1, 2, 3(full strength)
	int8_t rcBatInfo;			//range: -1(charging), 0(low), 1(mid), 2(high), 3(full)

	char receiverName[32];
	uint8_t receiverChannel;

	uint8_t* videoFrameBuf;
	uint8_t videoFrameCnt;	//0~255, if 255, next is 0. interface can use this to judge video frame is new

	bool needTakePicture;
	uint8_t takePicturePercent;
}LimeHal_LRCFace_t;

/* LVGL Interface can r/w, hardware driver can only read */
typedef struct
{
	bool appLoarderFirmwareIsRunning;
	bool appFirmwareScheLock;
	int16_t appRunningIndex;//if -1, not running.
}LimeHAL_AppRunInfo_t;

typedef struct
{
	uint8_t channelPercent[4][2];	//0~100
	int8_t channelPos[4];	// -100 ~ 100
	bool isCaliNeedRun;
	bool isCaliFinished;
}LimeHAL_CaliAppInfo_t;

typedef struct
{
	uint8_t receiver_Agreement;		//0: DBUS, 1:SBUS, 2:PP, 3:DEBUG
	uint8_t receiver_OutToggle;		//0: silent, 1: normal, 2: toggle

	uint16_t screen_AutoReduceLightTime;	//0: off, 1: 30sec, 2: 1min, 3: 5min
	uint16_t screen_AutoOffTime;			//0: off, 1: 30sec, 2: 1min, 3: 5min

	uint8_t time[3];					//[0]hour, [1]minute, [2]second
	uint16_t year;
	uint8_t month;
	uint8_t day;
	bool isRtcNeedSave;

	float storage_FlashTotalMB;
	float storage_FlashUsedPicMB;
	float storage_FlashUsedOthersMB;
	float storage_FlashUsedLimeOSMB;
	float storage_SDCardTotalMB;
	float storage_SDCardUsedPicMB;
	float storage_SDCardUsedOthersMB;

	bool storage_IsFlashDataNeedOutput;
	uint8_t storage_FlashOutputPercent;
	bool storage_IsInternalStorageNeedClean;
	uint8_t storage_InternalStorageCleanPercent;
	bool storage_IsExternalStorageNeedClean;
	uint8_t storage_ExternalStorageCleanPercent;

}LimeHAL_SettingInfo_t;

typedef struct
{
	uint8_t time[3];					//[0]hour, [1]minute，[2]second
	uint16_t year;					//also for global use
	uint8_t month;
	uint8_t day;
	int8_t rcBatInfo;				//range: -1(charging), 0(low), 1(mid), 2(high), 3(full)
	uint8_t messaRssi;				//range: 0(no connect), 1, 2, 3(full strength)
}LimeHAL_MainFaceInfo_t;

typedef struct
{
	char deletePhotoPath[128];
	bool isPhotoNeedDelete;
}LimeHal_PhotoFaceInfo_t;


typedef struct
{
	/*hardware online info, used when LimeRC power on, show Hello interface*/
    LimeHAL_Hardware_Online_Info_t onlineInfo;

	/*key info, as user input from hardware*/
    LimeHal_KeyInfo_t keyInfo;

	/*nrf info*/
	LimeHAL_NRF_Info_t nrfInfo;

	LimeHAL_MainFaceInfo_t mainFaceInfo;

	/*(very important)lrface info*/
	LimeHal_LRCFace_t lrfaceInfo;

	LimeHal_PhotoFaceInfo_t photoFaceInfo;

	LimeHAL_AppRunInfo_t appRunInfo;

	LimeHAL_CaliAppInfo_t caliAppInfo;

	LimeHAL_SettingInfo_t settingInfo;

}LimeHal_Info_t;

extern LimeHal_Info_t LimeHal_Info;


/*common API*/
LimeHal_Info_t *LimeHAL_GetInfoPin(void);
void LimeHAL_SelectNRF_To_Connect(uint16_t index);
bool LimeHAL_NRF_Has_ConnectedItem(void);
bool LimeHAL_IsAnyMenuKeyPressed(void);
int32_t absX(int32_t x);
float fmap(float x, float in_min, float in_max, float out_min, float out_max);
float fmapWithLimit(float x, float in_min, float in_max, float out_min, float out_max);
uint8_t Lime_CalDayInMonth(uint32_t year, uint8_t month);
int16_t LimeHAL_GetAppRunningIndex(void);
void LimeHAL_DelPhotoByPath(char* path);
void LimeHAL_CopyAllDataToSDCard(void);
void LimeHAL_FormatInternalStorage(void);
void LimeHAL_FormatExternalStorage(void);
uint8_t *LimeHAL_GetCopyAllDataToSDCardPercent(void);
uint8_t *LimeHAL_GetFormatInternalStoragePercent(void);
uint8_t *LimeHAL_GetFormatExternalStoragePercent(void);
void LimeHAL_VideoSetBuffer(uint8_t* buffer);
void LimeHAL_VideoAddedNewFrame(void);
void LimeHAL_SetReceiverName(char name[32]);
void LimeHAL_SetReceiverChannel(uint8_t channel);
void LimeHAL_SetBatteryInfo(int8_t batInfo);//-1(charging), 0(low), 1(mid), 2(high), 3(full)
void LimeHAL_SetTime(uint8_t hour, uint8_t minute, uint8_t second);
void LimeHAL_SetDate(uint16_t year, uint8_t month, uint8_t day);
bool LimeHAL_IsRtcShouldSave(void);
void LimeHAL_GetSettedTime(uint8_t* hour, uint8_t* minute, uint8_t* second);
void LimeHAL_GetSettedDate(uint16_t* year, uint8_t* month, uint8_t* day);
void LimeHAL_SetInitStep(uint8_t percent, const char* hardwareName);
void LimeHAL_SetSettedAutoReduceLightTimeAndAutoOffTime(uint16_t* reduceTime, uint16_t* offTime);
void LimeHAL_GetSettedAutoReduceLightTimeAndAutoOffTime(uint16_t* reduceTime, uint16_t* offTime);
void LimeHAL_TakePicture(void);
void LimeHAL_RockerCaliNeedRun(void);
uint8_t *LimeHAL_GetTakePicturePercent(void);

/*VS simulator API*/
void LimeHAL_SoftSimHardwareTimer_Init(void);
void LimeHAL_KeyPressed_Hook(uint8_t keyID);
void LimeHAL_SetAllKeyToReleased(void);

/*LVGL Interface API*/
void LimeHAL_AppRunInfo_runTake(bool isTake);	//0: give, 1:take
bool LimeHAL_IsAppFirmwareScheLock(void);
void LimeHAL_SyncAppRunStatus(int16_t index);

#endif // __Lime_App_Hal_H__
