#ifndef __LIME_APP_STARTFACE_H__
#define __LIME_APP_STARTFACE_H__

#include <stdint.h>
#if USING_LIME_HARDWARE
#include "main.h"
#include "lvgl.h"
#else
#include "../main/src/main.h"
#endif

void Lime_App_StartFace_Create(lv_obj_t* father, uint16_t width, uint16_t height);

void Lime_App_StartFace_Finish_Hook(void);

#endif // __LIME_APP_STARTFACE_H__
