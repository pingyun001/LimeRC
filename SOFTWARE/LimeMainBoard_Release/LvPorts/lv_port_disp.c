/**
 * @file lv_port_disp_templ.c
 *
 */

/*Copy this file as "lv_port_disp.c" and set this value to "1" to enable content*/
#if 1

/*********************
 *      INCLUDES
 *********************/
#include "lv_port_disp.h"
#include "oled_st7789.h"
/*********************
 *      DEFINES
 *********************/
#ifndef MY_DISP_HOR_RES
    #warning Please define or replace the macro MY_DISP_HOR_RES with the actual screen width, default value 320 is used for now.
    #define MY_DISP_HOR_RES    320
#endif

#ifndef MY_DISP_VER_RES
    #warning Please define or replace the macro MY_DISP_VER_RES with the actual screen height, default value 240 is used for now.
    #define MY_DISP_VER_RES    240
#endif

#define BYTE_PER_PIXEL (LV_COLOR_FORMAT_GET_SIZE(LV_COLOR_FORMAT_RGB565)) /*will be 2 for RGB565 */


static void disp_init(void);

static void disp_flush(lv_display_t * disp, const lv_area_t * area, uint8_t * px_map);

LV_ATTRIBUTE_MEM_ALIGN
static uint8_t buf_1_1[MY_DISP_HOR_RES * 240 * BYTE_PER_PIXEL]  __attribute__((section("LIME_RAM_D1"),aligned(32))) = {0};            /*A buffer for 240 rows*/
static uint8_t buf_1_2[MY_DISP_HOR_RES * 240 * BYTE_PER_PIXEL]  __attribute__((section("LIME_RAM_D1"),aligned(32))) = {0};            /*A buffer for 240 rows*/

void lv_port_disp_init(void)
{
    disp_init();

    lv_display_t * disp = lv_display_create(MY_DISP_HOR_RES, MY_DISP_VER_RES);
    lv_display_set_flush_cb(disp, disp_flush);

    lv_display_set_buffers(disp, buf_1_1, buf_1_2, sizeof(buf_1_1), LV_DISPLAY_RENDER_MODE_PARTIAL);

}

/**********************
 *   STATIC FUNCTIONS
 **********************/

/*Initialize your display and the required peripherals.*/
static void disp_init(void)
{
    /*You code here*/
}

volatile bool disp_flush_enabled = true;

/* Enable updating the screen (the flushing process) when disp_flush() is called by LVGL
 */
void disp_enable_update(void)
{
    disp_flush_enabled = true;
}

/* Disable updating the screen (the flushing process) when disp_flush() is called by LVGL
 */
void disp_disable_update(void)
{
    disp_flush_enabled = false;
}



static void disp_flush(lv_display_t * disp_drv, const lv_area_t * area, uint8_t * px_map)
{
  if(disp_flush_enabled) 
	{
		SCB_CleanDCache_by_Addr((uint32_t*)px_map, 
        (area->x2 - area->x1 + 1) * (area->y2 - area->y1 + 1) * sizeof(lv_color_t));
		
		__DSB();
		
		oled_st7789_ColorFill(area->x1, area->y1, area->x2, area->y2, (uint16_t*)px_map);
	}
  lv_display_flush_ready(disp_drv);
}

#else /*Enable this file at the top*/

/*This dummy typedef exists purely to silence -Wpedantic.*/
typedef int keep_pedantic_happy;
#endif
