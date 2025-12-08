#ifndef LIME_LOCKFACE_H
#define LIME_LOCKFACE_H

#include <stdint.h>
#if USING_LIME_HARDWARE
#include "main.h"
#include "lvgl.h"
#else
#include "../main/src/main.h"
#endif


void Lime_LockFace_Init(lv_obj_t * father);

void Lime_LockFace_SetTime(uint8_t hour, uint8_t minute);

void Lime_LockFace_SetDate(uint32_t year, uint8_t month, uint8_t day);

void Lime_LockFace_Destroy(void);


void Lime_LockFace_ChangePosition(bool isUpped);

#endif /* LIME_LOCKFACE_H */
