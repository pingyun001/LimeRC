#include "Lime_MessageBox.h"

LV_FONT_DECLARE(Lime_MessageBox_TitleFont);
LV_FONT_DECLARE(Lime_MessageBox_InfoFont);

static lv_obj_t* meBox = NULL;

static void meBox_timer_cb(lv_timer_t * timer);
static bool initFlag = false;
static uint8_t *glob_tracePercent = NULL;
static uint32_t glob_timeOut_ms = 0;
static lv_timer_t *scanTimer = NULL;

static lv_anim_t mainObj_InAnim;
static void mainObj_InAnim_cb(lv_anim_t * anim);

void Lime_MessageBox_Show(const char* title, const char* message, uint8_t *tracePercent, uint32_t timeOut_ms)
{
    if(meBox != NULL)
    {
        LV_LOG_ERROR("MessageBox is already shown");
        return;
    }

    meBox = lv_msgbox_create(NULL);
    lv_obj_set_size(meBox, 150, 100);
    lv_msgbox_add_title(meBox, title);
    lv_obj_t * header = lv_msgbox_get_header(meBox);
    lv_obj_set_size(header, 150, 30);
    lv_obj_t *titleLabel = lv_obj_get_child(header, 0);
    lv_obj_set_style_text_font(titleLabel, &Lime_MessageBox_TitleFont, LV_PART_MAIN | LV_STATE_DEFAULT);

    lv_obj_t * bodyLabel = lv_msgbox_add_text(meBox, message);
    lv_obj_set_style_text_font(bodyLabel, &Lime_MessageBox_InfoFont, LV_PART_MAIN | LV_STATE_DEFAULT);

    glob_tracePercent = tracePercent;
    glob_timeOut_ms = timeOut_ms;
    initFlag = true;

    lv_anim_init(&mainObj_InAnim);
    lv_anim_init(&mainObj_InAnim);
    lv_anim_set_var(&mainObj_InAnim, meBox);
    lv_anim_set_exec_cb(&mainObj_InAnim, (lv_anim_exec_xcb_t)lv_obj_set_y);
    lv_anim_set_values(&mainObj_InAnim, -200, 0);
    lv_anim_set_time(&mainObj_InAnim, 600);
    lv_anim_set_path_cb(&mainObj_InAnim, lv_anim_path_bounce);
    lv_anim_set_ready_cb(&mainObj_InAnim, mainObj_InAnim_cb);

    lv_anim_start(&mainObj_InAnim);
}
static void mainObj_InAnim_cb(lv_anim_t * anim)
{
    scanTimer = lv_timer_create(meBox_timer_cb, 10, NULL);
}

static void meBox_timer_cb(lv_timer_t * timer)
{
    static uint32_t time_ms = 0;
    static bool secondFinishFlag = false;


    if(initFlag)
    {
        initFlag = false;
        secondFinishFlag = false;
        time_ms = 0;
    }

    if(secondFinishFlag && (time_ms > 800))
    {
        goto finish;
    }

    /* tranced work finished */
    if((glob_tracePercent!= NULL) && (*glob_tracePercent == 100) && (!secondFinishFlag))
    {
        // LV_LOG_USER("Trace work finished");
        lv_obj_t * bodyLabel = lv_obj_get_child(lv_msgbox_get_content(meBox), 0);
        lv_label_set_text(bodyLabel, "Success!");
        lv_obj_set_style_text_color(bodyLabel, lv_color_hex(0x387d46), LV_PART_MAIN | LV_STATE_DEFAULT);
        secondFinishFlag = true;
        time_ms = 0;
    }

    /* time out */
    if(glob_timeOut_ms > 0 && time_ms >= glob_timeOut_ms)
    {
        goto finish;
    }

    time_ms += 10;
    return;


finish:
    lv_timer_del(scanTimer);
    scanTimer = NULL;
    lv_msgbox_close(meBox);
    meBox = NULL;
}

void Lime_MessageBox_Close(void)
{
    if(meBox != NULL)
    {
        lv_msgbox_close(meBox);
        meBox = NULL;
    }
    if(scanTimer != NULL)
    {
        lv_timer_del(scanTimer);
        scanTimer = NULL;
    }
}
