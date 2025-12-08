#ifndef __LIME_RTC_TIME_H
#define __LIME_RTC_TIME_H

#include "main.h"




void LimeRtc_PrintNowTime(void);

void LimeRtc_SetNowTime(uint8_t hour, uint8_t minute, uint8_t second);

void LimeRtc_SetNowDate(uint16_t year, uint8_t month, uint8_t day);

void LimeRtc_GetNowDate(uint16_t *year, uint8_t *month, uint8_t *day);

void LimeRtc_GetNowTime(uint8_t *hour, uint8_t *minute, uint8_t *second);

uint8_t LimeRtc_CheckAndInitDefaultTime(void);


#endif	//__LIME_RTC_TIME_H