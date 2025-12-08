#ifndef __LIME_APP_SETTINGS_ITEMSTORAGE_H__
#define __LIME_APP_SETTINGS_ITEMSTORAGE_H__

#include <stdint.h>
#if USING_LIME_HARDWARE
#include "main.h"
#include "lvgl.h"
#else
#include "../main/src/main.h"
#endif

#include "Lime_App_Settings.h"

void Lime_App_Settings_ItemStorage_Create(lv_obj_t* father);
void Lime_App_Settings_ItemStorage_Destroy(void);
void Lime_App_Settings_ItemStorage_Update(Lime_App_KeyOptions_e keyOption);




#endif // __LIME_APP_SETTINGS_ITEMSTORAGE_H__
