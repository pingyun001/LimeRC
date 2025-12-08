#ifndef __LVM_MAIN_FACE_H__
#define __LVM_MAIN_FACE_H__



#include <stdint.h>
#if USING_LIME_HARDWARE
#include "main.h"
#include "lvgl.h"
#else
#include "../main/src/main.h"
#endif


void Lime_LvMainFace_Init(void);

void Lime_MainFace_SetSwitch(uint8_t state);


#endif //__LVM_MAIN_FACE_H__
