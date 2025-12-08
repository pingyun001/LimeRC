#include "Lime_rtc_time.h"
#include "rtc.h"
#include "stdio.h"
#include "string.h"

#if 1
	#define DEBUG_LOG(...)	LEprintf(__VA_ARGS__)
#else
	#define DEBUG_LOG(...)
#endif

void LimeRtc_PrintNowTime(void)
{
	RTC_TimeTypeDef sTime = {0};
	RTC_DateTypeDef sDate = {0};
	HAL_RTC_GetTime(&hrtc, &sTime, RTC_FORMAT_BIN);
	HAL_RTC_GetDate(&hrtc, &sDate, RTC_FORMAT_BIN);
	
	LEprintf("%s():%d-%d-%d  %d-%d-%d\n", __FUNCTION__, (sDate.Year + 2000), sDate.Month, sDate.Date, sTime.Hours, sTime.Minutes, sTime.Seconds);
}


void LimeRtc_SetNowTime(uint8_t hour, uint8_t minute, uint8_t second)
{
	RTC_TimeTypeDef sTime = {0};
	
	sTime.Hours = hour;
	sTime.Minutes = minute;
	sTime.Seconds = second;
	
	HAL_RTC_SetTime(&hrtc, &sTime, RTC_FORMAT_BIN);
}

void LimeRtc_SetNowDate(uint16_t year, uint8_t month, uint8_t day)
{
	RTC_DateTypeDef sDate = {0};
	
	if(year > 2000)
		year -= 2000;
	else 
		return ;
	
	sDate.Year = year;
	sDate.Month = month;
	sDate.Date = day;
	
	HAL_RTC_SetDate(&hrtc, &sDate, RTC_FORMAT_BIN);
}

uint8_t LimeRtc_CheckAndInitDefaultTime(void)
{
	char compileDate[20] = __DATE__;
	char compileTime[20] = __TIME__;
	
	char compileYearStr[5] = {0};
	
	memcpy(compileYearStr, compileDate + strlen(compileDate) - 4, 4);
	
	uint32_t compileYear = 0;
	sscanf(compileYearStr, "%d", &compileYear);
	
	RTC_TimeTypeDef sTime = {0};
	RTC_DateTypeDef sDate = {0};
	HAL_RTC_GetTime(&hrtc, &sTime, RTC_FORMAT_BIN);
	HAL_RTC_GetDate(&hrtc, &sDate, RTC_FORMAT_BIN);
	
	uint32_t rtcYear = sDate.Year + 2000;
	
//	DEBUG_LOG("rtcYear:%d, CompileYear:%d\n", rtcYear, compileYear);
	
	if(rtcYear != compileYear)
	{
		DEBUG_LOG("SystemWasLosted RTC Power!!!\n");
		
		LimeRtc_SetNowTime(18,30,00);
		LimeRtc_SetNowDate(2025, 8, 28);
		
		return 1;
	}
	
	return 0;
}

void LimeRtc_GetNowDate(uint16_t *year, uint8_t *month, uint8_t *day)
{
	RTC_DateTypeDef sDate = {0};
	HAL_RTC_GetDate(&hrtc, &sDate, RTC_FORMAT_BIN);
	
	*year = sDate.Year + 2000;
	*month = sDate.Month;
	*day = sDate.Date;
}

void LimeRtc_GetNowTime(uint8_t *hour, uint8_t *minute, uint8_t *second)
{
	RTC_TimeTypeDef sTime = {0};
	HAL_RTC_GetTime(&hrtc, &sTime, RTC_FORMAT_BIN);
	
	*hour = sTime.Hours;
	*minute = sTime.Minutes;
	*second = sTime.Seconds;
}

