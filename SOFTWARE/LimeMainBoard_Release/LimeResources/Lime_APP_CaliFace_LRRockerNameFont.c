/*******************************************************************************
 * Size: 11 px
 * Bpp: 4
 * Opts: --bpp 4 --size 11 --no-compress --stride 1 --align 1 --font 微软雅黑Bbold.ttf --symbols 左右摇杆 --format lvgl -o Lime_APP_CaliFace_LRRockerNameFont.c
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



#ifndef LIME_APP_CALIFACE_LRROCKERNAMEFONT
#define LIME_APP_CALIFACE_LRROCKERNAMEFONT 1
#endif

#if LIME_APP_CALIFACE_LRROCKERNAMEFONT

/*-----------------
 *    BITMAPS
 *----------------*/

/*Store the image of the glyphs*/
static LV_ATTRIBUTE_LARGE_CONST const uint8_t glyph_bitmap[] = {
    /* U+53F3 "右" */
    0x0, 0x0, 0xc9, 0x0, 0x0, 0x6, 0x88, 0x8f,
    0xc8, 0x88, 0x83, 0xcf, 0xff, 0xff, 0xff, 0xff,
    0x60, 0x5, 0xf6, 0x0, 0x0, 0x0, 0x2, 0xff,
    0x77, 0x77, 0x74, 0x1, 0xdf, 0xff, 0xff, 0xff,
    0xa0, 0xdf, 0xec, 0x0, 0x0, 0xea, 0xa, 0x3c,
    0xc0, 0x0, 0xe, 0xa0, 0x0, 0xcd, 0x77, 0x77,
    0xea, 0x0, 0xc, 0xff, 0xff, 0xff, 0xa0, 0x0,
    0x0, 0x0, 0x0, 0x0, 0x0,

    /* U+5DE6 "左" */
    0x0, 0x0, 0xe8, 0x0, 0x0, 0x2, 0x77, 0x8f,
    0xb7, 0x77, 0x77, 0x4f, 0xff, 0xff, 0xff, 0xff,
    0xe0, 0x0, 0xec, 0x0, 0x0, 0x0, 0x0, 0x8f,
    0xa7, 0x77, 0x77, 0x10, 0x3f, 0xff, 0xff, 0xff,
    0xf3, 0x1d, 0xe1, 0x4, 0xf5, 0x0, 0x5, 0xf4,
    0x0, 0x4f, 0x50, 0x0, 0x16, 0x67, 0x79, 0xfa,
    0x77, 0x60, 0xc, 0xff, 0xff, 0xff, 0xfe,

    /* U+6447 "摇" */
    0x0, 0x0, 0x0, 0x0, 0x1, 0x10, 0x0, 0xd7,
    0x7d, 0xef, 0xff, 0xf3, 0x2, 0xe9, 0x47, 0x58,
    0x44, 0x91, 0xe, 0xff, 0x8f, 0x3e, 0x88, 0xf1,
    0x3, 0xe9, 0x1d, 0x64, 0x2b, 0x70, 0x0, 0xdb,
    0x5f, 0xff, 0xff, 0xf2, 0xe, 0xff, 0xaa, 0x3f,
    0x83, 0x30, 0xa, 0xe7, 0xcf, 0xff, 0xff, 0xfa,
    0x0, 0xd7, 0x8e, 0x3f, 0x85, 0xf4, 0x6, 0xf7,
    0x6f, 0xaf, 0xcb, 0xf3, 0xc, 0xd3, 0x38, 0x88,
    0x89, 0xf3,

    /* U+6746 "杆" */
    0x5, 0xf0, 0x5f, 0xff, 0xff, 0x77, 0xcf, 0xb6,
    0x79, 0xfa, 0x73, 0x9e, 0xfd, 0x40, 0x4f, 0x40,
    0x0, 0x9f, 0x0, 0x4, 0xf4, 0x0, 0x1f, 0xf9,
    0xaf, 0xff, 0xff, 0xea, 0xff, 0xee, 0x8a, 0xfa,
    0x87, 0xbb, 0xf4, 0x20, 0x4f, 0x40, 0x4, 0x5f,
    0x0, 0x4, 0xf4, 0x0, 0x5, 0xf0, 0x0, 0x4f,
    0x40, 0x0, 0x5f, 0x0, 0x4, 0xf4, 0x0, 0x0,
    0x0, 0x0, 0x0, 0x0, 0x0
};


/*---------------------
 *  GLYPH DESCRIPTION
 *--------------------*/

static const lv_font_fmt_txt_glyph_dsc_t glyph_dsc[] = {
    {.bitmap_index = 0, .adv_w = 0, .box_w = 0, .box_h = 0, .ofs_x = 0, .ofs_y = 0} /* id = 0 reserved */,
    {.bitmap_index = 0, .adv_w = 176, .box_w = 11, .box_h = 11, .ofs_x = 0, .ofs_y = -2},
    {.bitmap_index = 61, .adv_w = 176, .box_w = 11, .box_h = 10, .ofs_x = 0, .ofs_y = -1},
    {.bitmap_index = 116, .adv_w = 176, .box_w = 12, .box_h = 11, .ofs_x = -1, .ofs_y = -1},
    {.bitmap_index = 182, .adv_w = 176, .box_w = 11, .box_h = 11, .ofs_x = 0, .ofs_y = -2}
};

/*---------------------
 *  CHARACTER MAPPING
 *--------------------*/

static const uint16_t unicode_list_0[] = {
    0x0, 0x9f3, 0x1054, 0x1353
};

/*Collect the unicode lists and glyph_id offsets*/
static const lv_font_fmt_txt_cmap_t cmaps[] =
{
    {
        .range_start = 21491, .range_length = 4948, .glyph_id_start = 1,
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
const lv_font_t Lime_APP_CaliFace_LRRockerNameFont = {
#else
lv_font_t Lime_APP_CaliFace_LRRockerNameFont = {
#endif
    .get_glyph_dsc = lv_font_get_glyph_dsc_fmt_txt,    /*Function pointer to get glyph's data*/
    .get_glyph_bitmap = lv_font_get_bitmap_fmt_txt,    /*Function pointer to get glyph's bitmap*/
    .line_height = 12,          /*The maximum line height required by the font*/
    .base_line = 2,             /*Baseline measured from the bottom of the line*/
#if !(LVGL_VERSION_MAJOR == 6 && LVGL_VERSION_MINOR == 0)
    .subpx = LV_FONT_SUBPX_NONE,
#endif
#if LV_VERSION_CHECK(7, 4, 0) || LVGL_VERSION_MAJOR >= 8
    .underline_position = -1,
    .underline_thickness = 1,
#endif
    .dsc = &font_dsc,          /*The custom font data. Will be accessed by `get_glyph_bitmap/dsc` */
#if LV_VERSION_CHECK(8, 2, 0) || LVGL_VERSION_MAJOR >= 9
    .fallback = NULL,
#endif
    .user_data = NULL,
};



#endif /*#if LIME_APP_CALIFACE_LRROCKERNAMEFONT*/
