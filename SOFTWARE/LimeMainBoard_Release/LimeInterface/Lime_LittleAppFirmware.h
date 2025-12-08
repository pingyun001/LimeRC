#ifndef LITTLE_APP_FIRMWARE_H
#define LITTLE_APP_FIRMWARE_H

#include <stdint.h>

#if USING_LIME_HARDWARE
#include "main.h"
#include "lvgl.h"
#else
#include "../main/src/main.h"
#endif

/*
note:
1.The app icon size is 100x100, and do not change it.
*/

#define MH_APP_MAX_NUM              10          //The MAX num of app.
#define MH_APP_MAX_LENGTH_NAME      15          //The MAX length of app name.

typedef struct {
    char name[MH_APP_MAX_LENGTH_NAME];          //(user)The string of app-name.
    const lv_img_dsc_t* iconDst;                //(user)IconObjDst
                                                //(user)The function of app-loader,this function will be called when open animation is done.
    void (*app_Create)(lv_obj_t* father, uint16_t w, uint16_t h);
    void (*app_Destroy)(void);                  //(user)The function of app-destroy,this function will be called when close animation is started.

    uint8_t appIsDrawn;                         //(auto)The flag of whether the app is drawn on the page.

    lv_obj_t* appIconObj;                       //(auto)The icon object of the app.
    lv_obj_t* appNameObj;                       //(auto)The name object of the app.
}App_t;

typedef struct {
    lv_obj_t* bgObj;                            //(auto)The background object of the app-loader.
    lv_obj_t* fgObj;                            //(auto)The foreground object of the app-loader.

    lv_anim_t animFillScreen;                   //(auto)Step 1, fill screen
    lv_anim_t animDecMask;                      //(auto)Step 2, decrease mask

    uint16_t startX, startY;                    //(auto)animation step 1 start point
    uint16_t endX, endY;                        //(const)animation step 1 end point, all zero
    uint16_t startW, startH;                    //(auto)animation step 1 start point
    uint16_t endW, endH;                        //(const)animation step 1 end point, all screen width and height
    uint16_t startRadio, endRadio;              //(auto)animation step 1 start and end radio
}App_LoadAnim_t;

typedef struct {
    lv_obj_t* fatherLayer;                      //(auto)The father object of the app-loader, all the app will be drawn on this object.
    lv_obj_t* maskLayer;
    lv_obj_t* mainTabviewObj;                   //(auto)The main tabview object of the app-loader.

    App_t app[MH_APP_MAX_NUM];                  //(auto)The array of app.
    int16_t appNum;                             //(auto)The number of app in the app-loader.
    int16_t nowRunningAppIndex;                 //(auto)point to the index of the app that is running now. if -1, means no app is running now.

    App_LoadAnim_t anim;                        //(auto)The app-loader animation.
    // lv_anim_t anim;                             //(auto)The app-loader animation.
    bool animIsEnterApp;
    bool animIsExitApp;
}AppLoader_t;

void LittleAppFirmware_DrawApp(lv_obj_t* father);
void LittleAppFirmware_AddApp(  char *name,
                                const lv_img_dsc_t *PicIconDst,
                                void(*openFunc),
                                void(*closeFunc)) ;

void LittleAppFirmware_EnterApp(void);
void LittleAppFirmware_ExitApp(void);

void LittleAppFirmware_ExitAppWithAnim(void);

void LittleAppFirmware_ShowAppByIndex(int32_t index);
void LittleAppFirmware_ShowNextApp(void);
void LittleAppFirmware_ShowPrevApp(void);

uint8_t LittleAppFirmware_IsAppRunning(void);

#endif /* LITTLE_APP_FIRMWARE_H */
