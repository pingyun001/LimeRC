#ifndef __NRF_MESSAGE_H
#define __NRF_MESSAGE_H

#include "main.h"

void nrfMessage_taskYIELD_exHook(bool isISR);

bool* nrfMessage_getAutoConnEnPin(void);
uint32_t* nrfMessage_getAutoConnUIDPin(void);

#endif	//__NRF_MESSAGE_H

