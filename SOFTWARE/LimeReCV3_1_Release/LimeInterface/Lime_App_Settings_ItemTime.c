#include "Lime_App_Settings_ItemTime.h"
#include "Lime_App_Hal.h"
#include <stdio.h>
#include <string.h>

#define SETTING_SELECT_COLOR_HEX 0xffd08d
#define SETTING_UNSELECT_COLOR_HEX 0xffffff

static lv_obj_t* item_time_obj = NULL;

static lv_obj_t* itemYearBgObj = NULL;
static lv_obj_t* itemYearBgSelectedObj = NULL;
static lv_obj_t* itemMonthBgObj = NULL;
static lv_obj_t* itemMonthBgSelectedObj = NULL;
static lv_obj_t* itemDayBgObj = NULL;
static lv_obj_t* itemDayBgSelectedObj = NULL;
static lv_obj_t* itemHourBgObj = NULL;
static lv_obj_t* itemHourBgSelectedObj = NULL;
static lv_obj_t* itemMinuteBgObj = NULL;
static lv_obj_t* itemMinuteBgSelectedObj = NULL;
static lv_obj_t* itemSecondBgObj = NULL;
static lv_obj_t* itemSecondBgSelectedObj = NULL;


static int8_t nowSelectedItem = 1;      //-1:no select, 0:year, 1:month, 2:day, 3:hour, 4:minute, 5:second
static int8_t lastSelectedItem = -1;    //-1:no select, 0:year, 1:month, 2:day, 3:hour, 4:minute, 5:second
static int8_t nextSelectedItem = -1;    //-1:no select, 0:year, 1:month, 2:day, 3:hour, 4:minute, 5:second

static void setYearLabelByIndex(uint32_t yearSet);
static void setMonthLabelByIndex(uint8_t monthSet);
static void setDayLabelByIndex(uint8_t daySet);
static void setHourLabelByIndex(uint8_t hourSet);
static void setMinuteLabelByIndex(uint8_t minuteSet);
static void setSecondLabelByIndex(uint8_t secondSet);
static lv_obj_t* getBgObjByIndex(int8_t index);

LV_IMG_DECLARE(LR_Setting_LeftIcon);
LV_IMG_DECLARE(LR_Setting_RightIcon);
LV_FONT_DECLARE(LR_AppSetting_ItemName);

#define TIME_FACE_SELF_ASSERT() do{\
    if(item_time_obj == NULL){\
        LV_LOG_WARN("item_time_obj is NULL");\
        return;\
    }\
}while(0)

#define LIME_SETTING_CREATE_DEFAULT_ITEM(bgObj, fatherObj, name, defSelectName, defSelectNameObj)\
do\
{\
    bgObj = lv_obj_create(fatherObj);\
    lv_obj_set_size(bgObj, 188, 25);\
    lv_obj_set_style_bg_opa(bgObj, 255, LV_PART_MAIN | LV_STATE_DEFAULT);\
    lv_obj_set_style_bg_color(bgObj, lv_color_hex(SETTING_UNSELECT_COLOR_HEX), LV_PART_MAIN | LV_STATE_DEFAULT);\
    lv_obj_set_style_pad_all(bgObj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);\
    lv_obj_set_style_radius(bgObj, 8, LV_PART_MAIN | LV_STATE_DEFAULT);\
    lv_obj_set_style_shadow_width(bgObj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);\
    lv_obj_set_style_border_width(bgObj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);\
    \
    lv_obj_t* nameLabel = lv_label_create(bgObj);\
    lv_label_set_text(nameLabel, name);\
    lv_obj_set_size(nameLabel, 50, 15);\
    lv_obj_align(nameLabel, LV_ALIGN_LEFT_MID, 8, 0);\
    lv_obj_set_style_text_font(nameLabel, &LR_AppSetting_ItemName, LV_PART_MAIN | LV_STATE_DEFAULT);\
    lv_obj_set_style_text_align(nameLabel, LV_TEXT_ALIGN_LEFT, LV_PART_MAIN | LV_STATE_DEFAULT);\
    \
    lv_obj_t * rightIcon = lv_img_create(bgObj);\
    lv_img_set_src(rightIcon, &LR_Setting_RightIcon);\
    lv_obj_align_to(rightIcon, bgObj, LV_ALIGN_RIGHT_MID, -9, 0);\
    \
    defSelectNameObj = lv_label_create(bgObj);\
    lv_label_set_text(defSelectNameObj, defSelectName);\
    lv_obj_set_size(defSelectNameObj, 44, 15);\
    lv_obj_align_to(defSelectNameObj, rightIcon, LV_ALIGN_OUT_LEFT_MID, -5, 1);\
    lv_obj_set_style_text_font(defSelectNameObj, &LR_AppSetting_ItemName, LV_PART_MAIN | LV_STATE_DEFAULT);\
    lv_obj_set_style_text_align(defSelectNameObj, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_DEFAULT);\
    \
    lv_obj_t * leftIcon = lv_img_create(bgObj);\
    lv_img_set_src(leftIcon, &LR_Setting_LeftIcon);\
    lv_obj_align_to(leftIcon, defSelectNameObj, LV_ALIGN_OUT_LEFT_MID, -5, -1);\
}while(0)

void Lime_App_Settings_ItemTime_Create(lv_obj_t* father)
{
    item_time_obj = lv_obj_create(father);
    lv_obj_set_size(item_time_obj, lv_obj_get_width(father), lv_obj_get_height(father));
    lv_obj_set_style_bg_opa(item_time_obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    // lv_obj_set_style_bg_color(item_time_obj, lv_color_hex(0xFF0000), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_pad_all(item_time_obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_radius(item_time_obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_shadow_width(item_time_obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_border_width(item_time_obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);

    LIME_SETTING_CREATE_DEFAULT_ITEM(itemYearBgObj, item_time_obj, "年", "2024", itemYearBgSelectedObj);
    lv_obj_align(itemYearBgObj, LV_ALIGN_TOP_MID, 0, 40);\
    LIME_SETTING_CREATE_DEFAULT_ITEM(itemMonthBgObj, item_time_obj, "月", "12月", itemMonthBgSelectedObj);
    lv_obj_align_to(itemMonthBgObj, itemYearBgObj, LV_ALIGN_OUT_BOTTOM_MID, 0, 6);
    LIME_SETTING_CREATE_DEFAULT_ITEM(itemDayBgObj, item_time_obj, "日", "31日", itemDayBgSelectedObj);
    lv_obj_align_to(itemDayBgObj, itemMonthBgObj, LV_ALIGN_OUT_BOTTOM_MID, 0, 6);
    LIME_SETTING_CREATE_DEFAULT_ITEM(itemHourBgObj, item_time_obj, "时", "12时", itemHourBgSelectedObj);
    lv_obj_align_to(itemHourBgObj, itemDayBgObj, LV_ALIGN_OUT_BOTTOM_MID, 0, 6);
    LIME_SETTING_CREATE_DEFAULT_ITEM(itemMinuteBgObj, item_time_obj, "分", "30分", itemMinuteBgSelectedObj);
    lv_obj_align_to(itemMinuteBgObj, itemHourBgObj, LV_ALIGN_OUT_BOTTOM_MID, 0, 6);
    LIME_SETTING_CREATE_DEFAULT_ITEM(itemSecondBgObj, item_time_obj, "秒", "59秒", itemSecondBgSelectedObj);
    lv_obj_align_to(itemSecondBgObj, itemMinuteBgObj, LV_ALIGN_OUT_BOTTOM_MID, 0, 6);

    nowSelectedItem = -1;
    lastSelectedItem = -1;
    nextSelectedItem = -1;

    uint16_t* year_hal = &LimeHAL_GetInfoPin()->settingInfo.year;
    uint8_t* month_hal = &LimeHAL_GetInfoPin()->settingInfo.month;
    uint8_t* day_hal = &LimeHAL_GetInfoPin()->settingInfo.day;
    uint8_t* hour_hal = &LimeHAL_GetInfoPin()->settingInfo.time[0];
    uint8_t* minute_hal = &LimeHAL_GetInfoPin()->settingInfo.time[1];
    uint8_t* second_hal = &LimeHAL_GetInfoPin()->settingInfo.time[2];
    *year_hal = LimeHAL_GetInfoPin()->mainFaceInfo.year;
    *month_hal = LimeHAL_GetInfoPin()->mainFaceInfo.month;
    *day_hal = LimeHAL_GetInfoPin()->mainFaceInfo.day;
    *hour_hal = LimeHAL_GetInfoPin()->mainFaceInfo.time[0];
    *minute_hal = LimeHAL_GetInfoPin()->mainFaceInfo.time[1];
    *second_hal = LimeHAL_GetInfoPin()->mainFaceInfo.time[2];
    setYearLabelByIndex(*year_hal);
    setMonthLabelByIndex(*month_hal);
    setDayLabelByIndex(*day_hal);
    setHourLabelByIndex(*hour_hal);
    setMinuteLabelByIndex(*minute_hal);
    setSecondLabelByIndex(*second_hal);
}
void Lime_App_Settings_ItemTime_Destroy(void)
{
    if(item_time_obj!= NULL)
    {
        lv_obj_del(item_time_obj);
        item_time_obj = NULL;
    }

    LimeHAL_GetInfoPin()->settingInfo.isRtcNeedSave = true;
}

static void setYearLabelByIndex(uint32_t yearSet)
{
    TIME_FACE_SELF_ASSERT();

    yearSet = yearSet > 2100 ? 2100 : yearSet < 2010 ? 2010 : yearSet;
    char yearStr[10] = {0};
    snprintf(yearStr, sizeof(yearStr), "%d", yearSet);
    lv_label_set_text(itemYearBgSelectedObj, yearStr);
}
static void setMonthLabelByIndex(uint8_t monthSet)
{
    TIME_FACE_SELF_ASSERT();

    char monthStr[10] = {0};
    snprintf(monthStr, sizeof(monthStr), "%d月", monthSet);
    lv_label_set_text(itemMonthBgSelectedObj, monthStr);
}
static void setDayLabelByIndex(uint8_t daySet)
{
    TIME_FACE_SELF_ASSERT();

    char dayStr[10] = {0};
    snprintf(dayStr, sizeof(dayStr), "%d日", daySet);
    lv_label_set_text(itemDayBgSelectedObj, dayStr);
}
static void setHourLabelByIndex(uint8_t hourSet)
{
    TIME_FACE_SELF_ASSERT();

    char hourStr[10] = {0};
    snprintf(hourStr, sizeof(hourStr), "%d时", hourSet);
    lv_label_set_text(itemHourBgSelectedObj, hourStr);
}
static void setMinuteLabelByIndex(uint8_t minuteSet)
{
    TIME_FACE_SELF_ASSERT();

    char minuteStr[10] = {0};
    snprintf(minuteStr, sizeof(minuteStr), "%d分", minuteSet);
    lv_label_set_text(itemMinuteBgSelectedObj, minuteStr);
}
static void setSecondLabelByIndex(uint8_t secondSet)
{
    TIME_FACE_SELF_ASSERT();

    char secondStr[10] = {0};
    snprintf(secondStr, sizeof(secondStr), "%d秒", secondSet);
    lv_label_set_text(itemSecondBgSelectedObj, secondStr);
}

static lv_obj_t* getBgObjByIndex(int8_t index)
{
    switch(index)
    {
        case 0:
            return itemYearBgObj;
        case 1:
            return itemMonthBgObj;
        case 2:
            return itemDayBgObj;
        case 3:
            return itemHourBgObj;
        case 4:
            return itemMinuteBgObj;
        case 5:
            return itemSecondBgObj;
        default:
            LV_LOG_ERROR("index error");
            return NULL;
    }
}

void Lime_App_Settings_ItemTime_Update(Lime_App_KeyOptions_e keyOption)
{
    uint16_t* year_hal = &LimeHAL_GetInfoPin()->settingInfo.year;
    uint8_t* month_hal = &LimeHAL_GetInfoPin()->settingInfo.month;
    uint8_t* day_hal = &LimeHAL_GetInfoPin()->settingInfo.day;
    uint8_t* hour_hal = &LimeHAL_GetInfoPin()->settingInfo.time[0];
    uint8_t* minute_hal = &LimeHAL_GetInfoPin()->settingInfo.time[1];
    uint8_t* second_hal = &LimeHAL_GetInfoPin()->settingInfo.time[2];

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
                nowSelectedItem = (nowSelectedItem <= 0) ? 5 : (nowSelectedItem - 1);
                lastSelectedItem = (nowSelectedItem == 0) ? 5 : (nowSelectedItem - 1);
                nextSelectedItem = (nowSelectedItem == 5) ? 0 : (nowSelectedItem + 1);
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
                nowSelectedItem = (nowSelectedItem >= 5) ? 0 : (nowSelectedItem + 1);
                lastSelectedItem = (nowSelectedItem == 5) ? 0 : (nowSelectedItem + 1);
                nextSelectedItem = (nowSelectedItem == 0) ? 5 : (nowSelectedItem - 1);
            }
            lv_obj_set_style_bg_color(getBgObjByIndex(lastSelectedItem), lv_color_hex(SETTING_UNSELECT_COLOR_HEX), LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_bg_color(getBgObjByIndex(nextSelectedItem), lv_color_hex(SETTING_UNSELECT_COLOR_HEX), LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_bg_color(getBgObjByIndex(nowSelectedItem), lv_color_hex(SETTING_SELECT_COLOR_HEX), LV_PART_MAIN | LV_STATE_DEFAULT);

            break;
        }
        case Lime_App_KeyOptions_Left:
        {
            if(nowSelectedItem == -1)
                break;

            switch(nowSelectedItem)
            {
                case 0:
                    (*year_hal) = (*year_hal) < 2010 ? 2100 : (*year_hal) - 1;
                    setYearLabelByIndex(*year_hal);
                    if(*day_hal > Lime_CalDayInMonth(*year_hal, *month_hal))
                    {
                        *day_hal = Lime_CalDayInMonth(*year_hal, *month_hal);
                        setDayLabelByIndex(*day_hal);
                    }
                    break;
                case 1:
                    (*month_hal) = (*month_hal) < 2 ? 12 : (*month_hal) - 1;
                    setMonthLabelByIndex(*month_hal);
                    if(*day_hal > Lime_CalDayInMonth(*year_hal, *month_hal))
                    {
                        *day_hal = Lime_CalDayInMonth(*year_hal, *month_hal);
                        setDayLabelByIndex(*day_hal);
                    }
                    break;
                case 2:
                    (*day_hal) = (*day_hal) < 2 ? Lime_CalDayInMonth(*year_hal, *month_hal) : (*day_hal) - 1;
                    setDayLabelByIndex(*day_hal);
                    break;
                case 3:
                    (*hour_hal) = (*hour_hal) < 1 ? 23 : (*hour_hal) - 1;
                    setHourLabelByIndex(*hour_hal);
                    break;
                case 4:
                    (*minute_hal) = (*minute_hal) < 1 ? 59 : (*minute_hal) - 1;
                    setMinuteLabelByIndex(*minute_hal);
                    break;
                case 5:
                    (*second_hal) = (*second_hal) < 1 ? 59 : (*second_hal) - 1;
                    setSecondLabelByIndex(*second_hal);
                    break;
                default:
                    LV_LOG_ERROR("nowSelectedItem error");
                    break;
            }
            break;
        }
        case Lime_App_KeyOptions_Right:
        {
            if(nowSelectedItem == -1)
                break;

            switch(nowSelectedItem)
            {
                case 0:
                    (*year_hal) = (*year_hal) > 2100 ? 2010 : (*year_hal) + 1;
                    setYearLabelByIndex(*year_hal);
                    if(*day_hal > Lime_CalDayInMonth(*year_hal, *month_hal))
                    {
                        *day_hal = Lime_CalDayInMonth(*year_hal, *month_hal);
                        setDayLabelByIndex(*day_hal);
                    }
                    break;
                case 1:
                    (*month_hal) = (*month_hal) >= 12 ? 1 : (*month_hal) + 1;
                    setMonthLabelByIndex(*month_hal);
                    if(*day_hal > Lime_CalDayInMonth(*year_hal, *month_hal))
                    {
                        *day_hal = Lime_CalDayInMonth(*year_hal, *month_hal);
                        setDayLabelByIndex(*day_hal);
                    }
                    break;
                case 2:
                    (*day_hal) = (*day_hal) > (Lime_CalDayInMonth(*year_hal, *month_hal) - 1) ? 1 : (*day_hal) + 1;
                    setDayLabelByIndex(*day_hal);
                    break;
                case 3:
                    (*hour_hal) = (*hour_hal) > 22 ? 0 : (*hour_hal) + 1;
                    setHourLabelByIndex(*hour_hal);
                    break;
                case 4:
                    (*minute_hal) = (*minute_hal) > 58 ? 0 : (*minute_hal) + 1;
                    setMinuteLabelByIndex(*minute_hal);
                    break;
                case 5:
                    (*second_hal) = (*second_hal) > 58 ? 0 : (*second_hal) + 1;
                    setSecondLabelByIndex(*second_hal);
                    break;
                default:
                    LV_LOG_ERROR("nowSelectedItem error");
                    break;
            }
            break;
        }
        default:
            break;
    }
}
