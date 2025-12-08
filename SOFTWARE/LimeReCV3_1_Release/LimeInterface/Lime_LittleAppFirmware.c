#include "Lime_LittleAppFirmware.h"
#include "Lime_App_Hal.h"

#include <string.h>

static AppLoader_t AppLoader =
{
    .nowRunningAppIndex = -1,
};
static void timer_cb(lv_timer_t * timer);
static void anim_working_cb(void * var, int32_t v);
static void anim_finish_cb(lv_anim_t * anim);

LV_FONT_DECLARE(LR_AppName_Font);

void LittleAppFirmware_AddApp(char *name,
    const lv_img_dsc_t *PicIconDst,
    void(*openFunc),
    void(*closeFunc))
{
    if(name == NULL || PicIconDst == NULL || openFunc == NULL || closeFunc == NULL)
    {
        LV_LOG_ERROR("AppLoader: Invalid input parameter.");
        return;
    }
    if(AppLoader.appNum >= MH_APP_MAX_NUM)
    {
        LV_LOG_ERROR("AppLoader: Maximum number of apps reached.");
        return;
    }
    memcpy(AppLoader.app[AppLoader.appNum].name, name, strlen(name));
    AppLoader.app[AppLoader.appNum].iconDst = PicIconDst;
    AppLoader.app[AppLoader.appNum].app_Create = openFunc;
    AppLoader.app[AppLoader.appNum].app_Destroy = closeFunc;
    AppLoader.appNum++;
    LV_LOG_USER("AppLoader: Add app %s.", name);
}

void LittleAppFirmware_DrawApp(lv_obj_t* father)
{
    if(AppLoader.appNum == 0)
    {
        LV_LOG_ERROR("AppLoader: No app to show.");
        return;
    }

    AppLoader.mainTabviewObj = lv_tabview_create(father);
    lv_tabview_set_tab_bar_position(AppLoader.mainTabviewObj, LV_DIR_BOTTOM);
    lv_tabview_set_tab_bar_size(AppLoader.mainTabviewObj, 0);
    lv_obj_set_size(AppLoader.mainTabviewObj, LV_HOR_RES, LV_VER_RES);
    lv_obj_set_style_bg_opa(AppLoader.mainTabviewObj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_pad_all(AppLoader.mainTabviewObj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_radius(AppLoader.mainTabviewObj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_shadow_width(AppLoader.mainTabviewObj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_border_width(AppLoader.mainTabviewObj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);

    uint32_t index = 0;
    for(index = 0; index < AppLoader.appNum; index++)
    {
        lv_obj_t * tab = lv_tabview_add_tab(AppLoader.mainTabviewObj, " ");

        AppLoader.app[index].appIconObj = lv_img_create(tab);
        lv_img_set_src(AppLoader.app[index].appIconObj, AppLoader.app[index].iconDst);
        lv_obj_set_style_bg_opa(AppLoader.app[index].appIconObj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
        lv_obj_center(AppLoader.app[index].appIconObj);

        AppLoader.app[index].appNameObj = lv_label_create(tab);
        lv_label_set_text(AppLoader.app[index].appNameObj, AppLoader.app[index].name);
        lv_obj_set_style_bg_opa(AppLoader.app[index].appNameObj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
        lv_obj_set_style_text_font(AppLoader.app[index].appNameObj, &LR_AppName_Font, LV_PART_MAIN | LV_STATE_DEFAULT);
        lv_obj_set_style_text_color(AppLoader.app[index].appNameObj, lv_color_hex(0xFFFFFF), LV_PART_MAIN | LV_STATE_DEFAULT);
        lv_obj_align_to(AppLoader.app[index].appNameObj, AppLoader.app[index].appIconObj, LV_ALIGN_OUT_BOTTOM_MID, 0, 7);
    }

    /* app father Layer */
    lv_obj_t * appLayer = lv_obj_create(father);
    lv_obj_set_size(appLayer, LV_HOR_RES, LV_VER_RES);
    lv_obj_set_style_bg_opa(appLayer, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_pad_all(appLayer, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_radius(appLayer, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_shadow_width(appLayer, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_border_width(appLayer, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_add_flag(appLayer, LV_OBJ_FLAG_HIDDEN);
    AppLoader.fatherLayer = appLayer;

    /* app mask Layer */
    lv_obj_t *maskLayer = lv_obj_create(father);
    lv_obj_set_size(maskLayer, LV_HOR_RES, LV_VER_RES);
    lv_obj_set_style_bg_opa(maskLayer, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_pad_all(maskLayer, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_radius(maskLayer, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_shadow_width(maskLayer, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_border_width(maskLayer, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_add_flag(maskLayer, LV_OBJ_FLAG_HIDDEN);
    AppLoader.maskLayer = maskLayer;


    lv_anim_init(&AppLoader.anim.animFillScreen);
    lv_anim_set_var(&AppLoader.anim.animFillScreen, &AppLoader.anim.animFillScreen);
    lv_anim_set_exec_cb(&AppLoader.anim.animFillScreen, anim_working_cb);
    lv_anim_set_values(&AppLoader.anim.animFillScreen, 0, 100);
    lv_anim_set_time(&AppLoader.anim.animFillScreen, 400);
    lv_anim_set_path_cb(&AppLoader.anim.animFillScreen, lv_anim_path_ease_out);
    lv_anim_set_ready_cb(&AppLoader.anim.animFillScreen, anim_finish_cb);
    lv_anim_set_user_data(&AppLoader.anim.animFillScreen, (void*)&AppLoader.anim.animFillScreen);

    lv_anim_init(&AppLoader.anim.animDecMask);
    lv_anim_set_var(&AppLoader.anim.animDecMask, &AppLoader.anim.animDecMask);
    lv_anim_set_exec_cb(&AppLoader.anim.animDecMask, anim_working_cb);
    lv_anim_set_values(&AppLoader.anim.animDecMask, 0, 100);
    lv_anim_set_time(&AppLoader.anim.animDecMask, 200);
    lv_anim_set_path_cb(&AppLoader.anim.animDecMask, lv_anim_path_ease_out);
    lv_anim_set_ready_cb(&AppLoader.anim.animDecMask, anim_finish_cb);
    lv_anim_set_user_data(&AppLoader.anim.animDecMask, (void*)&AppLoader.anim.animDecMask);

    AppLoader.nowRunningAppIndex = -1;

    lv_timer_t * timer = lv_timer_create(timer_cb, 10, NULL);
}
void LittleAppFirmware_EnterApp(void)
{
    uint32_t index = lv_tabview_get_tab_active(AppLoader.mainTabviewObj);

    lv_obj_clear_flag(AppLoader.fatherLayer, LV_OBJ_FLAG_HIDDEN);

    LV_LOG_USER("AppLoader: Enter app index %d.", index);
    if(index >= AppLoader.appNum)
    {
        LV_LOG_ERROR("AppLoader: Invalid app index.");
        return;
    }
    AppLoader.nowRunningAppIndex = index;
    AppLoader.app[index].app_Create(AppLoader.fatherLayer, LV_HOR_RES, LV_VER_RES);

    LimeHAL_SyncAppRunStatus(index);
}
void LittleAppFirmware_ExitApp(void)
{
    if(AppLoader.nowRunningAppIndex == -1)
    {
        LV_LOG_ERROR("AppLoader: No app is running now.");
        return;
    }

    lv_obj_add_flag(AppLoader.fatherLayer, LV_OBJ_FLAG_HIDDEN);

    AppLoader.app[AppLoader.nowRunningAppIndex].app_Destroy();
    AppLoader.nowRunningAppIndex = -1;

    LimeHAL_SyncAppRunStatus(-1);

    LV_LOG_USER("AppLoader: Exit app index %d.", AppLoader.nowRunningAppIndex);
}

void LittleAppFirmware_ShowAppByIndex(int32_t index)
{
    if(AppLoader.nowRunningAppIndex != -1)
    {
        LV_LOG_ERROR("AppLoader: App is running now, can't show another app.");
        return;
    }
    if(index < 0 || index >= AppLoader.appNum)
    {
        LV_LOG_ERROR("AppLoader: Invalid app index.");
        return;
    }
    lv_tabview_set_active(AppLoader.mainTabviewObj, index, LV_ANIM_ON);
}
void LittleAppFirmware_ShowNextApp(void)
{
    if(AppLoader.nowRunningAppIndex != -1)
    {
        LV_LOG_ERROR("AppLoader: App is running now, can't show another app.");
        return;
    }
    int32_t index = lv_tabview_get_tab_active(AppLoader.mainTabviewObj);
    index++;
    if(index >= AppLoader.appNum)
    {
        index = 0;
    }
    lv_tabview_set_active(AppLoader.mainTabviewObj, index, LV_ANIM_ON);
}
void LittleAppFirmware_ShowPrevApp(void)
{
    if(AppLoader.nowRunningAppIndex != -1)
    {
        LV_LOG_ERROR("AppLoader: App is running now, can't show another app.");
        return;
    }
    int32_t index = lv_tabview_get_tab_active(AppLoader.mainTabviewObj);
    index--;
    if(index < 0)
    {
        index = AppLoader.appNum - 1;
    }
    lv_tabview_set_active(AppLoader.mainTabviewObj, index, LV_ANIM_ON);
}

uint8_t LittleAppFirmware_IsAppRunning(void)
{
    if(AppLoader.nowRunningAppIndex != -1)
        return 1;

    return 0;
}

static void anim_working_cb(void * var, int32_t v)
{
    void * userDataPin = lv_anim_get_user_data((const lv_anim_t*)var);

    if(userDataPin == &AppLoader.anim.animFillScreen)
    {
        if(AppLoader.animIsExitApp)
            v = 100 - v;

        int32_t w = fmapWithLimit(v, 40, 100, 100, 240);
        int32_t h = w;
        int32_t radio = fmap(v, 50, 100, 20, 0);
        uint8_t opa = fmapWithLimit(v, 0, 40, 10, 255);
        lv_obj_set_size(AppLoader.fatherLayer, w, h);
        lv_obj_set_style_radius(AppLoader.fatherLayer, radio, LV_PART_MAIN | LV_STATE_DEFAULT);
        lv_obj_set_style_bg_opa(AppLoader.fatherLayer, opa, LV_PART_MAIN | LV_STATE_DEFAULT);
        lv_obj_center(AppLoader.fatherLayer);
        lv_obj_clear_flag(AppLoader.fatherLayer, LV_OBJ_FLAG_HIDDEN);
    }
    else if(userDataPin == &AppLoader.anim.animDecMask)
    {
        if(AppLoader.animIsExitApp)
            v = 100 - v;

        uint8_t opa = fmap(v, 0, 100, 255, 0);
        lv_obj_set_style_bg_opa(AppLoader.maskLayer, opa, LV_PART_MAIN | LV_STATE_DEFAULT);
        lv_obj_clear_flag(AppLoader.maskLayer, LV_OBJ_FLAG_HIDDEN);
    }
    else
        LV_LOG_ERROR("Invalid anim parameter.");
}
static void anim_finish_cb(lv_anim_t * anim)
{
    void * userDataPin = lv_anim_get_user_data(anim);

    if(userDataPin == &AppLoader.anim.animFillScreen)
    {
        if(AppLoader.animIsEnterApp)
        {
            lv_obj_clear_flag(AppLoader.maskLayer, LV_OBJ_FLAG_HIDDEN);
            LittleAppFirmware_EnterApp();
            lv_anim_start(&AppLoader.anim.animDecMask);
        }
        else
        {
            lv_obj_add_flag(AppLoader.fatherLayer, LV_OBJ_FLAG_HIDDEN);
        }

        AppLoader.animIsExitApp = false;
    }
    else if(userDataPin == &AppLoader.anim.animDecMask)
    {
        if(AppLoader.animIsExitApp)
        {
            LittleAppFirmware_ExitApp();
            lv_anim_start(&AppLoader.anim.animFillScreen);
        }

        AppLoader.animIsEnterApp = false;

        lv_obj_add_flag(AppLoader.maskLayer, LV_OBJ_FLAG_HIDDEN);
    }
    else
        LV_LOG_ERROR("Invalid anim parameter.");
}

static void timer_cb(lv_timer_t * timer)
{
    static LimeHal_KeyInfo_t SavedkeyInfo;
    static bool appRun_Last = false;
    const LimeHal_KeyInfo_t *LimHalkeyInfo = (const LimeHal_KeyInfo_t *)&LimeHAL_GetInfoPin()->keyInfo;
    static uint8_t longPressCount = 0;

    /* when power on, app firmware run after app LRCFace run, so need to check app running status */
    if(LimeHAL_IsAppFirmwareScheLock())
    {
        appRun_Last = true;
        return;
    }

    /* exit from app LRCFace when power on*/
    if((appRun_Last == true) && (LimeHAL_IsAppFirmwareScheLock() == false))
    {
        appRun_Last = false;
        goto SyncEnd;
    }

    /* do not re-enter app when kr is pressed and animation is running */
    if(AppLoader.animIsExitApp)
    {
        goto SyncEnd;
    }

    /* check key status */
    if( !LittleAppFirmware_IsAppRunning())
    {
        if((SavedkeyInfo.sw_left != LimHalkeyInfo->sw_left) && ((LimHalkeyInfo->sw_left % 2) == 1))
        {
            LittleAppFirmware_ShowPrevApp();
        }
        else if((SavedkeyInfo.sw_right != LimHalkeyInfo->sw_right) && ((LimHalkeyInfo->sw_right % 2) == 1))
        {
            LittleAppFirmware_ShowNextApp();
        }
        else if((SavedkeyInfo.sw_set != LimHalkeyInfo->sw_set) && ((LimHalkeyInfo->sw_set % 2) == 1))
        {
            /* start enter app animation */
            // LittleAppFirmware_EnterApp();
            AppLoader.animIsEnterApp = true;
            lv_anim_start(&AppLoader.anim.animFillScreen);
        }
    }
    else
    {
        if((LimHalkeyInfo->kr % 2) == 1)
        {
            longPressCount ++;
        }
        else
        {
            longPressCount = 0;
        }

        if(longPressCount >= 80)
        {
            longPressCount = 0;

            /* start exit app animation */
            // LittleAppFirmware_ExitApp();
            AppLoader.animIsExitApp = true;
            lv_anim_start(&AppLoader.anim.animDecMask);
        }
    }


SyncEnd:
    SavedkeyInfo.sw_left = LimHalkeyInfo->sw_left;
    SavedkeyInfo.sw_right = LimHalkeyInfo->sw_right;
    SavedkeyInfo.sw_set = LimHalkeyInfo->sw_set;
    SavedkeyInfo.kr = LimHalkeyInfo->kr;
}

void LittleAppFirmware_ExitAppWithAnim(void)
{
    if(LittleAppFirmware_IsAppRunning())
    {
        AppLoader.animIsExitApp = true;
        lv_anim_start(&AppLoader.anim.animDecMask);
    }
    else
    {
        LV_LOG_ERROR("AppLoader: No app is running now.");
    }
}
