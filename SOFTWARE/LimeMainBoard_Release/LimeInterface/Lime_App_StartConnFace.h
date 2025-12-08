#ifndef __LIME_APP_START_CONN_FACE_H
#define __LIME_APP_START_CONN_FACE_H

#include <stdint.h>
#if USING_LIME_HARDWARE
#include "main.h"
#include "lvgl.h"
#else
#include "../main/src/main.h"
#endif


void Connect_App_Create(lv_obj_t* father, uint16_t width, uint16_t height);
void Connect_App_Destroy(void);

void Lime_App_StartConnFace_Create(lv_obj_t* father, uint16_t width, uint16_t height);
void Lime_App_StartConnFace_Finish_Hook(void);


#endif//__LIME_APP_START_CONN_FACE_H
