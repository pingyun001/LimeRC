#include "Lime_App_StartConnFace.h"
#include "Lime_App_Hal.h"
#include "Lime_LittleAppFirmware.h"

#include <string.h>

#define CONN_SELECT_COLOR_HEX 0xffd08d
#define CONN_UNSELECT_COLOR_HEX 0xc6c6c6

static lv_obj_t *start_conFace_obj = NULL;
static lv_obj_t *selectListObj = NULL;
static lv_obj_t *jumpBtnObj = NULL;
static lv_obj_t *waitingImg = NULL;
static lv_timer_t *connScanTimer = NULL;
static lv_anim_t titleInOutAnim;
static lv_anim_t selectListBgAnim;
static lv_anim_t jumpBtnAnim;

static bool isLimeFirmwareCalledTempMark = false;
static bool isLimeFirmwareCalledFlag = false;

static void timer_cb(lv_timer_t * timer);
static void conn_SyncItem(void);
static void conn_SyncJumpBtn(void);
static uint8_t Lime_LV_Obj_has_been_Selected(lv_obj_t *obj);
static void conn_Add_Item(const char *name, const LimeHAL_NRF_Status_e status);
static void conn_CloseMyself_And_GoToNextInterface(void);
static void conn_SyncItem_From_HalInfo(const LimeHAL_NRF_Item_Info_t *itemInfo, lv_obj_t * itemBg);
static void enter_anim_cb(lv_anim_t * anim);
static void exit_anim_cb(lv_anim_t * anim);

typedef enum {
    keyInputMove_NoMove = 0,
    keyInputMove_Up = 1,
    keyInputMove_Down,
    keyInputMove_PressSet,
}keyInputMove_e;

static void conn_move_selectPin(keyInputMove_e moveDir);

LV_IMG_DECLARE(LR_StartBG);
LV_IMG_DECLARE(LR_Conn_Waiting);
LV_IMG_DECLARE(LR_Conn_NRF_Bad);
LV_IMG_DECLARE(LR_Conn_NRF_Good);
LV_IMG_DECLARE(LR_Conn_NRF_History);
LV_FONT_DECLARE(LR_ConnFace_TitleFont);
LV_FONT_DECLARE(LR_ConnFace_ItemNameFont);
LV_FONT_DECLARE(LR_ConnFace_ItemConnStatusFont);

void Connect_App_Create(lv_obj_t* father, uint16_t width, uint16_t height)
{
    isLimeFirmwareCalledTempMark = true;

    Lime_App_StartConnFace_Create(father, width, height);
}


void Lime_App_StartConnFace_Create(lv_obj_t* father, uint16_t width, uint16_t height)
{
    if(father == NULL)
    {
        LV_LOG_ERROR("father is NULL");
        return;
    }

    if(start_conFace_obj != NULL)
    {
        LV_LOG_ERROR("start_conFace_obj is not NULL");
        return;
    }

    isLimeFirmwareCalledFlag = isLimeFirmwareCalledTempMark;
    isLimeFirmwareCalledTempMark = false;

    start_conFace_obj = lv_obj_create(father);
    lv_obj_set_size(start_conFace_obj, width, height);
    lv_obj_center(start_conFace_obj);
    lv_obj_set_style_bg_opa(start_conFace_obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_pad_all(start_conFace_obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_radius(start_conFace_obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_shadow_width(start_conFace_obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_border_width(start_conFace_obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_clear_flag(start_conFace_obj, LV_OBJ_FLAG_CLICKABLE);
    lv_obj_clear_flag(start_conFace_obj, LV_OBJ_FLAG_SCROLLABLE);
    lv_obj_set_style_bg_img_src(start_conFace_obj, &LR_StartBG, LV_PART_MAIN | LV_STATE_DEFAULT);

    lv_obj_t *titleLabel = lv_label_create(start_conFace_obj);
    lv_obj_set_pos(titleLabel, 38, 42);
    lv_label_set_text(titleLabel, "扫描接收机");
    lv_obj_set_style_text_font(titleLabel, &LR_ConnFace_TitleFont, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_color(titleLabel, lv_color_hex(0xFFFFFF), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_align(titleLabel, LV_TEXT_ALIGN_LEFT, LV_PART_MAIN | LV_STATE_DEFAULT);

    waitingImg = lv_img_create(start_conFace_obj);
    lv_image_set_src(waitingImg, &LR_Conn_Waiting);
    lv_obj_align_to(waitingImg, titleLabel, LV_ALIGN_OUT_RIGHT_MID, 5, 0);
    lv_image_set_pivot(waitingImg, 8, 9);
    lv_obj_add_flag(waitingImg, LV_OBJ_FLAG_HIDDEN);

    jumpBtnObj = lv_obj_create(start_conFace_obj);
    lv_obj_set_size(jumpBtnObj, 100, 25);
    lv_obj_set_pos(jumpBtnObj, 70, 193);
    lv_obj_set_style_radius(jumpBtnObj, 8, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_color(jumpBtnObj, lv_color_hex(CONN_UNSELECT_COLOR_HEX), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_border_width(jumpBtnObj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_pad_all(jumpBtnObj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_clear_flag(jumpBtnObj, LV_OBJ_FLAG_SCROLLABLE);

    lv_obj_t *jumpBtnLabel = lv_label_create(jumpBtnObj);
    lv_label_set_text(jumpBtnLabel, "暂不连接");
    lv_obj_set_size(jumpBtnLabel, 100, 13);
    lv_obj_align(jumpBtnLabel, LV_ALIGN_CENTER, 0, -2);
    lv_obj_set_style_text_font(jumpBtnLabel, &LR_ConnFace_TitleFont, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_color(jumpBtnLabel, lv_color_hex(0xFFFFFF), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_align(jumpBtnLabel, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_DEFAULT);

    selectListObj = lv_obj_create(start_conFace_obj);
    lv_obj_set_size(selectListObj, 180, 120);
    lv_obj_set_pos(selectListObj, 30, 60);
    lv_obj_set_style_radius(selectListObj, 10, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_color(selectListObj, lv_color_hex(0xFFFFFF), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_border_width(selectListObj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_shadow_width(selectListObj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_scrollbar_mode(selectListObj, LV_SCROLLBAR_MODE_OFF);
    lv_obj_set_style_pad_all(selectListObj, 6, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_pad_gap(selectListObj, 4, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_flex_flow(selectListObj, LV_FLEX_FLOW_COLUMN);
    lv_obj_set_flex_align(selectListObj, LV_FLEX_ALIGN_START, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER);

    /* animation initialization */
    lv_anim_init(&titleInOutAnim);
    lv_anim_set_var(&titleInOutAnim, titleLabel);
    lv_anim_set_exec_cb(&titleInOutAnim, (lv_anim_exec_xcb_t)lv_obj_set_y);
    lv_anim_set_values(&titleInOutAnim, -20, 42);
    lv_anim_set_time(&titleInOutAnim, 1000);
    lv_anim_set_path_cb(&titleInOutAnim, lv_anim_path_bounce);
    lv_anim_set_ready_cb(&titleInOutAnim, enter_anim_cb);

    lv_anim_init(&selectListBgAnim);
    lv_anim_set_var(&selectListBgAnim, selectListObj);
    lv_anim_set_exec_cb(&selectListBgAnim, (lv_anim_exec_xcb_t)lv_obj_set_y);
    lv_anim_set_values(&selectListBgAnim, -130, 60);
    lv_anim_set_time(&selectListBgAnim, 500);
    lv_anim_set_path_cb(&selectListBgAnim, lv_anim_path_bounce);

    lv_anim_init(&jumpBtnAnim);
    lv_anim_set_var(&jumpBtnAnim, jumpBtnObj);
    lv_anim_set_exec_cb(&jumpBtnAnim, (lv_anim_exec_xcb_t)lv_obj_set_y);
    lv_anim_set_values(&jumpBtnAnim, 250, 193);
    lv_anim_set_time(&jumpBtnAnim, 1000);
    lv_anim_set_path_cb(&jumpBtnAnim, lv_anim_path_bounce);

    lv_anim_start(&titleInOutAnim);
    lv_anim_start(&selectListBgAnim);
    lv_anim_start(&jumpBtnAnim);
}

static void enter_anim_cb(lv_anim_t * anim)
{
    static uint8_t isInit;
    isInit = 1;
    connScanTimer = lv_timer_create(timer_cb, 10, (void*)&isInit);

    lv_obj_clear_flag(waitingImg, LV_OBJ_FLAG_HIDDEN);
}


static void timer_cb(lv_timer_t * timer)
{
    static uint16_t incAng = 0;
    static uint8_t dividerImg = 0;
    static LimeHal_KeyInfo_t SavedkeyInfo;
    const LimeHal_KeyInfo_t *LimHalkeyInfo = (const LimeHal_KeyInfo_t *)&LimeHAL_GetInfoPin()->keyInfo;

    uint8_t *isInit = (uint8_t*)lv_timer_get_user_data(timer);

    if(*isInit)
    {
        *isInit = 0;

        SavedkeyInfo.sw_up = LimHalkeyInfo->sw_up;
        SavedkeyInfo.sw_set = LimHalkeyInfo->sw_set;
        SavedkeyInfo.sw_down = LimHalkeyInfo->sw_down;

        LV_LOG_USER("timer_cb init");
    }

    /* waitingImg rotate */
    if(dividerImg ++ > 10)
    {
        dividerImg = 0;
        incAng += 450;
        if(incAng >= 3600)
        {
            incAng = 0;
        }
        if(waitingImg != NULL)
        {
            lv_img_set_angle(waitingImg, incAng);
        }
    }

    /* sync nrf info by item*/
    conn_SyncItem();

    /* sync jump button */
    conn_SyncJumpBtn();

    /* key input move detect*/
    keyInputMove_e moveDir = keyInputMove_NoMove;
    if((SavedkeyInfo.sw_up != LimHalkeyInfo->sw_up) && ((LimHalkeyInfo->sw_up % 2) == 1))
        moveDir = keyInputMove_Up;
    else if((SavedkeyInfo.sw_down != LimHalkeyInfo->sw_down) && ((LimHalkeyInfo->sw_down % 2) == 1))
        moveDir = keyInputMove_Down;
    else if((SavedkeyInfo.sw_set != LimHalkeyInfo->sw_set) && ((LimHalkeyInfo->sw_set % 2) == 1))
        moveDir = keyInputMove_PressSet;

    /* move select pin or select item*/
    if(moveDir != keyInputMove_NoMove)
    {
        LV_LOG_USER("moveDir = %d", moveDir);
        conn_move_selectPin(moveDir);
    }

    /* sync Saved key Info */
    SavedkeyInfo.sw_up = LimHalkeyInfo->sw_up;
    SavedkeyInfo.sw_set = LimHalkeyInfo->sw_set;
    SavedkeyInfo.sw_down = LimHalkeyInfo->sw_down;
}

static void conn_SyncJumpBtn(void)
{
    lv_obj_t *jumpBtnLabel = lv_obj_get_child(jumpBtnObj, 0);

    /* connect app is called by Lime App Firmware */
    if(isLimeFirmwareCalledFlag)
    {
        if(strcmp(lv_label_get_text(jumpBtnLabel), "完成") != 0)
        {
            lv_label_set_text(jumpBtnLabel, "完成");
        }

        if( !Lime_LV_Obj_has_been_Selected(jumpBtnObj))
        {
            lv_obj_set_style_bg_color(jumpBtnObj, lv_color_hex(0x51ba53), LV_PART_MAIN | LV_STATE_DEFAULT);
        }

        /* do not need to sync  LimeHAL_NRF_Has_ConnectedItem() */
        return;
    }

    if(LimeHAL_NRF_Has_ConnectedItem() && (strcmp(lv_label_get_text(jumpBtnLabel), "下一步") != 0))
    {
        lv_label_set_text(jumpBtnLabel, "下一步");

        if( !Lime_LV_Obj_has_been_Selected(jumpBtnObj))
        {
            lv_obj_set_style_bg_color(jumpBtnObj, lv_color_hex(0x51ba53), LV_PART_MAIN | LV_STATE_DEFAULT);
        }
    }
    if(!LimeHAL_NRF_Has_ConnectedItem() && (strcmp(lv_label_get_text(jumpBtnLabel), "暂不连接") != 0))
    {
        lv_label_set_text(jumpBtnLabel, "暂不连接");
        if( !Lime_LV_Obj_has_been_Selected(jumpBtnObj))
        {
            lv_obj_set_style_bg_color(jumpBtnObj, lv_color_hex(CONN_UNSELECT_COLOR_HEX), LV_PART_MAIN | LV_STATE_DEFAULT);
        }
    }
}

static void conn_SyncItem(void)
{
    const LimeHAL_NRF_Info_t *halNrfInfo = &LimeHAL_GetInfoPin()->nrfInfo;
    uint16_t itemNum = lv_obj_get_child_cnt(selectListObj);
    uint16_t itemToShow = halNrfInfo->totalNum;

    if(itemNum != itemToShow)
    {
        /* delete all item */
        lv_obj_t *item = lv_obj_get_child(selectListObj, 0);
        while(item != NULL)
        {
            lv_obj_del(item);
            item = lv_obj_get_child(selectListObj, 0);
        }

        /* add all item as new item*/
        for(uint16_t i = 0; i < itemToShow; i++)
        {
            conn_Add_Item(halNrfInfo->itemInfo[i].name, halNrfInfo->itemInfo[i].status);
        }
    }
    else
    {
        /* sync all info to lvgl interface */
        for(uint16_t i = 0; i < itemToShow; i++)
        {
            lv_obj_t *itemBg = lv_obj_get_child(selectListObj, i);

            conn_SyncItem_From_HalInfo(&halNrfInfo->itemInfo[i], itemBg);
        }
    }
}

static void conn_SyncItem_From_HalInfo(const LimeHAL_NRF_Item_Info_t *itemInfo, lv_obj_t * itemBg)
{
    lv_obj_t *nameLabel = lv_obj_get_child(itemBg, 0);
    lv_obj_t *imgHistory = lv_obj_get_child(itemBg, 1);
    lv_obj_t *imgBad = lv_obj_get_child(itemBg, 2);
    lv_obj_t *imgGood = lv_obj_get_child(itemBg, 3);
    lv_obj_t *labelConnStatus = lv_obj_get_child(itemBg, 4);

    /* sync conn status */
    if(labelConnStatus != NULL)
    {
        const char *showedString = lv_label_get_text(labelConnStatus);
        if((itemInfo->status & LimeHAL_NRF_Status_Connecting) && (strcmp(showedString, "连接中") != 0))
        {
            lv_obj_clear_flag(labelConnStatus, LV_OBJ_FLAG_HIDDEN);
            lv_label_set_text(labelConnStatus, "连接中");
            lv_obj_set_style_text_color(labelConnStatus, lv_color_hex(0x2c2c2c), LV_PART_MAIN | LV_STATE_DEFAULT);

            if(imgHistory != NULL)
                lv_obj_add_flag(imgHistory, LV_OBJ_FLAG_HIDDEN);
            if(imgBad != NULL)
                lv_obj_add_flag(imgBad, LV_OBJ_FLAG_HIDDEN);
            if(imgGood != NULL)
                lv_obj_add_flag(imgGood, LV_OBJ_FLAG_HIDDEN);
        }

        if((itemInfo->status & LimeHAL_NRF_Status_ConnFailed) && (strcmp(showedString, "失败") != 0))
        {
            lv_obj_clear_flag(labelConnStatus, LV_OBJ_FLAG_HIDDEN);
            lv_label_set_text(labelConnStatus, "失败");
            lv_obj_set_style_text_color(labelConnStatus, lv_color_hex(0xc61402), LV_PART_MAIN | LV_STATE_DEFAULT);

            if(imgHistory != NULL)
                lv_obj_add_flag(imgHistory, LV_OBJ_FLAG_HIDDEN);
            if(imgBad != NULL)
                lv_obj_add_flag(imgBad, LV_OBJ_FLAG_HIDDEN);
            if(imgGood != NULL)
                lv_obj_add_flag(imgGood, LV_OBJ_FLAG_HIDDEN);
        }

        if((itemInfo->status & LimeHAL_NRF_Status_Connected) && (strcmp(showedString, "已连接") != 0))
        {
            lv_obj_clear_flag(labelConnStatus, LV_OBJ_FLAG_HIDDEN);
            lv_label_set_text(labelConnStatus, "已连接");
            lv_obj_set_style_text_color(labelConnStatus, lv_color_hex(0x63bb65), LV_PART_MAIN | LV_STATE_DEFAULT);

            if(imgHistory != NULL)
                lv_obj_add_flag(imgHistory, LV_OBJ_FLAG_HIDDEN);
            if(imgBad != NULL)
                lv_obj_add_flag(imgBad, LV_OBJ_FLAG_HIDDEN);
            if(imgGood != NULL)
                lv_obj_add_flag(imgGood, LV_OBJ_FLAG_HIDDEN);
        }

        if((itemInfo->status & (LimeHAL_NRF_Status_Connecting | LimeHAL_NRF_Status_ConnFailed | LimeHAL_NRF_Status_Connected)))
        {
            /* no need(no position on screen) to show image history, bad, good status */
            return;
        }
        else
        {
            lv_obj_add_flag(labelConnStatus, LV_OBJ_FLAG_HIDDEN);
        }
    }

    /* sync image history status */
    if(imgHistory != NULL)
    {
        if(((itemInfo->status & LimeHAL_NRF_Status_HistoryUsed) != 0) && lv_obj_has_flag(imgHistory, LV_OBJ_FLAG_HIDDEN))
        {
            lv_obj_clear_flag(imgHistory, LV_OBJ_FLAG_HIDDEN);
        }
        if(((itemInfo->status & LimeHAL_NRF_Status_HistoryUsed) == 0) && ( !lv_obj_has_flag(imgHistory, LV_OBJ_FLAG_HIDDEN)))
        {
            lv_obj_add_flag(imgHistory, LV_OBJ_FLAG_HIDDEN);
        }
    }

    /* sync image bad & good status */
    if((imgBad != NULL) && (imgGood != NULL))
    {
        if(((itemInfo->status & LimeHAL_NRF_Status_IsOnline) != 0) && lv_obj_has_flag(imgGood, LV_OBJ_FLAG_HIDDEN))
        {
            lv_obj_clear_flag(imgGood, LV_OBJ_FLAG_HIDDEN);
            lv_obj_add_flag(imgBad, LV_OBJ_FLAG_HIDDEN);
        }
        if(((itemInfo->status & LimeHAL_NRF_Status_IsOnline) == 0) && ( !lv_obj_has_flag(imgGood, LV_OBJ_FLAG_HIDDEN)))
        {
            lv_obj_add_flag(imgGood, LV_OBJ_FLAG_HIDDEN);
            lv_obj_clear_flag(imgBad, LV_OBJ_FLAG_HIDDEN);
        }
    }
}

static void conn_Add_Item(const char *name, const LimeHAL_NRF_Status_e status)
{
    if(selectListObj == NULL)
    {
        LV_LOG_ERROR("selectListObj is NULL");
        return;
    }

    lv_obj_t* bg = lv_obj_create(selectListObj);
    lv_obj_set_size(bg, 170, 22);
    lv_obj_set_style_bg_color(bg, lv_color_hex(CONN_UNSELECT_COLOR_HEX), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_pad_all(bg, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_radius(bg, 7, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_shadow_width(bg, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_border_width(bg, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_clear_flag(bg, LV_OBJ_FLAG_SCROLLABLE);

    /* index 0 */
    lv_obj_t *nameLabel = lv_label_create(bg);
    lv_obj_set_size(nameLabel, 90, 13);
    lv_label_set_text(nameLabel, name);
    lv_obj_set_style_text_font(nameLabel, &LR_ConnFace_ItemNameFont, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_align(nameLabel, LV_TEXT_ALIGN_LEFT, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_color(nameLabel, lv_color_hex(0xFFFFFF), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_label_set_long_mode(nameLabel, LV_LABEL_LONG_SCROLL_CIRCULAR);
    lv_obj_align(nameLabel, LV_ALIGN_LEFT_MID, 10, 0);

    /* index 1 */
    lv_obj_t *imgHistory = lv_img_create(bg);
    lv_image_set_src(imgHistory, &LR_Conn_NRF_History);
    lv_obj_align(imgHistory, LV_ALIGN_RIGHT_MID, -5, 0);
    lv_obj_add_flag(imgHistory, LV_OBJ_FLAG_HIDDEN);

    /* index 2 */
    lv_obj_t *imgBad = lv_img_create(bg);
    lv_image_set_src(imgBad, &LR_Conn_NRF_Bad);
    lv_obj_align(imgBad, LV_ALIGN_RIGHT_MID, -27, 0);
    lv_obj_add_flag(imgBad, LV_OBJ_FLAG_HIDDEN);

    /* index 3 */
    lv_obj_t *imgGood = lv_img_create(bg);
    lv_image_set_src(imgGood, &LR_Conn_NRF_Good);
    lv_obj_align_to(imgGood, imgBad, LV_ALIGN_CENTER, 0, 0);
    lv_obj_add_flag(imgGood, LV_OBJ_FLAG_HIDDEN);

    /* index 4 */
    lv_obj_t *labelConnStatus = lv_label_create(bg);
    lv_obj_set_size(labelConnStatus, 90, 13);
    lv_label_set_text(labelConnStatus, "...");
    lv_obj_set_style_text_font(labelConnStatus, &LR_ConnFace_ItemConnStatusFont, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_color(labelConnStatus, lv_color_hex(0x63bb65), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_align(labelConnStatus, LV_TEXT_ALIGN_RIGHT, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_align(labelConnStatus, LV_ALIGN_RIGHT_MID, -10, 0);
    lv_obj_add_flag(labelConnStatus, LV_OBJ_FLAG_HIDDEN);

    if(status & LimeHAL_NRF_Status_HistoryUsed)
        lv_obj_clear_flag(imgHistory, LV_OBJ_FLAG_HIDDEN);
    if(status & LimeHAL_NRF_Status_IsOnline)
        lv_obj_clear_flag(imgGood, LV_OBJ_FLAG_HIDDEN);
    else
        lv_obj_clear_flag(imgBad, LV_OBJ_FLAG_HIDDEN);
}


static uint32_t Lime_lv_color_to_U32(lv_color_t color)
{
    uint32_t u32Val = 0;
    u32Val |= (color.red & 0xFF) << 24;
    u32Val |= (color.green & 0xFF) << 16;
    u32Val |= (color.blue & 0xFF) << 8;

    return u32Val;
}
static uint8_t Lime_LV_Obj_has_been_Selected(lv_obj_t *obj)
{
    if(obj == NULL)
    {
        LV_LOG_ERROR("obj is NULL");
        return 0;
    }

    uint32_t colorScr = Lime_lv_color_to_U32(lv_obj_get_style_bg_color(obj, LV_PART_MAIN));
    uint32_t colorDst = Lime_lv_color_to_U32(lv_color_hex(CONN_SELECT_COLOR_HEX));

    if(colorScr == colorDst)
        return 1;

    return 0;
}

static void conn_move_selectPin(keyInputMove_e moveDir)
{
    lv_obj_t* lastSelectObj = NULL;
    lv_obj_t* nowSelectObj = NULL;
    lv_obj_t* nextSelectObj = NULL;
    uint16_t listItemNum = lv_obj_get_child_cnt(selectListObj);
    int16_t selectItemNum = -1;

    /* find 3 selectObj */
    if(listItemNum == 0)
    {
        /* no item, jump btn is select */
        lastSelectObj = jumpBtnObj;
        nowSelectObj = jumpBtnObj;
        nextSelectObj = jumpBtnObj;
    }
    else
    {
        /* find selectObj */
        for(uint16_t i = 0; i < listItemNum; i++)
        {
            lv_obj_t *item = lv_obj_get_child(selectListObj, i);
            uint32_t colorScr = Lime_lv_color_to_U32(lv_obj_get_style_bg_color(item, LV_PART_MAIN));
            uint32_t colorDst = Lime_lv_color_to_U32(lv_color_hex(CONN_SELECT_COLOR_HEX));

            if(item == NULL)
            {
                LV_LOG_ERROR("item is NULL");
                return;
            }

            //if(colorScr == colorDst)
            if(Lime_LV_Obj_has_been_Selected(item))
            {
                if(i == 0)
                {
                    lastSelectObj = jumpBtnObj;
                    nowSelectObj = item;
                    if(listItemNum > 1)
                        nextSelectObj = lv_obj_get_child(selectListObj, 1);
                    else
                        nextSelectObj = jumpBtnObj;
                }
                else if(i == (listItemNum - 1))
                {
                    lastSelectObj = lv_obj_get_child(selectListObj, i - 1);
                    nowSelectObj = item;
                    nextSelectObj = jumpBtnObj;
                }
                else
                {
                    lastSelectObj = lv_obj_get_child(selectListObj, i - 1);
                    nowSelectObj = item;
                    nextSelectObj = lv_obj_get_child(selectListObj, i + 1);
                }

                selectItemNum = i;

                break;
            }
        }

        /* no item select, item 0 is select */
        if(lastSelectObj == NULL || nowSelectObj == NULL || nextSelectObj == NULL)
        {
            if(Lime_LV_Obj_has_been_Selected(jumpBtnObj))
            {
                lastSelectObj = lv_obj_get_child(selectListObj, -1);
                nowSelectObj = jumpBtnObj;
                nextSelectObj = lv_obj_get_child(selectListObj, 0);
            }
            else
            {
                lastSelectObj = lv_obj_get_child(selectListObj, 0);
                nowSelectObj = lv_obj_get_child(selectListObj, 0);
                nextSelectObj = lv_obj_get_child(selectListObj, 0);
            }
        }
    }

    if(lastSelectObj == NULL || nowSelectObj == NULL || nextSelectObj == NULL)
    {
        LV_LOG_ERROR("lastSelectObj or nowSelectObj or nextSelectObj is NULL");
        return;
    }

    switch(moveDir)
    {
        case keyInputMove_Up:
        {
            lv_obj_set_style_bg_color(nowSelectObj, lv_color_hex(CONN_UNSELECT_COLOR_HEX), LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_bg_color(nextSelectObj, lv_color_hex(CONN_UNSELECT_COLOR_HEX), LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_bg_color(lastSelectObj, lv_color_hex(CONN_SELECT_COLOR_HEX), LV_PART_MAIN | LV_STATE_DEFAULT);

            /* if connect success, keep jumpBtn as green*/
            if(( !Lime_LV_Obj_has_been_Selected(jumpBtnObj) ) && LimeHAL_NRF_Has_ConnectedItem())
            {
                lv_obj_set_style_bg_color(jumpBtnObj, lv_color_hex(0x51ba53), LV_PART_MAIN | LV_STATE_DEFAULT);
            }

            /* if connect app is called by Lime App Firmware, keep deselected jumpBtn as green*/
            if(( !Lime_LV_Obj_has_been_Selected(jumpBtnObj) ) && isLimeFirmwareCalledFlag)
            {
                lv_obj_set_style_bg_color(jumpBtnObj, lv_color_hex(0x51ba53), LV_PART_MAIN | LV_STATE_DEFAULT);
            }

            break;
        }
        case keyInputMove_Down:
        {
            lv_obj_set_style_bg_color(lastSelectObj, lv_color_hex(CONN_UNSELECT_COLOR_HEX), LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_bg_color(nowSelectObj, lv_color_hex(CONN_UNSELECT_COLOR_HEX), LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_bg_color(nextSelectObj, lv_color_hex(CONN_SELECT_COLOR_HEX), LV_PART_MAIN | LV_STATE_DEFAULT);

            /* if connect success, keep jumpBtn as green*/
            if(( !Lime_LV_Obj_has_been_Selected(jumpBtnObj) ) && LimeHAL_NRF_Has_ConnectedItem())
            {
                lv_obj_set_style_bg_color(jumpBtnObj, lv_color_hex(0x51ba53), LV_PART_MAIN | LV_STATE_DEFAULT);
            }

            /* if connect app is called by Lime App Firmware, keep deselected jumpBtn as green*/
            if(( !Lime_LV_Obj_has_been_Selected(jumpBtnObj) ) && isLimeFirmwareCalledFlag)
            {
                lv_obj_set_style_bg_color(jumpBtnObj, lv_color_hex(0x51ba53), LV_PART_MAIN | LV_STATE_DEFAULT);
            }

            break;
        }
        case keyInputMove_PressSet:
        {
            if(nowSelectObj == jumpBtnObj)
            {
                LV_LOG_USER("close this interface");

                if(isLimeFirmwareCalledFlag)
                    LittleAppFirmware_ExitAppWithAnim();
                else
                    conn_CloseMyself_And_GoToNextInterface();
            }
            else
            {
                LV_LOG_USER("select item %d", selectItemNum);
                //if selectItemNum == -1, value is invalid.
                if(selectItemNum != -1)
                {
                    LimeHAL_SelectNRF_To_Connect(selectItemNum);
                }
            }
            break;
        }
        default:
            LV_LOG_ERROR("moveDir is error");
            break;
    }
}

/* run animation and close this interface */
static void conn_CloseMyself_And_GoToNextInterface(void)
{
    /* del the main scan timer*/
    if(connScanTimer != NULL)
    {
        lv_timer_del(connScanTimer);
        connScanTimer = NULL;
    }

    /*hide the waitingImg*/
    lv_obj_add_flag(waitingImg, LV_OBJ_FLAG_HIDDEN);

    /* running exit animation */
    lv_anim_set_values(&titleInOutAnim, 42, -20);
    lv_anim_set_ready_cb(&titleInOutAnim, exit_anim_cb);
    lv_anim_set_values(&selectListBgAnim, 60, -130);
    lv_anim_set_values(&jumpBtnAnim, 193, 250);

    lv_anim_start(&titleInOutAnim);
    lv_anim_start(&selectListBgAnim);
    lv_anim_start(&jumpBtnAnim);
    //when animation is done, call the exit_anim_cb()
}

static void exit_anim_cb(lv_anim_t * anim)
{
    /* delete the start_conFace_obj */
    if(start_conFace_obj != NULL)
    {
        lv_obj_del(start_conFace_obj);
        start_conFace_obj = NULL;
    }

    /* jump to next interface */
    Lime_App_StartConnFace_Finish_Hook();
}

void Connect_App_Destroy(void)
{
    /* del the main scan timer*/
    if(connScanTimer != NULL)
    {
        lv_timer_del(connScanTimer);
        connScanTimer = NULL;
    }

    /* del the start_conFace_obj */
    if(start_conFace_obj != NULL)
    {
        lv_obj_del(start_conFace_obj);
        start_conFace_obj = NULL;
    }
}

/*
__weak void Lime_App_StartConnFace_Finish_Hook(void)
{

}

*/
