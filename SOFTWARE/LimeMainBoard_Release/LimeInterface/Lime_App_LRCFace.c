#include "Lime_App_LRCFace.h"
#include "Lime_App_Hal.h"
#include <string.h>
#include <stdio.h>
#include "Lime_MessageBox.h"

static lv_obj_t *LRC_FirstBgMaskForExit = NULL;
static lv_obj_t *LRC_AppFace_obj = NULL;
static lv_obj_t *LRC_AppFirstInitMask_img = NULL;

static lv_obj_t *HeadBar_obj = NULL;
static lv_obj_t *nowTime_label = NULL;
static lv_obj_t *battery_img = NULL;
static lv_obj_t *rcRssi_img = NULL;
static lv_obj_t *hdRssi_img = NULL;
static lv_obj_t *video_canvas = NULL;

static lv_obj_t *receiverBg_obj = NULL;

static lv_anim_t LRC_AppFace_FirstCreatAnim;
static lv_anim_t LRC_AppFace_titleInOutAnim;
static lv_anim_t LRC_AppFace_receiverInOutAnimA;
static lv_anim_t LRC_AppFace_FirstExitAnim;
static lv_timer_t *infoScanTimer = NULL;
static lv_timer_t *videoScanTimer = NULL;

static uint8_t LRC_inOutAnimRunStatus = 2;      //0: out, 1: running(no used), 2: in, 3: exit(first time)
static bool LRC_InitialCreateFlag = false;

const static lv_image_dsc_t *getHeadBarBatImgIndex(int8_t batteryLevel);
const static lv_image_dsc_t *getHeadBarRssiImgIndex(uint8_t rssiLevel);

static void LRC_AppFirstInitMaskImg_anim_cb(lv_anim_t * anim);
static void LRC_AppFace_titleInOutAnim_cb(lv_anim_t * anim);
static void infoScanTimer_cb(lv_timer_t * timer);
static void videoScanTimer_cb(lv_timer_t * timer);
static void LRC_SyncHeadBarInfo(const LimeHal_LRCFace_t * info);
static void LRC_SyncReceiverInfo(const LimeHal_LRCFace_t * info, lv_obj_t *receiverBg_obj);
static void LRC_AppFace_DelAllObjAndReturn(void);
static void anim_working_cb(void * var, int32_t v);
static void anim_finish_cb(lv_anim_t * anim);

typedef enum
{
    LRC_keyInputMove_None,
    LRC_keyInputMove_MidPress,
    LRC_keyInputMove_returnPress,
    LRC_keyInputMove_rightPress,
    LRC_keyInputMove_MAX,
}LRC_keyInputMove_e;
static void LRC_KeyPressed_Hook(LRC_keyInputMove_e keyDir);


LV_IMG_DECLARE(LR_StartBG);
LV_IMG_DECLARE(cameraDemo1);
LV_IMG_DECLARE(LR_Bat_Charge);
LV_IMG_DECLARE(LR_Bat_Full);
LV_IMG_DECLARE(LR_Bat_High);
LV_IMG_DECLARE(LR_Bat_Mid);
LV_IMG_DECLARE(LR_Bat_Low);
LV_IMG_DECLARE(LR_Wireless_Conn_Full);
LV_IMG_DECLARE(LR_Wireless_Conn_Mid);
LV_IMG_DECLARE(LR_Wireless_Conn_Low);
LV_IMG_DECLARE(LR_Wireless_NoConn);
LV_FONT_DECLARE(Lime_APP_LRCFace_TimeFont);
LV_FONT_DECLARE(Lime_APP_LRCFace_HeadBarLittleFont);

LV_IMG_DECLARE(LR_Receiver_Icon);
LV_IMG_DECLARE(LR_Name_Icon);
LV_FONT_DECLARE(Lime_APP_LRCFace_ReceiverInfoFont);

// void LRC_AppFace_Create(lv_obj_t *father, uint16_t w, uint16_t h)
// {
//     if(father != NULL)
//     {
//         LV_LOG_ERROR("LRC_AppFace_Create: father is not NULL");
//         return;
//     }

//     LRC_AppFace_InitialCreate(father, w, h);
// }

void LRC_AppFace_Destroy(void)
{
    LRC_AppFace_DelAllObjAndReturn();
}


void LRC_AppFace_InitialCreate(lv_obj_t *father, uint16_t width, uint16_t height)
{
    if(LRC_AppFace_obj != NULL)
    {
        LV_LOG_ERROR("LRC_AppFace_FirstCreate: LRC_AppFace_obj is not NULL");
        return;
    }

    if(father == NULL)
    {
        /* register app LRC face is running(for power up use)*/
        LimeHAL_AppRunInfo_runTake(true);

        LRC_FirstBgMaskForExit = lv_obj_create(lv_scr_act());
        lv_obj_set_size(LRC_FirstBgMaskForExit, width, height);
        lv_obj_center(LRC_FirstBgMaskForExit);
        lv_obj_set_style_pad_all(LRC_FirstBgMaskForExit, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
        lv_obj_set_style_radius(LRC_FirstBgMaskForExit, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
        lv_obj_set_style_shadow_width(LRC_FirstBgMaskForExit, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
        lv_obj_set_style_border_width(LRC_FirstBgMaskForExit, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
        lv_obj_clear_flag(LRC_FirstBgMaskForExit, LV_OBJ_FLAG_CLICKABLE);
        lv_obj_clear_flag(LRC_FirstBgMaskForExit, LV_OBJ_FLAG_SCROLLABLE);

        lv_anim_init(&LRC_AppFace_FirstExitAnim);
        lv_anim_set_var(&LRC_AppFace_FirstExitAnim, LRC_FirstBgMaskForExit);
        lv_anim_set_exec_cb(&LRC_AppFace_FirstExitAnim, anim_working_cb);
        lv_anim_set_values(&LRC_AppFace_FirstExitAnim, 240, 30);
        lv_anim_set_time(&LRC_AppFace_FirstExitAnim, 400);
        lv_anim_set_path_cb(&LRC_AppFace_FirstExitAnim, lv_anim_path_ease_out);
        lv_anim_set_ready_cb(&LRC_AppFace_FirstExitAnim, anim_finish_cb);

        LRC_AppFace_obj = lv_obj_create(LRC_FirstBgMaskForExit);

        LRC_InitialCreateFlag = true;

        LimeHAL_SyncAppRunStatus(0);
    }
    else
    {
        LRC_AppFace_obj = lv_obj_create(father);

        LRC_InitialCreateFlag = false;
    }

    lv_obj_set_size(LRC_AppFace_obj, width, height);
    lv_obj_center(LRC_AppFace_obj);
    lv_obj_set_style_bg_opa(LRC_AppFace_obj, 255, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_color(LRC_AppFace_obj, lv_color_hex(0x1f1f1f), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_pad_all(LRC_AppFace_obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_radius(LRC_AppFace_obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_shadow_width(LRC_AppFace_obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_border_width(LRC_AppFace_obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_clear_flag(LRC_AppFace_obj, LV_OBJ_FLAG_CLICKABLE);
    lv_obj_clear_flag(LRC_AppFace_obj, LV_OBJ_FLAG_SCROLLABLE);

    if(LimeHAL_GetInfoPin()->lrfaceInfo.videoFrameBuf == NULL)
    {
        lv_obj_t * img = lv_img_create(LRC_AppFace_obj);
        lv_img_set_src(img, &cameraDemo1);
        lv_obj_set_size(img, 240, 240);
        lv_obj_center(img);
    }
    else
    {
        /* add canvas */
        video_canvas = lv_canvas_create(LRC_AppFace_obj);
        lv_obj_set_size(video_canvas, 240, 240);
        lv_obj_center(video_canvas);
        lv_canvas_set_buffer(video_canvas, LimeHAL_GetInfoPin()->lrfaceInfo.videoFrameBuf, 240, 240, LV_COLOR_FORMAT_RGB565);

        videoScanTimer = lv_timer_create(videoScanTimer_cb, 33, NULL);
    }


    HeadBar_obj = lv_obj_create(LRC_AppFace_obj);
    lv_obj_set_size(HeadBar_obj, width, 24);
    lv_obj_align_to(HeadBar_obj, LRC_AppFace_obj, LV_ALIGN_TOP_MID, 0, -25);
    lv_obj_set_style_bg_color(HeadBar_obj, lv_color_hex(0x000000), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(HeadBar_obj, 179, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_pad_all(HeadBar_obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_radius(HeadBar_obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_shadow_width(HeadBar_obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_border_width(HeadBar_obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_clear_flag(HeadBar_obj, LV_OBJ_FLAG_CLICKABLE);
    lv_obj_clear_flag(HeadBar_obj, LV_OBJ_FLAG_SCROLLABLE);

    nowTime_label = lv_label_create(HeadBar_obj);
    lv_label_set_text(nowTime_label, "12:30");
    lv_obj_set_style_text_font(nowTime_label, &Lime_APP_LRCFace_TimeFont, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_align(nowTime_label, LV_TEXT_ALIGN_LEFT, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_color(nowTime_label, lv_color_hex(0xffffff), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_align_to(nowTime_label, HeadBar_obj, LV_ALIGN_LEFT_MID, 6, 0);

    lv_obj_t * RC_Label = lv_label_create(HeadBar_obj);
    lv_label_set_text(RC_Label, "RC");
    lv_obj_set_style_text_font(RC_Label, &Lime_APP_LRCFace_HeadBarLittleFont, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_color(RC_Label, lv_color_hex(0xffffff), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_align_to(RC_Label, HeadBar_obj, LV_ALIGN_LEFT_MID, 148, 5);

    rcRssi_img = lv_img_create(HeadBar_obj);
    lv_img_set_src(rcRssi_img, &LR_Wireless_Conn_Full);
    lv_obj_align_to(rcRssi_img, HeadBar_obj, LV_ALIGN_LEFT_MID, 153, 0);

    lv_obj_t * HD_Label = lv_label_create(HeadBar_obj);
    lv_label_set_text(HD_Label, "HD");
    lv_obj_set_style_text_font(HD_Label, &Lime_APP_LRCFace_HeadBarLittleFont, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_color(HD_Label, lv_color_hex(0xffffff), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_align_to(HD_Label, HeadBar_obj, LV_ALIGN_LEFT_MID, 176, 5);

    hdRssi_img = lv_img_create(HeadBar_obj);
    lv_img_set_src(hdRssi_img, &LR_Wireless_Conn_Mid);
    lv_obj_align_to(hdRssi_img, HeadBar_obj, LV_ALIGN_LEFT_MID, 183, 0);

    battery_img = lv_img_create(HeadBar_obj);
    lv_img_set_src(battery_img, &LR_Bat_Full);
    lv_obj_align_to(battery_img, HeadBar_obj, LV_ALIGN_RIGHT_MID, -5, 0);



    receiverBg_obj = lv_obj_create(LRC_AppFace_obj);
    lv_obj_set_size(receiverBg_obj, 65, 45);
    lv_obj_align_to(receiverBg_obj, LRC_AppFace_obj, LV_ALIGN_TOP_LEFT, -70, 34);
    lv_obj_set_style_bg_color(receiverBg_obj, lv_color_hex(0x000000), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(receiverBg_obj, 102, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_pad_all(receiverBg_obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_radius(receiverBg_obj, 8, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_shadow_width(receiverBg_obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_border_width(receiverBg_obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_clear_flag(receiverBg_obj, LV_OBJ_FLAG_CLICKABLE);
    lv_obj_clear_flag(receiverBg_obj, LV_OBJ_FLAG_SCROLLABLE);

    lv_obj_t * nameIcon = lv_img_create(receiverBg_obj);
    lv_img_set_src(nameIcon, &LR_Name_Icon);
    lv_obj_align_to(nameIcon, receiverBg_obj, LV_ALIGN_TOP_LEFT, 8, 8);

    lv_obj_t * receiverIcon = lv_img_create(receiverBg_obj);
    lv_img_set_src(receiverIcon, &LR_Receiver_Icon);
    lv_obj_align_to(receiverIcon, receiverBg_obj, LV_ALIGN_TOP_LEFT, 7, 25);
    const LimeHal_LRCFace_t * infoPin = (const LimeHal_LRCFace_t *)(&LimeHAL_GetInfoPin()->lrfaceInfo);
    LRC_SyncHeadBarInfo(infoPin);

    /* child 2: receiver name */
    lv_obj_t * labelReceiverName = lv_label_create(receiverBg_obj);
    lv_label_set_text(labelReceiverName, "NO-CONNECTION");
    lv_obj_set_size(labelReceiverName, 35, 12);
    lv_obj_set_style_text_font(labelReceiverName, &Lime_APP_LRCFace_ReceiverInfoFont, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_color(labelReceiverName, lv_color_hex(0xffffff), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_align(labelReceiverName, LV_TEXT_ALIGN_RIGHT, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_label_set_long_mode(labelReceiverName, LV_LABEL_LONG_SCROLL_CIRCULAR);
    lv_obj_set_style_anim_time(labelReceiverName, 5000, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_align_to(labelReceiverName, receiverBg_obj, LV_ALIGN_TOP_RIGHT, -8, 8);

    /* child 3: receiver battery voltage */
    lv_obj_t* receiverChannelLabel = lv_label_create(receiverBg_obj);
    lv_label_set_text(receiverChannelLabel, "CH0");
    lv_obj_set_size(receiverChannelLabel, 35, 12);
    lv_obj_set_style_text_font(receiverChannelLabel, &Lime_APP_LRCFace_ReceiverInfoFont, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_color(receiverChannelLabel, lv_color_hex(0xffffff), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_align(receiverChannelLabel, LV_TEXT_ALIGN_RIGHT, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_align_to(receiverChannelLabel, receiverBg_obj, LV_ALIGN_TOP_RIGHT, -8, 27);


    if(LRC_InitialCreateFlag)   //(LRC_AppFirstInitMask_img == NULL) &&
    {
        LRC_AppFirstInitMask_img = lv_img_create(LRC_AppFace_obj);
        lv_img_set_src(LRC_AppFirstInitMask_img, &LR_StartBG);
        lv_obj_set_size(LRC_AppFirstInitMask_img, 240, 240);
        lv_obj_center(LRC_AppFirstInitMask_img);

        lv_anim_init(&LRC_AppFace_FirstCreatAnim);
        lv_anim_set_var(&LRC_AppFace_FirstCreatAnim, LRC_AppFirstInitMask_img);
        lv_anim_set_exec_cb(&LRC_AppFace_FirstCreatAnim, (lv_anim_exec_xcb_t)lv_obj_set_y);
        lv_anim_set_values(&LRC_AppFace_FirstCreatAnim, 0, -250);
        lv_anim_set_time(&LRC_AppFace_FirstCreatAnim, 500);
        lv_anim_set_path_cb(&LRC_AppFace_FirstCreatAnim, lv_anim_path_ease_in);
        lv_anim_set_ready_cb(&LRC_AppFace_FirstCreatAnim, LRC_AppFirstInitMaskImg_anim_cb);

        lv_anim_start(&LRC_AppFace_FirstCreatAnim);
    }
    else
    {
        /* jump animation */
        LRC_AppFirstInitMaskImg_anim_cb(NULL);
    }
}

static void LRC_AppFirstInitMaskImg_anim_cb(lv_anim_t * anim)
{
    if(LRC_AppFirstInitMask_img != NULL)
    {
        lv_obj_del(LRC_AppFirstInitMask_img);
        LRC_AppFirstInitMask_img = NULL;
    }

    lv_anim_init(&LRC_AppFace_titleInOutAnim);
    lv_anim_set_var(&LRC_AppFace_titleInOutAnim, HeadBar_obj);
    lv_anim_set_exec_cb(&LRC_AppFace_titleInOutAnim, (lv_anim_exec_xcb_t)lv_obj_set_y);
    lv_anim_set_values(&LRC_AppFace_titleInOutAnim, -25, 0);
    lv_anim_set_time(&LRC_AppFace_titleInOutAnim, 300);
    lv_anim_set_path_cb(&LRC_AppFace_titleInOutAnim, lv_anim_path_ease_in);
    lv_anim_set_ready_cb(&LRC_AppFace_titleInOutAnim, LRC_AppFace_titleInOutAnim_cb);

    lv_anim_init(&LRC_AppFace_receiverInOutAnimA);
    lv_anim_set_var(&LRC_AppFace_receiverInOutAnimA, receiverBg_obj);
    lv_anim_set_exec_cb(&LRC_AppFace_receiverInOutAnimA, (lv_anim_exec_xcb_t)lv_obj_set_x);
    lv_anim_set_values(&LRC_AppFace_receiverInOutAnimA, -70, 7);
    lv_anim_set_time(&LRC_AppFace_receiverInOutAnimA, 200);
    lv_anim_set_path_cb(&LRC_AppFace_receiverInOutAnimA, lv_anim_path_ease_in);
    lv_anim_set_delay(&LRC_AppFace_receiverInOutAnimA, 200);

    lv_anim_start(&LRC_AppFace_titleInOutAnim);
    lv_anim_start(&LRC_AppFace_receiverInOutAnimA);

    LRC_inOutAnimRunStatus = 2;
}

static void LRC_AppFace_titleInOutAnim_cb(lv_anim_t * anim)
{
    if(infoScanTimer == NULL)
    {
        static uint32_t initFlag = 1;
        infoScanTimer = lv_timer_create(infoScanTimer_cb, 33, &initFlag);
    }

    if(LRC_inOutAnimRunStatus == 3)
    {
        if(LRC_InitialCreateFlag == true)
        {
            lv_anim_start(&LRC_AppFace_FirstExitAnim);
        }
    }
}

static void infoScanTimer_cb(lv_timer_t * timer)
{
    static uint8_t divider1 = 0;

    const LimeHal_LRCFace_t * infoPin = (const LimeHal_LRCFace_t *)(&LimeHAL_GetInfoPin()->lrfaceInfo);

    /* first create scan timer*/
    uint32_t *initFlagPin = lv_timer_get_user_data(timer);
    if(*initFlagPin)
    {
        *initFlagPin = 0;
        divider1 = 0;
        goto SyncKeyInfoEnd;
    }

    /* sync head bar and receiver info */
    if(divider1 ++ > 5)
    {
        divider1 = 0;
        LRC_SyncHeadBarInfo(infoPin);
    }
    if(divider1 == 3)
    {
        LRC_SyncReceiverInfo(infoPin, receiverBg_obj);
    }

    /* sync key status */
    const LimeHal_KeyInfo_t* keyInfo = (const LimeHal_KeyInfo_t *)(&LimeHAL_GetInfoPin()->keyInfo);
    static uint8_t keyMidCnt = 0;
    static uint8_t keyReturnCnt = 0;
    static uint8_t keyRightCnt = 0;
    static uint16_t longPressKrCnt = 0;
    LRC_keyInputMove_e keyDir = LRC_keyInputMove_None;
    if(((keyInfo->sw_set % 2) == 1) && (keyMidCnt != keyInfo->sw_set))
    {
        keyDir = LRC_keyInputMove_MidPress;
    }
    else if(((keyInfo->kr % 2) == 1) && (keyReturnCnt != keyInfo->kr))
    {
        /* key return short press*/
        //no use
    }
    else if(((keyInfo->sw_right % 2) == 1) && (keyRightCnt != keyInfo->sw_right))
    {
        keyDir = LRC_keyInputMove_rightPress;
        LV_LOG_USER("key right short press");
    }

    /* key return long press 800ms for first power on*/
    if((keyInfo->kr % 2) == 1)
        longPressKrCnt ++;
    else
        longPressKrCnt = 0;
    if(longPressKrCnt > 24)
    {
        longPressKrCnt = 0;

        if(LRC_InitialCreateFlag)
            keyDir = LRC_keyInputMove_returnPress;
    }


    LRC_KeyPressed_Hook(keyDir);

SyncKeyInfoEnd:
    keyMidCnt = keyInfo->sw_set;
    keyReturnCnt = keyInfo->kr;
    keyRightCnt = keyInfo->sw_right;
}

static void LRC_KeyPressed_Hook(LRC_keyInputMove_e keyDir)
{
    if(keyDir > LRC_keyInputMove_MAX)
    {
        LV_LOG_ERROR("keyDir is out of range:%d", keyDir);
        return;
    }

    switch(keyDir)
    {
        case LRC_keyInputMove_MidPress:
        {
            if(LRC_inOutAnimRunStatus == 0)
            {
                lv_anim_set_values(&LRC_AppFace_titleInOutAnim, lv_obj_get_y(HeadBar_obj), 0);
                lv_anim_set_values(&LRC_AppFace_receiverInOutAnimA, lv_obj_get_x(receiverBg_obj), 7);

                lv_anim_start(&LRC_AppFace_titleInOutAnim);
                lv_anim_start(&LRC_AppFace_receiverInOutAnimA);

                LRC_inOutAnimRunStatus = 2;
            }
            else if(LRC_inOutAnimRunStatus == 2)
            {
                lv_anim_set_values(&LRC_AppFace_titleInOutAnim, lv_obj_get_y(HeadBar_obj), -25);
                lv_anim_set_values(&LRC_AppFace_receiverInOutAnimA, lv_obj_get_x(receiverBg_obj), -70);

                lv_anim_start(&LRC_AppFace_titleInOutAnim);
                lv_anim_start(&LRC_AppFace_receiverInOutAnimA);

                LRC_inOutAnimRunStatus = 0;
            }
            break;
        }
        case LRC_keyInputMove_returnPress:
        {
            if(LRC_inOutAnimRunStatus == 2)
            {
                lv_anim_set_values(&LRC_AppFace_titleInOutAnim, lv_obj_get_y(HeadBar_obj), -25);
                lv_anim_set_values(&LRC_AppFace_receiverInOutAnimA, lv_obj_get_x(receiverBg_obj), -70);

                lv_anim_start(&LRC_AppFace_titleInOutAnim);
                lv_anim_start(&LRC_AppFace_receiverInOutAnimA);
            }
            else
            {
                if(LRC_InitialCreateFlag == true)
                {
                    lv_anim_start(&LRC_AppFace_FirstExitAnim);
                }
                else
                {
                    LRC_AppFace_DelAllObjAndReturn();
                }
            }

            LimeHAL_SyncAppRunStatus(-1);
            LRC_inOutAnimRunStatus = 3;

            break;
        }
        case LRC_keyInputMove_rightPress:
        {
            LimeHal_LRCFace_t *lrfaceInfo = &LimeHAL_GetInfoPin()->lrfaceInfo;
            if(lrfaceInfo->videoRssi == 0)
            {
                Lime_MessageBox_Show("警告","无法拍照-未连接图传接收机", NULL, 1000);
            }
            else
            {
                Lime_MessageBox_Show("进度","正在保存照片", LimeHAL_GetTakePicturePercent(), 2000);
                LimeHAL_TakePicture();
            }

            break;
        }
		default:
			break;
    }
}

static void LRC_SyncReceiverInfo(const LimeHal_LRCFace_t * info, lv_obj_t *receiverBg_obj)
{
    /* LRC_AppFace_obj can represent all obj */
    if(LRC_AppFace_obj == NULL)
    {
        LV_LOG_ERROR("LRC_AppFace_obj is NULL");
        return;
    }

    /* update receiver info */
    const char * nameTargetStr = (const char *)(&info->receiverName);
    lv_obj_t * nameLabel = lv_obj_get_child(receiverBg_obj, 2);
    if(memcmp(nameTargetStr, lv_label_get_text(nameLabel), strlen(nameTargetStr)) != 0)
    {
        lv_label_set_text(nameLabel, nameTargetStr);
    }

    /* udpate channel */
    uint8_t channel = info->receiverChannel > 128 ? 128 : info->receiverChannel;
    lv_obj_t * channelLabel = lv_obj_get_child(receiverBg_obj, 3);
    lime_base_set_label_string(channelLabel, "CH%d", channel);
}

static void LRC_SyncHeadBarInfo(const LimeHal_LRCFace_t * info)
{
    /* LRC_AppFace_obj can represent all obj */
    if(LRC_AppFace_obj == NULL)
    {
        LV_LOG_ERROR("LRC_AppFace_obj is NULL");
        return;
    }

    /* update all img.*/
    const lv_img_dsc_t * rcRssiImgTarget = getHeadBarRssiImgIndex(info->messaRssi);
    const lv_img_dsc_t * hdRssiImgTarget = getHeadBarRssiImgIndex(info->videoRssi);
    const lv_img_dsc_t * batteryImgTarget = getHeadBarBatImgIndex(info->rcBatInfo);
    if(lv_img_get_src(hdRssi_img) != hdRssiImgTarget)
    {
        lv_img_set_src(hdRssi_img, hdRssiImgTarget);
    }
    if(lv_img_get_src(rcRssi_img) != rcRssiImgTarget)
    {
        lv_img_set_src(rcRssi_img, rcRssiImgTarget);
    }
    if(lv_img_get_src(battery_img) != batteryImgTarget)
    {
        lv_img_set_src(battery_img, batteryImgTarget);
    }

    /* update time */
    uint8_t hourTarget = (info->time[0] > 59) ? 59 : info->time[0];
    uint8_t minTarget = (info->time[1] > 59) ? 59 : info->time[1];
    char timeStr[6] = {0};
    snprintf(timeStr, 6, "%02d:%02d", hourTarget, minTarget);
    if(memcmp(timeStr, lv_label_get_text(nowTime_label), 6) != 0)
    {
        lv_label_set_text(nowTime_label, timeStr);
    }
}


const static lv_image_dsc_t *getHeadBarBatImgIndex(int8_t batteryLevel)
{
    /* range: -1(charging), 0(low), 1(mid), 2(high), 3(full) */
    switch(batteryLevel)
    {
        case -1:
            return &LR_Bat_Charge;
        case 0:
            return &LR_Bat_Low;
        case 1:
            return &LR_Bat_Mid;
        case 2:
            return &LR_Bat_High;
        case 3:
            return &LR_Bat_Full;
        default:
        {
            LV_LOG_ERROR("ImgIndex is out of range:%d", batteryLevel);
            return &LR_Bat_Low;
        }
    }
}
const static lv_image_dsc_t *getHeadBarRssiImgIndex(uint8_t rssiLevel)
{
    /* range: 0(no connect), 1, 2, 3(full strength) */
    switch(rssiLevel)
    {
        case 0:
            return &LR_Wireless_NoConn;
        case 1:
            return &LR_Wireless_Conn_Low;
        case 2:
            return &LR_Wireless_Conn_Mid;
        case 3:
            return &LR_Wireless_Conn_Full;
        default:
        {
            LV_LOG_ERROR("ImgIndex is out of range:%d", rssiLevel);
            return &LR_Wireless_NoConn;
        }
    }
}

static void anim_working_cb(void * var, int32_t v)
{
    lv_obj_t * obj = (lv_obj_t *)var;
    lv_obj_set_size(obj, v, v);
    // lv_obj_set_style_opa(obj, v, LV_PART_ANY | LV_STATE_DEFAULT);
    lv_obj_center(obj);
}

static void anim_finish_cb(lv_anim_t * anim)
{
    LRC_AppFace_DelAllObjAndReturn();
}

static void LRC_AppFace_DelAllObjAndReturn(void)
{
    if(infoScanTimer != NULL)
    {
        lv_timer_del(infoScanTimer);
        infoScanTimer = NULL;
    }
    if(videoScanTimer != NULL)
    {
        lv_timer_del(videoScanTimer);
        videoScanTimer = NULL;
        video_canvas = NULL;
    }
    if(LRC_AppFace_obj != NULL)
    {
        lv_obj_del(LRC_AppFace_obj);
        LRC_AppFace_obj = NULL;
    }
    if(LRC_FirstBgMaskForExit != NULL)
    {
        lv_obj_del(LRC_FirstBgMaskForExit);
        LRC_FirstBgMaskForExit = NULL;
    }

    /* unregister running status */
    LimeHAL_AppRunInfo_runTake(false);

    if(LRC_InitialCreateFlag)
    {
        Lime_App_LRCFace_Finish_Hook();
    }

}

static void videoScanTimer_cb(lv_timer_t * timer)
{
    if(video_canvas == NULL)
        return;

    static uint8_t lastCnt = 0;
    const LimeHal_LRCFace_t * info = (const LimeHal_LRCFace_t *)(&LimeHAL_GetInfoPin()->lrfaceInfo);
    if(info->videoFrameCnt != lastCnt)
    {
        /* canvas update */
        lv_obj_invalidate(video_canvas);
    }

    lastCnt = info->videoFrameCnt;
}

/*

__weak void Lime_App_LRCFace_Finish_Hook(void)
{
    LV_LOG_USER("Lime_App_LRCFace_Finish_Hook");
}


*/
