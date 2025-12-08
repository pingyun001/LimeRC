#include "Lime_App_RCCtrl.h"

static lv_obj_t* RCCtrlFaceObj = NULL;

void RCCtrl_App_Create(lv_obj_t* father, uint16_t w, uint16_t h)
{
    if(father == NULL || RCCtrlFaceObj != NULL)
    {
        LV_LOG_ERROR("father is NULL or RCCtrlFaceObj is not NULL");
        return;
    }

    RCCtrlFaceObj = lv_obj_create(father);
    lv_obj_set_size(RCCtrlFaceObj, w, h);
    lv_obj_center(RCCtrlFaceObj);

    lv_obj_t *sw = lv_switch_create(RCCtrlFaceObj);
    lv_obj_set_size(sw, 100, 50);
    lv_obj_align(sw, LV_ALIGN_CENTER, 0, 0);
}
void RCCtrl_App_Destroy(void)
{
    lv_obj_del(RCCtrlFaceObj);
    RCCtrlFaceObj = NULL;
}
