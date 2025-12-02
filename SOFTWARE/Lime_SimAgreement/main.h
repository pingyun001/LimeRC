#ifndef _MAIN_H_
#define _MAIN_H_

#include <stdint.h>
#include <stdbool.h>

/* ST HAL Functions */
typedef enum 
{
  HAL_OK       = 0x00U,
  HAL_ERROR    = 0x01U,
  HAL_BUSY     = 0x02U,
  HAL_TIMEOUT  = 0x03U
} HAL_StatusTypeDef;

#define HAL_UART_Init(x) HAL_OK
#define HAL_UART_DeInit(x) HAL_OK
#define HAL_UART_Transmit(x, y, z, w) HAL_OK
#define HAL_UART_Transmit_IT(x, y, z) HAL_OK
#define HAL_GetTick() 0

#endif /* _MAIN_H_ */
