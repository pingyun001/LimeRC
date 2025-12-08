#ifndef __Lime_App_Settings_ItemReceiver_H__
#define __Lime_App_Settings_ItemReceiver_H__

#include <stdint.h>
#if USING_LIME_HARDWARE
#include "main.h"
#include "lvgl.h"
#else
#include "../main/src/main.h"
#endif

#include "Lime_App_Settings.h"

void Lime_App_Settings_ItemReceiver_Create(lv_obj_t* father);
void Lime_App_Settings_ItemReceiver_Destroy(void);
void Lime_App_Settings_ItemReceiver_Update(Lime_App_KeyOptions_e keyOption);


#endif // __Lime_App_Settings_ItemReceiver_H__
