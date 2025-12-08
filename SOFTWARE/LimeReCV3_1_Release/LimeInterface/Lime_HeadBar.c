#include "Lime_HeadBar.h"
#include "string.h"
#include "stdio.h"
#include "Lime_App_Hal.h"

lv_obj_t * headBarFace = NULL;
lv_obj_t * batIconImg = NULL;
lv_obj_t * connIconImg = NULL;
lv_obj_t * timeLabel = NULL;

LV_IMG_DECLARE(LR_Bat_Charge);
LV_IMG_DECLARE(LR_Bat_Low);
LV_IMG_DECLARE(LR_Bat_Mid);
LV_IMG_DECLARE(LR_Bat_High);
LV_IMG_DECLARE(LR_Bat_Full);
LV_IMG_DECLARE(LR_Wireless_Conn_Full);
LV_IMG_DECLARE(LR_Wireless_Conn_Mid);
LV_IMG_DECLARE(LR_Wireless_Conn_Low);
LV_IMG_DECLARE(LR_Wireless_NoConn);
LV_FONT_DECLARE(LR_InfoBar_EnFont);

static void infoScanTimer_cb(lv_timer_t * timer);

void Lime_HeadBar_Create(lv_obj_t *parent)
{
    headBarFace = lv_obj_create(parent);
    lv_obj_set_size(headBarFace, LV_HOR_RES, 20);
    lv_obj_set_style_bg_opa(headBarFace, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_pad_all(headBarFace, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_radius(headBarFace, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_shadow_width(headBarFace, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    // lv_obj_set_style_line_width(headBarFace, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_border_width(headBarFace, 0, LV_PART_MAIN | LV_STATE_DEFAULT);

    batIconImg = lv_img_create(headBarFace);
    lv_img_set_src(batIconImg, &LR_Bat_Low);
    lv_obj_align_to(batIconImg, headBarFace, LV_ALIGN_TOP_RIGHT, -5, 5);

    connIconImg = lv_img_create(headBarFace);
    lv_img_set_src(connIconImg, &LR_Wireless_Conn_Full);
    lv_obj_align_to(connIconImg, headBarFace, LV_ALIGN_TOP_RIGHT, -35, 1);

    timeLabel = lv_label_create(headBarFace);
    lv_label_set_text(timeLabel, "12:34");
    lv_obj_set_style_text_font(timeLabel, &LR_InfoBar_EnFont, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_color(timeLabel, lv_color_hex(0xFFFFFF), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_align_to(timeLabel, headBarFace, LV_ALIGN_TOP_LEFT, 7, 6);

    lv_timer_t *infoScanTimer = lv_timer_create(infoScanTimer_cb, 1000, NULL);
}

void Lime_HeadBar_SetBatteryLevel(int8_t level)
{
    if(batIconImg == NULL) return;
    switch(level)
    {
        case 0:
            lv_img_set_src(batIconImg, &LR_Bat_Low);
            break;
        case 1:
            lv_img_set_src(batIconImg, &LR_Bat_Mid);
            break;
        case 2:
            lv_img_set_src(batIconImg, &LR_Bat_High);
            break;
        case 3:
            lv_img_set_src(batIconImg, &LR_Bat_Full);
            break;
        case -1:
            lv_img_set_src(batIconImg, &LR_Bat_Charge);
        default:
            break;
    }
    lv_obj_align_to(batIconImg, headBarFace, LV_ALIGN_TOP_RIGHT, -5, 5);
}

void Lime_HeadBar_SetConnectionStatus(int8_t connLevel)//range: 0(no connect), 1, 2, 3(full strength)
{
    if(connIconImg == NULL) return;

    switch(connLevel)
    {
        case 0:
            lv_img_set_src(connIconImg, &LR_Wireless_NoConn);
            break;
        case 1:
            lv_img_set_src(connIconImg, &LR_Wireless_Conn_Low);
            break;
        case 2:
            lv_img_set_src(connIconImg, &LR_Wireless_Conn_Mid);
            break;
        case 3:
            lv_img_set_src(connIconImg, &LR_Wireless_Conn_Full);

            break;
        default:
            break;
    }
}
void Lime_HeadBar_SetTime(uint8_t hour, uint8_t minute)
{
    if(timeLabel == NULL) return;
    if(hour > 24)return;
    if(minute > 60)return;

    char timeStr[10];
    snprintf(timeStr, 10, "%02d:%02d", hour, minute);
    lv_label_set_text(timeLabel, timeStr);
    lv_obj_align_to(timeLabel, headBarFace, LV_ALIGN_TOP_LEFT, 7, 6);
}

static void infoScanTimer_cb(lv_timer_t * timer)
{
    LimeHAL_MainFaceInfo_t *mainFaceInfo = NULL;
    mainFaceInfo = &LimeHAL_GetInfoPin()->mainFaceInfo;

    Lime_HeadBar_SetBatteryLevel(mainFaceInfo->rcBatInfo);
    Lime_HeadBar_SetTime(mainFaceInfo->time[0], mainFaceInfo->time[1]);
    Lime_HeadBar_SetConnectionStatus(mainFaceInfo->messaRssi);
}
