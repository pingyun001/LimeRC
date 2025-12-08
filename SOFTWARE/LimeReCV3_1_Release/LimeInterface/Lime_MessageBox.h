#ifndef __LIME_MESSAGE_BOX_H__
#define __LIME_MESSAGE_BOX_H__

#include <stdint.h>

#if USING_LIME_HARDWARE
#include "main.h"
#include "lvgl.h"
#else
#include "../main/src/main.h"
#endif

void Lime_MessageBox_Show(const char* title, const char* message, uint8_t *tracePercent, uint32_t timeOut_ms);
void Lime_MessageBox_Close(void);

#endif // __LIME_MESSAGE_BOX_H__
