#include "Lime_App_CaliFace.h"
#include "Lime_App_Hal.h"
#include "Lime_LittleAppFirmware.h"
#include <stdio.h>
#include <string.h>

static lv_obj_t* CaliFaceObj = NULL;
static lv_obj_t* CaliFacePage1Obj = NULL;
static lv_obj_t* CaliFacePage2Obj = NULL;
static lv_obj_t* bgLimeImg = NULL;
static lv_obj_t* RCModelImg = NULL;
static lv_obj_t* InstrucObj = NULL;
static lv_obj_t* startCaliBtnObj = NULL;
static lv_obj_t* finishCaliBtnObj = NULL;
static lv_obj_t* RockerLObj = NULL;
static lv_obj_t* RockerRObj = NULL;
static lv_anim_t IconInAnim;
static lv_anim_t TextInAnim;
static lv_anim_t BtnInOutAnim;
static lv_anim_t LimeIconInOutAnim;
static lv_anim_t RockerLAnim;
static lv_anim_t RockerRAnim;
static lv_timer_t *caliScanTimer = NULL;


LV_IMG_DECLARE(LR_StartBG);
LV_IMG_DECLARE(LR_RC_Img);
LV_IMG_DECLARE(LR_StartLogo);
LV_FONT_DECLARE(Lime_APP_CaliFace_DefaultFont);
LV_FONT_DECLARE(Lime_APP_CaliFace_BottonFont);
LV_FONT_DECLARE(Lime_APP_CaliFace_PercentFont);
LV_FONT_DECLARE(Lime_APP_CaliFace_LRRockerNameFont);

static void CaliFace_Page1_Create(lv_obj_t* father, uint16_t w, uint16_t h);
static void CaliFace_Page2_Create(lv_obj_t* father, uint16_t w, uint16_t h);
static lv_obj_t* CaliFace_Rocker_Create(lv_obj_t* father, char LorR);
static void enter_anim_cb(lv_anim_t * anim);
static void page1ToPage2_anim_cb(lv_anim_t * anim);
static void cali_scan_timer_cb(lv_timer_t * timer);
static void cali_animToPage2(void);
static void syncRockerPosAndStatus(const LimeHAL_CaliAppInfo_t *caliInfo);
static void syncPercentLabel(lv_obj_t* percentLabelObj, uint8_t percent);

void CaliFace_App_Create(lv_obj_t* father, uint16_t w, uint16_t h)
{
    if(CaliFaceObj  != NULL)
    {
        LV_LOG_ERROR("CaliFaceObj is not NULL");
        return;
    }

    CaliFaceObj = lv_obj_create(father);
    lv_obj_set_size(CaliFaceObj, w, h);
    lv_obj_center(CaliFaceObj);
    lv_obj_set_style_pad_all(CaliFaceObj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_radius(CaliFaceObj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_shadow_width(CaliFaceObj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_border_width(CaliFaceObj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_img_src(CaliFaceObj, &LR_StartBG, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_clear_flag(CaliFaceObj, LV_OBJ_FLAG_SCROLLABLE);

    bgLimeImg = lv_img_create(CaliFaceObj);
    lv_img_set_src(bgLimeImg, &LR_StartLogo);
    lv_obj_set_pos(bgLimeImg, -27, 160);
    lv_img_set_zoom(bgLimeImg, (140.0f / 85.0f) * 255.0f);
    lv_obj_set_style_opa(bgLimeImg, 100, LV_PART_MAIN | LV_STATE_DEFAULT);


    CaliFace_Page1_Create(CaliFaceObj, w, h);
    CaliFace_Page2_Create(CaliFaceObj, w, h);
    lv_obj_add_flag(CaliFacePage2Obj, LV_OBJ_FLAG_HIDDEN);

    /* animation initialization */
    lv_anim_init(&IconInAnim);
    lv_anim_set_var(&IconInAnim, RCModelImg);
    lv_anim_set_exec_cb(&IconInAnim, (lv_anim_exec_xcb_t)lv_obj_set_y);
    lv_anim_set_values(&IconInAnim, -70, 22);
    lv_anim_set_time(&IconInAnim, 800);
    lv_anim_set_path_cb(&IconInAnim, lv_anim_path_bounce);
    lv_anim_set_ready_cb(&IconInAnim, enter_anim_cb);

    lv_anim_init(&TextInAnim);
    lv_anim_set_var(&TextInAnim, InstrucObj);
    lv_anim_set_exec_cb(&TextInAnim, (lv_anim_exec_xcb_t)lv_obj_set_y);
    lv_anim_set_values(&TextInAnim, -300, -65);
    lv_anim_set_time(&TextInAnim, 500);
    lv_anim_set_path_cb(&TextInAnim, lv_anim_path_bounce);

    lv_anim_init(&BtnInOutAnim);
    lv_anim_set_var(&BtnInOutAnim, startCaliBtnObj);
    lv_anim_set_exec_cb(&BtnInOutAnim, (lv_anim_exec_xcb_t)lv_obj_set_y);
    lv_anim_set_values(&BtnInOutAnim, 100, -20);
    lv_anim_set_time(&BtnInOutAnim, 500);
    lv_anim_set_path_cb(&BtnInOutAnim, lv_anim_path_bounce);

    lv_anim_init(&LimeIconInOutAnim);
    lv_anim_set_var(&LimeIconInOutAnim, bgLimeImg);
    lv_anim_set_exec_cb(&LimeIconInOutAnim, (lv_anim_exec_xcb_t)lv_obj_set_x);
    lv_anim_set_values(&LimeIconInOutAnim, -120, -27);
    lv_anim_set_delay(&LimeIconInOutAnim, 300);
    lv_anim_set_time(&LimeIconInOutAnim, 1200);
    lv_anim_set_path_cb(&LimeIconInOutAnim, lv_anim_path_ease_in_out);

    lv_anim_start(&IconInAnim);
    lv_anim_start(&TextInAnim);
    lv_anim_start(&BtnInOutAnim);
    lv_anim_start(&LimeIconInOutAnim);
}

static void enter_anim_cb(lv_anim_t * anim)
{
    if(caliScanTimer != NULL)
    {
        LV_LOG_ERROR("caliScanTimer is not NULL");
        return;
    }

    static uint8_t initFlag;
    initFlag = 1;
    caliScanTimer = lv_timer_create(cali_scan_timer_cb, 10, (void*)&initFlag);
}

static void cali_scan_timer_cb(lv_timer_t * timer)
{
    uint8_t *initFlag = (uint8_t*)lv_timer_get_user_data(timer);
    const LimeHal_KeyInfo_t *halKeyInfo = &LimeHAL_GetInfoPin()->keyInfo;
    static uint8_t keySetCnt = 0;

    if(*initFlag == 1)
    {
        if((halKeyInfo->sw_set % 2) == 0)
        {
            *initFlag = 0;
            keySetCnt = halKeyInfo->sw_set;
        }

        return;
    }

    /* polling for cali status */
    LimeHAL_CaliAppInfo_t *caliInfo = &LimeHAL_GetInfoPin()->caliAppInfo;
    if(caliInfo->isCaliFinished == true)
    {
        if(finishCaliBtnObj != NULL)
        {
            lv_obj_set_style_bg_color(finishCaliBtnObj, lv_color_hex(0x03af03), LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_label_set_text(lv_obj_get_child(finishCaliBtnObj, 0), "完成");
        }
    }
    else
    {
        if(finishCaliBtnObj != NULL)
        {
            lv_obj_set_style_bg_color(finishCaliBtnObj, lv_color_hex(0x4a4a4a), LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_label_set_text(lv_obj_get_child(finishCaliBtnObj, 0), "校准中...");
        }
    }

    /* key set is pressed */
    if(((halKeyInfo->sw_set % 2) == 1) && (keySetCnt != halKeyInfo->sw_set))
    {
        LV_LOG_USER("Key Set is pressed, Cnt:%d", halKeyInfo->sw_set);

        if(lv_obj_has_flag(CaliFacePage2Obj, LV_OBJ_FLAG_HIDDEN))
        {
            cali_animToPage2();
            LimeHAL_RockerCaliNeedRun();
        }
        else if(caliInfo->isCaliFinished == true)
            LittleAppFirmware_ExitAppWithAnim();
    }

    syncRockerPosAndStatus(caliInfo);

    keySetCnt = halKeyInfo->sw_set;
}

static void syncPercentLabel(lv_obj_t* percentLabelObj, uint8_t percent)
{
    if(percentLabelObj == NULL)
    {
        LV_LOG_ERROR("percentLabelObj is NULL");
        return;
    }

    char tempBuf[6] = {0};

    percent = (percent > 100) ? 100 : percent;
    snprintf(tempBuf, 5, "%d%%", percent);
    lv_label_set_text(percentLabelObj, tempBuf);
    if(percent == 100)
        lv_obj_set_style_text_color(percentLabelObj, lv_color_hex(0x03af03), LV_PART_MAIN | LV_STATE_DEFAULT);
    else
        lv_obj_set_style_text_color(percentLabelObj, lv_color_hex(0x4a4a4a), LV_PART_MAIN | LV_STATE_DEFAULT);
}

static void syncRockerPosAndStatus(const LimeHAL_CaliAppInfo_t *caliInfo)
{
    if(RockerLObj == NULL || RockerRObj == NULL)
    {
        LV_LOG_ERROR("RockerLObj or RockerRObj is NULL");
        return;
    }

    const int16_t posAbsMax = 30;

    /* rocker L */
    lv_obj_t* rockerLHandleObj = lv_obj_get_child(RockerLObj, 2);
    lv_obj_t* rockerLLingObj = lv_obj_get_child(RockerLObj, 1);
    int16_t rockerLPosX, rockerLPosY;
    rockerLPosX = fmapWithLimit(caliInfo->channelPos[2], -100, 100, -posAbsMax, posAbsMax);
    rockerLPosY = fmapWithLimit(caliInfo->channelPos[3], -100, 100, -posAbsMax, posAbsMax);
    lv_obj_align_to(rockerLHandleObj, rockerLLingObj, LV_ALIGN_CENTER, rockerLPosX, rockerLPosY);

    /* rocker R */
    lv_obj_t* rockerRHandleObj = lv_obj_get_child(RockerRObj, 2);
    lv_obj_t* rockerRLingObj = lv_obj_get_child(RockerRObj, 1);
    int16_t rockerRPosX, rockerRPosY;
    rockerRPosX = fmapWithLimit(caliInfo->channelPos[0], -100, 100, -posAbsMax, posAbsMax);
    rockerRPosY = fmapWithLimit(caliInfo->channelPos[1], -100, 100, -posAbsMax, posAbsMax);
    lv_obj_align_to(rockerRHandleObj, rockerRLingObj, LV_ALIGN_CENTER, rockerRPosX, rockerRPosY);

    syncPercentLabel(lv_obj_get_child(RockerLObj, 3), caliInfo->channelPercent[0][0]);
    syncPercentLabel(lv_obj_get_child(RockerLObj, 4), caliInfo->channelPercent[0][1]);
    syncPercentLabel(lv_obj_get_child(RockerLObj, 5), caliInfo->channelPercent[1][0]);
    syncPercentLabel(lv_obj_get_child(RockerLObj, 6), caliInfo->channelPercent[1][1]);
    syncPercentLabel(lv_obj_get_child(RockerRObj, 3), caliInfo->channelPercent[2][0]);
    syncPercentLabel(lv_obj_get_child(RockerRObj, 4), caliInfo->channelPercent[2][1]);
    syncPercentLabel(lv_obj_get_child(RockerRObj, 5), caliInfo->channelPercent[3][0]);
    syncPercentLabel(lv_obj_get_child(RockerRObj, 6), caliInfo->channelPercent[3][1]);
}

static void cali_animToPage2(void)
{
    lv_anim_set_values(&IconInAnim, 22, -90);
    lv_anim_set_time(&IconInAnim, 400);
    lv_anim_set_values(&TextInAnim, -65, -300);
    lv_anim_set_time(&TextInAnim, 300);
    lv_anim_set_values(&BtnInOutAnim, -20, 100);
    lv_anim_set_time(&BtnInOutAnim, 200);
    lv_anim_set_values(&LimeIconInOutAnim, -27, -120);
    lv_anim_set_delay(&LimeIconInOutAnim, 0);
    lv_anim_set_time(&LimeIconInOutAnim, 300);
    lv_anim_set_ready_cb(&IconInAnim, page1ToPage2_anim_cb);

    lv_anim_start(&IconInAnim);
    lv_anim_start(&TextInAnim);
    lv_anim_start(&BtnInOutAnim);
    lv_anim_start(&LimeIconInOutAnim);
}

static void page1ToPage2_anim_cb(lv_anim_t * anim)
{
    lv_obj_add_flag(CaliFacePage1Obj, LV_OBJ_FLAG_HIDDEN);
    lv_obj_clear_flag(CaliFacePage2Obj, LV_OBJ_FLAG_HIDDEN);

    lv_anim_init(&RockerLAnim);
    lv_anim_set_var(&RockerLAnim, RockerLObj);
    lv_anim_set_exec_cb(&RockerLAnim, (lv_anim_exec_xcb_t)lv_obj_set_y);
    lv_anim_set_values(&RockerLAnim, -100, 50);
    lv_anim_set_time(&RockerLAnim, 500);
    lv_anim_set_path_cb(&RockerLAnim, lv_anim_path_bounce);

    lv_anim_init(&RockerRAnim);
    lv_anim_set_var(&RockerRAnim, RockerRObj);
    lv_anim_set_exec_cb(&RockerRAnim, (lv_anim_exec_xcb_t)lv_obj_set_y);
    lv_anim_set_values(&RockerRAnim, -100, 50);
    lv_anim_set_delay(&RockerRAnim, 60);
    lv_anim_set_time(&RockerRAnim, 500);
    lv_anim_set_path_cb(&RockerRAnim, lv_anim_path_bounce);

    lv_anim_set_var(&BtnInOutAnim, finishCaliBtnObj);
    lv_anim_set_values(&BtnInOutAnim, 100, -20);

    lv_anim_set_values(&LimeIconInOutAnim, -120, -27);
    lv_anim_set_delay(&LimeIconInOutAnim, 300);
    lv_anim_set_time(&LimeIconInOutAnim, 1200);

    lv_anim_start(&RockerLAnim);
    lv_anim_start(&RockerRAnim);
    lv_anim_start(&BtnInOutAnim);
    lv_anim_start(&LimeIconInOutAnim);
}

static lv_obj_t* CaliFace_Rocker_Create(lv_obj_t* father, char LorR)
{
    if(LorR != 'L' && LorR != 'R')
    {
        LV_LOG_ERROR("LorR is not L or R");
        return NULL;
    }
    if(father == NULL)
    {
        LV_LOG_ERROR("father is NULL");
        return NULL;
    }

    lv_obj_t * bgObj = lv_obj_create(father);
    lv_obj_set_size(bgObj, 112, 114);
    // lv_obj_set_style_bg_color(bgObj, lv_color_hex(0xFF0000), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(bgObj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_pad_all(bgObj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_radius(bgObj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_shadow_width(bgObj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_border_width(bgObj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_clear_flag(bgObj, LV_OBJ_FLAG_SCROLLABLE);

    /* index 0 */
    lv_obj_t * line1Obj = lv_obj_create(bgObj);
    lv_obj_set_size(line1Obj, 60, 2);
    lv_obj_align(line1Obj, LV_ALIGN_TOP_MID, 0, 45);
    lv_obj_set_style_bg_color(line1Obj, lv_color_hex(0x4a4a4a), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_pad_all(line1Obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_radius(line1Obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_shadow_width(line1Obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_border_width(line1Obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);

    /* index 1 */
    lv_obj_t * line2Obj = lv_obj_create(bgObj);
    lv_obj_set_size(line2Obj, 2, 60);
    lv_obj_align_to(line2Obj, line1Obj, LV_ALIGN_CENTER, 0, 0);
    lv_obj_set_style_bg_color(line2Obj, lv_color_hex(0x4a4a4a), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_pad_all(line2Obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_radius(line2Obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_shadow_width(line2Obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_border_width(line2Obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);

    /* index 2 */
    lv_obj_t * handle = lv_obj_create(bgObj);
    lv_obj_set_size(handle, 12, 12);
    lv_obj_align_to(handle, line1Obj, LV_ALIGN_CENTER, 0, 0);
    lv_obj_set_style_bg_color(handle, lv_color_hex(0xffffff), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_pad_all(handle, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_radius(handle, 6, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_shadow_width(handle, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_border_width(handle, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_t * handle2 = lv_obj_create(handle);
    lv_obj_set_size(handle2, 8, 8);
    lv_obj_align_to(handle2, line1Obj, LV_ALIGN_CENTER, 0, 0);
    lv_obj_set_style_bg_color(handle2, lv_color_hex(0x4a4a4a), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_pad_all(handle2, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_radius(handle2, 4, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_shadow_width(handle2, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_border_width(handle2, 0, LV_PART_MAIN | LV_STATE_DEFAULT);

    /* index 3 */
    lv_obj_t * percentL_Label = lv_label_create(bgObj);
    lv_label_set_text(percentL_Label, "100\%");
    lv_obj_set_size(percentL_Label, 27, 8);
    lv_obj_align_to(percentL_Label, line1Obj, LV_ALIGN_OUT_LEFT_MID, -2, 0);
    lv_obj_set_style_text_font(percentL_Label, &Lime_APP_CaliFace_PercentFont, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_color(percentL_Label, lv_color_hex(0xffffff), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_align(percentL_Label, LV_TEXT_ALIGN_RIGHT, LV_PART_MAIN | LV_STATE_DEFAULT);

    /* index 4 */
    lv_obj_t * percentR_Label = lv_label_create(bgObj);
    lv_label_set_text(percentR_Label, "100\%");
    lv_obj_set_size(percentR_Label, 27, 8);
    lv_obj_align_to(percentR_Label, line1Obj, LV_ALIGN_OUT_RIGHT_MID, 2, 0);
    lv_obj_set_style_text_font(percentR_Label, &Lime_APP_CaliFace_PercentFont, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_color(percentR_Label, lv_color_hex(0xffffff), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_align(percentR_Label, LV_TEXT_ALIGN_LEFT, LV_PART_MAIN | LV_STATE_DEFAULT);

    /* index 5 */
    lv_obj_t * percentH_Label = lv_label_create(bgObj);
    lv_label_set_text(percentH_Label, "100\%");
    lv_obj_set_size(percentH_Label, 27, 8);
    lv_obj_align_to(percentH_Label, line2Obj, LV_ALIGN_OUT_TOP_MID, 0, -3);
    lv_obj_set_style_text_font(percentH_Label, &Lime_APP_CaliFace_PercentFont, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_color(percentH_Label, lv_color_hex(0xffffff), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_align(percentH_Label, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_DEFAULT);

    /* index 6 */
    lv_obj_t * percentB_Label = lv_label_create(bgObj);
    lv_label_set_text(percentB_Label, "100\%");
    lv_obj_set_size(percentB_Label, 27, 8);
    lv_obj_align_to(percentB_Label, line2Obj, LV_ALIGN_OUT_BOTTOM_MID, 0, 4);
    lv_obj_set_style_text_font(percentB_Label, &Lime_APP_CaliFace_PercentFont, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_color(percentB_Label, lv_color_hex(0xffffff), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_align(percentB_Label, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_DEFAULT);

    /* index 7 */
    lv_obj_t* name_Label = lv_label_create(bgObj);
    lv_label_set_text(name_Label, LorR == 'L' ? "左摇杆" : "右摇杆");
    lv_obj_set_size(name_Label, 50, 12);
    lv_obj_align(name_Label, LV_ALIGN_BOTTOM_MID, 0, 0);
    lv_obj_set_style_text_font(name_Label, &Lime_APP_CaliFace_LRRockerNameFont, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_color(name_Label, lv_color_hex(0xffffff), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_align(name_Label, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_DEFAULT);

    return bgObj;
}

static void CaliFace_Page1_Create(lv_obj_t* father, uint16_t w, uint16_t h)
{
    if(CaliFacePage1Obj  != NULL)
    {
        LV_LOG_ERROR("CaliFacePage1Obj is not NULL");
        return;
    }

    CaliFacePage1Obj = lv_obj_create(father);
    lv_obj_set_size(CaliFacePage1Obj, w, h);
    lv_obj_center(CaliFacePage1Obj);
    lv_obj_set_style_pad_all(CaliFacePage1Obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_radius(CaliFacePage1Obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_shadow_width(CaliFacePage1Obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_border_width(CaliFacePage1Obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(CaliFacePage1Obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_clear_flag(CaliFacePage1Obj, LV_OBJ_FLAG_SCROLLABLE);

    InstrucObj = lv_obj_create(CaliFacePage1Obj);
    lv_obj_set_size(InstrucObj, 188, 55);
    lv_obj_align(InstrucObj, LV_ALIGN_BOTTOM_MID, 0, -65);
    // lv_obj_set_style_bg_color(InstrucObj, lv_color_hex(0xFF0000), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(InstrucObj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_pad_all(InstrucObj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_radius(InstrucObj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_shadow_width(InstrucObj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_border_width(InstrucObj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_clear_flag(InstrucObj, LV_OBJ_FLAG_SCROLLABLE);

    lv_obj_t *instrLabel1 = lv_label_create(InstrucObj);
    lv_label_set_text(instrLabel1, "· 开始前，请将所有摇杆放于中位");
    lv_obj_set_size(instrLabel1, 188, 12);
    lv_obj_align(instrLabel1, LV_ALIGN_TOP_MID, 0, 0);
    lv_obj_set_style_text_color(instrLabel1, lv_color_hex(0xFFFFFF), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_font(instrLabel1, &Lime_APP_CaliFace_DefaultFont, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_align(instrLabel1, LV_TEXT_ALIGN_LEFT, LV_PART_MAIN | LV_STATE_DEFAULT);

    lv_obj_t *instrLabel2 = lv_label_create(InstrucObj);
    lv_label_set_text(instrLabel2, "· 开始后，请将所有摇杆拨到各方向");
    lv_obj_set_size(instrLabel2, 188, 12);
    lv_obj_align_to(instrLabel2, instrLabel1, LV_ALIGN_OUT_BOTTOM_MID, 0, 6);
    lv_obj_set_style_text_color(instrLabel2, lv_color_hex(0xFFFFFF), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_font(instrLabel2, &Lime_APP_CaliFace_DefaultFont, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_align(instrLabel2, LV_TEXT_ALIGN_LEFT, LV_PART_MAIN | LV_STATE_DEFAULT);

    lv_obj_t *instrLabel3 = lv_label_create(InstrucObj);
    lv_label_set_text(instrLabel3, "  最大值，并保持到百分比变绿");
    lv_obj_set_size(instrLabel3, 188, 12);
    lv_obj_align_to(instrLabel3, instrLabel2, LV_ALIGN_OUT_BOTTOM_MID, 0, 6);
    lv_obj_set_style_text_color(instrLabel3, lv_color_hex(0xFFFFFF), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_font(instrLabel3, &Lime_APP_CaliFace_DefaultFont, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_align(instrLabel3, LV_TEXT_ALIGN_LEFT, LV_PART_MAIN | LV_STATE_DEFAULT);

    RCModelImg = lv_img_create(CaliFacePage1Obj);
    lv_img_set_src(RCModelImg, &LR_RC_Img);
    lv_obj_align(RCModelImg, LV_ALIGN_TOP_MID, 0, 22);

    startCaliBtnObj = lv_obj_create(CaliFacePage1Obj);
    lv_obj_set_size(startCaliBtnObj, 115, 30);
    lv_obj_align(startCaliBtnObj, LV_ALIGN_BOTTOM_MID, 0, -24);
    lv_obj_set_style_bg_color(startCaliBtnObj, lv_color_hex(0xecbd1c), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_radius(startCaliBtnObj, 8, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_pad_all(startCaliBtnObj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_shadow_width(startCaliBtnObj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_border_width(startCaliBtnObj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_clear_flag(startCaliBtnObj, LV_OBJ_FLAG_SCROLLABLE);

    lv_obj_t *startCaliLabel = lv_label_create(startCaliBtnObj);
    lv_label_set_text(startCaliLabel, "开始校准");
    lv_obj_align(startCaliLabel, LV_ALIGN_CENTER, 0, 0);
    lv_obj_set_style_text_color(startCaliLabel, lv_color_hex(0xFFFFFF), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_font(startCaliLabel, &Lime_APP_CaliFace_BottonFont, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_align(startCaliLabel, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_DEFAULT);
}
static void CaliFace_Page2_Create(lv_obj_t* father, uint16_t w, uint16_t h)
{
    if(CaliFacePage2Obj  != NULL)
    {
        LV_LOG_ERROR("CaliFacePage2Obj is not NULL");
        return;
    }

    CaliFacePage2Obj = lv_obj_create(father);
    lv_obj_set_size(CaliFacePage2Obj, w, h);
    lv_obj_center(CaliFacePage2Obj);
    lv_obj_set_style_pad_all(CaliFacePage2Obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_radius(CaliFacePage2Obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(CaliFacePage2Obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_shadow_width(CaliFacePage2Obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_border_width(CaliFacePage2Obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_clear_flag(CaliFacePage2Obj, LV_OBJ_FLAG_SCROLLABLE);

    RockerLObj = CaliFace_Rocker_Create(CaliFacePage2Obj, 'L');
    if(RockerLObj != NULL)
    {
        lv_obj_align(RockerLObj, LV_ALIGN_TOP_LEFT, 4, 50);
    }
    RockerRObj = CaliFace_Rocker_Create(CaliFacePage2Obj, 'R');
    if(RockerRObj != NULL)
    {
        lv_obj_align(RockerRObj, LV_ALIGN_TOP_RIGHT, -4, 50);
    }

    finishCaliBtnObj = lv_obj_create(CaliFacePage2Obj);
    lv_obj_set_size(finishCaliBtnObj, 115, 30);
    lv_obj_align(finishCaliBtnObj, LV_ALIGN_BOTTOM_MID, 0, -24);
    lv_obj_set_style_bg_color(finishCaliBtnObj, lv_color_hex(0x4a4a4a), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_radius(finishCaliBtnObj, 8, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_pad_all(finishCaliBtnObj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_shadow_width(finishCaliBtnObj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_border_width(finishCaliBtnObj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_clear_flag(finishCaliBtnObj, LV_OBJ_FLAG_SCROLLABLE);

    lv_obj_t *startCaliLabel = lv_label_create(finishCaliBtnObj);
    lv_label_set_text(startCaliLabel, "校准中...");
    lv_obj_align(startCaliLabel, LV_ALIGN_CENTER, 0, 0);
    lv_obj_set_style_text_color(startCaliLabel, lv_color_hex(0xFFFFFF), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_font(startCaliLabel, &Lime_APP_CaliFace_BottonFont, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_align(startCaliLabel, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_DEFAULT);
}
void CaliFace_App_Destroy(void)
{
    if(CaliFaceObj != NULL)
    {
        lv_obj_del(CaliFaceObj);
        CaliFaceObj = NULL;
        CaliFacePage1Obj = NULL;
        CaliFacePage2Obj = NULL;
    }

    if(caliScanTimer != NULL)
    {
        lv_timer_del(caliScanTimer);
        caliScanTimer = NULL;
    }
}
