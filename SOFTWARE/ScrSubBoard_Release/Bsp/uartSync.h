#ifndef __UART_SYNC_H
#define __UART_SYNC_H

#include "main.h"
#include "key.h"

HAL_StatusTypeDef uartSync_Handle(const KeyInfo_t* keyInfo, const uint16_t adcData[4]);

HAL_StatusTypeDef uartSync_VisiableHandle(const KeyInfo_t* keyInfo, const uint16_t adcData[4]);


#endif	//__UART_SYNC_H
