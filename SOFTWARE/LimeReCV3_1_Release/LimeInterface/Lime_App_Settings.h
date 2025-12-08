#ifndef __LIME_APP_SETTINGS_H__
#define __LIME_APP_SETTINGS_H__


#include <stdint.h>
#if USING_LIME_HARDWARE
#include "main.h"
#include "lvgl.h"
#else
#include "../main/src/main.h"
#endif

typedef enum
{
    Lime_App_KeyOptions_None = 0,
    Lime_App_KeyOptions_Up,
    Lime_App_KeyOptions_Down,
    Lime_App_KeyOptions_Left,
    Lime_App_KeyOptions_Right,
    Lime_App_KeyOptions_Set,
    Lime_App_KeyOptions_Return,
}Lime_App_KeyOptions_e;

#define LIME_APP_MAX_SETTING_NAME_LEN 32
#define LIME_APP_MAX_SETTING_ITEM_NUM 10

typedef struct
{
    char name[LIME_APP_MAX_SETTING_NAME_LEN];
    const lv_img_dsc_t* icon;

    void (*itemCreate)(lv_obj_t* father);
    void (*itemDestroy)(void);
    void (*itemUpdate)(Lime_App_KeyOptions_e keyOption);

    /* private use */
    lv_obj_t* itemObj;
    bool isSelected;

}Lime_App_Settings_Item_t;


/* public functions */
void Setting_App_Create(lv_obj_t* father, uint16_t width, uint16_t height);
void Setting_App_Destroy(void);

/* private functions */
static void Setting_App_AddItem(char *name,
                         const lv_img_dsc_t* icon,
                         void (*itemCreate)(lv_obj_t* father),
                         void (*itemDestroy)(void),
                         void (*itemUpdate)(Lime_App_KeyOptions_e keyOption));

#endif // __LIME_APP_SETTINGS_H__
