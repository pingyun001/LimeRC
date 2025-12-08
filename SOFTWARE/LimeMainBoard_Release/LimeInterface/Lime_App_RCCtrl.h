#ifndef __LIME_APP_RCCtrl_H__
#define __LIME_APP_RCCtrl_H__

#include <stdint.h>

#if USING_LIME_HARDWARE
#include "main.h"
#include "lvgl.h"
#else
#include "../main/src/main.h"
#endif

void RCCtrl_App_Create(lv_obj_t* father, uint16_t w, uint16_t h);
void RCCtrl_App_Destroy(void);

#endif //__LIME_APP_RCCtrl_H__
