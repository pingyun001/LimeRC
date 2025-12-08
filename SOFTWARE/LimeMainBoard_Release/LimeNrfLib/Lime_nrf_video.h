#ifndef __LIME_NRF_VIDEO_H
#define __LIME_NRF_VIDEO_H

#include "main.h"
#include "Lime_nrf_master.h"


typedef struct
{
	uint8_t* buf;
	uint8_t id;
	uint32_t length;
	uint32_t finish_time;
}video_pack_data_t;

HAL_StatusTypeDef Lime_nrf_video_Init(uint8_t channel, bool isReceiver);

HAL_StatusTypeDef Lime_nrf_video_SendData(uint8_t* srcBuf, uint32_t totalSizeToSend);

video_pack_data_t *Lime_nrf_video_GetAvaliableBuf(void);

void Lime_nrf_video_exti_irq_hook(void);
void Lime_nrf_video_spi_irq_hook(void);

#endif	//__LIME_NRF_VIDEO_H
