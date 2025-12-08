#ifndef __LIME_HEADBAR_H
#define __LIME_HEADBAR_H

#include <stdint.h>
#if USING_LIME_HARDWARE
#include "main.h"
#include "lvgl.h"
#else
#include "../main/src/main.h"
#endif

void Lime_HeadBar_Create(lv_obj_t *parent);

void Lime_HeadBar_SetBatteryLevel(int8_t level);
void Lime_HeadBar_SetConnectionStatus(int8_t connLevel);
void Lime_HeadBar_SetTime(uint8_t hour, uint8_t minute);

#endif /* __LIME_HEADBAR_H */
