/*******************************************************************************
 * Size: 8 px
 * Bpp: 4
 * Opts: --bpp 4 --size 8 --no-compress --stride 1 --align 1 --font 微软雅黑Bbold.ttf --symbols 1234567890% --format lvgl -o Lime_APP_CaliFace_PercentFont.c
 ******************************************************************************/

#ifdef __has_include
    #if __has_include("lvgl.h")
        #ifndef LV_LVGL_H_INCLUDE_SIMPLE
            #define LV_LVGL_H_INCLUDE_SIMPLE
        #endif
    #endif
#endif

#ifdef LV_LVGL_H_INCLUDE_SIMPLE
    #include "lvgl.h"
#else
    #include "lvgl/lvgl.h"
#endif



#ifndef LIME_APP_CALIFACE_PERCENTFONT
#define LIME_APP_CALIFACE_PERCENTFONT 1
#endif

#if LIME_APP_CALIFACE_PERCENTFONT

/*-----------------
 *    BITMAPS
 *----------------*/

/*Store the image of the glyphs*/
static LV_ATTRIBUTE_LARGE_CONST const uint8_t glyph_bitmap[] = {
    /* U+0025 "%" */
    0x2d, 0xc0, 0x1d, 0x0, 0x84, 0x94, 0xa5, 0x0,
    0x85, 0x97, 0xc0, 0x0, 0x2c, 0xac, 0x7d, 0x80,
    0x0, 0x5a, 0xc1, 0xd0, 0x0, 0xd1, 0xd0, 0xd0,
    0x7, 0x80, 0x6c, 0x80,

    /* U+0030 "0" */
    0x9, 0xfa, 0x5, 0xe3, 0xe5, 0x9a, 0xb, 0x8a,
    0x90, 0xa9, 0x9a, 0xb, 0x86, 0xe3, 0xe4, 0xb,
    0xf9, 0x0,

    /* U+0031 "1" */
    0x18, 0xc3, 0x3, 0x9f, 0x40, 0x0, 0xf4, 0x0,
    0xf, 0x40, 0x0, 0xf4, 0x0, 0x2f, 0x51, 0x4f,
    0xff, 0x80,

    /* U+0032 "2" */
    0x1c, 0xfb, 0x3, 0x63, 0xf5, 0x0, 0xe, 0x50,
    0x6, 0xe0, 0x4, 0xe3, 0x3, 0xf7, 0x31, 0x8f,
    0xff, 0x60,

    /* U+0033 "3" */
    0x1d, 0xf9, 0x1, 0x35, 0xf3, 0x0, 0x3f, 0x10,
    0xbf, 0x80, 0x1, 0x4f, 0x42, 0x23, 0xf5, 0x4e,
    0xfa, 0x0,

    /* U+0034 "4" */
    0x0, 0x9f, 0x0, 0x3e, 0xf0, 0xc, 0x6f, 0x6,
    0x93, 0xf0, 0xde, 0xef, 0xa1, 0x14, 0xf1, 0x0,
    0x3f, 0x0,

    /* U+0035 "5" */
    0x2f, 0xff, 0x22, 0xf2, 0x20, 0x2f, 0x0, 0x2,
    0xff, 0xa0, 0x2, 0x4f, 0x51, 0x23, 0xf4, 0x3e,
    0xf9, 0x0,

    /* U+0036 "6" */
    0x5, 0xef, 0x21, 0xf5, 0x20, 0x6d, 0xcc, 0x18,
    0xf3, 0xd7, 0x7c, 0xa, 0x94, 0xf3, 0xd6, 0x9,
    0xfa, 0x0,

    /* U+0037 "7" */
    0x9f, 0xff, 0x81, 0x22, 0xe4, 0x0, 0x3e, 0x0,
    0x9, 0x90, 0x0, 0xf4, 0x0, 0x5e, 0x0, 0xb,
    0x90, 0x0,

    /* U+0038 "8" */
    0xb, 0xfb, 0x6, 0xd2, 0xe6, 0x4f, 0x5e, 0x30,
    0xcf, 0xb0, 0x7c, 0x1d, 0x79, 0xb2, 0xc8, 0x2c,
    0xfb, 0x10,

    /* U+0039 "9" */
    0x1b, 0xf9, 0x8, 0xc2, 0xe4, 0x9b, 0xd, 0x73,
    0xff, 0xe8, 0x0, 0xd, 0x62, 0x25, 0xf1, 0x3f,
    0xe5, 0x0
};


/*---------------------
 *  GLYPH DESCRIPTION
 *--------------------*/

static const lv_font_fmt_txt_glyph_dsc_t glyph_dsc[] = {
    {.bitmap_index = 0, .adv_w = 0, .box_w = 0, .box_h = 0, .ofs_x = 0, .ofs_y = 0} /* id = 0 reserved */,
    {.bitmap_index = 0, .adv_w = 119, .box_w = 8, .box_h = 7, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 28, .adv_w = 79, .box_w = 5, .box_h = 7, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 46, .adv_w = 79, .box_w = 5, .box_h = 7, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 64, .adv_w = 79, .box_w = 5, .box_h = 7, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 82, .adv_w = 79, .box_w = 5, .box_h = 7, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 100, .adv_w = 79, .box_w = 5, .box_h = 7, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 118, .adv_w = 79, .box_w = 5, .box_h = 7, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 136, .adv_w = 79, .box_w = 5, .box_h = 7, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 154, .adv_w = 79, .box_w = 5, .box_h = 7, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 172, .adv_w = 79, .box_w = 5, .box_h = 7, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 190, .adv_w = 79, .box_w = 5, .box_h = 7, .ofs_x = 0, .ofs_y = 0}
};

/*---------------------
 *  CHARACTER MAPPING
 *--------------------*/



/*Collect the unicode lists and glyph_id offsets*/
static const lv_font_fmt_txt_cmap_t cmaps[] =
{
    {
        .range_start = 37, .range_length = 1, .glyph_id_start = 1,
        .unicode_list = NULL, .glyph_id_ofs_list = NULL, .list_length = 0, .type = LV_FONT_FMT_TXT_CMAP_FORMAT0_TINY
    },
    {
        .range_start = 48, .range_length = 10, .glyph_id_start = 2,
        .unicode_list = NULL, .glyph_id_ofs_list = NULL, .list_length = 0, .type = LV_FONT_FMT_TXT_CMAP_FORMAT0_TINY
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
    .cmap_num = 2,
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
const lv_font_t Lime_APP_CaliFace_PercentFont = {
#else
lv_font_t Lime_APP_CaliFace_PercentFont = {
#endif
    .get_glyph_dsc = lv_font_get_glyph_dsc_fmt_txt,    /*Function pointer to get glyph's data*/
    .get_glyph_bitmap = lv_font_get_bitmap_fmt_txt,    /*Function pointer to get glyph's bitmap*/
    .line_height = 7,          /*The maximum line height required by the font*/
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



#endif /*#if LIME_APP_CALIFACE_PERCENTFONT*/
