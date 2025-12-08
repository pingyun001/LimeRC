/*******************************************************************************
 * Size: 7 px
 * Bpp: 4
 * Opts: --bpp 4 --size 7 --no-compress --font 微软雅黑.ttf --symbols RCHD --format lvgl -o Lime_APP_LRCFace_HeadBarLittleFont.c
 ******************************************************************************/

#ifdef LV_LVGL_H_INCLUDE_SIMPLE
#include "lvgl.h"
#else
#include "lvgl/lvgl.h"
#endif

#ifndef LIME_APP_LRCFACE_HEADBARLITTLEFONT
#define LIME_APP_LRCFACE_HEADBARLITTLEFONT 1
#endif

#if LIME_APP_LRCFACE_HEADBARLITTLEFONT

/*-----------------
 *    BITMAPS
 *----------------*/

/*Store the image of the glyphs*/
static LV_ATTRIBUTE_LARGE_CONST const uint8_t glyph_bitmap[] = {
    /* U+0043 "C" */
    0x8, 0xaa, 0x36, 0x60, 0x0, 0x91, 0x0, 0x7,
    0x50, 0x0, 0x9, 0xa9, 0x30,

    /* U+0044 "D" */
    0x5c, 0x9a, 0x15, 0x60, 0x1a, 0x56, 0x0, 0xb5,
    0x60, 0x1a, 0x5c, 0x99, 0x10,

    /* U+0048 "H" */
    0x56, 0x0, 0xb5, 0x60, 0xb, 0x5c, 0xaa, 0xb5,
    0x60, 0xb, 0x56, 0x0, 0xb0,

    /* U+0052 "R" */
    0x5c, 0xa8, 0x5, 0x60, 0xb0, 0x5c, 0xd4, 0x5,
    0x63, 0x90, 0x56, 0x8, 0x30
};


/*---------------------
 *  GLYPH DESCRIPTION
 *--------------------*/

static const lv_font_fmt_txt_glyph_dsc_t glyph_dsc[] = {
    {.bitmap_index = 0, .adv_w = 0, .box_w = 0, .box_h = 0, .ofs_x = 0, .ofs_y = 0} /* id = 0 reserved */,
    {.bitmap_index = 0, .adv_w = 75, .box_w = 5, .box_h = 5, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 13, .adv_w = 85, .box_w = 5, .box_h = 5, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 26, .adv_w = 87, .box_w = 5, .box_h = 5, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 39, .adv_w = 73, .box_w = 5, .box_h = 5, .ofs_x = 0, .ofs_y = 0}
};

/*---------------------
 *  CHARACTER MAPPING
 *--------------------*/

static const uint16_t unicode_list_0[] = {
    0x0, 0x1, 0x5, 0xf
};

/*Collect the unicode lists and glyph_id offsets*/
static const lv_font_fmt_txt_cmap_t cmaps[] =
{
    {
        .range_start = 67, .range_length = 16, .glyph_id_start = 1,
        .unicode_list = unicode_list_0, .glyph_id_ofs_list = NULL, .list_length = 4, .type = LV_FONT_FMT_TXT_CMAP_SPARSE_TINY
    }
};



/*--------------------
 *  ALL CUSTOM DATA
 *--------------------*/

#if LVGL_VERSION_MAJOR == 8
/*Store all the custom data of the font*/
static  lv_font_fmt_txt_glyph_cache_t cache;
#endif

#if LVGL_VERSION_MAJOR >= 8
static const lv_font_fmt_txt_dsc_t font_dsc = {
#else
static lv_font_fmt_txt_dsc_t font_dsc = {
#endif
    .glyph_bitmap = glyph_bitmap,
    .glyph_dsc = glyph_dsc,
    .cmaps = cmaps,
    .kern_dsc = NULL,
    .kern_scale = 0,
    .cmap_num = 1,
    .bpp = 4,
    .kern_classes = 0,
    .bitmap_format = 0,
#if LVGL_VERSION_MAJOR == 8
    .cache = &cache
#endif
};



/*-----------------
 *  PUBLIC FONT
 *----------------*/

/*Initialize a public general font descriptor*/
#if LVGL_VERSION_MAJOR >= 8
const lv_font_t Lime_APP_LRCFace_HeadBarLittleFont = {
#else
lv_font_t Lime_APP_LRCFace_HeadBarLittleFont = {
#endif
    .get_glyph_dsc = lv_font_get_glyph_dsc_fmt_txt,    /*Function pointer to get glyph's data*/
    .get_glyph_bitmap = lv_font_get_bitmap_fmt_txt,    /*Function pointer to get glyph's bitmap*/
    .line_height = 5,          /*The maximum line height required by the font*/
    .base_line = 0,             /*Baseline measured from the bottom of the line*/
#if !(LVGL_VERSION_MAJOR == 6 && LVGL_VERSION_MINOR == 0)
    .subpx = LV_FONT_SUBPX_NONE,
#endif
#if LV_VERSION_CHECK(7, 4, 0) || LVGL_VERSION_MAJOR >= 8
    .underline_position = -1,
    .underline_thickness = 0,
#endif
    .dsc = &font_dsc,          /*The custom font data. Will be accessed by `get_glyph_bitmap/dsc` */
#if LV_VERSION_CHECK(8, 2, 0) || LVGL_VERSION_MAJOR >= 9
    .fallback = NULL,
#endif
    .user_data = NULL,
};



#endif /*#if LIME_APP_LRCFACE_HEADBARLITTLEFONT*/

