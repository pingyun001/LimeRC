#ifndef __NRF_PHOTO_H
#define __NRF_PHOTO_H

#include "main.h"
#include <stdbool.h>

void nrfPhoto_main(void const * argument);
void nrfPhoto_taskYIELD_exHook(bool isISR);

/* extern API */
void nrfPhotoTask_ReStart(uint8_t channel);
void nrfPhotoTask_Suspend(void);

bool nrfPhotoTask_IsSuspend(void);

#endif	//__NRF_PHOTO_H
