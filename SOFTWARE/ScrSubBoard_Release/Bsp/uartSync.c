#include "uartSync.h"
#include "usart.h"
#include "crc.h"
#include <string.h>
#include <stdio.h>


HAL_StatusTypeDef uartSync_Handle(const KeyInfo_t* keyInfo, const uint16_t adcData[4])
{
	static uint8_t sendBuf[32] = {0};
	
	uint16_t adcDataCache[4] = {0};
	adcDataCache[0] = 4096 - adcData[0];
	adcDataCache[1] =  adcData[1];
	adcDataCache[2] =  adcData[2];
	adcDataCache[3] = 4096 - adcData[3];
	
	sendBuf[0] = 'L';
	sendBuf[1] = 'R';
	memcpy(sendBuf + 2, keyInfo, 20);
	memcpy(sendBuf + 22, adcDataCache, 8);
	sendBuf[30] = HAL_CRC_Calculate(&hcrc, (uint32_t*)sendBuf, 30);
	sendBuf[31] = '\n';
	
	return HAL_UART_Transmit_DMA(&huart1, sendBuf, sizeof(sendBuf));
}

HAL_StatusTypeDef uartSync_VisiableHandle(const KeyInfo_t* keyInfo, const uint16_t adcData[4])
{
	static uint8_t sendBuf[50] = {0};
	uint16_t length = 0;
	
	length = snprintf((char*)sendBuf, sizeof(sendBuf), "fiveKey:up:%d,down:%d,l:%d,r:%d,s:%d,", keyInfo->sw_up, keyInfo->sw_down, keyInfo->sw_left, keyInfo->sw_right, keyInfo->sw_set);
	HAL_UART_Transmit(&huart1,sendBuf, length, 100);
	
	length = snprintf((char*)sendBuf, sizeof(sendBuf), "upperKey1:%d,2:%d,3:%d,4:%d,5:%d,6:%d,", keyInfo->l_down, keyInfo->l_mid, keyInfo->l_up, keyInfo->r_down, keyInfo->r_mid, keyInfo->r_up);
	HAL_UART_Transmit(&huart1,sendBuf, length, 100);
	
	length = snprintf((char*)sendBuf, sizeof(sendBuf), "joyL:%d,R:%d,", keyInfo->joy_l, keyInfo->joy_r);
	HAL_UART_Transmit(&huart1,sendBuf, length, 100);
	
	length = snprintf((char*)sendBuf, sizeof(sendBuf), "Analog:%df, %df, %df, %df,", adcData[0], adcData[1], adcData[2], adcData[3]);
	HAL_UART_Transmit(&huart1,sendBuf, length, 100);
	
	length = snprintf((char*)sendBuf, sizeof(sendBuf), "HardwareVersion:%d,", keyInfo->reversion);
	HAL_UART_Transmit(&huart1,sendBuf, length, 100);
	
	length = snprintf((char*)sendBuf, sizeof(sendBuf), "ScanTime:%d,", keyInfo->refreshTime);
	HAL_UART_Transmit(&huart1,sendBuf, length, 100);
	
	length = snprintf((char*)sendBuf, sizeof(sendBuf), "End\n");
	HAL_UART_Transmit(&huart1,sendBuf, length, 100);
	
	return HAL_OK;
}


