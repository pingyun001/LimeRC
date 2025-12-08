#include "Lime_App_Settings_ItemStorage.h"
#include "Lime_App_Hal.h"
#include <stdio.h>
#include <string.h>

#include "Lime_MessageBox.h"

#define SETTING_SELECT_COLOR_HEX 0xffd08d
#define SETTING_UNSELECT_COLOR_HEX 0xbffbff

static lv_obj_t* item_storage_obj = NULL;

static lv_obj_t* outputToSDDataObj = NULL;
static lv_obj_t* formatInternalFlashObj = NULL;
static lv_obj_t* formatSDCardObj = NULL;

static lv_obj_t* internalUsedLabel = NULL;
static lv_obj_t* internalUnUsedLabel = NULL;
static lv_obj_t* internalBarObj = NULL;
static lv_obj_t* externalBarObj = NULL;
static lv_obj_t* externalUsedLabel = NULL;
static lv_obj_t* externalUnUsedLabel = NULL;


LV_FONT_DECLARE(LR_AppSetting_ItemName);
LV_FONT_DECLARE(LR_AppSetting_ItemStorageIndiReguFont);
LV_FONT_DECLARE(LR_AppSetting_ItemStorageIndiBoldFont);

static int8_t nowSelectedItem = 1;      //-1:no select, 0:outputToSDCard, 1:formatInternalFlash, 2:formatSDCard
static int8_t lastSelectedItem = -1;    //-1:no select, 0:outputToSDCard, 1:formatInternalFlash, 2:formatSDCard
static int8_t nextSelectedItem = -1;    //-1:no select, 0:outputToSDCard, 1:formatInternalFlash, 2:formatSDCard

static lv_obj_t* getBgObjByIndex(int8_t index);
static void syncIndicatorInfo_FromHal(void);

#define TIME_FACE_SELF_ASSERT() do{\
    if(item_storage_obj == NULL){\
        LV_LOG_WARN("item_time_obj is NULL");\
        return;\
    }\
}while(0)

#define LIME_SETTING_CREATE_DEFAULT_BUTTON_ITEM(bgObj, fatherObj, name, nameColor)\
do\
{\
    bgObj = lv_obj_create(fatherObj);\
    lv_obj_set_size(bgObj, 188, 25);\
    lv_obj_set_style_bg_opa(bgObj, 200, LV_PART_MAIN | LV_STATE_DEFAULT);\
    lv_obj_set_style_bg_color(bgObj, lv_color_hex(SETTING_UNSELECT_COLOR_HEX), LV_PART_MAIN | LV_STATE_DEFAULT);\
    lv_obj_set_style_pad_all(bgObj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);\
    lv_obj_set_style_radius(bgObj, 8, LV_PART_MAIN | LV_STATE_DEFAULT);\
    lv_obj_set_style_shadow_width(bgObj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);\
    lv_obj_set_style_border_width(bgObj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);\
    \
    lv_obj_t* nameLabel = lv_label_create(bgObj);\
    lv_label_set_text(nameLabel, name);\
    lv_obj_set_size(nameLabel, 120, 15);\
    lv_obj_align(nameLabel, LV_ALIGN_CENTER, 0, 0);\
    lv_obj_set_style_text_font(nameLabel, &LR_AppSetting_ItemName, LV_PART_MAIN | LV_STATE_DEFAULT);\
    lv_obj_set_style_text_align(nameLabel, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_DEFAULT);\
    lv_obj_set_style_text_color(nameLabel, nameColor, LV_PART_MAIN | LV_STATE_DEFAULT);\
}while(0)

void Lime_App_Settings_ItemStorage_Create(lv_obj_t* father)
{
    item_storage_obj = lv_obj_create(father);
    lv_obj_set_size(item_storage_obj, lv_obj_get_width(father), lv_obj_get_height(father));
    lv_obj_set_style_bg_opa(item_storage_obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_color(item_storage_obj, lv_color_hex(SETTING_UNSELECT_COLOR_HEX), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_pad_all(item_storage_obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_radius(item_storage_obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_shadow_width(item_storage_obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_border_width(item_storage_obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);

    LIME_SETTING_CREATE_DEFAULT_BUTTON_ITEM(outputToSDDataObj, item_storage_obj, "导出数据到SD卡", lv_color_hex(0x000000));
    lv_obj_align_to(outputToSDDataObj, item_storage_obj, LV_ALIGN_TOP_MID, 0, 135);
    LIME_SETTING_CREATE_DEFAULT_BUTTON_ITEM(formatInternalFlashObj, item_storage_obj, "格式化机内存储", lv_color_hex(0xFF0000));
    lv_obj_align_to(formatInternalFlashObj, outputToSDDataObj, LV_ALIGN_OUT_BOTTOM_MID, 0, 7);
    LIME_SETTING_CREATE_DEFAULT_BUTTON_ITEM(formatSDCardObj, item_storage_obj, "格式化SD卡", lv_color_hex(0xFF0000));
    lv_obj_align_to(formatSDCardObj, formatInternalFlashObj, LV_ALIGN_OUT_BOTTOM_MID, 0, 7);

    nowSelectedItem = -1;
    lastSelectedItem = -1;
    nextSelectedItem = -1;

    lv_obj_t* indicatorObj = lv_obj_create(item_storage_obj);
    lv_obj_set_size(indicatorObj, 188, 90);
    lv_obj_align(indicatorObj, LV_ALIGN_TOP_MID, 0, 39);
    lv_obj_set_style_bg_opa(indicatorObj, 200, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_color(indicatorObj, lv_color_hex(0xbffbff), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_pad_all(indicatorObj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_radius(indicatorObj, 8, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_shadow_width(indicatorObj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_border_width(indicatorObj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);

    /* internal storage indicator */
    lv_obj_t* internalTitleLabel = lv_label_create(indicatorObj);
    lv_label_set_text(internalTitleLabel, "内部存储");
    lv_obj_set_size(internalTitleLabel, 38, 12);
    lv_obj_set_pos(internalTitleLabel, 8, 6);
    lv_obj_set_style_text_font(internalTitleLabel, &LR_AppSetting_ItemStorageIndiBoldFont, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_align(internalTitleLabel, LV_TEXT_ALIGN_LEFT, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_color(internalTitleLabel, lv_color_hex(0x000000), LV_PART_MAIN | LV_STATE_DEFAULT);

    internalUsedLabel = lv_label_create(indicatorObj);
    lv_label_set_text(internalUsedLabel, "1.4 MB/32 MB已使用");
    lv_obj_set_size(internalUsedLabel, 124, 12);
    lv_obj_align_to(internalUsedLabel, internalTitleLabel, LV_ALIGN_OUT_RIGHT_MID, 8, 0);
    lv_obj_set_style_text_font(internalUsedLabel, &LR_AppSetting_ItemStorageIndiReguFont, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_align(internalUsedLabel, LV_TEXT_ALIGN_RIGHT, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_color(internalUsedLabel, lv_color_hex(0x000000), LV_PART_MAIN | LV_STATE_DEFAULT);

    internalBarObj = lv_obj_create(indicatorObj);
    lv_obj_set_size(internalBarObj, 172, 14);
    lv_obj_align_to(internalBarObj, internalTitleLabel, LV_ALIGN_OUT_BOTTOM_LEFT, 0, 0);
    lv_obj_set_style_bg_opa(internalBarObj, 200, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_color(internalBarObj, lv_color_hex(0xf3f2f8), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_pad_all(internalBarObj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_radius(internalBarObj, 3, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_shadow_width(internalBarObj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_border_width(internalBarObj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_clear_flag(internalBarObj, LV_OBJ_FLAG_SCROLLABLE);
    lv_obj_t * internalBarPartAObj = lv_obj_create(internalBarObj);
    lv_obj_set_size(internalBarPartAObj, 60, 14);
    lv_obj_align_to(internalBarPartAObj, internalBarObj, LV_ALIGN_LEFT_MID, 0, 0);
    lv_obj_set_style_bg_opa(internalBarPartAObj, 255, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_color(internalBarPartAObj, lv_color_hex(0xff9501), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_pad_all(internalBarPartAObj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_radius(internalBarPartAObj, 3, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_shadow_width(internalBarPartAObj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_border_width(internalBarPartAObj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_t * internalBarPartBObj = lv_obj_create(internalBarObj);
    lv_obj_set_size(internalBarPartBObj, 30, 14);
    lv_obj_align_to(internalBarPartBObj, internalBarPartAObj, LV_ALIGN_OUT_RIGHT_MID, -3, 0);
    lv_obj_set_style_bg_opa(internalBarPartBObj, 255, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_color(internalBarPartBObj, lv_color_hex(0xff3b2f), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_pad_all(internalBarPartBObj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_radius(internalBarPartBObj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_shadow_width(internalBarPartBObj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_border_width(internalBarPartBObj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_t * internalBarPartCObj = lv_obj_create(internalBarObj);
    lv_obj_set_size(internalBarPartCObj, 15, 14);
    lv_obj_align_to(internalBarPartCObj, internalBarPartBObj, LV_ALIGN_OUT_RIGHT_MID, -3, 0);
    lv_obj_set_style_bg_opa(internalBarPartCObj, 255, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_color(internalBarPartCObj, lv_color_hex(0xc8c7cd), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_pad_all(internalBarPartCObj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_radius(internalBarPartCObj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_shadow_width(internalBarPartCObj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_border_width(internalBarPartCObj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);

    internalUnUsedLabel = lv_label_create(indicatorObj);
    lv_label_set_text(internalUnUsedLabel, "30.12 MB");
    lv_obj_set_size(internalUnUsedLabel, 124, 12);
    lv_obj_align_to(internalUnUsedLabel, internalBarObj, LV_ALIGN_RIGHT_MID, -6, 0);
    lv_obj_set_style_text_font(internalUnUsedLabel, &LR_AppSetting_ItemStorageIndiReguFont, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_align(internalUnUsedLabel, LV_TEXT_ALIGN_RIGHT, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_color(internalUnUsedLabel, lv_color_hex(0x1f1f1f), LV_PART_MAIN | LV_STATE_DEFAULT);

    /* sd card indicator */
    lv_obj_t* externalTitleLabel = lv_label_create(indicatorObj);
    lv_label_set_text(externalTitleLabel, "SD卡");
    lv_obj_set_size(externalTitleLabel, 38, 12);
    lv_obj_set_pos(externalTitleLabel, 8, 40);
    lv_obj_set_style_text_font(externalTitleLabel, &LR_AppSetting_ItemStorageIndiBoldFont, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_align(externalTitleLabel, LV_TEXT_ALIGN_LEFT, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_color(externalTitleLabel, lv_color_hex(0x000000), LV_PART_MAIN | LV_STATE_DEFAULT);

    externalUsedLabel = lv_label_create(indicatorObj);
    lv_label_set_text(externalUsedLabel, "1.4 MB/32 GB已使用");
    lv_obj_set_size(externalUsedLabel, 124, 12);
    lv_obj_align_to(externalUsedLabel, externalTitleLabel, LV_ALIGN_OUT_RIGHT_MID, 8, 0);
    lv_obj_set_style_text_font(externalUsedLabel, &LR_AppSetting_ItemStorageIndiReguFont, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_align(externalUsedLabel, LV_TEXT_ALIGN_RIGHT, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_color(externalUsedLabel, lv_color_hex(0x000000), LV_PART_MAIN | LV_STATE_DEFAULT);

    externalBarObj = lv_obj_create(indicatorObj);
    lv_obj_set_size(externalBarObj, 172, 14);
    lv_obj_align_to(externalBarObj, externalTitleLabel, LV_ALIGN_OUT_BOTTOM_LEFT, 0, 0);
    lv_obj_set_style_bg_opa(externalBarObj, 200, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_color(externalBarObj, lv_color_hex(0xf3f2f8), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_pad_all(externalBarObj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_radius(externalBarObj, 3, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_shadow_width(externalBarObj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_border_width(externalBarObj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_clear_flag(externalBarObj, LV_OBJ_FLAG_SCROLLABLE);
    lv_obj_t * externalBarPartAObj = lv_obj_create(externalBarObj);
    lv_obj_set_size(externalBarPartAObj, 45, 14);
    lv_obj_align_to(externalBarPartAObj, externalBarObj, LV_ALIGN_LEFT_MID, 0, 0);
    lv_obj_set_style_bg_opa(externalBarPartAObj, 255, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_color(externalBarPartAObj, lv_color_hex(0xff9501), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_pad_all(externalBarPartAObj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_radius(externalBarPartAObj, 3, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_shadow_width(externalBarPartAObj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_border_width(externalBarPartAObj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_t * externalBarPartBObj = lv_obj_create(externalBarObj);
    lv_obj_set_size(externalBarPartBObj, 19, 14);
    lv_obj_align_to(externalBarPartBObj, externalBarPartAObj, LV_ALIGN_OUT_RIGHT_MID, -3, 0);
    lv_obj_set_style_bg_opa(externalBarPartBObj, 255, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_color(externalBarPartBObj, lv_color_hex(0xff3b2f), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_pad_all(externalBarPartBObj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_radius(externalBarPartBObj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_shadow_width(externalBarPartBObj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_border_width(externalBarPartBObj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);

    externalUnUsedLabel = lv_label_create(indicatorObj);
    lv_label_set_text(externalUnUsedLabel, "31.66 GB");
    lv_obj_set_size(externalUnUsedLabel, 124, 12);
    lv_obj_align_to(externalUnUsedLabel, externalBarObj, LV_ALIGN_RIGHT_MID, -6, 0);
    lv_obj_set_style_text_font(externalUnUsedLabel, &LR_AppSetting_ItemStorageIndiReguFont, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_align(externalUnUsedLabel, LV_TEXT_ALIGN_RIGHT, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_color(externalUnUsedLabel, lv_color_hex(0x1f1f1f), LV_PART_MAIN | LV_STATE_DEFAULT);

    /* legend */
    lv_obj_t * legendAObj = lv_obj_create(indicatorObj);
    lv_obj_set_size(legendAObj, 6, 6);
    lv_obj_set_pos(legendAObj, 10, 76);
    lv_obj_set_style_bg_opa(legendAObj, 255, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_color(legendAObj, lv_color_hex(0xff9501), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_radius(legendAObj, 3, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_shadow_width(legendAObj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_border_width(legendAObj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_t * legendALabelObj = lv_label_create(indicatorObj);
    lv_label_set_text(legendALabelObj, "照片");
    lv_obj_set_size(legendALabelObj, 21, 12);
    lv_obj_align_to(legendALabelObj, legendAObj, LV_ALIGN_OUT_RIGHT_MID, 3, 0);
    lv_obj_set_style_text_font(legendALabelObj, &LR_AppSetting_ItemStorageIndiReguFont, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_align(legendALabelObj, LV_TEXT_ALIGN_LEFT, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_color(legendALabelObj, lv_color_hex(0x000000), LV_PART_MAIN | LV_STATE_DEFAULT);

    lv_obj_t * legendBObj = lv_obj_create(indicatorObj);
    lv_obj_set_size(legendBObj, 6, 6);
    lv_obj_align_to(legendBObj, legendAObj, LV_ALIGN_OUT_RIGHT_MID, 33, 0);
    lv_obj_set_style_bg_opa(legendBObj, 255, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_color(legendBObj, lv_color_hex(0xff3b2f), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_radius(legendBObj, 3, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_shadow_width(legendBObj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_border_width(legendBObj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_t * legendBLabelObj = lv_label_create(indicatorObj);
    lv_label_set_text(legendBLabelObj, "其它");
    lv_obj_set_size(legendBLabelObj, 21, 12);
    lv_obj_align_to(legendBLabelObj, legendBObj, LV_ALIGN_OUT_RIGHT_MID, 3, 0);
    lv_obj_set_style_text_font(legendBLabelObj, &LR_AppSetting_ItemStorageIndiReguFont, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_align(legendBLabelObj, LV_TEXT_ALIGN_LEFT, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_color(legendBLabelObj, lv_color_hex(0x000000), LV_PART_MAIN | LV_STATE_DEFAULT);

    lv_obj_t * legendCObj = lv_obj_create(indicatorObj);
    lv_obj_set_size(legendCObj, 6, 6);
    lv_obj_align_to(legendCObj, legendBObj, LV_ALIGN_OUT_RIGHT_MID, 33, 0);
    lv_obj_set_style_bg_opa(legendCObj, 255, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_color(legendCObj, lv_color_hex(0xadadad), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_radius(legendCObj, 3, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_shadow_width(legendCObj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_border_width(legendCObj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_t * legendCLabelObj = lv_label_create(indicatorObj);
    lv_label_set_text(legendCLabelObj, "青柠OS");
    lv_obj_set_size(legendCLabelObj, 35, 12);
    lv_obj_align_to(legendCLabelObj, legendCObj, LV_ALIGN_OUT_RIGHT_MID, 3, 0);
    lv_obj_set_style_text_font(legendCLabelObj, &LR_AppSetting_ItemStorageIndiReguFont, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_align(legendCLabelObj, LV_TEXT_ALIGN_LEFT, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_color(legendCLabelObj, lv_color_hex(0x000000), LV_PART_MAIN | LV_STATE_DEFAULT);

    syncIndicatorInfo_FromHal();

}
void Lime_App_Settings_ItemStorage_Destroy(void)
{
    if(item_storage_obj != NULL)
    {
        lv_obj_del(item_storage_obj);
        item_storage_obj = NULL;
    }
}

static lv_obj_t* getBgObjByIndex(int8_t index)
{
    switch(index)
    {
        case 0:
            return outputToSDDataObj;
        case 1:
            return formatInternalFlashObj;
        case 2:
            return formatSDCardObj;
        default:
            LV_LOG_ERROR("index error");
            return NULL;
    }
}

static void syncIndicatorInfo_FromHal(void)
{
    TIME_FACE_SELF_ASSERT();

    LimeHAL_SettingInfo_t *halSettingInfoPin = &LimeHAL_GetInfoPin()->settingInfo;

    static char tempStr[30];
    float fz, fm, per;

    fz = halSettingInfoPin->storage_FlashUsedPicMB +
        halSettingInfoPin->storage_FlashUsedOthersMB +
        halSettingInfoPin->storage_FlashUsedLimeOSMB;
    fm = halSettingInfoPin->storage_FlashTotalMB;
    fm = (fm < 0) ? 0 : (fm > 999) ? 999 : fm;
    fz = (fz < 0) ? 0 : (fz > fm) ? fm : fz;

    memset(tempStr, 0, sizeof(tempStr));
    snprintf(tempStr, sizeof(tempStr), "%.1f %s", fz > 1024 ? (fz / 1024) : fz, (fz > 1024) ? "GB" : "MB");
    snprintf(tempStr + strlen(tempStr), sizeof(tempStr) - strlen(tempStr), "/%.1f %s已使用", fm > 1024 ? (fm / 1024) : fm, (fm > 1024) ? "GB" : "MB");
    lv_label_set_text(internalUsedLabel, tempStr);

    memset(tempStr, 0, sizeof(tempStr));
    snprintf(tempStr, sizeof(tempStr), "%.2f %s", (fm - fz > 1024) ? ((fm - fz) / 1024) : (fm - fz), (fm - fz > 1024) ? "GB" : "MB");
    lv_label_set_text(internalUnUsedLabel, tempStr);

    per = (halSettingInfoPin->storage_FlashUsedPicMB / fm) * 100;
    per = (per < 5) ? 5 : (per > 95) ? 95 : per;
    lv_obj_set_width(lv_obj_get_child(internalBarObj, 0), 172 * per / 100);
    lv_obj_align_to(lv_obj_get_child(internalBarObj, 0), internalBarObj, LV_ALIGN_LEFT_MID, 0, 0);

    per = (halSettingInfoPin->storage_FlashUsedOthersMB / fm) * 100;
    per = (per < 5) ? 5 : (per > 95) ? 95 : per;
    lv_obj_set_width(lv_obj_get_child(internalBarObj, 1), 172 * per / 100);
    lv_obj_align_to(lv_obj_get_child(internalBarObj, 1), lv_obj_get_child(internalBarObj, 0), LV_ALIGN_OUT_RIGHT_MID, -3, 0);

    per = (halSettingInfoPin->storage_FlashUsedLimeOSMB / fm) * 100;
    per = (per < 5) ? 5 : (per > 95) ? 95 : per;
    lv_obj_set_width(lv_obj_get_child(internalBarObj, 2), 172 * per / 100);
    lv_obj_align_to(lv_obj_get_child(internalBarObj, 2), lv_obj_get_child(internalBarObj, 1), LV_ALIGN_OUT_RIGHT_MID, -3, 0);


    /* SD Card */
    fz = halSettingInfoPin->storage_SDCardUsedPicMB +
        halSettingInfoPin->storage_SDCardUsedOthersMB;
    fm = halSettingInfoPin->storage_SDCardTotalMB;
    fm = (fm < 0) ? 0 : (fm > 256 * 1024) ? (256 * 1024) : fm;
    fz = (fz < 0) ? 0 : (fz > fm) ? fm : fz;

    memset(tempStr, 0, sizeof(tempStr));
    snprintf(tempStr, sizeof(tempStr), "%.1f %s", fz > 1024 ? (fz / 1024) : fz, (fz > 1024) ? "GB" : "MB");
    snprintf(tempStr + strlen(tempStr), sizeof(tempStr) - strlen(tempStr), "/%.1f %s已使用", fm > 1024 ? (fm / 1024) : fm, (fm > 1024) ? "GB" : "MB");
    lv_label_set_text(externalUsedLabel, tempStr);

    memset(tempStr, 0, sizeof(tempStr));
    snprintf(tempStr, sizeof(tempStr), "%.2f %s", (fm - fz > 1024) ? ((fm - fz) / 1024) : (fm - fz), (fm - fz > 1024) ? "GB" : "MB");
    lv_label_set_text(externalUnUsedLabel, tempStr);

    per = (halSettingInfoPin->storage_SDCardUsedPicMB / fm) * 100;
    per = (per < 5) ? 5 : (per > 95) ? 95 : per;
    per = (fm == 0) ? 0 : per;
    lv_obj_set_width(lv_obj_get_child(externalBarObj, 0), 172 * per / 100);
    lv_obj_align_to(lv_obj_get_child(externalBarObj, 0), externalBarObj, LV_ALIGN_LEFT_MID, 0, 0);

    per = (halSettingInfoPin->storage_SDCardUsedOthersMB / fm) * 100;
    per = (per < 5) ? 5 : (per > 95) ? 95 : per;
    per = (fm == 0) ? 0 : per;
    lv_obj_set_width(lv_obj_get_child(externalBarObj, 1), 172 * per / 100);
    lv_obj_align_to(lv_obj_get_child(externalBarObj, 1), lv_obj_get_child(externalBarObj, 0), LV_ALIGN_OUT_RIGHT_MID, -3, 0);
}

void Lime_App_Settings_ItemStorage_Update(Lime_App_KeyOptions_e keyOption)
{
    TIME_FACE_SELF_ASSERT();

    switch(keyOption)
    {
        case Lime_App_KeyOptions_Up:
        {
            if(nowSelectedItem == -1)
            {
                nowSelectedItem = 0;
                lastSelectedItem = 0;
                nextSelectedItem = 0;
            }
            else
            {
                nowSelectedItem = (nowSelectedItem <= 0) ? 2 : (nowSelectedItem - 1);
                lastSelectedItem = (nowSelectedItem == 0) ? 2 : (nowSelectedItem - 1);
                nextSelectedItem = (nowSelectedItem == 2) ? 0 : (nowSelectedItem + 1);
            }
            lv_obj_set_style_bg_color(getBgObjByIndex(lastSelectedItem), lv_color_hex(SETTING_UNSELECT_COLOR_HEX), LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_bg_color(getBgObjByIndex(nextSelectedItem), lv_color_hex(SETTING_UNSELECT_COLOR_HEX), LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_bg_color(getBgObjByIndex(nowSelectedItem), lv_color_hex(SETTING_SELECT_COLOR_HEX), LV_PART_MAIN | LV_STATE_DEFAULT);

            break;
        }
        case Lime_App_KeyOptions_Down:
        {
            if(nowSelectedItem == -1)
            {
                nowSelectedItem = 0;
                lastSelectedItem = 0;
                nextSelectedItem = 0;
            }
            else
            {
                nowSelectedItem = (nowSelectedItem >= 2) ? 0 : (nowSelectedItem + 1);
                lastSelectedItem = (nowSelectedItem == 2) ? 0 : (nowSelectedItem + 1);
                nextSelectedItem = (nowSelectedItem == 0) ? 2 : (nowSelectedItem - 1);
            }
            lv_obj_set_style_bg_color(getBgObjByIndex(lastSelectedItem), lv_color_hex(SETTING_UNSELECT_COLOR_HEX), LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_bg_color(getBgObjByIndex(nextSelectedItem), lv_color_hex(SETTING_UNSELECT_COLOR_HEX), LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_bg_color(getBgObjByIndex(nowSelectedItem), lv_color_hex(SETTING_SELECT_COLOR_HEX), LV_PART_MAIN | LV_STATE_DEFAULT);

            break;
        }
        case Lime_App_KeyOptions_Set:
        {
            switch(nowSelectedItem)
            {
                case 0:
                {
                    Lime_MessageBox_Show("进度", "正在进行复制，请勿进行其它操作", LimeHAL_GetCopyAllDataToSDCardPercent(), 10000);
                    LimeHAL_CopyAllDataToSDCard();
                    break;
                }
                case 1:
                {
                    Lime_MessageBox_Show("进度", "正在格式化内部存储，请勿进行其它操作", LimeHAL_GetFormatInternalStoragePercent(), 10000);
                    LimeHAL_FormatInternalStorage();
                    break;
                }
                case 2:
                {
                    Lime_MessageBox_Show("进度", "正在格式化SD卡，请勿进行其它操作", LimeHAL_GetFormatExternalStoragePercent(), 10000);
                    LimeHAL_FormatExternalStorage();
                    break;
                }
            }
            break;
        }
        default:
        {
            syncIndicatorInfo_FromHal();
            break;
        }
    }
}
