#ifndef LIMESIM_FIVE_KEY_H
#define LIMESIM_FIVE_KEY_H

#include <stdint.h>
#if USING_LIME_HARDWARE
#include "main.h"
#include "lvgl.h"
#else
#include "../main/src/main.h"
#endif

void Lime_SimFiveKey_Init(lv_obj_t *father);
void Lime_SimFiveKey_Destroy(void);

uint8_t Lime_SimFiveKey_GetNowPressingKey(void);

#endif /* LIMESIM_FIVE_KEY_H */
