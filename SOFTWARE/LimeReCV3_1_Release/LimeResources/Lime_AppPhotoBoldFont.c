/*******************************************************************************
 * Size: 10 px
 * Bpp: 4
 * Opts: --bpp 4 --size 10 --no-compress --stride 1 --align 1 --font 微软雅黑Bbold.ttf --symbols 删除 --format lvgl -o Lime_AppPhotoBoldFont.c
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



#ifndef LIME_APPPHOTOBOLDFONT
#define LIME_APPPHOTOBOLDFONT 1
#endif

#if LIME_APPPHOTOBOLDFONT

/*-----------------
 *    BITMAPS
 *----------------*/

/*Store the image of the glyphs*/
static LV_ATTRIBUTE_LARGE_CONST const uint8_t glyph_bitmap[] = {
    /* U+5220 "删" */
    0x8, 0xff, 0x7f, 0xf5, 0x5f, 0x30, 0x89, 0xe7,
    0xad, 0x89, 0xf3, 0x8, 0x8e, 0x79, 0xd8, 0x9f,
    0x30, 0xbc, 0xfb, 0xce, 0xa9, 0xf3, 0xc, 0xdf,
    0xde, 0xfc, 0x9f, 0x30, 0x97, 0xe7, 0x8d, 0x89,
    0xf3, 0xa, 0x6e, 0x88, 0xd8, 0x9f, 0x30, 0xd5,
    0xeb, 0x6d, 0x32, 0xf3, 0xb, 0x9e, 0xda, 0xe4,
    0xfd, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0,

    /* U+9664 "除" */
    0x4f, 0xfe, 0x5, 0xf9, 0x0, 0x4, 0xe8, 0xc2,
    0xf9, 0xf8, 0x0, 0x4e, 0xbc, 0xfa, 0x29, 0xfd,
    0x4, 0xef, 0x7d, 0xff, 0xfb, 0x90, 0x4e, 0xb6,
    0x45, 0xf7, 0x43, 0x4, 0xe6, 0xca, 0xbf, 0xba,
    0x70, 0x4e, 0xfc, 0xd4, 0xf7, 0xd1, 0x4, 0xe5,
    0xae, 0x7f, 0x3d, 0xc0, 0x4e, 0x3, 0x5f, 0xd1,
    0x33, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0
};


/*---------------------
 *  GLYPH DESCRIPTION
 *--------------------*/

static const lv_font_fmt_txt_glyph_dsc_t glyph_dsc[] = {
    {.bitmap_index = 0, .adv_w = 0, .box_w = 0, .box_h = 0, .ofs_x = 0, .ofs_y = 0} /* id = 0 reserved */,
    {.bitmap_index = 0, .adv_w = 160, .box_w = 11, .box_h = 10, .ofs_x = -1, .ofs_y = -2},
    {.bitmap_index = 55, .adv_w = 160, .box_w = 11, .box_h = 10, .ofs_x = 0, .ofs_y = -2}
};

/*---------------------
 *  CHARACTER MAPPING
 *--------------------*/

static const uint16_t unicode_list_0[] = {
    0x0, 0x4444
};

/*Collect the unicode lists and glyph_id offsets*/
static const lv_font_fmt_txt_cmap_t cmaps[] =
{
    {
        .range_start = 21024, .range_length = 17477, .glyph_id_start = 1,
        .unicode_list = unicode_list_0, .glyph_id_ofs_list = NULL, .list_length = 2, .type = LV_FONT_FMT_TXT_CMAP_SPARSE_TINY
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
const lv_font_t Lime_AppPhotoBoldFont = {
#else
lv_font_t Lime_AppPhotoBoldFont = {
#endif
    .get_glyph_dsc = lv_font_get_glyph_dsc_fmt_txt,    /*Function pointer to get glyph's data*/
    .get_glyph_bitmap = lv_font_get_bitmap_fmt_txt,    /*Function pointer to get glyph's bitmap*/
    .line_height = 10,          /*The maximum line height required by the font*/
    .base_line = 2,             /*Baseline measured from the bottom of the line*/
#if !(LVGL_VERSION_MAJOR == 6 && LVGL_VERSION_MINOR == 0)
    .subpx = LV_FONT_SUBPX_NONE,
#endif
#if LV_VERSION_CHECK(7, 4, 0) || LVGL_VERSION_MAJOR >= 8
    .underline_position = -1,
    .underline_thickness = 1,
#endif
    // .static_bitmap = 0,
    .dsc = &font_dsc,          /*The custom font data. Will be accessed by `get_glyph_bitmap/dsc` */
#if LV_VERSION_CHECK(8, 2, 0) || LVGL_VERSION_MAJOR >= 9
    .fallback = NULL,
#endif
    .user_data = NULL,
};



#endif /*#if LIME_APPPHOTOBOLDFONT*/
