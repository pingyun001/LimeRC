#ifndef __LIME_APP_CALIFACE_H__
#define __LIME_APP_CALIFACE_H__

#include <stdint.h>

#if USING_LIME_HARDWARE
#include "main.h"
#include "lvgl.h"
#else
#include "../main/src/main.h"
#endif

void CaliFace_App_Create(lv_obj_t* father, uint16_t w, uint16_t h);
void CaliFace_App_Destroy(void);

#endif // __LIME_APP_CALIFACE_H__
