#ifndef __Lime_App_Base_H__
#define __Lime_App_Base_H__

#include <stdint.h>
#include <string.h>
#include <stdio.h>
#if USING_LIME_HARDWARE
#include "main.h"
#include "lvgl.h"
#else
#include "../main/src/main.h"
#endif

/* err check */
#define MLV_BASE_OBJ_NULL_CHECK(obj) \
    do { \
        if((obj) == NULL) { \
            LV_LOG_USER("Error: %s is NULL\n", #obj); \
            return; \
        } \
    } while(0)

#define MLV_BASE_OBJ_NULL_CHECK_RETURN_NULL(obj) \
    do { \
        if((obj) == NULL) { \
            LV_LOG_USER("Error: %s is NULL\n", #obj); \
            return (NULL); \
        } \
    } while(0)

/* malloc and free */
#if defined(MLV_USING_CUSTOM_MALLOC) && (MLV_USING_CUSTOM_MALLOC == 1)
    #include "FreeRTOS.h"
    #include "task.h"
    #define MLV_BASE_MALLOC(size) pvPortMalloc(size)
    #define MLV_BASE_FREE(ptr) vPortFree(ptr)
#else
    #define MLV_BASE_MALLOC(size) lv_malloc(size)
    #define MLV_BASE_FREE(ptr) lv_free(ptr)
#endif

void lime_base_set_label_string(lv_obj_t * lable_obj, const char *format, ...);

#endif // __Lime_App_Base_H__
