#include "Lime_App_Settings.h"
#include "Lime_App_Hal.h"
#include <stdio.h>
#include <string.h>

#include "Lime_App_Settings_ItemReceiver.h"
#include "Lime_App_Settings_ItemScreen.h"
#include "Lime_App_Settings_ItemTime.h"
#include "Lime_App_Settings_ItemStorage.h"
#include "Lime_App_Settings_ItemAboutMe.h"

#define SETTING_SELECT_COLOR_HEX 0xffd08d
#define SETTING_UNSELECT_COLOR_HEX 0xffffff

static Lime_App_Settings_Item_t Setting_App_Items[LIME_APP_MAX_SETTING_ITEM_NUM];
static uint32_t Setting_App_ItemNum = 0;
static int32_t  NowRunningIndex = -1;

static lv_obj_t* settingFaceObj = NULL;
static lv_obj_t* settingHeadBarObj = NULL;
static lv_timer_t* mainScan_timer = NULL;
static lv_obj_t* PrimaryDrawPicBedObj = NULL;
static lv_obj_t* SecondaryDrawPicBedObj = NULL;
static lv_anim_t animHeadBarRun;

LV_IMG_DECLARE(LR_Setting_AboutIcon);
LV_IMG_DECLARE(LR_Setting_ReceiverIcon);
LV_IMG_DECLARE(LR_Setting_RightIcon);
LV_IMG_DECLARE(LR_Setting_ReturnIcon);
LV_IMG_DECLARE(LR_Setting_ScreenIcon);
LV_IMG_DECLARE(LR_Setting_StorageIcon);
LV_IMG_DECLARE(LR_Setting_TimeIcon);
LV_IMG_DECLARE(LR_StartBG);
LV_FONT_DECLARE(LR_AppSetting_ItemName);

static void mainScan_timer_cb(lv_timer_t * timer);
static void anim_working_cb(void * var, int32_t v);
static void anim_finish_cb(lv_anim_t * anim);
static void Setting_App_KeyOptionHandle(Lime_App_KeyOptions_e keyOption, bool isReset);
static Lime_App_KeyOptions_e getKey_Option(const LimeHal_KeyInfo_t* keyA, const LimeHal_KeyInfo_t* keyB);
static void settingHeadBar_AnimToNextTitle(const char* itemName);

void Setting_App_Create(lv_obj_t* father, uint16_t width, uint16_t height)
{
    Setting_App_ItemNum = 0;
    NowRunningIndex = -1;

    if(settingFaceObj != NULL || father == NULL)
    {
        LV_LOG_ERROR("Setting_App_Create: Invalid parameters");
        return;
    }

    settingFaceObj = lv_obj_create(father);
    lv_obj_set_size(settingFaceObj, width, height);
    lv_obj_set_style_bg_opa(settingFaceObj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_pad_all(settingFaceObj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_radius(settingFaceObj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_shadow_width(settingFaceObj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_border_width(settingFaceObj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_img_src(settingFaceObj, &LR_StartBG, LV_PART_MAIN | LV_STATE_DEFAULT);

    PrimaryDrawPicBedObj = lv_obj_create(settingFaceObj);
    lv_obj_set_size(PrimaryDrawPicBedObj, width, height);
    lv_obj_center(PrimaryDrawPicBedObj);
    lv_obj_set_style_bg_opa(PrimaryDrawPicBedObj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_pad_all(PrimaryDrawPicBedObj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_radius(PrimaryDrawPicBedObj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_shadow_width(PrimaryDrawPicBedObj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_border_width(PrimaryDrawPicBedObj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);

    Setting_App_AddItem("接收机", &LR_Setting_ReceiverIcon, Lime_App_Settings_ItemReceiver_Create, Lime_App_Settings_ItemReceiver_Destroy, Lime_App_Settings_ItemReceiver_Update);
    Setting_App_AddItem("时间", &LR_Setting_TimeIcon, Lime_App_Settings_ItemTime_Create, Lime_App_Settings_ItemTime_Destroy, Lime_App_Settings_ItemTime_Update);
    Setting_App_AddItem("屏幕", &LR_Setting_ScreenIcon, Lime_App_Settings_ItemScreen_Create, Lime_App_Settings_ItemScreen_Destroy, Lime_App_Settings_ItemScreen_Update);
    Setting_App_AddItem("存储", &LR_Setting_StorageIcon, Lime_App_Settings_ItemStorage_Create, Lime_App_Settings_ItemStorage_Destroy, Lime_App_Settings_ItemStorage_Update);
    Setting_App_AddItem("关于", &LR_Setting_AboutIcon, Lime_App_Settings_ItemAboutMe_Create, Lime_App_Settings_ItemAboutMe_Destroy, Lime_App_Settings_ItemAboutMe_Update);

    uint16_t startY = 56;
    for(uint32_t i = 0; i < Setting_App_ItemNum; i++)
    {
        lv_obj_t* itemBg = lv_obj_create(PrimaryDrawPicBedObj);
        lv_obj_set_size(itemBg, 175, 26);
        lv_obj_set_pos(itemBg, 33, startY);
        lv_obj_set_style_pad_all(itemBg, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
        lv_obj_set_style_radius(itemBg, 8, LV_PART_MAIN | LV_STATE_DEFAULT);
        lv_obj_set_style_shadow_width(itemBg, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
        lv_obj_set_style_border_width(itemBg, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
        lv_obj_set_style_bg_opa(itemBg, 200, LV_PART_MAIN | LV_STATE_DEFAULT);

        lv_obj_t * itemIcon = lv_img_create(itemBg);
        lv_img_set_src(itemIcon, Setting_App_Items[i].icon);
        lv_obj_align(itemIcon, LV_ALIGN_LEFT_MID, 8, 0);

        lv_obj_t* itemName = lv_label_create(itemBg);
        lv_obj_set_size(itemName, 93, 15);
        lv_obj_align_to(itemName, itemIcon, LV_ALIGN_OUT_RIGHT_MID, 6, 0);
        lv_label_set_text(itemName, Setting_App_Items[i].name);
        lv_obj_set_style_text_font(itemName, &LR_AppSetting_ItemName, LV_PART_MAIN | LV_STATE_DEFAULT);
        lv_obj_set_style_text_color(itemName, lv_color_hex(0x000000), LV_PART_MAIN | LV_STATE_DEFAULT);
        lv_label_set_long_mode(itemName, LV_LABEL_LONG_CLIP);
        lv_obj_set_style_text_align(itemName, LV_TEXT_ALIGN_LEFT, LV_PART_MAIN | LV_STATE_DEFAULT);

        lv_obj_t * itemRightIcon = lv_img_create(itemBg);
        lv_img_set_src(itemRightIcon, &LR_Setting_RightIcon);
        lv_obj_align(itemRightIcon, LV_ALIGN_RIGHT_MID, -13, 0);

        Setting_App_Items[i].itemObj = itemBg;
        Setting_App_Items[i].isSelected = false;

        startY += 32;
    }

    SecondaryDrawPicBedObj = lv_obj_create(settingFaceObj);
    lv_obj_set_size(SecondaryDrawPicBedObj, width, height);
    lv_obj_center(SecondaryDrawPicBedObj);
    lv_obj_set_style_bg_opa(SecondaryDrawPicBedObj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_pad_all(SecondaryDrawPicBedObj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_radius(SecondaryDrawPicBedObj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_shadow_width(SecondaryDrawPicBedObj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_border_width(SecondaryDrawPicBedObj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);

    settingHeadBarObj = lv_obj_create(settingFaceObj);
    lv_obj_set_size(settingHeadBarObj, width, 30);
    lv_obj_align(settingHeadBarObj, LV_ALIGN_TOP_MID, 0, 0);
    lv_obj_set_style_bg_opa(settingHeadBarObj, 120, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_color(settingHeadBarObj, lv_color_hex(0xffbcbc), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_pad_all(settingHeadBarObj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_radius(settingHeadBarObj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_shadow_width(settingHeadBarObj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_border_width(settingHeadBarObj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);

    /* child 0 */
    lv_obj_t* settingHeadLabel = lv_label_create(settingHeadBarObj);
    lv_obj_set_size(settingHeadLabel, 60, 15);
    lv_obj_center(settingHeadLabel);
    lv_label_set_text(settingHeadLabel, "设置");
    lv_obj_set_style_text_font(settingHeadLabel, &LR_AppSetting_ItemName, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_color(settingHeadLabel, lv_color_hex(0x000000), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_align(settingHeadLabel, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_DEFAULT);

    /* child 1 */
    lv_obj_t* settingItemLabel = lv_label_create(settingHeadBarObj);
    lv_obj_set_size(settingItemLabel, 60, 15);
    lv_obj_center(settingItemLabel);
    lv_label_set_text(settingItemLabel, "设置");
    lv_obj_set_style_text_font(settingItemLabel, &LR_AppSetting_ItemName, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_color(settingItemLabel, lv_color_hex(0x000000), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_align(settingItemLabel, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_add_flag(settingItemLabel, LV_OBJ_FLAG_HIDDEN);

    /* child 2 */
    lv_obj_t* returnIcon = lv_img_create(settingHeadBarObj);
    lv_img_set_src(returnIcon, &LR_Setting_ReturnIcon);
    lv_obj_align(returnIcon, LV_ALIGN_LEFT_MID, 10, 0);
    lv_obj_add_flag(returnIcon, LV_OBJ_FLAG_HIDDEN);


    lv_anim_init(&animHeadBarRun);
    lv_anim_set_var(&animHeadBarRun, &animHeadBarRun);
    lv_anim_set_exec_cb(&animHeadBarRun, anim_working_cb);
    lv_anim_set_values(&animHeadBarRun, 0, 100);
    lv_anim_set_time(&animHeadBarRun, 400);
    lv_anim_set_path_cb(&animHeadBarRun, lv_anim_path_ease_out);
    lv_anim_set_ready_cb(&animHeadBarRun, anim_finish_cb);
    lv_anim_set_user_data(&animHeadBarRun, (void*)&animHeadBarRun);

    static uint8_t timerInitFlag;
    timerInitFlag = 1;
    mainScan_timer = lv_timer_create(mainScan_timer_cb, 30, (void*)&timerInitFlag);
}
void Setting_App_Destroy(void)
{
    if(settingFaceObj != NULL)
    {
        lv_obj_del(settingFaceObj);
        settingFaceObj = NULL;
    }

    if(mainScan_timer != NULL)
    {
        lv_timer_del(mainScan_timer);
        mainScan_timer = NULL;
    }
}

static void Setting_App_AddItem(char *name,
                         const lv_img_dsc_t* icon,
                         void (*itemCreate)(lv_obj_t* father),
                         void (*itemDestroy)(void),
                         void (*itemUpdate)(Lime_App_KeyOptions_e keyOption))
{
    if(Setting_App_ItemNum >= LIME_APP_MAX_SETTING_ITEM_NUM)
    {
        LV_LOG_ERROR("Setting_App_AddItem: Too many items");
        return;
    }

    if((name == NULL) || (icon == NULL) || (itemCreate == NULL) || (itemDestroy == NULL) || (itemUpdate == NULL))
    {
        LV_LOG_ERROR("Setting_App_AddItem: Invalid parameters");
        return;
    }

    snprintf(Setting_App_Items[Setting_App_ItemNum].name, LIME_APP_MAX_SETTING_NAME_LEN, "%s", name);
    Setting_App_Items[Setting_App_ItemNum].icon = icon;
    Setting_App_Items[Setting_App_ItemNum].itemCreate = itemCreate;
    Setting_App_Items[Setting_App_ItemNum].itemDestroy = itemDestroy;
    Setting_App_Items[Setting_App_ItemNum].itemUpdate = itemUpdate;

    Setting_App_ItemNum++;
}

static Lime_App_KeyOptions_e getKey_Option(const LimeHal_KeyInfo_t* keyA, const LimeHal_KeyInfo_t* keyB)
{
    if((keyA->sw_up != keyB->sw_up) && ((keyA->sw_up % 2) == 1))
    {
        return Lime_App_KeyOptions_Up;
    }
    else if((keyA->sw_down != keyB->sw_down) && ((keyA->sw_down % 2) == 1))
    {
        return Lime_App_KeyOptions_Down;
    }
    else if((keyA->sw_left != keyB->sw_left) && ((keyA->sw_left % 2) == 1))
    {
        return Lime_App_KeyOptions_Left;
    }
    else if((keyA->sw_right != keyB->sw_right) && ((keyA->sw_right % 2) == 1))
    {
        return Lime_App_KeyOptions_Right;
    }
    else if((keyA->sw_set != keyB->sw_set) && ((keyA->sw_set % 2) == 1))
    {
        return Lime_App_KeyOptions_Set;
    }
    else if((keyA->kr != keyB->kr) && (keyA->kr != 0))
    {
        return Lime_App_KeyOptions_Return;
    }
    else
    {
        return Lime_App_KeyOptions_None;
    }
}

static void mainScan_timer_cb(lv_timer_t * timer)
{
    static LimeHal_KeyInfo_t SavedkeyInfo;
    const LimeHal_KeyInfo_t *LimHalkeyInfo = (const LimeHal_KeyInfo_t *)&LimeHAL_GetInfoPin()->keyInfo;
    Lime_App_KeyOptions_e keyOption = Lime_App_KeyOptions_None;

    /* init all static variable */
    uint8_t *timerInitFlag = (uint8_t*)lv_timer_get_user_data(timer);

    if(*timerInitFlag)
    {
        *timerInitFlag = 0;
        Setting_App_KeyOptionHandle(Lime_App_KeyOptions_None, true);
        goto Sync_Key_Info;
    }

    /* check key info */
    keyOption = getKey_Option(LimHalkeyInfo, &SavedkeyInfo);

    // if(keyOption != Lime_App_KeyOptions_None)
    //     LV_LOG_USER("keyOption: %d", keyOption);


    Setting_App_KeyOptionHandle(keyOption, false);


    /* sync Saved key Info */
Sync_Key_Info:
    SavedkeyInfo.sw_up = LimHalkeyInfo->sw_up;
    SavedkeyInfo.sw_set = LimHalkeyInfo->sw_set;
    SavedkeyInfo.sw_left = LimHalkeyInfo->sw_left;
    SavedkeyInfo.sw_right = LimHalkeyInfo->sw_right;
    SavedkeyInfo.sw_down = LimHalkeyInfo->sw_down;
    SavedkeyInfo.kr = LimHalkeyInfo->kr;
}

static bool isEnterSubItem = false;
static void anim_working_cb(void * var, int32_t v)
{
    if(settingFaceObj == NULL)
    {
        LV_LOG_ERROR("anim_working_cb: settingFaceObj is NULL");
        return;
    }

    lv_obj_t* settingHeadLabel = lv_obj_get_child(settingHeadBarObj, 0);
    lv_obj_t* settingItemLabel = lv_obj_get_child(settingHeadBarObj, 1);
    lv_obj_t* returnIcon = lv_obj_get_child(settingHeadBarObj, 2);

    float x = fmapWithLimit(v, 0, 100, 91, 5);
    uint8_t clrChG = fmapWithLimit(v, 0, 100, 0, 0x7A);  //#007afd
    uint8_t clrChB = fmapWithLimit(v, 0, 100, 0, 0xFD);
    lv_obj_align_to(settingHeadLabel, settingHeadBarObj, LV_ALIGN_LEFT_MID , x, 0);
    lv_obj_set_style_text_color(settingHeadLabel, lv_color_hex(0x00 << 16 | clrChG << 8 | clrChB), LV_PART_MAIN | LV_STATE_DEFAULT);

    uint8_t opaCh2A = fmapWithLimit(v, 50, 100, 0, 255);
    lv_obj_set_style_opa(settingItemLabel, opaCh2A, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_clear_flag(settingItemLabel, LV_OBJ_FLAG_HIDDEN);

    uint8_t opaChC = fmapWithLimit(v, 60, 100, 0, 255);
    lv_obj_set_style_opa(returnIcon, opaChC, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_img_recolor(returnIcon, lv_color_hex(0x2ea3ff), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_clear_flag(returnIcon, LV_OBJ_FLAG_HIDDEN);
}
static void anim_finish_cb(lv_anim_t * anim)
{
    if(settingFaceObj == NULL)
    {
        LV_LOG_ERROR("anim_finish_cb: settingFaceObj is NULL");
        return;
    }

    lv_obj_t* settingHeadLabel = lv_obj_get_child(settingHeadBarObj, 0);
    lv_obj_t* settingItemLabel = lv_obj_get_child(settingHeadBarObj, 1);
    lv_obj_t* returnIcon = lv_obj_get_child(settingHeadBarObj, 2);

    if(isEnterSubItem)
    {
        lv_obj_clear_flag(settingItemLabel, LV_OBJ_FLAG_HIDDEN);
        lv_obj_clear_flag(returnIcon, LV_OBJ_FLAG_HIDDEN);
    }
    else
    {
        lv_obj_add_flag(settingItemLabel, LV_OBJ_FLAG_HIDDEN);
        lv_obj_add_flag(returnIcon, LV_OBJ_FLAG_HIDDEN);
    }
}

static void settingHeadBar_AnimToNextTitle(const char* itemName)
{
    if(settingFaceObj == NULL)
    {
        LV_LOG_ERROR("settingHeadBar_AnimToNextTitle: settingFaceObj is NULL");
        return;
    }

    lv_obj_t* settingItemLabel = lv_obj_get_child(settingHeadBarObj, 1);

    if(itemName != NULL)
    {
        /* enter sub item */
        isEnterSubItem = true;

        lv_label_set_text(settingItemLabel, itemName);
        lv_anim_set_values(&animHeadBarRun, 0, 100);
        lv_anim_start(&animHeadBarRun);
    }
    else
    {
        /* exit sub item */
        isEnterSubItem = false;

        lv_anim_set_values(&animHeadBarRun, 100, 0);
        lv_anim_start(&animHeadBarRun);
    }

}

#define LV_SETTING_ITEM_IS_SELECTED(index, isSelect) \
    do{\
        if((index > Setting_App_ItemNum) || (Setting_App_Items[index].itemObj == NULL))\
        {\
            LV_LOG_ERROR("LV_SETTING_ITEM_IS_SELECTED: Invalid index");\
        }\
        if(isSelect)\
        {\
            lv_obj_set_style_bg_color(Setting_App_Items[index].itemObj, lv_color_hex(SETTING_SELECT_COLOR_HEX), LV_PART_MAIN | LV_STATE_DEFAULT);\
            Setting_App_Items[index].isSelected = true;\
        }\
        else\
        {\
            lv_obj_set_style_bg_color(Setting_App_Items[index].itemObj, lv_color_hex(SETTING_UNSELECT_COLOR_HEX), LV_PART_MAIN | LV_STATE_DEFAULT);\
            Setting_App_Items[index].isSelected = false;\
        }\
    }while(0)


static void Setting_App_KeyOptionHandle(Lime_App_KeyOptions_e keyOption, bool isReset)
{
    static int32_t lastSelectIndex = 0;
    static int32_t nowSelectIndex = -1;
    static int32_t nextSelectIndex = 0;

    if(isReset)
    {
        lastSelectIndex = 0;
        nowSelectIndex = -1;
        nextSelectIndex = 0;

        return;
    }

    if(Setting_App_ItemNum == 0)
    {
        LV_LOG_ERROR("Setting_App_KeyOptionHandle: No item");
        return;
    }

    if(NowRunningIndex == -1)
    {
        for(uint32_t i = 0; i < Setting_App_ItemNum; i++)
        {
            if(Setting_App_Items[i].isSelected)
            {
                nowSelectIndex = i;
                if(i == 0)
                {
                    if(Setting_App_ItemNum > 1)
                    {
                        nextSelectIndex = 1;
                        lastSelectIndex = Setting_App_ItemNum - 1;
                    }
                }
                else if(i == Setting_App_ItemNum - 1)
                {
                    nextSelectIndex = 0;
                    lastSelectIndex = Setting_App_ItemNum - 2;
                }
                else
                {
                    nextSelectIndex = i + 1;
                    lastSelectIndex = i - 1;
                }
                break;
            }
        }

        switch(keyOption)
        {
            case Lime_App_KeyOptions_Up:
            {
                nowSelectIndex = (nowSelectIndex == -1) ? 0 : nowSelectIndex;

                LV_SETTING_ITEM_IS_SELECTED(nowSelectIndex, false);
                LV_SETTING_ITEM_IS_SELECTED(nextSelectIndex, false);
                LV_SETTING_ITEM_IS_SELECTED(lastSelectIndex, true);

                break;
            }
            case Lime_App_KeyOptions_Down:
            {
                nowSelectIndex = (nowSelectIndex == -1) ? 0 : nowSelectIndex;

                LV_SETTING_ITEM_IS_SELECTED(lastSelectIndex, false);
                LV_SETTING_ITEM_IS_SELECTED(nowSelectIndex, false);
                LV_SETTING_ITEM_IS_SELECTED(nextSelectIndex, true);

                break;
            }
            case Lime_App_KeyOptions_Set:
            {
                if(nowSelectIndex == -1)
                {
                    break;
                }

                NowRunningIndex = nowSelectIndex;

                Setting_App_Items[NowRunningIndex].itemCreate(SecondaryDrawPicBedObj);
                settingHeadBar_AnimToNextTitle((const char*)Setting_App_Items[NowRunningIndex].name);

                lv_obj_add_flag(PrimaryDrawPicBedObj, LV_OBJ_FLAG_HIDDEN);

                break;
            }
						default:
							break;
        }
    }
    else
    {
        if(keyOption == Lime_App_KeyOptions_Return)
        {
            Setting_App_Items[NowRunningIndex].itemDestroy();
            settingHeadBar_AnimToNextTitle(NULL);
            NowRunningIndex = -1;

            lv_obj_clear_flag(PrimaryDrawPicBedObj, LV_OBJ_FLAG_HIDDEN);
        }
        else
        {
            Setting_App_Items[NowRunningIndex].itemUpdate(keyOption);
        }
    }
}
