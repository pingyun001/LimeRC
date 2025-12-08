#include "Lime_App_Settings_ItemScreen.h"
#include "Lime_App_Hal.h"

#define SETTING_SELECT_COLOR_HEX 0xffd08d
#define SETTING_UNSELECT_COLOR_HEX 0xffffff

static lv_obj_t* item_screen_obj = NULL;
static lv_obj_t* autoLockBgObj = NULL;
static lv_obj_t* autoLockSelectedObj = NULL;
static lv_obj_t* autoTurnOffBgObj = NULL;
static lv_obj_t* autoTurnOffSelectObj = NULL;

static int8_t nowSelectedItem = 1;     //-1:no select, 0:screenLock, 1:screenTurnOff

LV_IMG_DECLARE(LR_Setting_LeftIcon);
LV_IMG_DECLARE(LR_Setting_RightIcon);
LV_FONT_DECLARE(LR_AppSetting_ItemName);

static void setScreenReduceLightTimeLabelByIndex(uint16_t ReduceLightTime);
static void setScreenTurnOffLabelByIndex(uint16_t turnOffTime);

void Lime_App_Settings_ItemScreen_Create(lv_obj_t* father)
{
    nowSelectedItem = -1;

    item_screen_obj = lv_obj_create(father);
    lv_obj_set_size(item_screen_obj, lv_obj_get_width(father), lv_obj_get_height(father));
    lv_obj_set_style_bg_opa(item_screen_obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_color(item_screen_obj, lv_color_hex(0xbffbff), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_pad_all(item_screen_obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_radius(item_screen_obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_shadow_width(item_screen_obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_border_width(item_screen_obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);

    autoLockBgObj = lv_obj_create(item_screen_obj);
    lv_obj_set_size(autoLockBgObj, 188, 25);
    lv_obj_align(autoLockBgObj, LV_ALIGN_TOP_MID, 0, 38);
    lv_obj_set_style_bg_opa(autoLockBgObj, 200, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_color(autoLockBgObj, lv_color_hex(SETTING_UNSELECT_COLOR_HEX), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_pad_all(autoLockBgObj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_radius(autoLockBgObj, 8, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_shadow_width(autoLockBgObj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_border_width(autoLockBgObj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);

    lv_obj_t* agreementLabel = lv_label_create(autoLockBgObj);
    lv_label_set_text(agreementLabel, "自动减弱背光");
    lv_obj_set_size(agreementLabel, 84, 15);
    lv_obj_align(agreementLabel, LV_ALIGN_LEFT_MID, 8, 0);
    lv_obj_set_style_text_font(agreementLabel, &LR_AppSetting_ItemName, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_align(agreementLabel, LV_TEXT_ALIGN_LEFT, LV_PART_MAIN | LV_STATE_DEFAULT);

    lv_obj_t * rightIcon = lv_img_create(autoLockBgObj);
    lv_img_set_src(rightIcon, &LR_Setting_RightIcon);
    lv_obj_align_to(rightIcon, autoLockBgObj, LV_ALIGN_RIGHT_MID, -9, 0);

    autoLockSelectedObj = lv_label_create(autoLockBgObj);
    lv_label_set_text(autoLockSelectedObj, "永不");
    lv_obj_set_size(autoLockSelectedObj, 44, 15);
    lv_obj_align_to(autoLockSelectedObj, rightIcon, LV_ALIGN_OUT_LEFT_MID, -5, 1);
    lv_obj_set_style_text_font(autoLockSelectedObj, &LR_AppSetting_ItemName, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_align(autoLockSelectedObj, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_DEFAULT);

    lv_obj_t * leftIcon = lv_img_create(autoLockBgObj);
    lv_img_set_src(leftIcon, &LR_Setting_LeftIcon);
    lv_obj_align_to(leftIcon, autoLockSelectedObj, LV_ALIGN_OUT_LEFT_MID, -5, -1);

    autoTurnOffBgObj = lv_obj_create(item_screen_obj);
    lv_obj_set_size(autoTurnOffBgObj, 188, 25);
    lv_obj_align_to(autoTurnOffBgObj, autoLockBgObj, LV_ALIGN_OUT_BOTTOM_MID, 0, 6);
    lv_obj_set_style_bg_opa(autoTurnOffBgObj, 200, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_color(autoTurnOffBgObj, lv_color_hex(SETTING_UNSELECT_COLOR_HEX), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_pad_all(autoTurnOffBgObj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_radius(autoTurnOffBgObj, 8, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_shadow_width(autoTurnOffBgObj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_border_width(autoTurnOffBgObj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);

    lv_obj_t* outputLabel = lv_label_create(autoTurnOffBgObj);
    lv_label_set_text(outputLabel, "自动关闭屏幕");
    lv_obj_set_size(outputLabel, 84, 15);
    lv_obj_align(outputLabel, LV_ALIGN_TOP_LEFT, 8, 6);
    lv_obj_set_style_text_font(outputLabel, &LR_AppSetting_ItemName, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_align(outputLabel, LV_TEXT_ALIGN_LEFT, LV_PART_MAIN | LV_STATE_DEFAULT);

    rightIcon = lv_img_create(autoTurnOffBgObj);
    lv_img_set_src(rightIcon, &LR_Setting_RightIcon);
    lv_obj_align_to(rightIcon, autoTurnOffBgObj, LV_ALIGN_TOP_RIGHT, -9, 9);

    autoTurnOffSelectObj = lv_label_create(autoTurnOffBgObj);
    lv_label_set_text(autoTurnOffSelectObj, "永不");
    lv_obj_set_size(autoTurnOffSelectObj, 44, 15);
    lv_obj_align_to(autoTurnOffSelectObj, rightIcon, LV_ALIGN_OUT_LEFT_MID, -5, 1);
    lv_obj_set_style_text_font(autoTurnOffSelectObj, &LR_AppSetting_ItemName, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_align(autoTurnOffSelectObj, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_DEFAULT);

    leftIcon = lv_img_create(autoTurnOffBgObj);
    lv_img_set_src(leftIcon, &LR_Setting_LeftIcon);
    lv_obj_align_to(leftIcon, autoTurnOffSelectObj, LV_ALIGN_OUT_LEFT_MID, -5, -1);

    /* sync hal data */
    uint16_t* screenAutoLock_hal = &LimeHAL_GetInfoPin()->settingInfo.screen_AutoReduceLightTime;
    uint16_t* screenAutoOff_hal = &LimeHAL_GetInfoPin()->settingInfo.screen_AutoOffTime;
    setScreenReduceLightTimeLabelByIndex(*screenAutoLock_hal);
    setScreenTurnOffLabelByIndex(*screenAutoOff_hal);
}
void Lime_App_Settings_ItemScreen_Destroy(void)
{
    LV_LOG_USER("Lime_App_Settings_ItemScreen_Destroy");

    if(item_screen_obj != NULL)
    {
        lv_obj_del(item_screen_obj);
        item_screen_obj = NULL;
    }
}

static void setScreenReduceLightTimeLabelByIndex(uint16_t ReduceLightTime)
{
    if(item_screen_obj == NULL)
    {
        LV_LOG_ERROR("item_screen_obj is NULL");
        return;
    }

    switch(ReduceLightTime)
    {
        case 0:
            lv_label_set_text(autoLockSelectedObj, "永不");
            break;
        case 1:
            lv_label_set_text(autoLockSelectedObj, "30秒");
            break;
        case 2:
            lv_label_set_text(autoLockSelectedObj, "1分钟");
            break;
        case 3:
            lv_label_set_text(autoLockSelectedObj, "5分钟");
            break;
        default:
            LV_LOG_ERROR("ReduceLightTime is invalid");
            break;
    }
}
static void setScreenTurnOffLabelByIndex(uint16_t turnOffTime)
{
    if(item_screen_obj == NULL)
    {
        LV_LOG_ERROR("item_screen_obj is NULL");
        return;
    }

    switch(turnOffTime)
    {
        case 0:
            lv_label_set_text(autoTurnOffSelectObj, "永不");
            break;
        case 1:
            lv_label_set_text(autoTurnOffSelectObj, "30秒");
            break;
        case 2:
            lv_label_set_text(autoTurnOffSelectObj, "1分钟");
            break;
        case 3:
            lv_label_set_text(autoTurnOffSelectObj, "5分钟");
            break;
        default:
            LV_LOG_ERROR("turnOffTime is invalid");

    }
}

void Lime_App_Settings_ItemScreen_Update(Lime_App_KeyOptions_e keyOption)
{
    uint16_t* screenAutoLock_hal = &LimeHAL_GetInfoPin()->settingInfo.screen_AutoReduceLightTime;
    uint16_t* screenAutoOff_hal = &LimeHAL_GetInfoPin()->settingInfo.screen_AutoOffTime;

    switch(keyOption)
    {
        case Lime_App_KeyOptions_Up:
        {
            if(nowSelectedItem == -1)
            {
                nowSelectedItem = 0;
            }
            else
            {
                nowSelectedItem = 1 - nowSelectedItem;
            }
            if(nowSelectedItem == 0)
            {
                lv_obj_set_style_bg_color(autoLockBgObj, lv_color_hex(SETTING_SELECT_COLOR_HEX), LV_PART_MAIN | LV_STATE_DEFAULT);
                lv_obj_set_style_bg_color(autoTurnOffBgObj, lv_color_hex(SETTING_UNSELECT_COLOR_HEX), LV_PART_MAIN | LV_STATE_DEFAULT);
            }
            else
            {
                lv_obj_set_style_bg_color(autoTurnOffBgObj, lv_color_hex(SETTING_SELECT_COLOR_HEX), LV_PART_MAIN | LV_STATE_DEFAULT);
                lv_obj_set_style_bg_color(autoLockBgObj, lv_color_hex(SETTING_UNSELECT_COLOR_HEX), LV_PART_MAIN | LV_STATE_DEFAULT);
            }
            break;
        }
        case Lime_App_KeyOptions_Down:
        {
            if(nowSelectedItem == -1)
            {
                nowSelectedItem = 1;
            }
            else
            {
                nowSelectedItem = 1 - nowSelectedItem;
            }
            if(nowSelectedItem == 0)
            {
                lv_obj_set_style_bg_color(autoLockBgObj, lv_color_hex(SETTING_SELECT_COLOR_HEX), LV_PART_MAIN | LV_STATE_DEFAULT);
                lv_obj_set_style_bg_color(autoTurnOffBgObj, lv_color_hex(SETTING_UNSELECT_COLOR_HEX), LV_PART_MAIN | LV_STATE_DEFAULT);
            }
            else
            {
                lv_obj_set_style_bg_color(autoTurnOffBgObj, lv_color_hex(SETTING_SELECT_COLOR_HEX), LV_PART_MAIN | LV_STATE_DEFAULT);
                lv_obj_set_style_bg_color(autoLockBgObj, lv_color_hex(SETTING_UNSELECT_COLOR_HEX), LV_PART_MAIN | LV_STATE_DEFAULT);
            }
            break;
        }
        // case Lime_App_KeyOptions_Set:
        case Lime_App_KeyOptions_Right:
        {
            if(nowSelectedItem == -1)
            {
                break;
            }

            if(nowSelectedItem == 0)
            {
                *screenAutoLock_hal = (*screenAutoLock_hal >= 3) ? 0 : (*screenAutoLock_hal + 1);
                setScreenReduceLightTimeLabelByIndex(* screenAutoLock_hal);
            }
            else
            {
                *screenAutoOff_hal = (*screenAutoOff_hal >= 3) ? 0 : (*screenAutoOff_hal + 1);
                setScreenTurnOffLabelByIndex(* screenAutoOff_hal);
            }
            break;
        }
        case Lime_App_KeyOptions_Left:
        {
            if(nowSelectedItem == -1)
            {
                break;
            }

            if(nowSelectedItem == 0)
            {
                *screenAutoLock_hal = (*screenAutoLock_hal == 0) ? 3 : (*screenAutoLock_hal - 1);
                setScreenReduceLightTimeLabelByIndex(*screenAutoLock_hal);
            }
            else
            {
                *screenAutoOff_hal = (*screenAutoOff_hal == 0) ? 3 : (*screenAutoOff_hal - 1);
                setScreenTurnOffLabelByIndex(*screenAutoOff_hal);
            }
            break;
        }
        default:
            break;
    }
}
