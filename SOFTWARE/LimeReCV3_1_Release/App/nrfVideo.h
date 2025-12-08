#ifndef __NRF_VIDEO_APP_H
#define __NRF_VIDEO_APP_H

#include "main.h"


void nrfVideo_taskYIELD_exHook(bool isISR);

void nrfVideo_Stop(void);
void nrfVideo_Start(uint8_t channel);

#endif	//__NRF_VIDEO_APP_H
