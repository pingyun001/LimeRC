#include "Lime_App_StartFace.h"
#include "Lime_App_Hal.h"

#include <stdio.h>

static lv_obj_t *startFaceObj = NULL;
static lv_obj_t *initPercentBarBgObj  = NULL;
static lv_obj_t *initPercentBarFpObj  = NULL;
static lv_obj_t *logoImg = NULL;
static lv_obj_t *initPercentTextObj = NULL;
static lv_obj_t* initInfoTextObj = NULL;
static lv_anim_t logoInOutAnim;
static lv_anim_t perBarInOutAnim;

static void timer_cb(lv_timer_t * timer);
static void enter_anim_cb(lv_anim_t * anim);
static void exit_anim_cb(lv_anim_t * anim);
static void set_percent_bar_value(uint8_t percent);
static void refreshInitInfoText(const char* text);

LV_IMG_DECLARE(LR_StartBG);
LV_IMG_DECLARE(LR_StartLogo);
LV_FONT_DECLARE(LR_StartFace_PerFont);
LV_FONT_DECLARE(LR_StartFace_InfoFont);

void Lime_App_StartFace_Create(lv_obj_t* father, uint16_t width, uint16_t height)
{
    if(father == NULL)
    {
        LV_LOG_ERROR("father is NULL");
        return;
    }

    if(startFaceObj != NULL)
    {
        LV_LOG_ERROR("StartFaceObj is not NULL");
        return;
    }

    startFaceObj = lv_obj_create(lv_scr_act());
    lv_obj_set_size(startFaceObj, width, height);
    lv_obj_center(startFaceObj);
    lv_obj_set_style_bg_opa(startFaceObj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_pad_all(startFaceObj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_radius(startFaceObj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_shadow_width(startFaceObj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_border_width(startFaceObj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_clear_flag(startFaceObj, LV_OBJ_FLAG_CLICKABLE);
    lv_obj_clear_flag(startFaceObj, LV_OBJ_FLAG_SCROLLABLE);
    lv_obj_set_style_bg_img_src(startFaceObj, &LR_StartBG, LV_PART_MAIN | LV_STATE_DEFAULT);


    logoImg = lv_img_create(startFaceObj);
    lv_img_set_src(logoImg, &LR_StartLogo);
    lv_obj_align(logoImg, LV_ALIGN_TOP_MID, 0, 41);

    initPercentBarBgObj = lv_obj_create(startFaceObj);
    lv_obj_set_size(initPercentBarBgObj, 150, 16);
    lv_obj_align(initPercentBarBgObj, LV_ALIGN_TOP_MID, 0, 160);
    lv_obj_set_style_bg_color(initPercentBarBgObj, lv_color_hex(0xFFFFFF), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_pad_all(initPercentBarBgObj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_radius(initPercentBarBgObj, 8, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_shadow_width(initPercentBarBgObj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_border_width(initPercentBarBgObj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_clear_flag(initPercentBarBgObj, LV_OBJ_FLAG_CLICKABLE);
    lv_obj_clear_flag(initPercentBarBgObj, LV_OBJ_FLAG_SCROLLABLE);

    initPercentBarFpObj = lv_obj_create(initPercentBarBgObj);
    lv_obj_set_size(initPercentBarFpObj, 146, 12);
    lv_obj_set_pos(initPercentBarFpObj, 2, 2);
    lv_obj_set_style_bg_color(initPercentBarFpObj, lv_color_hex(0xf0db59), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_pad_all(initPercentBarFpObj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_radius(initPercentBarFpObj, 6, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_shadow_width(initPercentBarFpObj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_border_width(initPercentBarFpObj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_clear_flag(initPercentBarFpObj, LV_OBJ_FLAG_CLICKABLE);
    lv_obj_clear_flag(initPercentBarFpObj, LV_OBJ_FLAG_SCROLLABLE);

    initPercentTextObj = lv_label_create(initPercentBarFpObj);
    lv_obj_set_size(initPercentTextObj, 24, 10);
    lv_label_set_text(initPercentTextObj, "0");
    lv_obj_align(initPercentTextObj, LV_ALIGN_RIGHT_MID, -2, 1);
    lv_obj_set_style_text_font(initPercentTextObj, &LR_StartFace_PerFont, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_color(initPercentTextObj, lv_color_hex(0xFFFFFF), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_align(initPercentTextObj, LV_TEXT_ALIGN_RIGHT, LV_PART_MAIN | LV_STATE_DEFAULT);

    set_percent_bar_value(0);

    initInfoTextObj = lv_label_create(startFaceObj);
    lv_obj_set_size(initInfoTextObj, 144, 10);
    lv_label_set_text(initInfoTextObj, " ");
    lv_obj_align_to(initInfoTextObj, initPercentBarBgObj, LV_ALIGN_OUT_BOTTOM_LEFT, 4, 3);
    lv_obj_set_style_text_font(initInfoTextObj, &LR_StartFace_InfoFont, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_color(initInfoTextObj, lv_color_hex(0xFFFFFF), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_align(initInfoTextObj, LV_TEXT_ALIGN_LEFT, LV_PART_MAIN | LV_STATE_DEFAULT);

    /* animation initialization */
    lv_anim_init(&logoInOutAnim);
    lv_anim_set_var(&logoInOutAnim, logoImg);
    lv_anim_set_exec_cb(&logoInOutAnim, (lv_anim_exec_xcb_t)lv_obj_set_y);
    lv_anim_set_values(&logoInOutAnim, -78, 41);
    lv_anim_set_time(&logoInOutAnim, 1000);
    lv_anim_set_path_cb(&logoInOutAnim, lv_anim_path_bounce);
    lv_anim_set_ready_cb(&logoInOutAnim, enter_anim_cb);

    lv_anim_init(&perBarInOutAnim);
    lv_anim_set_var(&perBarInOutAnim, initPercentBarBgObj);
    lv_anim_set_exec_cb(&perBarInOutAnim, (lv_anim_exec_xcb_t)lv_obj_set_y);
    lv_anim_set_values(&perBarInOutAnim, 240 + 20, 160);
    lv_anim_set_time(&perBarInOutAnim, 1000);
    lv_anim_set_delay(&perBarInOutAnim, 100);
    lv_anim_set_path_cb(&perBarInOutAnim, lv_anim_path_bounce);

    lv_anim_start(&logoInOutAnim);
    lv_anim_start(&perBarInOutAnim);
    //when animation is done, call the enter_anim_cb()
}

static void enter_anim_cb(lv_anim_t * anim)
{
    lv_timer_t * timer = lv_timer_create(timer_cb, 6, NULL);
    //start timer, then call the timer_cb()
}

static void timer_cb(lv_timer_t * timer)
{
    static uint8_t testPercent = 0;
    const LimeHAL_Hardware_Online_Info_t* initInfoPin = (const LimeHAL_Hardware_Online_Info_t*)&LimeHAL_GetInfoPin()->onlineInfo;
    uint8_t percentMax = initInfoPin->initPercent;

    set_percent_bar_value(testPercent);

    refreshInitInfoText(initInfoPin->hardwareName);

    testPercent += 1;
    if(testPercent > percentMax)
        testPercent = percentMax;

    /* check if all hardware pass, run exit animation */
    if(initInfoPin->allHardwarePass && (testPercent == percentMax))
    {
        lv_timer_del(timer);

        /* running exit animation */
        refreshInitInfoText(" ");
        lv_anim_set_values(&logoInOutAnim, 41, -90);
        lv_anim_set_values(&perBarInOutAnim, 160, 240 + 20);
        lv_anim_set_ready_cb(&logoInOutAnim, exit_anim_cb);
        lv_anim_start(&logoInOutAnim);
        lv_anim_start(&perBarInOutAnim);
        //when animation is done, call the exit_anim_cb()
    }
}

static void exit_anim_cb(lv_anim_t * anim)
{
    if(startFaceObj == NULL)
    {
        LV_LOG_ERROR("startFaceObj is NULL");
        return;
    }
    lv_obj_del(startFaceObj);
    startFaceObj = NULL;

    /* all finish, run another interface*/
    Lime_App_StartFace_Finish_Hook();
}
//all finish, run another interface

/*-----------------static function------------------*/

static void set_percent_bar_value(uint8_t percent)
{
    char tempBuf[6] = {0};
    if(initPercentBarFpObj == NULL)
    {
        LV_LOG_ERROR("initPercentBarFpObj is NULL");
        return;
    }
    if(initPercentTextObj == NULL)
    {
        LV_LOG_ERROR("initPercentTextObj is NULL");
        return;
    }

    if(percent > 100)
        percent = 100;

    snprintf(tempBuf, 5, "%d", percent);

    lv_label_set_text(initPercentTextObj, tempBuf);
    lv_obj_set_width(initPercentBarFpObj, (percent * 146) / 100);
}

static void refreshInitInfoText(const char* text)
{
    char *tempReadText = NULL;
    if(initInfoTextObj == NULL)
    {
        LV_LOG_ERROR("initInfoTextObj is NULL");
        return;
    }

    tempReadText = lv_label_get_text(initInfoTextObj);

    if(lv_strcmp(tempReadText, text) == 0)
        return;

    lv_label_set_text(initInfoTextObj, text);
}

/*
__weak void Lime_App_StartFace_Finish_Hook(void)
{
    #error "You need to implement Lime_App_StartFace_Finish_Hook in your application"
}
*/
