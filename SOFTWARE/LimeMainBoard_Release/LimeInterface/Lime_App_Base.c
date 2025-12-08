#include "Lime_App_Base.h"

void lime_base_set_label_string(lv_obj_t * lable_obj, const char *format, ...)
{
#define MLV_BASE_TEMP_BUF_SIZE  256

    MLV_BASE_OBJ_NULL_CHECK(lable_obj);

    /* malloc str buffer */
    char *synthesize_str = MLV_BASE_MALLOC(MLV_BASE_TEMP_BUF_SIZE);
    if(synthesize_str == NULL)
    {
        LV_LOG_ERROR("Memory allocation failed");
        return;
    }

    /* Synthesize target string */
    va_list args;
    uint32_t length;
    va_start(args, format);
    length = vsnprintf(synthesize_str, MLV_BASE_TEMP_BUF_SIZE, format, args);
    va_end(args);
    if(length >= MLV_BASE_TEMP_BUF_SIZE)
    {
        LV_LOG_WARN("Label string is too long");
        length = MLV_BASE_TEMP_BUF_SIZE - 1;
        synthesize_str[length] = '\0';
    }

    /* Get now showed string */
    char *remain_power_showed_str = lv_label_get_text(lable_obj);

    /* Compare and update */
    if(memcmp(remain_power_showed_str, synthesize_str, length) != 0)
    {
        lv_label_set_text(lable_obj, synthesize_str);
    }

    /* free buffer */
    MLV_BASE_FREE(synthesize_str);
}
