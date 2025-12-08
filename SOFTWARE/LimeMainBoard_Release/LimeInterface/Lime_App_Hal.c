#include "Lime_App_Hal.h"
#include <stdio.h>
#include <string.h>

LimeHal_Info_t LimeHal_Info =
{
		.keyInfo.s_l = 2,
		.keyInfo.s_r = 2,

//    .settingInfo.day = 15,
//    .settingInfo.month = 9,
//    .settingInfo.year = 2025,
//    .settingInfo.time = {12, 30, 0},

//    .settingInfo.storage_FlashTotalMB = 28.2f,
//    .settingInfo.storage_FlashUsedPicMB = 1.2f,
//    .settingInfo.storage_FlashUsedOthersMB = 0.5f,
//    .settingInfo.storage_FlashUsedLimeOSMB = 3.2f,
//    .settingInfo.storage_SDCardTotalMB = 0 * 1024.0f,
//    .settingInfo.storage_SDCardUsedPicMB = 0,
//    .settingInfo.storage_SDCardUsedOthersMB = 0 * 1024.0f,
};

/*common API*/
LimeHal_Info_t *LimeHAL_GetInfoPin(void)
{
    return &LimeHal_Info;
}

int32_t absX(int32_t x)
{
    return x < 0? -x : x;
}

float fmap(float x, float in_min, float in_max, float out_min, float out_max)
{
    return (x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
}

float fmapWithLimit(float x, float in_min, float in_max, float out_min, float out_max)
{
    float returnVal = (x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;

    if(out_min > out_max)
    {
        float temp = out_min;
        out_min = out_max;
        out_max = temp;
    }

    returnVal = (returnVal < out_min)? out_min : returnVal;
    returnVal = (returnVal > out_max)? out_max : returnVal;

    return returnVal;
}

uint8_t Lime_CalDayInMonth(uint32_t year, uint8_t month)
{
    const uint8_t days_per_month[] = {
        31, 28, 31, 30, 31, 30,
        31, 31, 30, 31, 30, 31
    };

    if (month < 1 || month > 12)
        return 0;

    if (month == 2) {
        if ((year % 400 == 0) || (year % 4 == 0 && year % 100 != 0)) {
            return 29;
        }
        return 28;
    }

    return days_per_month[month - 1];
}

int16_t LimeHAL_GetAppRunningIndex(void)
{
    if(LimeHal_Info.appRunInfo.appLoarderFirmwareIsRunning == false)
        return -1;

    return LimeHal_Info.appRunInfo.appRunningIndex;
}

void LimeHAL_DelPhotoByPath(char* path)
{
    if(LimeHal_Info.photoFaceInfo.isPhotoNeedDelete)
    {
        LV_LOG_USER("Photo is already in delete list");
        return;
    }

    LimeHal_Info.photoFaceInfo.isPhotoNeedDelete = true;
    memset(LimeHal_Info.photoFaceInfo.deletePhotoPath, 0, sizeof(LimeHal_Info.photoFaceInfo.deletePhotoPath));
    snprintf(LimeHal_Info.photoFaceInfo.deletePhotoPath, sizeof(LimeHal_Info.photoFaceInfo.deletePhotoPath), "%s", path);

    LV_LOG_USER("Delete photo by path:%s", path);
}

void LimeHAL_CopyAllDataToSDCard(void)
{
		LimeHal_Info.settingInfo.storage_IsFlashDataNeedOutput = true;
		LimeHal_Info.settingInfo.storage_FlashOutputPercent = 0;
    LV_LOG_USER("Copy all data to SD card");
}
void LimeHAL_FormatInternalStorage(void)
{
		LimeHal_Info.settingInfo.storage_IsInternalStorageNeedClean = true;
		LimeHal_Info.settingInfo.storage_InternalStorageCleanPercent = 0;
    LV_LOG_USER("Format internal storage");
}
void LimeHAL_FormatExternalStorage(void)
{
		LimeHal_Info.settingInfo.storage_IsExternalStorageNeedClean = true;
		LimeHal_Info.settingInfo.storage_ExternalStorageCleanPercent = 0;
    LV_LOG_USER("Format external storage");
}
uint8_t *LimeHAL_GetCopyAllDataToSDCardPercent(void)
{
	return &LimeHal_Info.settingInfo.storage_FlashOutputPercent;
}
uint8_t *LimeHAL_GetFormatInternalStoragePercent(void)
{
	return &LimeHal_Info.settingInfo.storage_InternalStorageCleanPercent;
}
uint8_t *LimeHAL_GetFormatExternalStoragePercent(void)
{
	return &LimeHal_Info.settingInfo.storage_ExternalStorageCleanPercent;
}

void LimeHAL_VideoSetBuffer(uint8_t* buffer)
{
	LimeHal_Info.lrfaceInfo.videoFrameBuf = buffer;
}
void LimeHAL_VideoAddedNewFrame(void)
{
	LimeHal_Info.lrfaceInfo.videoFrameCnt ++;
}

void LimeHAL_SetReceiverName(char name[32])
{
	uint16_t tarNameLen = strlen(name);
	tarNameLen = tarNameLen > 31 ? 31 : tarNameLen;
	memset(LimeHal_Info.lrfaceInfo.receiverName, 0, 32);
	memcpy(LimeHal_Info.lrfaceInfo.receiverName, name, tarNameLen);
}
void LimeHAL_SetReceiverChannel(uint8_t channel)
{
	LimeHal_Info.lrfaceInfo.receiverChannel = channel;
}
void LimeHAL_SetBatteryInfo(int8_t batInfo) //-1(charging), 0(low), 1(mid), 2(high), 3(full)
{
    LimeHal_Info.lrfaceInfo.rcBatInfo = batInfo;
    LimeHal_Info.mainFaceInfo.rcBatInfo = batInfo;
}
void LimeHAL_SetTime(uint8_t hour, uint8_t minute, uint8_t second)
{
    LimeHal_Info.mainFaceInfo.time[0] = hour;
    LimeHal_Info.mainFaceInfo.time[1] = minute;
    LimeHal_Info.mainFaceInfo.time[2] = second;

    LimeHal_Info.lrfaceInfo.time[0] = hour;
    LimeHal_Info.lrfaceInfo.time[1] = minute;
    LimeHal_Info.lrfaceInfo.time[2] = second;
}
void LimeHAL_SetDate(uint16_t year, uint8_t month, uint8_t day)
{
    LimeHal_Info.mainFaceInfo.year = year;
    LimeHal_Info.mainFaceInfo.month = month;
    LimeHal_Info.mainFaceInfo.day = day;
}
bool LimeHAL_IsRtcShouldSave(void)
{
    if(LimeHal_Info.settingInfo.isRtcNeedSave)
    {
        LimeHal_Info.settingInfo.isRtcNeedSave = false;
        return true;
    }

    return false;
}
void LimeHAL_GetSettedTime(uint8_t* hour, uint8_t* minute, uint8_t* second)
{
    *hour = LimeHal_Info.settingInfo.time[0];
    *minute = LimeHal_Info.settingInfo.time[1];
    *second = LimeHal_Info.settingInfo.time[2];
}
void LimeHAL_GetSettedDate(uint16_t* year, uint8_t* month, uint8_t* day)
{
    *year = LimeHal_Info.settingInfo.year;
    *month = LimeHal_Info.settingInfo.month;
    *day = LimeHal_Info.settingInfo.day;
}
void LimeHAL_SetInitStep(uint8_t percent, const char* hardwareName)
{
    LimeHal_Info.onlineInfo.initPercent = percent;
    memset(LimeHal_Info.onlineInfo.hardwareName, 0, sizeof(LimeHal_Info.onlineInfo.hardwareName));
    snprintf(LimeHal_Info.onlineInfo.hardwareName, sizeof(LimeHal_Info.onlineInfo.hardwareName), "%s", hardwareName);
    LimeHal_Info.onlineInfo.allHardwarePass = percent == 100 ? 1 : 0;
}
void LimeHAL_SetSettedAutoReduceLightTimeAndAutoOffTime(uint16_t* reduceTime, uint16_t* offTime)
{
    LimeHal_Info.settingInfo.screen_AutoReduceLightTime = *reduceTime;
    LimeHal_Info.settingInfo.screen_AutoOffTime = *offTime;
}
void LimeHAL_GetSettedAutoReduceLightTimeAndAutoOffTime(uint16_t* reduceTime, uint16_t* offTime)
{
    *reduceTime = LimeHal_Info.settingInfo.screen_AutoReduceLightTime;
    *offTime = LimeHal_Info.settingInfo.screen_AutoOffTime;
}
void LimeHAL_TakePicture(void)
{
    LimeHal_Info.lrfaceInfo.needTakePicture = true;
    LimeHal_Info.lrfaceInfo.takePicturePercent = 0;
}
uint8_t *LimeHAL_GetTakePicturePercent(void)
{
    return &LimeHal_Info.lrfaceInfo.takePicturePercent;
}
void LimeHAL_RockerCaliNeedRun(void)
{
    LimeHal_Info.caliAppInfo.isCaliNeedRun = true;
    LimeHal_Info.caliAppInfo.isCaliFinished = false;
}

void LimeHAL_SelectNRF_To_Connect(uint16_t index)
{
    if(index >= LimeHal_Info.nrfInfo.totalNum)
    {
        LV_LOG_ERROR("NRF index out of range:%d", index);
        return;
    }

    LimeHal_Info.nrfInfo.w_wantUsedIndex = index;
    LimeHal_Info.nrfInfo.w_startConn = 1;
}

bool LimeHAL_IsAnyMenuKeyPressed(void)
{
    if(LimeHal_Info.keyInfo.sw_up % 2)
        return true;
    if(LimeHal_Info.keyInfo.sw_set % 2)
        return true;
    if(LimeHal_Info.keyInfo.sw_left % 2)
        return true;
    if(LimeHal_Info.keyInfo.sw_right % 2)
        return true;
    if(LimeHal_Info.keyInfo.sw_down % 2)
        return true;
    if(LimeHal_Info.keyInfo.kr % 2)
        return true;

    return false;
}

/*VS simulator API*/
static void timer_cb(lv_timer_t * timer);

void LimeHAL_KeyPressed_Hook(uint8_t keyID)
{
    switch(keyID)
    {
        case 0:
            LimeHal_Info.keyInfo.sw_up ++;
            break;
        case 1:
            LimeHal_Info.keyInfo.sw_set ++;
            break;
        case 2:
            LimeHal_Info.keyInfo.sw_left ++;
            break;
        case 3:
            LimeHal_Info.keyInfo.sw_right ++;
            break;
        case 4:
            LimeHal_Info.keyInfo.sw_down ++;
            break;
        case 5:
            LimeHal_Info.keyInfo.kr ++;
            break;
        default:
            LV_LOG_WARN("Unknown keyID:%d", keyID);
    }

    // LV_LOG_USER("sw_up:%d, sw_set:%d, sw_left:%d, sw_right:%d, sw_down:%d", LimeHal_Info.keyInfo.sw_up, LimeHal_Info.keyInfo.sw_set, LimeHal_Info.keyInfo.sw_left, LimeHal_Info.keyInfo.sw_right, LimeHal_Info.keyInfo.sw_down);
}

void LimeHAL_SetAllKeyToReleased(void)
{
    LimeHal_Info.keyInfo.sw_up = 0;
    LimeHal_Info.keyInfo.sw_set = 0;
    LimeHal_Info.keyInfo.sw_left = 0;
    LimeHal_Info.keyInfo.sw_right = 0;
    LimeHal_Info.keyInfo.sw_down = 0;
    LimeHal_Info.keyInfo.kr = 0;
}

void LimeHAL_SoftSimHardwareTimer_Init(void)
{
    lv_timer_t * timer = lv_timer_create(timer_cb, 500, NULL);
}

void LimeHAL_AppRunInfo_appFirmwareIsRunning(bool isRunning)
{
    LimeHal_Info.appRunInfo.appLoarderFirmwareIsRunning = isRunning;
}

void LimeHAL_AppRunInfo_runTake(bool isTake)	//0: give, 1:take
{
    LimeHal_Info.appRunInfo.appFirmwareScheLock = isTake;
}

bool LimeHAL_IsAppFirmwareScheLock(void)
{
    return LimeHal_Info.appRunInfo.appFirmwareScheLock;
}

void LimeHAL_SyncAppRunStatus(int16_t index)
{
    LimeHal_Info.appRunInfo.appLoarderFirmwareIsRunning = true;
    LimeHal_Info.appRunInfo.appRunningIndex = index;
}

/*typedef struct {
    uint8_t keySubBoard;
    uint8_t eeprom;
    uint8_t qflash;
    uint8_t nrfVideo;
    uint8_t nrfMessage;

}LimeHAL_Hardware_Online_Info_t;*/

static void timer_cb(lv_timer_t * timer)
{
    static uint32_t totalRunCnt = 0;
    static uint32_t mainLRCFaceCnt = 0;

    totalRunCnt++;

    // LimeHal_Info.lrfaceInfo.messaRssi = mainLRCFaceCnt;
    // LimeHal_Info.lrfaceInfo.videoRssi = mainLRCFaceCnt;
    // LimeHal_Info.lrfaceInfo.rcBatInfo = mainLRCFaceCnt - 1;

    // LimeHal_Info.lrfaceInfo.time[0] = totalRunCnt;
    // LimeHal_Info.lrfaceInfo.time[1] = totalRunCnt + 4;

    LimeHal_Info.mainFaceInfo.messaRssi = totalRunCnt % 4;

//    LimeHal_Info.lrfaceInfo.receiverBatVal = totalRunCnt * 1.1f;
//    snprintf(LimeHal_Info.lrfaceInfo.receiverName, 32, "LRRLRLRRRRLR1234");

    mainLRCFaceCnt ++;

    if(mainLRCFaceCnt > 3)
    {
        mainLRCFaceCnt = 0;
    }

    /* test cali app */
    if(totalRunCnt == 5)
    {
        LimeHal_Info.caliAppInfo.isCaliFinished = true;
    }
    LimeHal_Info.caliAppInfo.channelPos[0] = -totalRunCnt;
    LimeHal_Info.caliAppInfo.channelPos[1] = totalRunCnt;
    LimeHal_Info.caliAppInfo.channelPos[2] = 25;
    LimeHal_Info.caliAppInfo.channelPos[3] = -100;
    LimeHal_Info.caliAppInfo.channelPercent[0][0] = 5;
    LimeHal_Info.caliAppInfo.channelPercent[0][1] = 22;
    LimeHal_Info.caliAppInfo.channelPercent[1][0] = 34;
    LimeHal_Info.caliAppInfo.channelPercent[1][1] = totalRunCnt;
    LimeHal_Info.caliAppInfo.channelPercent[2][0] = 78;
    LimeHal_Info.caliAppInfo.channelPercent[2][1] = 90;
    LimeHal_Info.caliAppInfo.channelPercent[3][0] = 96;
    LimeHal_Info.caliAppInfo.channelPercent[3][1] = 100;



    /*sim Hardware Init sequence*/
    if(totalRunCnt == 1)
    {
        LimeHal_Info.onlineInfo.initPercent = 20;
        memset(LimeHal_Info.onlineInfo.hardwareName, 0, sizeof(LimeHal_Info.onlineInfo.hardwareName));
        snprintf(LimeHal_Info.onlineInfo.hardwareName, sizeof(LimeHal_Info.onlineInfo.hardwareName), "KeySubBoard");
    }
    if(totalRunCnt == 2)
    {
        LimeHal_Info.onlineInfo.initPercent = 30;
        memset(LimeHal_Info.onlineInfo.hardwareName, 0, sizeof(LimeHal_Info.onlineInfo.hardwareName));
        snprintf(LimeHal_Info.onlineInfo.hardwareName, sizeof(LimeHal_Info.onlineInfo.hardwareName), "EEPROM");
    }
    if(totalRunCnt == 3)
    {
        LimeHal_Info.onlineInfo.initPercent = 50;
        memset(LimeHal_Info.onlineInfo.hardwareName, 0, sizeof(LimeHal_Info.onlineInfo.hardwareName));
        snprintf(LimeHal_Info.onlineInfo.hardwareName, sizeof(LimeHal_Info.onlineInfo.hardwareName), "QFlash");
    }
    if(totalRunCnt == 4)
    {
        LimeHal_Info.onlineInfo.initPercent = 75;
        memset(LimeHal_Info.onlineInfo.hardwareName, 0, sizeof(LimeHal_Info.onlineInfo.hardwareName));
        snprintf(LimeHal_Info.onlineInfo.hardwareName, sizeof(LimeHal_Info.onlineInfo.hardwareName), "NRF Video");
    }
    if(totalRunCnt == 5)
    {
        LimeHal_Info.onlineInfo.initPercent = 90;
        memset(LimeHal_Info.onlineInfo.hardwareName, 0, sizeof(LimeHal_Info.onlineInfo.hardwareName));
        snprintf(LimeHal_Info.onlineInfo.hardwareName, sizeof(LimeHal_Info.onlineInfo.hardwareName), "NRF Message");
    }
    if(totalRunCnt == 6)
    {
        LimeHal_Info.onlineInfo.initPercent = 100;
        memset(LimeHal_Info.onlineInfo.hardwareName, 0, sizeof(LimeHal_Info.onlineInfo.hardwareName));
        snprintf(LimeHal_Info.onlineInfo.hardwareName, sizeof(LimeHal_Info.onlineInfo.hardwareName), "Self-Test Success");
    }
    if(totalRunCnt == 7)
    {
        LV_LOG_USER("All Hardware Online");
        LimeHal_Info.onlineInfo.allHardwarePass = 1;
    }

    /*sim nrf scan sequence*/
   if(totalRunCnt == 10)
   {
       LV_LOG_USER("NRF Scan Start");
   }
   if(totalRunCnt == 11)
   {
       snprintf(LimeHal_Info.nrfInfo.itemInfo[0].name, sizeof(LimeHal_Info.nrfInfo.itemInfo->name), "CF:2D:05:30:E4");
       LimeHal_Info.nrfInfo.itemInfo[0].rssi = 2;
       LimeHal_Info.nrfInfo.itemInfo[0].status = LimeHAL_NRF_Status_Enabled | LimeHAL_NRF_Status_HistoryUsed | LimeHAL_NRF_Status_IsOnline;
       LimeHal_Info.nrfInfo.totalNum = 1;
       LV_LOG_USER("SoftSim:NRF[%d] added, name:%s, rssi:%d, status:%d", 0, LimeHal_Info.nrfInfo.itemInfo[0].name, LimeHal_Info.nrfInfo.itemInfo[0].rssi, LimeHal_Info.nrfInfo.itemInfo[0].status);
   }
   if(totalRunCnt == 12)
   {
       snprintf(LimeHal_Info.nrfInfo.itemInfo[1].name, sizeof(LimeHal_Info.nrfInfo.itemInfo->name), "23:6C:08:23:A4");
       LimeHal_Info.nrfInfo.itemInfo[1].rssi = 0;
       LimeHal_Info.nrfInfo.itemInfo[1].status = LimeHAL_NRF_Status_Enabled | LimeHAL_NRF_Status_HistoryUsed;
       LimeHal_Info.nrfInfo.totalNum = 2;
       LV_LOG_USER("SoftSim:NRF[%d] added, name:%s, rssi:%d, status:%d", 1, LimeHal_Info.nrfInfo.itemInfo[1].name, LimeHal_Info.nrfInfo.itemInfo[1].rssi, LimeHal_Info.nrfInfo.itemInfo[1].status);
   }
   if(totalRunCnt == 13)
   {
       snprintf(LimeHal_Info.nrfInfo.itemInfo[2].name, sizeof(LimeHal_Info.nrfInfo.itemInfo->name), "26:CE:44:12:30");
       LimeHal_Info.nrfInfo.itemInfo[2].rssi = 1;
       LimeHal_Info.nrfInfo.itemInfo[2].status = LimeHAL_NRF_Status_Enabled | LimeHAL_NRF_Status_IsOnline;
       LimeHal_Info.nrfInfo.totalNum = 3;
       LV_LOG_USER("SoftSim:NRF[%d] added, name:%s, rssi:%d, status:%d", 2, LimeHal_Info.nrfInfo.itemInfo[2].name, LimeHal_Info.nrfInfo.itemInfo[2].rssi, LimeHal_Info.nrfInfo.itemInfo[2].status);

       snprintf(LimeHal_Info.nrfInfo.itemInfo[3].name, sizeof(LimeHal_Info.nrfInfo.itemInfo->name), "67:55:33:22:11");
       LimeHal_Info.nrfInfo.itemInfo[3].rssi = 1;
       LimeHal_Info.nrfInfo.itemInfo[3].status = LimeHAL_NRF_Status_Enabled | LimeHAL_NRF_Status_IsOnline;
       LimeHal_Info.nrfInfo.totalNum = 4;
       LV_LOG_USER("SoftSim:NRF[%d] added, name:%s, rssi:%d, status:%d", 2, LimeHal_Info.nrfInfo.itemInfo[3].name, LimeHal_Info.nrfInfo.itemInfo[3].rssi, LimeHal_Info.nrfInfo.itemInfo[3].status);

       // snprintf(LimeHal_Info.nrfInfo.itemInfo[4].name, sizeof(LimeHal_Info.nrfInfo.itemInfo->name), "aa:cc:dd:43:65");
       // LimeHal_Info.nrfInfo.itemInfo[4].rssi = 1;
       // LimeHal_Info.nrfInfo.itemInfo[4].status = LimeHAL_NRF_Status_Enabled | LimeHAL_NRF_Status_IsOnline;
       // LimeHal_Info.nrfInfo.totalNum = 5;
       // LV_LOG_USER("SoftSim:NRF[%d] added, name:%s, rssi:%d, status:%d", 2, LimeHal_Info.nrfInfo.itemInfo[4].name, LimeHal_Info.nrfInfo.itemInfo[4].rssi, LimeHal_Info.nrfInfo.itemInfo[4].status);
   }
   if(totalRunCnt >= 13)
   {
       static uint32_t startCnt = 0;
       static uint16_t w_startConn_LastVal = 0;

       if((LimeHal_Info.nrfInfo.w_startConn) && (w_startConn_LastVal == 0))
       {
           startCnt = totalRunCnt;
       }

       if(LimeHal_Info.nrfInfo.w_startConn)
       {
           LV_LOG_USER("SoftSim: NRF Connect Start, index:%d", LimeHal_Info.nrfInfo.w_wantUsedIndex);

           /* unconnect */
           if(LimeHal_Info.nrfInfo.itemInfo[LimeHal_Info.nrfInfo.w_wantUsedIndex].status & LimeHAL_NRF_Status_Connected)
           {
               LimeHal_Info.nrfInfo.itemInfo[LimeHal_Info.nrfInfo.w_wantUsedIndex].status &=~ (LimeHAL_NRF_Status_ConnFailed | LimeHAL_NRF_Status_Connected | LimeHAL_NRF_Status_Connecting);
               LimeHal_Info.nrfInfo.w_startConn = 0;

               goto connectEnd;
           }

           /* simulate conn finish */
           if(totalRunCnt - startCnt >= 2)
           {
               LimeHal_Info.nrfInfo.itemInfo[LimeHal_Info.nrfInfo.w_wantUsedIndex].status &=~ (LimeHAL_NRF_Status_ConnFailed | LimeHAL_NRF_Status_Connected | LimeHAL_NRF_Status_Connecting);

               if(LimeHal_Info.nrfInfo.w_wantUsedIndex == 1)
               {
                   LimeHal_Info.nrfInfo.itemInfo[LimeHal_Info.nrfInfo.w_wantUsedIndex].status |= LimeHAL_NRF_Status_ConnFailed;
               }
               else
               {
                   LimeHal_Info.nrfInfo.itemInfo[LimeHal_Info.nrfInfo.w_wantUsedIndex].status |= LimeHAL_NRF_Status_Connected;
                   LimeHal_Info.nrfInfo.nowUsedIndex = LimeHal_Info.nrfInfo.w_wantUsedIndex;
               }

               LimeHal_Info.nrfInfo.w_startConn = 0;
           }
           else
           {
               LimeHal_Info.nrfInfo.itemInfo[LimeHal_Info.nrfInfo.w_wantUsedIndex].status &=~ (LimeHAL_NRF_Status_ConnFailed | LimeHAL_NRF_Status_Connected | LimeHAL_NRF_Status_Connecting);
               LimeHal_Info.nrfInfo.itemInfo[LimeHal_Info.nrfInfo.w_wantUsedIndex].status |= LimeHAL_NRF_Status_Connecting;
           }
           LV_LOG_USER("SoftSim: NRF Connect Success, index:%d", LimeHal_Info.nrfInfo.w_wantUsedIndex);
       }

connectEnd:
       w_startConn_LastVal = LimeHal_Info.nrfInfo.w_startConn;
   }

	 /* sim file copy */
	 if(totalRunCnt >= 13)
	 {
		 if(LimeHal_Info.settingInfo.storage_IsFlashDataNeedOutput)
		 {
			 LimeHal_Info.settingInfo.storage_IsFlashDataNeedOutput = false;
			 LimeHal_Info.settingInfo.storage_FlashOutputPercent = 100;
		 }
		 if(LimeHal_Info.settingInfo.storage_IsInternalStorageNeedClean)
		 {
			 LimeHal_Info.settingInfo.storage_IsInternalStorageNeedClean = false;
			 LimeHal_Info.settingInfo.storage_InternalStorageCleanPercent = 100;
		 }
		 if(LimeHal_Info.settingInfo.storage_IsExternalStorageNeedClean)
		 {
			 LimeHal_Info.settingInfo.storage_IsExternalStorageNeedClean = false;
			 LimeHal_Info.settingInfo.storage_ExternalStorageCleanPercent = 100;
		 }
	 }

    /* sim take picture */
    {
        if(LimeHal_Info.lrfaceInfo.needTakePicture)
        {
            LimeHal_Info.lrfaceInfo.needTakePicture = false;
            LimeHal_Info.lrfaceInfo.takePicturePercent = 100;
            LV_LOG_USER("SoftSim: Take Picture Success");
        }
    }

    if(totalRunCnt % 2)
    {
        LimeHal_Info.settingInfo.storage_SDCardTotalMB = 0 * 1024.0f;
        LimeHal_Info.settingInfo.storage_SDCardUsedPicMB = 0;
        LimeHal_Info.settingInfo.storage_SDCardUsedOthersMB = 0 * 1024.0f;
    }
    else
    {
        LimeHal_Info.settingInfo.storage_SDCardTotalMB = 32 * 1024.0f;
        LimeHal_Info.settingInfo.storage_SDCardUsedPicMB = 12.3;
        LimeHal_Info.settingInfo.storage_SDCardUsedOthersMB = 2 * 1024.0f;
    }

}

bool LimeHAL_NRF_Has_ConnectedItem(void)
{
    uint16_t index = LimeHal_Info.nrfInfo.nowUsedIndex;
    if(LimeHal_Info.nrfInfo.itemInfo[index].status & LimeHAL_NRF_Status_Connected)
        return true;

    return false;
}
