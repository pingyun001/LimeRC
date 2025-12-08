#include "Lime_App_Settings_ItemAboutMe.h"
#include <stdio.h>

#define SETTING_SELECT_COLOR_HEX 0xffd08d
#define SETTING_UNSELECT_COLOR_HEX 0xffffff

LV_FONT_DECLARE(LR_AppSetting_ItemName);
LV_FONT_DECLARE(LR_AppSetting_ItemExplain);
LV_IMG_DECLARE(LR_StartLogo);


static lv_obj_t* item_aboutMe_obj = NULL;
static lv_obj_t* mainNameObj = NULL;
static lv_obj_t* softVersionObj = NULL;
static lv_obj_t* bulidTimeObj = NULL;

static lv_obj_t* hwPCBVersionObj = NULL;
static lv_obj_t* hwCPUObj = NULL;
static lv_obj_t* mainFreqObj = NULL;

void Lime_App_Settings_ItemAboutMe_Create(lv_obj_t* father)
{
    item_aboutMe_obj = lv_obj_create(father);
    lv_obj_set_size(item_aboutMe_obj, lv_obj_get_width(father), lv_obj_get_height(father));
    lv_obj_set_style_bg_opa(item_aboutMe_obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_color(item_aboutMe_obj, lv_color_hex(0x2f2f2f), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_pad_all(item_aboutMe_obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_radius(item_aboutMe_obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_shadow_width(item_aboutMe_obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_border_width(item_aboutMe_obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);

    /* software version */
    lv_obj_t* swVersionObj = lv_obj_create(item_aboutMe_obj);
    lv_obj_set_size(swVersionObj, 188, 65);
    lv_obj_align(swVersionObj, LV_ALIGN_TOP_MID, 0, 62);
    lv_obj_set_style_bg_opa(swVersionObj, 200, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_color(swVersionObj, lv_color_hex(SETTING_UNSELECT_COLOR_HEX), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_pad_all(swVersionObj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_radius(swVersionObj, 8, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_shadow_width(swVersionObj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_border_width(swVersionObj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);

    lv_obj_t* mainNameTitleObj = lv_label_create(swVersionObj);
    lv_label_set_text(mainNameTitleObj, "名称");
    lv_obj_set_size(mainNameTitleObj, 30, 13);
    lv_obj_align(mainNameTitleObj, LV_ALIGN_TOP_LEFT, 8, 4);
    lv_obj_set_style_text_font(mainNameTitleObj, &LR_AppSetting_ItemName, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_align(mainNameTitleObj, LV_TEXT_ALIGN_LEFT, LV_PART_MAIN | LV_STATE_DEFAULT);
    mainNameObj = lv_label_create(swVersionObj);
    lv_label_set_text(mainNameObj, "LimeRC");
    lv_obj_set_size(mainNameObj, 68, 13);
    lv_obj_align(mainNameObj, LV_ALIGN_TOP_RIGHT, -9, 6);
    lv_obj_set_style_text_font(mainNameObj, &LR_AppSetting_ItemExplain, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_align(mainNameObj, LV_TEXT_ALIGN_RIGHT, LV_PART_MAIN | LV_STATE_DEFAULT);

    lv_obj_t* particitionObj = lv_obj_create(swVersionObj);
    lv_obj_set_size(particitionObj, 170, 1);
    lv_obj_align(particitionObj, LV_ALIGN_TOP_LEFT, 8, 21);
    lv_obj_set_style_bg_color(particitionObj, lv_color_hex(0x1f1f1f), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_pad_all(particitionObj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_radius(particitionObj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_shadow_width(particitionObj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_border_width(particitionObj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);

    lv_obj_t* softVertionTitleObj = lv_label_create(swVersionObj);
    lv_label_set_text(softVertionTitleObj, "软件版本");
    lv_obj_set_size(softVertionTitleObj, 50, 13);
    lv_obj_align(softVertionTitleObj, LV_ALIGN_TOP_LEFT, 8, 25);
    lv_obj_set_style_text_font(softVertionTitleObj, &LR_AppSetting_ItemName, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_align(softVertionTitleObj, LV_TEXT_ALIGN_LEFT, LV_PART_MAIN | LV_STATE_DEFAULT);
    softVersionObj = lv_label_create(swVersionObj);
    lv_label_set_text(softVersionObj, GLOBAL_SOFTWARE_VERSION);
    lv_obj_set_size(softVersionObj, 68, 13);
    lv_obj_align(softVersionObj, LV_ALIGN_TOP_RIGHT, -9, 27);
    lv_obj_set_style_text_font(softVersionObj, &LR_AppSetting_ItemExplain, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_align(softVersionObj, LV_TEXT_ALIGN_RIGHT, LV_PART_MAIN | LV_STATE_DEFAULT);

    particitionObj = lv_obj_create(swVersionObj);
    lv_obj_set_size(particitionObj, 170, 1);
    lv_obj_align(particitionObj, LV_ALIGN_TOP_LEFT, 8, 42);
    lv_obj_set_style_bg_color(particitionObj, lv_color_hex(0x1f1f1f), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_pad_all(particitionObj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_radius(particitionObj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_shadow_width(particitionObj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_border_width(particitionObj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);

		char bulidStr[32] = {0};
    snprintf(bulidStr, sizeof(bulidStr), "%s,%s", __TIME__, __DATE__);
    lv_obj_t* bulidTimeTitleObj = lv_label_create(swVersionObj);
    lv_label_set_text(bulidTimeTitleObj, "构建于");
    lv_obj_set_size(bulidTimeTitleObj, 50, 13);
    lv_obj_align(bulidTimeTitleObj, LV_ALIGN_TOP_LEFT, 8, 46);
    lv_obj_set_style_text_font(bulidTimeTitleObj, &LR_AppSetting_ItemName, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_align(bulidTimeTitleObj, LV_TEXT_ALIGN_LEFT, LV_PART_MAIN | LV_STATE_DEFAULT);
    bulidTimeObj = lv_label_create(swVersionObj);
    lv_label_set_text(bulidTimeObj, bulidStr);
    lv_obj_set_size(bulidTimeObj, 107, 13);
    lv_obj_align(bulidTimeObj, LV_ALIGN_TOP_RIGHT, -9, 48);
    lv_obj_set_style_text_font(bulidTimeObj, &LR_AppSetting_ItemExplain, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_align(bulidTimeObj, LV_TEXT_ALIGN_RIGHT, LV_PART_MAIN | LV_STATE_DEFAULT);

    /* hardware version */
    lv_obj_t* hwVersionObj = lv_obj_create(item_aboutMe_obj);
    lv_obj_set_size(hwVersionObj, 188, 65);
    lv_obj_align_to(hwVersionObj, swVersionObj, LV_ALIGN_OUT_BOTTOM_MID, 0, 5);
    lv_obj_set_style_bg_opa(hwVersionObj, 200, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_color(hwVersionObj, lv_color_hex(SETTING_UNSELECT_COLOR_HEX), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_pad_all(hwVersionObj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_radius(hwVersionObj, 8, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_shadow_width(hwVersionObj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_border_width(hwVersionObj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);

    lv_obj_t* hwPCBVersionTitleObj = lv_label_create(hwVersionObj);
    lv_label_set_text(hwPCBVersionTitleObj, "硬件版本");
    lv_obj_set_size(hwPCBVersionTitleObj, 50, 13);
    lv_obj_align(hwPCBVersionTitleObj, LV_ALIGN_TOP_LEFT, 8, 4);
    lv_obj_set_style_text_font(hwPCBVersionTitleObj, &LR_AppSetting_ItemName, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_align(hwPCBVersionTitleObj, LV_TEXT_ALIGN_LEFT, LV_PART_MAIN | LV_STATE_DEFAULT);
    hwPCBVersionObj = lv_label_create(hwVersionObj);
    lv_label_set_text(hwPCBVersionObj, "V3.1");
    lv_obj_set_size(hwPCBVersionObj, 68, 13);
    lv_obj_align(hwPCBVersionObj, LV_ALIGN_TOP_RIGHT, -9, 6);
    lv_obj_set_style_text_font(hwPCBVersionObj, &LR_AppSetting_ItemExplain, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_align(hwPCBVersionObj, LV_TEXT_ALIGN_RIGHT, LV_PART_MAIN | LV_STATE_DEFAULT);

    particitionObj = lv_obj_create(hwVersionObj);
    lv_obj_set_size(particitionObj, 170, 1);
    lv_obj_align(particitionObj, LV_ALIGN_TOP_LEFT, 8, 21);
    lv_obj_set_style_bg_color(particitionObj, lv_color_hex(0x1f1f1f), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_pad_all(particitionObj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_radius(particitionObj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_shadow_width(particitionObj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_border_width(particitionObj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);

    lv_obj_t* hwCPUTitleObj = lv_label_create(hwVersionObj);
    lv_label_set_text(hwCPUTitleObj, "处理器");
    lv_obj_set_size(hwCPUTitleObj, 50, 13);
    lv_obj_align(hwCPUTitleObj, LV_ALIGN_TOP_LEFT, 8, 25);
    lv_obj_set_style_text_font(hwCPUTitleObj, &LR_AppSetting_ItemName, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_align(hwCPUTitleObj, LV_TEXT_ALIGN_LEFT, LV_PART_MAIN | LV_STATE_DEFAULT);
    hwCPUObj = lv_label_create(hwVersionObj);
    lv_label_set_text(hwCPUObj, "Cortex-M7");
    lv_obj_set_size(hwCPUObj, 68, 13);
    lv_obj_align(hwCPUObj, LV_ALIGN_TOP_RIGHT, -9, 27);
    lv_obj_set_style_text_font(hwCPUObj, &LR_AppSetting_ItemExplain, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_align(hwCPUObj, LV_TEXT_ALIGN_RIGHT, LV_PART_MAIN | LV_STATE_DEFAULT);

    particitionObj = lv_obj_create(hwVersionObj);
    lv_obj_set_size(particitionObj, 170, 1);
    lv_obj_align(particitionObj, LV_ALIGN_TOP_LEFT, 8, 42);
    lv_obj_set_style_bg_color(particitionObj, lv_color_hex(0x1f1f1f), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_pad_all(particitionObj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_radius(particitionObj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_shadow_width(particitionObj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_border_width(particitionObj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);

    lv_obj_t* mainFreqTitleObj = lv_label_create(hwVersionObj);
    lv_label_set_text(mainFreqTitleObj, "当前主频");
    lv_obj_set_size(mainFreqTitleObj, 50, 13);
    lv_obj_align(mainFreqTitleObj, LV_ALIGN_TOP_LEFT, 8, 46);
    lv_obj_set_style_text_font(mainFreqTitleObj, &LR_AppSetting_ItemName, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_align(mainFreqTitleObj, LV_TEXT_ALIGN_LEFT, LV_PART_MAIN | LV_STATE_DEFAULT);
    mainFreqObj = lv_label_create(hwVersionObj);
    lv_label_set_text(mainFreqObj, "480MHz");
    lv_obj_set_size(mainFreqObj, 107, 13);
    lv_obj_align(mainFreqObj, LV_ALIGN_TOP_RIGHT, -9, 48);
    lv_obj_set_style_text_font(mainFreqObj, &LR_AppSetting_ItemExplain, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_align(mainFreqObj, LV_TEXT_ALIGN_RIGHT, LV_PART_MAIN | LV_STATE_DEFAULT);

    /* author */
    lv_obj_t* authorObj = lv_obj_create(item_aboutMe_obj);
    lv_obj_set_size(authorObj, 188, 25);
    lv_obj_align(authorObj, LV_ALIGN_TOP_MID, 0, 202);
    lv_obj_set_style_bg_opa(authorObj, 200, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_color(authorObj, lv_color_hex(SETTING_UNSELECT_COLOR_HEX), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_pad_all(authorObj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_radius(authorObj, 8, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_shadow_width(authorObj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_border_width(authorObj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_t* authorLabelObj = lv_label_create(authorObj);
    lv_label_set_text(authorLabelObj, "bilibili 平韵の小窝 于 2023夏");
    lv_obj_set_size(authorLabelObj, 180, 17);
    lv_obj_align(authorLabelObj, LV_ALIGN_CENTER, 0, 2);
    lv_obj_set_style_text_font(authorLabelObj, &LR_AppSetting_ItemName, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_align(authorLabelObj, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_label_set_long_mode(authorLabelObj, LV_LABEL_LONG_SCROLL_CIRCULAR);

    lv_obj_t* limeLogoImg = lv_img_create(item_aboutMe_obj);
    lv_img_set_src(limeLogoImg, &LR_StartLogo);
    lv_obj_align(limeLogoImg, LV_ALIGN_TOP_MID, 0, 20);
    lv_img_set_zoom(limeLogoImg, 180);
}
void Lime_App_Settings_ItemAboutMe_Destroy(void)
{
    if(item_aboutMe_obj!= NULL)
    {
        lv_obj_del(item_aboutMe_obj);
        item_aboutMe_obj = NULL;
    }
}
void Lime_App_Settings_ItemAboutMe_Update(Lime_App_KeyOptions_e keyOption)
{

}
