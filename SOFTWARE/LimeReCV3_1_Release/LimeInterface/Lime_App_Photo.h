#ifndef _LIME_APP_PHOTO_H_
#define _LIME_APP_PHOTO_H_

#include <stdint.h>
#if USING_LIME_HARDWARE
#include "main.h"
#include "lvgl.h"
#else
#include "../main/src/main.h"
#endif

typedef enum
{
    typeUnknown = 0,
    typeGif,
    typeJpg,
}imgType_e;

void Photo_App_Create(lv_obj_t *father, uint16_t w, uint16_t h);
void Photo_App_Destroy(void);

#endif /* _LIME_APP_PHOTO_H_ */
