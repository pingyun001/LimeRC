#include "Lime_LvMainFace.h"
#include "lvgl.h"
#include "Lime_SimFiveKey.h"

#include "Lime_LockFace.h"
#include "Lime_HeadBar.h"
#include "Lime_LittleAppFirmware.h"
#include "Lime_App_RCCtrl.h"
#include "Lime_App_Photo.h"

#include "Lime_App_Hal.h"
#include "Lime_App_StartFace.h"
#include "Lime_App_StartConnFace.h"
#include "Lime_App_LRCFace.h"
#include "Lime_App_CaliFace.h"
#include "Lime_App_Settings.h"

#include "Lime_MessageBox.h"

#if USING_LIME_HARDWARE

#endif


static lv_obj_t* mainFaceObj = NULL;

LV_IMG_DECLARE(LR_BG_SkylineCartoon);
LV_IMG_DECLARE(LR_Icon_App_Cali);
LV_IMG_DECLARE(LR_Icon_App_Photos);
LV_IMG_DECLARE(LR_Icon_App_RCCtrl);
LV_IMG_DECLARE(LR_Icon_App_Settings);
LV_IMG_DECLARE(LR_Icon_App_Connections);

uint8_t timerCount = 0;

static void timer_cb(lv_timer_t * timer)
{
    timerCount += 10;
}

void Lime_LvMainFace_Init(void)
{
    // LV_LOG_USER("CompileTime:%s,%s", __DATE__, __TIME__);

    mainFaceObj = lv_obj_create(lv_scr_act());
    lv_obj_set_size(mainFaceObj, LV_HOR_RES, LV_VER_RES);
    lv_obj_set_style_bg_opa(mainFaceObj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_pad_all(mainFaceObj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_radius(mainFaceObj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_shadow_width(mainFaceObj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_border_width(mainFaceObj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);



    /* just test message box */
    // Lime_MessageBox_Show("进度", "正在进行复制，请勿进行其它操作", &timerCount, 2000);
    // lv_timer_t * timer = lv_timer_create(timer_cb, 10, NULL);

    // /* jump start face */
    // Lime_App_StartConnFace_Finish_Hook();
    // return;

    Lime_App_StartFace_Create(mainFaceObj, 240, 240);

    /* start simulator hardware timer */
#if !USING_LIME_HARDWARE
    LimeHAL_SoftSimHardwareTimer_Init();
#endif
}

void Lime_App_StartFace_Finish_Hook(void)
{
    LV_LOG_USER("Lime_App_StartFace_Finish_Hook");
#if 1
    Lime_App_StartConnFace_Create(mainFaceObj, 240, 240);
#else
    LRC_AppFace_InitialCreate(240, 240);
#endif

#if !USING_LIME_HARDWARE
    Lime_SimFiveKey_Init(lv_scr_act());
#endif
}

void Lime_App_StartConnFace_Finish_Hook(void)
{
#if !USING_LIME_HARDWARE
    Lime_SimFiveKey_Destroy();
#endif

    lv_obj_t* bg = lv_img_create(mainFaceObj);
    lv_img_set_src(bg, &LR_BG_SkylineCartoon);
    lv_obj_center(bg);

    Lime_HeadBar_Create(mainFaceObj);

    LittleAppFirmware_AddApp("RC遥控", &LR_Icon_App_RCCtrl, LRC_AppFace_InitialCreate, LRC_AppFace_Destroy);//fixed, do not modify
    LittleAppFirmware_AddApp("图库", &LR_Icon_App_Photos, Photo_App_Create, Photo_App_Destroy);
    LittleAppFirmware_AddApp("接收机", &LR_Icon_App_Connections, Connect_App_Create, Connect_App_Destroy);
    LittleAppFirmware_AddApp("摇杆校准", &LR_Icon_App_Cali, CaliFace_App_Create, CaliFace_App_Destroy);
    LittleAppFirmware_AddApp("设置", &LR_Icon_App_Settings, Setting_App_Create, Setting_App_Destroy);

    LittleAppFirmware_DrawApp(mainFaceObj);

    // Lime_LockFace_Init(mainFaceObj);    //顺序不能更改！
    // Lime_LockFace_SetTime(18, 30);
    // Lime_LockFace_SetDate(2025, 2, 13);

    LRC_AppFace_InitialCreate(NULL, 240, 240);
#if !USING_LIME_HARDWARE
    Lime_SimFiveKey_Init(lv_scr_act());
#endif
}

void Lime_App_LRCFace_Finish_Hook(void)
{
    /* refresh simulator key to top layer*/
    // Lime_SimFiveKey_Destroy();
    // Lime_SimFiveKey_Init(lv_scr_act());
}
