#include "Lime_vofaConn.h"
#include "stdio.h"
#include "usart.h"

int8_t Vofa_sendOneImegeOfJpeg(uint8_t *buff, uint32_t size)
{
	uint8_t imageHead[40] = {0};
	uint32_t i = 0;
	
	snprintf((char*)imageHead, 40, "\nimage:%d,%d,%d,%d,%d\n", 1, size, -1, -1, 27);//id, size, w, h, format
	
	while(imageHead[i] != '\0')
	{
		while (__HAL_UART_GET_FLAG(&huart5, UART_FLAG_TXE) == RESET);
		UART5->DR = imageHead[i];
		
		i ++;
		if(i >= 40)return 0;
	}
	
	for(i = 0; i < size; i++)
	{
		while (__HAL_UART_GET_FLAG(&huart5, UART_FLAG_TXE) == RESET);
		UART5->DR = buff[i];
	}
}




