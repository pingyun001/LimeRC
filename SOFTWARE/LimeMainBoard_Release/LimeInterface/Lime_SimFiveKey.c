#include "Lime_SimFiveKey.h"
#include "lvgl.h"
#include "Lime_LvMainFace.h"

#include "Lime_App_Hal.h"

lv_obj_t* keyUp = NULL;
lv_obj_t* keyDown = NULL;
lv_obj_t* keyLeft = NULL;
lv_obj_t* keyRight = NULL;
lv_obj_t* keyMiddle = NULL;
lv_obj_t* keyReturn = NULL;

static lv_obj_t* keyFace = NULL;

#define KEY_BG_OPA 100

static void SimKeyCallback(lv_event_t * e);

void Lime_SimFiveKey_Init(lv_obj_t *father)
{
    if(keyFace != NULL)
    {
        LV_LOG_ERROR("keyFace is not NULL");
        return;
    }

    keyFace = lv_obj_create(father);
    lv_obj_set_size(keyFace, 240, 240);
    lv_obj_center(keyFace);
    lv_obj_set_style_pad_all(keyFace, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(keyFace, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_border_width(keyFace, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_shadow_width(keyFace, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_radius(keyFace, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_clear_flag(keyFace, LV_OBJ_FLAG_CLICKABLE);
    lv_obj_clear_flag(keyFace, LV_OBJ_FLAG_SCROLLABLE);

    keyUp = lv_btn_create(keyFace);
    lv_obj_set_size(keyUp, 40, 20);
    lv_obj_align(keyUp, LV_ALIGN_TOP_MID, 0, 9);
    lv_obj_set_style_radius(keyUp, 4, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_color(keyUp, lv_color_hex(0x6794c3), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_shadow_width(keyUp, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(keyUp, KEY_BG_OPA, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_add_event_cb(keyUp, SimKeyCallback, LV_EVENT_PRESSED, NULL);
    lv_obj_add_event_cb(keyUp, SimKeyCallback, LV_EVENT_RELEASED, NULL);

    keyDown = lv_btn_create(keyFace);
    lv_obj_set_size(keyDown, 40, 20);
    lv_obj_align(keyDown, LV_ALIGN_BOTTOM_MID, 0, -9);
    lv_obj_set_style_radius(keyDown, 4, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_color(keyDown, lv_color_hex(0x6794c3), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_shadow_width(keyDown, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(keyDown, KEY_BG_OPA, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_add_event_cb(keyDown, SimKeyCallback, LV_EVENT_PRESSED, NULL);
    lv_obj_add_event_cb(keyDown, SimKeyCallback, LV_EVENT_RELEASED, NULL);

    keyLeft = lv_btn_create(keyFace);
    lv_obj_set_size(keyLeft, 20, 40);
    lv_obj_align(keyLeft, LV_ALIGN_LEFT_MID, 9, 0);
    lv_obj_set_style_radius(keyLeft, 4, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_color(keyLeft, lv_color_hex(0x6794c3), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_shadow_width(keyLeft, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(keyLeft, KEY_BG_OPA, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_add_event_cb(keyLeft, SimKeyCallback, LV_EVENT_PRESSED, NULL);
    lv_obj_add_event_cb(keyLeft, SimKeyCallback, LV_EVENT_RELEASED, NULL);

    keyRight = lv_btn_create(keyFace);
    lv_obj_set_size(keyRight, 20, 40);
    lv_obj_align(keyRight, LV_ALIGN_RIGHT_MID, -9, 0);
    lv_obj_set_style_radius(keyRight, 4, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_color(keyRight, lv_color_hex(0x6794c3), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_shadow_width(keyRight, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(keyRight, KEY_BG_OPA, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_add_event_cb(keyRight, SimKeyCallback, LV_EVENT_PRESSED, NULL);
    lv_obj_add_event_cb(keyRight, SimKeyCallback, LV_EVENT_RELEASED, NULL);

    keyMiddle = lv_btn_create(keyFace);
    lv_obj_set_size(keyMiddle, 40, 40);
    lv_obj_align(keyMiddle, LV_ALIGN_CENTER, 0, 0);
    lv_obj_set_style_radius(keyMiddle, 4, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_color(keyMiddle, lv_color_hex(0x6794c3), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_shadow_width(keyMiddle, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(keyMiddle, KEY_BG_OPA, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_add_event_cb(keyMiddle, SimKeyCallback, LV_EVENT_PRESSED, NULL);
    lv_obj_add_event_cb(keyMiddle, SimKeyCallback, LV_EVENT_RELEASED, NULL);

    keyReturn = lv_btn_create(keyFace);
    lv_obj_set_size(keyReturn, 40, 30);
    lv_obj_align(keyReturn, LV_ALIGN_CENTER, -60, 60);
    lv_obj_set_style_radius(keyReturn, 4, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_color(keyReturn, lv_color_hex(0x7f434a), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_shadow_width(keyReturn, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(keyReturn, KEY_BG_OPA, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_add_event_cb(keyReturn, SimKeyCallback, LV_EVENT_PRESSED, NULL);
    lv_obj_add_event_cb(keyReturn, SimKeyCallback, LV_EVENT_RELEASED, NULL);
}

void Lime_SimFiveKey_Destroy(void)
{
    if(keyFace == NULL)
    {
        LV_LOG_ERROR("keyFace is NULL");
        return;
    }

    lv_obj_del(keyFace);
    keyFace = NULL;

    LimeHAL_SetAllKeyToReleased();
}

uint8_t Lime_SimFiveKey_GetNowPressingKey(void)
{
    if(lv_obj_has_state(keyUp, LV_STATE_PRESSED))
    {
        return 1;
    }
    if(lv_obj_has_state(keyDown, LV_STATE_PRESSED))
    {
        return 2;
    }
    if(lv_obj_has_state(keyLeft, LV_STATE_PRESSED))
    {
        return 3;
    }
    if(lv_obj_has_state(keyRight, LV_STATE_PRESSED))
    {
        return 4;
    }
    if(lv_obj_has_state(keyMiddle, LV_STATE_PRESSED))
    {
        return 5;
    }
    return 0;
}



static void SimKeyCallback(lv_event_t * e)
{
    lv_obj_t * obj = lv_event_get_target(e);
    lv_event_code_t code = lv_event_get_code(e);
    static lv_event_code_t lastMidCode = LV_EVENT_CLICKED;

    if(obj == keyUp)
    {
        LimeHAL_KeyPressed_Hook(0);
    }
    else if(obj == keyMiddle)
    {
        LimeHAL_KeyPressed_Hook(1);
    }
    else if(obj == keyLeft)
    {
        LimeHAL_KeyPressed_Hook(2);
    }
    else if(obj == keyRight)
    {
        LimeHAL_KeyPressed_Hook(3);
    }
    else if(obj == keyDown)
    {
        LimeHAL_KeyPressed_Hook(4);
    }
    else if(obj == keyReturn)
    {
        LimeHAL_KeyPressed_Hook(5);
    }
}
