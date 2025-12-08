#include "Lime_LockFace.h"
#include "string.h"
#include "stdio.h"

#define AUTO_FREE_MEMORY    1                   //在用户上拉锁屏壁纸后，是否释放内存

static lv_obj_t * lockFace_father = NULL;
static lv_obj_t * lock_face = NULL;
static lv_obj_t * mainTimeLabel = NULL;
static lv_obj_t * mainDataLabel = NULL;
static lv_obj_t * remainInfoLabel = NULL;
static lv_obj_t* gifBg = NULL;

static lv_anim_t changePosAnim = {0};

LV_FONT_DECLARE(LR_LockFace_EnSource_MainTimeFont);
LV_FONT_DECLARE(LR_LockFace_ChSource_MainDataFont);
LV_FONT_DECLARE(LR_LockFace_ChSource_RemainFont);

static void changePosAnim_working_cb(void * var, int32_t v);
static void changePosAnim_finished_cb(lv_anim_t * anim);

void Lime_LockFace_Init(lv_obj_t * father)
{
    LV_LOG_USER("Lime_LockFace_Init");
    if(father != NULL)
    {
        lockFace_father = father;
    }
    lock_face = lv_obj_create(father);
    lv_obj_set_size(lock_face, 240, 240);
    lv_obj_set_y(lock_face, -240);
    lv_obj_set_style_pad_all(lock_face, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_border_width(lock_face, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_shadow_width(lock_face, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_radius(lock_face, 0, LV_PART_MAIN | LV_STATE_DEFAULT);

    gifBg = lv_gif_create(lock_face);
#if USING_LIME_HARDWARE
    lv_gif_set_src(gifBg, "0:LimeLvResources/scenery1.gif");
#else
    lv_gif_set_src(gifBg, "D:LimeLvResources/scenery1.gif");

#endif
    lv_obj_center(gifBg);


    mainTimeLabel = lv_label_create(lock_face);
    lv_label_set_text(mainTimeLabel, "12:34");
    lv_obj_set_style_text_font(mainTimeLabel, &LR_LockFace_EnSource_MainTimeFont, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_color(mainTimeLabel, lv_color_hex(0x000000), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_align_to(mainTimeLabel, lock_face, LV_ALIGN_TOP_MID, 0, 40);

    mainDataLabel = lv_label_create(lock_face);
    lv_label_set_text(mainDataLabel, "贰零贰伍年 贰月拾贰日");
    lv_obj_set_style_text_font(mainDataLabel, &LR_LockFace_ChSource_MainDataFont, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_color(mainDataLabel, lv_color_hex(0x000000), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_align_to(mainDataLabel, lock_face, LV_ALIGN_TOP_MID, 0, 106);

    remainInfoLabel = lv_label_create(lock_face);
    lv_label_set_text(remainInfoLabel, "按下中键解锁");
    lv_obj_set_style_text_font(remainInfoLabel, &LR_LockFace_ChSource_RemainFont, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_color(remainInfoLabel, lv_color_hex(0xFFFFFF), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_align_to(remainInfoLabel, lock_face, LV_ALIGN_BOTTOM_MID, 0, -8);

    lv_anim_init(&changePosAnim);
    lv_anim_set_var(&changePosAnim, lock_face);
    lv_anim_set_exec_cb(&changePosAnim, changePosAnim_working_cb);
    lv_anim_set_values(&changePosAnim, 0, 240);
    lv_anim_set_time(&changePosAnim, 1000);
    lv_anim_set_path_cb(&changePosAnim, lv_anim_path_ease_in_out);
    lv_anim_set_ready_cb(&changePosAnim, changePosAnim_finished_cb);
}

void Lime_LockFace_Destroy(void)
{
#if AUTO_FREE_MEMORY
    if(lock_face!= NULL)
    {
        LV_LOG_USER("Lime_LockFace_Destroy");
        lv_obj_del(lock_face);
        lock_face = NULL;
    }
#endif
}

void Lime_LockFace_SetTime(uint8_t hour, uint8_t minute)
{
    if (lock_face == NULL || mainTimeLabel == NULL || hour > 23 || minute > 59)
    {
        LV_LOG_ERROR("lock_face or mainTimeLabel is NULL or hour or minute is invalid");
        return;
    }

    char timeStr[6];
    snprintf(timeStr, 6, "%02d:%02d", hour, minute);
    lv_label_set_text(mainTimeLabel, timeStr);
}

const char* chineseStr[11] = {"零", "壹", "贰", "叁", "肆", "伍", "陆", "柒", "捌", "玖", "拾"};
void Lime_LockFace_SetDate(uint32_t year, uint8_t month, uint8_t day) {
    if (lock_face == NULL || mainDataLabel == NULL || month > 12 || day > 31) {
        LV_LOG_ERROR("lock_face or mainDataLabel is NULL or month/day invalid");
        return;
    }

    // 处理年份部分（直接拆分为四位独立数字）
    char yearPart[16];
    snprintf(yearPart, sizeof(yearPart), "%s%s%s%s",
             chineseStr[year / 1000 % 10],
             chineseStr[year / 100 % 10],
             chineseStr[year / 10 % 10],
             chineseStr[year % 10]);

    // 处理月份部分
    char monthPart[16] = {0};
    if (month >= 10) {
        switch (month) {
            case 10: snprintf(monthPart, sizeof(monthPart), "拾月"); break;
            case 11: snprintf(monthPart, sizeof(monthPart), "拾壹月"); break;
            case 12: snprintf(monthPart, sizeof(monthPart), "拾贰月"); break;
        }
    } else {
        snprintf(monthPart, sizeof(monthPart), "%s月", chineseStr[month]);
    }

    // 处理日期部分
    char dayPart[16] = {0};
    if (day <= 9) {
        snprintf(dayPart, sizeof(dayPart), "%s日", chineseStr[day]);
    } else if (day == 10) {
        snprintf(dayPart, sizeof(dayPart), "拾日");
    } else if (day >= 11 && day <= 19) {
        snprintf(dayPart, sizeof(dayPart), "拾%s日", chineseStr[day % 10]);
    } else if (day == 20) {
        snprintf(dayPart, sizeof(dayPart), "贰拾日");
    } else if (day >= 21 && day <= 29) {
        snprintf(dayPart, sizeof(dayPart), "贰拾%s日", chineseStr[day % 10]);
    } else if (day == 30) {
        snprintf(dayPart, sizeof(dayPart), "叁拾日");
    } else if (day == 31) {
        snprintf(dayPart, sizeof(dayPart), "叁拾壹日");
    }

    // 组合最终字符串
    char dateStr[64];
    snprintf(dateStr, sizeof(dateStr), "%s年 %s%s", yearPart, monthPart, dayPart);
    lv_label_set_text(mainDataLabel, dateStr);
    lv_obj_align_to(mainDataLabel, lock_face, LV_ALIGN_TOP_MID, 0, 106);
}

void Lime_LockFace_ChangePosition(bool isUpped)
{
    int32_t nowPosY = 0;

    if (lock_face == NULL)
    {
        Lime_LockFace_Init(lockFace_father);
        lv_obj_set_pos(lock_face, 0, -240);

        nowPosY = -240;
    }
    else
    {
        nowPosY = lv_obj_get_y(lock_face);
    }

    lv_gif_pause(gifBg);


    if (isUpped)
    {
        lv_anim_set_values(&changePosAnim, nowPosY, -240);
    }
    else
    {
        lv_anim_set_values(&changePosAnim, nowPosY, 0);
    }

    lv_anim_start(&changePosAnim);
}

static void changePosAnim_working_cb(void * var, int32_t v)
{
    // LV_LOG_USER("changePosAnim_working_cb: %d", v);

    lv_obj_t * obj = (lv_obj_t *)var;
    lv_obj_set_y(obj, v);
}

static void changePosAnim_finished_cb(lv_anim_t * anim)
{
    LV_LOG_USER("changePosAnim_finished_cb");
    lv_gif_resume(gifBg);

    if(lv_obj_get_y(lock_face) == -240)
    {
        Lime_LockFace_Destroy();
    }
}
