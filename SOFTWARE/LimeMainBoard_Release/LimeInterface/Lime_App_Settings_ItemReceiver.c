#include "Lime_App_Settings_ItemReceiver.h"
#include "Lime_App_Hal.h"

#define SETTING_SELECT_COLOR_HEX 0xffd08d
#define SETTING_UNSELECT_COLOR_HEX 0xffffff

static lv_obj_t* item_receiver_obj = NULL;
static lv_obj_t* outputAgreementBgObj = NULL;
static lv_obj_t* outputAgreementSelectedObj = NULL;
static lv_obj_t* outputLevelBgObj = NULL;
static lv_obj_t* outputLevelToggObj = NULL;

static int8_t nowSelectedItem = 1;     //-1:no select, 0:agreement, 1:level

LV_IMG_DECLARE(LR_Setting_LeftIcon);
LV_IMG_DECLARE(LR_Setting_RightIcon);
LV_FONT_DECLARE(LR_AppSetting_ItemName);
LV_FONT_DECLARE(LR_AppSetting_ItemExplain);

static void setScreenLockTimeLabelByIndex(uint8_t receiver_Agreement);
static void setScreenTurnOffLabelByIndex(uint8_t receiver_OutToggle);

void Lime_App_Settings_ItemReceiver_Create(lv_obj_t* father)
{
    nowSelectedItem = -1;

    item_receiver_obj = lv_obj_create(father);
    lv_obj_set_size(item_receiver_obj, lv_obj_get_width(father), lv_obj_get_height(father));
    lv_obj_set_style_bg_opa(item_receiver_obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_color(item_receiver_obj, lv_color_hex(0xbffbff), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_pad_all(item_receiver_obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_radius(item_receiver_obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_shadow_width(item_receiver_obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_border_width(item_receiver_obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);

    outputAgreementBgObj = lv_obj_create(item_receiver_obj);
    lv_obj_set_size(outputAgreementBgObj, 188, 25);
    lv_obj_align(outputAgreementBgObj, LV_ALIGN_TOP_MID, 0, 38);
    lv_obj_set_style_bg_opa(outputAgreementBgObj, 200, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_color(outputAgreementBgObj, lv_color_hex(SETTING_UNSELECT_COLOR_HEX), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_pad_all(outputAgreementBgObj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_radius(outputAgreementBgObj, 8, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_shadow_width(outputAgreementBgObj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_border_width(outputAgreementBgObj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);

    lv_obj_t* agreementLabel = lv_label_create(outputAgreementBgObj);
    lv_label_set_text(agreementLabel, "输出协议");
    lv_obj_set_size(agreementLabel, 50, 15);
    lv_obj_align(agreementLabel, LV_ALIGN_LEFT_MID, 8, 0);
    lv_obj_set_style_text_font(agreementLabel, &LR_AppSetting_ItemName, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_align(agreementLabel, LV_TEXT_ALIGN_LEFT, LV_PART_MAIN | LV_STATE_DEFAULT);

    lv_obj_t * rightIcon = lv_img_create(outputAgreementBgObj);
    lv_img_set_src(rightIcon, &LR_Setting_RightIcon);
    lv_obj_align_to(rightIcon, outputAgreementBgObj, LV_ALIGN_RIGHT_MID, -9, 0);

    outputAgreementSelectedObj = lv_label_create(outputAgreementBgObj);
    lv_label_set_text(outputAgreementSelectedObj, "DBUS");
    lv_obj_set_size(outputAgreementSelectedObj, 44, 15);
    lv_obj_align_to(outputAgreementSelectedObj, rightIcon, LV_ALIGN_OUT_LEFT_MID, -5, 1);
    lv_obj_set_style_text_font(outputAgreementSelectedObj, &LR_AppSetting_ItemName, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_align(outputAgreementSelectedObj, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_DEFAULT);

    lv_obj_t * leftIcon = lv_img_create(outputAgreementBgObj);
    lv_img_set_src(leftIcon, &LR_Setting_LeftIcon);
    lv_obj_align_to(leftIcon, outputAgreementSelectedObj, LV_ALIGN_OUT_LEFT_MID, -5, -1);

    outputLevelBgObj = lv_obj_create(item_receiver_obj);
    lv_obj_set_size(outputLevelBgObj, 188, 50);
    lv_obj_align_to(outputLevelBgObj, outputAgreementBgObj, LV_ALIGN_OUT_BOTTOM_MID, 0, 6);
    lv_obj_set_style_bg_opa(outputLevelBgObj, 200, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_color(outputLevelBgObj, lv_color_hex(SETTING_UNSELECT_COLOR_HEX), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_pad_all(outputLevelBgObj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_radius(outputLevelBgObj, 8, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_shadow_width(outputLevelBgObj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_border_width(outputLevelBgObj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);

    lv_obj_t* outputLabel = lv_label_create(outputLevelBgObj);
    lv_label_set_text(outputLabel, "输出电平");
    lv_obj_set_size(outputLabel, 50, 15);
    lv_obj_align(outputLabel, LV_ALIGN_TOP_LEFT, 8, 6);
    lv_obj_set_style_text_font(outputLabel, &LR_AppSetting_ItemName, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_align(outputLabel, LV_TEXT_ALIGN_LEFT, LV_PART_MAIN | LV_STATE_DEFAULT);

    rightIcon = lv_img_create(outputLevelBgObj);
    lv_img_set_src(rightIcon, &LR_Setting_RightIcon);
    lv_obj_align_to(rightIcon, outputLevelBgObj, LV_ALIGN_TOP_RIGHT, -9, 9);

    outputLevelToggObj = lv_label_create(outputLevelBgObj);
    lv_label_set_text(outputLevelToggObj, "正常");
    lv_obj_set_size(outputLevelToggObj, 44, 15);
    lv_obj_align_to(outputLevelToggObj, rightIcon, LV_ALIGN_OUT_LEFT_MID, -5, 1);
    lv_obj_set_style_text_font(outputLevelToggObj, &LR_AppSetting_ItemName, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_align(outputLevelToggObj, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_DEFAULT);

    leftIcon = lv_img_create(outputLevelBgObj);
    lv_img_set_src(leftIcon, &LR_Setting_LeftIcon);
    lv_obj_align_to(leftIcon, outputLevelToggObj, LV_ALIGN_OUT_LEFT_MID, -5, -1);

    lv_obj_t* indicatorObj = lv_obj_create(outputLevelBgObj);
    lv_obj_set_size(indicatorObj, 176, 1);
    lv_obj_center(indicatorObj);
    lv_obj_set_style_bg_opa(indicatorObj, 255, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_color(indicatorObj, lv_color_hex(0x616161), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_pad_all(indicatorObj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_radius(indicatorObj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_shadow_width(indicatorObj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_border_width(indicatorObj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);

    lv_obj_t* outputLevelExplainBg = lv_obj_create(outputLevelBgObj);
    lv_obj_set_size(outputLevelExplainBg, 175, 18);
    lv_obj_align(outputLevelExplainBg, LV_ALIGN_TOP_LEFT, 8, 29);
    lv_obj_set_style_bg_opa(outputLevelExplainBg, 53, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_color(outputLevelExplainBg, lv_color_hex(0x000000), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_pad_all(outputLevelExplainBg, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_radius(outputLevelExplainBg, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_shadow_width(outputLevelExplainBg, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_border_width(outputLevelExplainBg, 0, LV_PART_MAIN | LV_STATE_DEFAULT);

    lv_obj_t* outputLevelExplain = lv_label_create(outputLevelBgObj);
    lv_label_set_text(outputLevelExplain, "  >正常:空闲高电平,数据低电平,反向则反之.");
    lv_obj_set_size(outputLevelExplain, 171, 15);
    lv_obj_align(outputLevelExplain, LV_ALIGN_TOP_LEFT, 8, 33);
    lv_obj_set_style_text_font(outputLevelExplain, &LR_AppSetting_ItemExplain, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_align(outputLevelExplain, LV_TEXT_ALIGN_LEFT, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_label_set_long_mode(outputLevelExplain, LV_LABEL_LONG_SCROLL_CIRCULAR);

    lv_obj_t* outputLevelExplainBg2 = lv_obj_create(outputLevelBgObj);
    lv_obj_set_size(outputLevelExplainBg2, 4, 18);
    lv_obj_align_to(outputLevelExplainBg2, outputLevelExplainBg, LV_ALIGN_LEFT_MID, 0, 0);
    lv_obj_set_style_bg_opa(outputLevelExplainBg2, 255, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_color(outputLevelExplainBg2, lv_color_hex(0x0affa6), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_pad_all(outputLevelExplainBg2, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_radius(outputLevelExplainBg2, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_shadow_width(outputLevelExplainBg2, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_border_width(outputLevelExplainBg2, 0, LV_PART_MAIN | LV_STATE_DEFAULT);

    /* sync hal data */
    uint8_t* recAgreement_hal = &LimeHAL_GetInfoPin()->settingInfo.receiver_Agreement;
    uint8_t* recOutToggle_hal = &LimeHAL_GetInfoPin()->settingInfo.receiver_OutToggle;
    setScreenLockTimeLabelByIndex(*recAgreement_hal);
    setScreenTurnOffLabelByIndex(*recOutToggle_hal);
}
void Lime_App_Settings_ItemReceiver_Destroy(void)
{
    LV_LOG_USER("Lime_App_Settings_ItemReceiver_Destroy");

    if(item_receiver_obj != NULL)
    {
        lv_obj_del(item_receiver_obj);
        item_receiver_obj = NULL;
    }
}

static void setScreenLockTimeLabelByIndex(uint8_t receiver_Agreement)
{
    if(item_receiver_obj == NULL)
    {
        LV_LOG_ERROR("item_receiver_obj is NULL");
        return;
    }

    switch(receiver_Agreement)
    {
        case 0:
            lv_label_set_text(outputAgreementSelectedObj, "DBUS");
            break;
        case 1:
            lv_label_set_text(outputAgreementSelectedObj, "SBUS");
            break;
        case 2:
            lv_label_set_text(outputAgreementSelectedObj, "PP");
            break;
        case 3:
            lv_label_set_text(outputAgreementSelectedObj, "DEBUG");
            break;
        default:
            LV_LOG_ERROR("receiver_Agreement is invalid");
            break;
    }
}
static void setScreenTurnOffLabelByIndex(uint8_t receiver_OutToggle)
{
    if(item_receiver_obj == NULL)
    {
        LV_LOG_ERROR("item_receiver_obj is NULL");
        return;
    }

    switch(receiver_OutToggle)
    {
        case 0:
            lv_label_set_text(outputLevelToggObj, "静默");
            break;
        case 1:
            lv_label_set_text(outputLevelToggObj, "正常");
            break;
        case 2:
            lv_label_set_text(outputLevelToggObj, "反向");
            break;
        default:
            LV_LOG_ERROR("receiver_OutToggle is invalid");
            break;
    }
}

void Lime_App_Settings_ItemReceiver_Update(Lime_App_KeyOptions_e keyOption)
{
    // if(keyOption != Lime_App_KeyOptions_None)
    //     LV_LOG_USER("Lime_App_Settings_ItemReceiver_Update: %d", keyOption);

    uint8_t* recAgreement_hal = &LimeHAL_GetInfoPin()->settingInfo.receiver_Agreement;
    uint8_t* recOutToggle_hal = &LimeHAL_GetInfoPin()->settingInfo.receiver_OutToggle;

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
                lv_obj_set_style_bg_color(outputAgreementBgObj, lv_color_hex(SETTING_SELECT_COLOR_HEX), LV_PART_MAIN | LV_STATE_DEFAULT);
                lv_obj_set_style_bg_color(outputLevelBgObj, lv_color_hex(SETTING_UNSELECT_COLOR_HEX), LV_PART_MAIN | LV_STATE_DEFAULT);
            }
            else
            {
                lv_obj_set_style_bg_color(outputLevelBgObj, lv_color_hex(SETTING_SELECT_COLOR_HEX), LV_PART_MAIN | LV_STATE_DEFAULT);
                lv_obj_set_style_bg_color(outputAgreementBgObj, lv_color_hex(SETTING_UNSELECT_COLOR_HEX), LV_PART_MAIN | LV_STATE_DEFAULT);
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
                lv_obj_set_style_bg_color(outputAgreementBgObj, lv_color_hex(SETTING_SELECT_COLOR_HEX), LV_PART_MAIN | LV_STATE_DEFAULT);
                lv_obj_set_style_bg_color(outputLevelBgObj, lv_color_hex(SETTING_UNSELECT_COLOR_HEX), LV_PART_MAIN | LV_STATE_DEFAULT);
            }
            else
            {
                lv_obj_set_style_bg_color(outputLevelBgObj, lv_color_hex(SETTING_SELECT_COLOR_HEX), LV_PART_MAIN | LV_STATE_DEFAULT);
                lv_obj_set_style_bg_color(outputAgreementBgObj, lv_color_hex(SETTING_UNSELECT_COLOR_HEX), LV_PART_MAIN | LV_STATE_DEFAULT);
            }
            break;
        }
        case Lime_App_KeyOptions_Set:
        case Lime_App_KeyOptions_Right:
        {
            if(nowSelectedItem == -1)
            {
                break;
            }

            if(nowSelectedItem == 0)
            {
                *recAgreement_hal = (*recAgreement_hal >= 3) ? 0 : (*recAgreement_hal + 1);
                setScreenLockTimeLabelByIndex(* recAgreement_hal);
            }
            else
            {
                *recOutToggle_hal = (*recOutToggle_hal >= 2) ? 0 : (*recOutToggle_hal + 1);
                setScreenTurnOffLabelByIndex(* recOutToggle_hal);
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
                *recAgreement_hal = (*recAgreement_hal == 0) ? 3 : (*recAgreement_hal - 1);
                setScreenLockTimeLabelByIndex(*recAgreement_hal);
            }
            else
            {
                *recOutToggle_hal = (*recOutToggle_hal == 0) ? 2 : (*recOutToggle_hal - 1);
                setScreenTurnOffLabelByIndex(*recOutToggle_hal);
            }
            break;
        }
        default:
            break;
    }
}
