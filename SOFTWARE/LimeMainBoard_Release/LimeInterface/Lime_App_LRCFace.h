#ifndef __LIME_APP_LRCFACE_H__
#define __LIME_APP_LRCFACE_H__

#include "Lime_App_Base.h"

#if USING_LIME_HARDWARE
#include "main.h"
#include "lvgl.h"
#else
#include "../main/src/main.h"
#endif

void LRC_AppFace_InitialCreate(lv_obj_t *father, uint16_t width, uint16_t height);    //if father is NULL, means first create.
void LRC_AppFace_Create(lv_obj_t *father, uint16_t w, uint16_t h);
void LRC_AppFace_Destroy(void);

void Lime_App_LRCFace_Finish_Hook(void);

#endif // __LIME_APP_LRCFACE_H__
