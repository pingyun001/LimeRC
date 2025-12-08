#include "Lime_App_Photo.h"
#include "Lime_App_Hal.h"
#include <string.h>
#include <stdio.h>
#include <ctype.h>

#define PHOTO_SELECT_COLOR_HEX 0xffd08d
#define PHOTO_UNSELECT_COLOR_HEX 0xffffff

typedef enum
{
    photo_KeyOption_None = 0,
    photo_KeyOption_Up,
    photo_KeyOption_Down,
    photo_KeyOption_Left,
    photo_KeyOption_Right,
    photo_KeyOption_Set,
    photo_KeyOption_Kr,
}photo_KeyOption_e;

typedef enum
{
    photoDelAnim_status_Idle = 0,
    photoDelAnim_status_ShowDelLabel,
    photoDelAnim_status_HidwDelLabel,
    photoDelAnim_status_DelPhoto,
}photoDelAnim_status_e;

static lv_obj_t * photo_app_obj = NULL;
static lv_obj_t * photo_list_obj = NULL;
static lv_timer_t *photoKeyScanTimer = NULL;

static lv_obj_t * photo_openWindow_obj = NULL;
static lv_obj_t *photo_del_obj = NULL;
static lv_anim_t mainObj_InAnim;
static lv_anim_t titleObj_InOutAnim;

static lv_anim_t delObj_delTransAnim;
static photoDelAnim_status_e photoAnimStatus;
static lv_obj_t* delObj_targetObj = NULL;

static uint32_t Lime_ScanDiskToFindPhoto(const char *path);
static imgType_e Lime_GetImgType(const char* path);
static void to_lower_case(char *str);
static void list_click_event_cb(lv_event_t * e);
static void timer_cb(lv_timer_t * timer);
static void keyScan_workHandle(photo_KeyOption_e dir, bool isReset);
static void openWindowToShowPhotoByPath(const char *path);
static void mainObj_InAnim_cb(lv_anim_t * anim);
static void delObj_delTransAnim_working_cb(void * var, int32_t v);
static void delObj_delTransAnim_finish_cb(lv_anim_t * anim);

const char pathRoot[] = "D:/LimeLvResources/";

LV_IMG_DECLARE(LR_StartBG);
LV_IMG_DECLARE(LR_PhotoNotSupport);
LV_FONT_DECLARE(Lime_AppPhotoBoldFont);
LV_FONT_DECLARE(LR_AppPhotoNameList_Font);

void Photo_App_Create(lv_obj_t *father, uint16_t w, uint16_t h)
{
    if(photo_app_obj != NULL)
    {
        LV_LOG_ERROR("Photo_App_Create: photo_app_obj is not NULL");
        return;
    }

    photo_app_obj = lv_obj_create(father);
    lv_obj_set_size(photo_app_obj, w, h);
    lv_obj_center(photo_app_obj);
    lv_obj_set_style_bg_color(photo_app_obj, lv_color_hex(0xFFFFFF), 0);
    lv_obj_set_style_border_width(photo_app_obj, 0, 0);
    lv_obj_set_style_radius(photo_app_obj, 0, 0);
    lv_obj_set_style_pad_all(photo_app_obj, 0, 0);
    lv_obj_set_style_shadow_width(photo_app_obj, 0, 0);
    lv_obj_clear_flag(photo_app_obj, LV_OBJ_FLAG_SCROLLABLE);

    lv_obj_set_style_bg_img_src(photo_app_obj, &LR_StartBG, 0);

    photo_list_obj = lv_list_create(photo_app_obj);
    lv_obj_set_size(photo_list_obj, 200, 200);
    lv_obj_center(photo_list_obj);


    uint32_t totalFileNum = Lime_ScanDiskToFindPhoto(pathRoot);
    if(totalFileNum == 0)
    {
        LV_LOG_USER("No photo found");
    }

    lv_anim_init(&mainObj_InAnim);
    lv_anim_init(&mainObj_InAnim);
    lv_anim_set_var(&mainObj_InAnim, photo_list_obj);
    lv_anim_set_exec_cb(&mainObj_InAnim, (lv_anim_exec_xcb_t)lv_obj_set_y);
    lv_anim_set_values(&mainObj_InAnim, -200, 0);
    lv_anim_set_time(&mainObj_InAnim, 600);
    lv_anim_set_path_cb(&mainObj_InAnim, lv_anim_path_bounce);
    lv_anim_set_ready_cb(&mainObj_InAnim, mainObj_InAnim_cb);

    photoAnimStatus = photoDelAnim_status_Idle;
    lv_anim_init(&delObj_delTransAnim);
    // lv_anim_set_var(&delObj_delTransAnim, &delObj_delTransAnim);
    lv_anim_set_exec_cb(&delObj_delTransAnim, delObj_delTransAnim_working_cb);
    lv_anim_set_values(&delObj_delTransAnim, 0, 100);
    lv_anim_set_time(&delObj_delTransAnim, 300);
    lv_anim_set_path_cb(&delObj_delTransAnim, lv_anim_path_ease_out);
    lv_anim_set_ready_cb(&delObj_delTransAnim, delObj_delTransAnim_finish_cb);

    lv_anim_start(&mainObj_InAnim);
}

static void mainObj_InAnim_cb(lv_anim_t * anim)
{
    if(photoKeyScanTimer == NULL)
    {
        static uint32_t initFlag = 1;
        photoKeyScanTimer = lv_timer_create(timer_cb, 20, &initFlag);
    }
}
void Photo_App_Destroy(void)
{
    if(photoKeyScanTimer != NULL)
    {
        lv_timer_del(photoKeyScanTimer);
        photoKeyScanTimer = NULL;
    }
    if(photo_app_obj == NULL)
    {
        LV_LOG_ERROR("Photo_App_Destroy: photo_app_obj is NULL");
        return;
    }
    lv_obj_del(photo_app_obj);
    photo_app_obj = NULL;

    photo_openWindow_obj = NULL;
}

static uint32_t Lime_ScanDiskToFindPhoto(const char *path)
{
    uint32_t totalFileNum = 0;
    lv_fs_dir_t dir;
    char file_name[256];  // 存储文件/文件夹名称的缓冲区

    lv_fs_res_t res = lv_fs_dir_open(&dir, path);
    if (res != LV_FS_RES_OK) {
        LV_LOG_USER("cannot open directory: %s", path);
        return 0;
    }

    while (true)
    {
        // 读取下一个目录项（文件或文件夹）
        res = lv_fs_dir_read(&dir, file_name, sizeof(file_name));
        if (res != LV_FS_RES_OK || file_name[0] == '\0') {
            break;  // 没有更多文件或文件夹，结束遍历
        }

        if(Lime_GetImgType(file_name) != typeUnknown)
        {
            to_lower_case(file_name);
            lv_obj_t * btn = lv_list_add_btn(photo_list_obj, NULL, file_name);
            lv_obj_add_event_cb(btn, list_click_event_cb, LV_EVENT_CLICKED, NULL);
            lv_obj_set_height(btn, 30);

            lv_obj_set_style_text_font(btn, &LR_AppPhotoNameList_Font, LV_PART_MAIN | LV_STATE_DEFAULT);
            totalFileNum++;
            LV_LOG_USER("AddName: %s", file_name);
        }

        LV_LOG_USER("name: %s", file_name);
    }

    lv_fs_dir_close(&dir);
    return totalFileNum;
}

static void to_lower_case(char *str)
{
    while (*str)
    {
        *str = tolower((unsigned char)*str);
        str++;
    }
}

static imgType_e Lime_GetImgType(const char* path)
{
    uint16_t pathLength = strlen(path);
    if(pathLength > 4)
    {
        if((memcmp(path + pathLength - 3, "gif", 3) == 0) || (memcmp(path + pathLength - 3, "GIF", 3) == 0))
        {
            return typeGif;
        }
        else if((memcmp(path + pathLength - 3, "jpg", 3) == 0) || (memcmp(path + pathLength - 3, "JPG", 3) == 0))
        {
            return typeJpg;
        }
    }
    return typeUnknown;
}

static void list_click_event_cb(lv_event_t * e)
{
    lv_obj_t * list = lv_event_get_target(e);  // 获取被点击的list控件
    lv_obj_t * btn = lv_event_get_target(e);   // 获取被点击的按钮（即list的项）

    const char * btn_text = lv_list_get_btn_text(list, btn);

    LV_LOG_USER(">>>click :%s", btn_text);

    openWindowToShowPhotoByPath(btn_text);
}

static void openWindowToShowPhotoByPath(const char *path)
{
    if((photo_openWindow_obj != NULL) || (path == NULL) || (photo_app_obj == NULL))
    {
        LV_LOG_ERROR("photo_openWindow_obj or path or photo_app_obj is NULL");
        return;
    }

    char fullPath[64] = {0};

    snprintf(fullPath, sizeof(fullPath), "%s%s", pathRoot, path);

    photo_openWindow_obj = lv_obj_create(photo_app_obj);
    lv_obj_set_size(photo_openWindow_obj, 240, 240);
    lv_obj_center(photo_openWindow_obj);
    lv_obj_set_style_bg_color(photo_openWindow_obj, lv_color_hex(0x000000), 0);
    lv_obj_set_style_border_width(photo_openWindow_obj, 0, 0);
    lv_obj_set_style_radius(photo_openWindow_obj, 0, 0);
    lv_obj_set_style_shadow_width(photo_openWindow_obj, 0, 0);
    lv_obj_set_style_pad_all(photo_openWindow_obj, 0, 0);
    lv_obj_clear_flag(photo_openWindow_obj, LV_OBJ_FLAG_SCROLLABLE);

    /* index 0 */
    lv_obj_t * photoNotSupportImg = lv_img_create(photo_openWindow_obj);
    lv_img_set_src(photoNotSupportImg, &LR_PhotoNotSupport);
    lv_obj_center(photoNotSupportImg);

    /* index 1 */
    if(Lime_GetImgType(path) == typeGif)
    {
        lv_obj_t *img = lv_gif_create(photo_openWindow_obj);
        lv_gif_set_src(img, fullPath);
        lv_obj_center(img);
    }
    else
    {
        lv_obj_t * img = lv_img_create(photo_openWindow_obj);
        lv_img_set_src(img, fullPath);
        lv_obj_center(img);
    }

    /* index 2 */
    lv_obj_t * titelObj = lv_obj_create(photo_openWindow_obj);
    lv_obj_set_size(titelObj, 240, 30);
    lv_obj_align(titelObj, LV_ALIGN_TOP_MID, 0, 0);
    lv_obj_set_style_bg_color(titelObj, lv_color_hex(0x000000), 0);
    lv_obj_set_style_bg_opa(titelObj, 140, 0);
    lv_obj_set_style_border_width(titelObj, 0, 0);
    lv_obj_set_style_radius(titelObj, 0, 0);
    lv_obj_set_style_shadow_width(titelObj, 0, 0);
    lv_obj_clear_flag(titelObj, LV_OBJ_FLAG_SCROLLABLE);

    lv_obj_t * titelLabel = lv_label_create(titelObj);
    lv_label_set_text(titelLabel, path);
    lv_obj_set_size(titelLabel, 180, 15);
    lv_obj_center(titelLabel);
    lv_obj_set_style_text_align(titelLabel, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_font(titelLabel, &LR_AppPhotoNameList_Font, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_color(titelLabel, lv_color_hex(0xFFFFFF), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_label_set_long_mode(titelLabel, LV_LABEL_LONG_SCROLL_CIRCULAR);
}

static void timer_cb(lv_timer_t * timer)
{
    /* first create scan timer*/
    uint32_t *initFlagPin = lv_timer_get_user_data(timer);

    if(*initFlagPin)
    {
        *initFlagPin = 0;

        keyScan_workHandle(photo_KeyOption_None, true);
        goto SyncKeyInfoEnd;
    }

    /* scan key info */
    const LimeHal_KeyInfo_t* keyInfo = (const LimeHal_KeyInfo_t *)(&LimeHAL_GetInfoPin()->keyInfo);
    static LimeHal_KeyInfo_t LastKeyInfo = {0};
    photo_KeyOption_e keyOption = photo_KeyOption_None;
    if(((keyInfo->sw_up % 2) == 1) && (keyInfo->sw_up != LastKeyInfo.sw_up))
    {
        keyOption = photo_KeyOption_Up;
    }
    if(((keyInfo->sw_down % 2) == 1) && (keyInfo->sw_down != LastKeyInfo.sw_down))
    {
        keyOption = photo_KeyOption_Down;
    }
    if(((keyInfo->sw_set % 2) == 1) && (keyInfo->sw_set != LastKeyInfo.sw_set))
    {
        keyOption = photo_KeyOption_Set;
    }
    if(((keyInfo->kr % 2) == 1) && (keyInfo->kr != LastKeyInfo.kr))
    {
        keyOption = photo_KeyOption_Kr;
    }
    if(((keyInfo->sw_left % 2) == 1) && (keyInfo->sw_left != LastKeyInfo.sw_left))
    {
        keyOption = photo_KeyOption_Left;
    }

    keyScan_workHandle(keyOption, false);

SyncKeyInfoEnd:
    LastKeyInfo.sw_up = keyInfo->sw_up;
    LastKeyInfo.sw_down = keyInfo->sw_down;
    LastKeyInfo.sw_left = keyInfo->sw_left;
    LastKeyInfo.sw_right = keyInfo->sw_right;
    LastKeyInfo.sw_set = keyInfo->sw_set;
    LastKeyInfo.kr = keyInfo->kr;
    LastKeyInfo.sw_left = keyInfo->sw_left;
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
    uint32_t colorDst = Lime_lv_color_to_U32(lv_color_hex(PHOTO_SELECT_COLOR_HEX));

    if(colorScr == colorDst)
        return 1;

    return 0;
}

static int16_t photoMenu_moveSelectPin(photo_KeyOption_e moveDir)
{
    lv_obj_t* lastSelectObj = NULL;
    lv_obj_t* nowSelectObj = NULL;
    lv_obj_t* nextSelectObj = NULL;
    bool anyItemIsSelected = false;

    if(photo_list_obj == NULL || photo_app_obj == NULL)
    {
        LV_LOG_ERROR("photo_list_obj or photo_app_obj is NULL");
        return -1;
    }

    uint16_t totalPhotoNum = lv_obj_get_child_count(photo_list_obj);
    LV_LOG_USER("totalPhotoNum: %d", totalPhotoNum);
    if(totalPhotoNum == 0)
    {
        LV_LOG_USER("no photo found");
        return -1;
    }

    for(uint16_t i = 0; i < totalPhotoNum; i++)
    {
        lv_obj_t * childObj = lv_obj_get_child(photo_list_obj, i);
        if(childObj == NULL)
        {
            LV_LOG_ERROR("childObj is NULL");
            continue;
        }
        if(Lime_LV_Obj_has_been_Selected(childObj))
        {
            if(i == 0)
            {
                nowSelectObj = childObj;

                if(totalPhotoNum == 1)
                {
                    lastSelectObj = nowSelectObj;
                    nextSelectObj = nowSelectObj;
                }
                else
                {
                    lastSelectObj = lv_obj_get_child(photo_list_obj, totalPhotoNum - 1);
                    nextSelectObj = lv_obj_get_child(photo_list_obj, 1);
                }
            }
            else if(i == totalPhotoNum - 1)
            {
                nowSelectObj = childObj;
                lastSelectObj = lv_obj_get_child(photo_list_obj, i - 1);
                nextSelectObj = lv_obj_get_child(photo_list_obj, 0);
            }
            else
            {
                nowSelectObj = childObj;
                lastSelectObj = lv_obj_get_child(photo_list_obj, i - 1);
                nextSelectObj = lv_obj_get_child(photo_list_obj, i + 1);
            }

            anyItemIsSelected = true;
            break;
        }
    }

    if( !anyItemIsSelected)
    {
        lv_obj_set_style_bg_color(lv_obj_get_child(photo_list_obj, 0), lv_color_hex(PHOTO_SELECT_COLOR_HEX), LV_PART_MAIN | LV_STATE_DEFAULT);
        LV_LOG_USER("no item is selected");
        return 0;
    }

    if((lastSelectObj == NULL) || (nowSelectObj == NULL) || (nextSelectObj == NULL))
    {
        LV_LOG_ERROR("lastSelectObj or nowSelectObj or nextSelectObj is NULL");
        return -1;
    }

    if(moveDir == photo_KeyOption_Up)
    {
        lv_obj_set_style_bg_color(nowSelectObj, lv_color_hex(PHOTO_UNSELECT_COLOR_HEX), LV_PART_MAIN | LV_STATE_DEFAULT);
        lv_obj_set_style_bg_color(nextSelectObj, lv_color_hex(PHOTO_UNSELECT_COLOR_HEX), LV_PART_MAIN | LV_STATE_DEFAULT);
        lv_obj_set_style_bg_color(lastSelectObj, lv_color_hex(PHOTO_SELECT_COLOR_HEX), LV_PART_MAIN | LV_STATE_DEFAULT);
        lv_obj_scroll_to_view(lastSelectObj, LV_ANIM_ON);
    }
    else if(moveDir == photo_KeyOption_Down)
    {
        lv_obj_set_style_bg_color(lastSelectObj, lv_color_hex(PHOTO_UNSELECT_COLOR_HEX), LV_PART_MAIN | LV_STATE_DEFAULT);
        lv_obj_set_style_bg_color(nowSelectObj, lv_color_hex(PHOTO_UNSELECT_COLOR_HEX), LV_PART_MAIN | LV_STATE_DEFAULT);
        lv_obj_set_style_bg_color(nextSelectObj, lv_color_hex(PHOTO_SELECT_COLOR_HEX), LV_PART_MAIN | LV_STATE_DEFAULT);
        lv_obj_scroll_to_view(nextSelectObj, LV_ANIM_ON);
    }

    int16_t newSelectIndex = -1;
    for(uint16_t i = 0; i < totalPhotoNum; i++)
    {
        lv_obj_t * childObj = lv_obj_get_child(photo_list_obj, i);
        if(Lime_LV_Obj_has_been_Selected(childObj))
        {
            newSelectIndex = i;
            break;
        }
    }

    return newSelectIndex;
}

static void keyScan_workHandle(photo_KeyOption_e dir, bool isReset)
{
    static int16_t nowSelectIndex = -1;
    if(isReset)
    {
        nowSelectIndex = -1;
        return;
    }

    if(dir == photo_KeyOption_None)
        return;

    if(photo_del_obj != NULL)
    {
        /* waiting for delete someone or cancel */
        if(dir == photo_KeyOption_Set)
        {
            lv_obj_t * selectObj = lv_obj_get_child(photo_list_obj, nowSelectIndex);
            if((selectObj != NULL) && (nowSelectIndex != -1))
            {

                nowSelectIndex -= 1;
                nowSelectIndex = (nowSelectIndex < 0) ? 0 : nowSelectIndex;

                lv_obj_set_style_bg_color(selectObj, lv_color_hex(PHOTO_UNSELECT_COLOR_HEX), LV_PART_MAIN | LV_STATE_DEFAULT);

                selectObj = lv_obj_get_child(photo_list_obj, nowSelectIndex);
                if(selectObj != NULL)
                    lv_obj_set_style_bg_color(selectObj, lv_color_hex(PHOTO_SELECT_COLOR_HEX), LV_PART_MAIN | LV_STATE_DEFAULT);

                photoAnimStatus = photoDelAnim_status_DelPhoto;
                lv_anim_start(&delObj_delTransAnim);

                return;
            }
            else
            {
                LV_LOG_ERROR("selectObj or nowSelectIndex is NULL");
            }
        }

        photoAnimStatus = photoDelAnim_status_HidwDelLabel;
        lv_anim_start(&delObj_delTransAnim);

        return;
    }

    if(photo_openWindow_obj != NULL)
    {
        /* photo has been opened, waiting for close*/
        if(dir == photo_KeyOption_Kr)
        {
            lv_obj_del(photo_openWindow_obj);
            photo_openWindow_obj = NULL;
        }
        return;
    }


    if(dir == photo_KeyOption_Up)
    {
        LV_LOG_USER("keyOption: Up");
        nowSelectIndex = photoMenu_moveSelectPin(dir);
    }
    else if(dir == photo_KeyOption_Down)
    {
        LV_LOG_USER("keyOption: Down");
        nowSelectIndex = photoMenu_moveSelectPin(dir);
    }
    else if(dir == photo_KeyOption_Left)
    {
        LV_LOG_USER("keyOption: Left");
        lv_obj_t * selectObj = lv_obj_get_child(photo_list_obj, nowSelectIndex);
        if((selectObj != NULL) && (nowSelectIndex != -1))
        {
            if(photo_del_obj == NULL)
            {
                photo_del_obj = lv_obj_create(photo_app_obj);
                lv_obj_set_size(photo_del_obj, 50, 29);
                lv_obj_align_to(photo_del_obj, selectObj, LV_ALIGN_RIGHT_MID, 10, 0);
                lv_obj_set_style_radius(photo_del_obj, 0, 0);
                lv_obj_set_style_border_width(photo_del_obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
                lv_obj_set_style_shadow_width(photo_del_obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
                lv_obj_set_style_bg_color(photo_del_obj, lv_color_hex(0xFF0000), 0);


                lv_obj_t * delLabel = lv_label_create(photo_del_obj);
                lv_label_set_text(delLabel, "删除");
                lv_obj_align_to(delLabel, photo_del_obj, LV_ALIGN_CENTER, 0, 3);
                lv_obj_set_style_text_color(delLabel, lv_color_hex(0xFFFFFF), 0);
                lv_obj_set_style_text_font(delLabel, &Lime_AppPhotoBoldFont, 0);
                lv_obj_clear_flag(photo_del_obj, LV_OBJ_FLAG_SCROLLABLE);

                photoAnimStatus = photoDelAnim_status_ShowDelLabel;
                delObj_targetObj = selectObj;
                lv_anim_start(&delObj_delTransAnim);
            }
            else
            {
                lv_obj_del(photo_del_obj);
                photo_del_obj = NULL;
            }
        }
    }
    else if(dir == photo_KeyOption_Set)
    {
        LV_LOG_USER("keyOption: Set");
        lv_obj_t * selectObj = lv_obj_get_child(photo_list_obj, nowSelectIndex);
        if((selectObj != NULL) && (nowSelectIndex != -1))
        {
            if(photo_openWindow_obj == NULL)
                lv_obj_send_event(selectObj, LV_EVENT_CLICKED, NULL);

        }
    }
    else if(dir == photo_KeyOption_Kr)
    {

    }
}

static void delObj_delTransAnim_working_cb(void * var, int32_t v)
{
    lv_obj_t *obj = photo_del_obj;
    if(obj == NULL || delObj_targetObj == NULL)
    {
        LV_LOG_ERROR("obj or delObj_targetObj is NULL");
        return;
    }

    if(photoAnimStatus == photoDelAnim_status_ShowDelLabel)
    {
        int32_t width = fmap(v, 0, 100, 0, 50);
        lv_obj_set_width(obj, width);
        lv_obj_align_to(obj, delObj_targetObj, LV_ALIGN_RIGHT_MID, 10, 0);
    }
    else if(photoAnimStatus == photoDelAnim_status_HidwDelLabel)
    {
        int32_t width = fmap(v, 100, 0, 0, 50);
        lv_obj_set_width(obj, width);
        lv_obj_align_to(obj, delObj_targetObj, LV_ALIGN_RIGHT_MID, 10, 0);
    }
    else if(photoAnimStatus == photoDelAnim_status_DelPhoto)
    {
        int32_t width = fmap(v, 0, 100, 50, 200);
        int32_t height = fmap(v, 0, 100, 30, 0);
        lv_obj_set_size(obj, width, height);
        lv_obj_set_height(delObj_targetObj, height);
        lv_obj_align_to(obj, delObj_targetObj, LV_ALIGN_RIGHT_MID, 10, 0);
    }
}
static void delObj_delTransAnim_finish_cb(lv_anim_t * anim)
{
    if(photo_del_obj == NULL || delObj_targetObj == NULL)
    {
        LV_LOG_ERROR("obj or delObj_targetObj is NULL");
        return;
    }

    if(photoAnimStatus == photoDelAnim_status_ShowDelLabel)
    {
        //no use
    }
    else if(photoAnimStatus == photoDelAnim_status_HidwDelLabel)
    {
        lv_obj_del(photo_del_obj);
        photo_del_obj = NULL;
    }
    else if(photoAnimStatus == photoDelAnim_status_DelPhoto)
    {
        /* delete photo UI */
        lv_obj_del(photo_del_obj);
        photo_del_obj = NULL;

        /* delete photo on disk */
        const char *path = lv_list_get_button_text(photo_list_obj, delObj_targetObj);
        char fullPath[64] = {0};
        snprintf(fullPath, sizeof(fullPath), "%s%s", pathRoot, path);
        LimeHAL_DelPhotoByPath(fullPath);

        /* update photo list */
        lv_obj_del(delObj_targetObj);
        delObj_targetObj = NULL;
    }

    photoAnimStatus = photoDelAnim_status_Idle;
}
