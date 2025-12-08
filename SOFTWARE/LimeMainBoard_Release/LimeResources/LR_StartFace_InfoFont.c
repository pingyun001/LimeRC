/*******************************************************************************
 * Size: 9 px
 * Bpp: 1
 * Opts: --bpp 1 --size 9 --no-compress --font 微软雅黑.ttf --symbols abcdefghijklmnopqrstuvwxyz ABCDEFGHIJKLMNOPQRSTUVWXYZ123456789.-! --format lvgl -o LR_StartFace_InfoFont.c
 ******************************************************************************/

#ifdef LV_LVGL_H_INCLUDE_SIMPLE
#include "lvgl.h"
#else
#include "lvgl/lvgl.h"
#endif

#ifndef LR_STARTFACE_INFOFONT
#define LR_STARTFACE_INFOFONT 1
#endif

#if LR_STARTFACE_INFOFONT

/*-----------------
 *    BITMAPS
 *----------------*/

/*Store the image of the glyphs*/
static LV_ATTRIBUTE_LARGE_CONST const uint8_t glyph_bitmap[] = {
    /* U+0020 " " */
    0x0,

    /* U+0021 "!" */
    0xfa,

    /* U+002D "-" */
    0xc0,

    /* U+002E "." */
    0x80,

    /* U+0031 "1" */
    0xe1, 0x8, 0x42, 0x13, 0xe0,

    /* U+0032 "2" */
    0x70, 0x42, 0x22, 0x23, 0xe0,

    /* U+0033 "3" */
    0xe1, 0x16, 0x11, 0xe0,

    /* U+0034 "4" */
    0x31, 0x95, 0x2f, 0x88, 0x40,

    /* U+0035 "5" */
    0xf8, 0x8e, 0x11, 0xe0,

    /* U+0036 "6" */
    0x7c, 0xe9, 0x99, 0x60,

    /* U+0037 "7" */
    0xf1, 0x22, 0x44, 0x40,

    /* U+0038 "8" */
    0x79, 0x96, 0x99, 0x60,

    /* U+0039 "9" */
    0x69, 0x97, 0x13, 0xe0,

    /* U+0041 "A" */
    0x30, 0xc2, 0x92, 0x79, 0x18, 0x40,

    /* U+0042 "B" */
    0xf9, 0x9e, 0x99, 0xe0,

    /* U+0043 "C" */
    0x3a, 0x21, 0x8, 0x61, 0xe0,

    /* U+0044 "D" */
    0xf2, 0x28, 0x61, 0x86, 0x2f, 0x0,

    /* U+0045 "E" */
    0xf8, 0x8e, 0x88, 0xf0,

    /* U+0046 "F" */
    0xf8, 0x8e, 0x88, 0x80,

    /* U+0047 "G" */
    0x3d, 0x8, 0x27, 0x87, 0x17, 0xc0,

    /* U+0048 "H" */
    0x8c, 0x63, 0xf8, 0xc6, 0x20,

    /* U+0049 "I" */
    0xe9, 0x24, 0xb8,

    /* U+004A "J" */
    0x24, 0x92, 0x70,

    /* U+004B "K" */
    0x95, 0x29, 0x8a, 0x4a, 0x60,

    /* U+004C "L" */
    0x88, 0x88, 0x88, 0xf0,

    /* U+004D "M" */
    0xc3, 0x8f, 0x1d, 0x5a, 0xb6, 0x64, 0x80,

    /* U+004E "N" */
    0xc7, 0x1a, 0x65, 0x96, 0x38, 0xc0,

    /* U+004F "O" */
    0x38, 0x8a, 0xc, 0x18, 0x38, 0x9e, 0x0,

    /* U+0050 "P" */
    0xe9, 0x99, 0xe8, 0x80,

    /* U+0051 "Q" */
    0x38, 0x8a, 0xc, 0x18, 0x28, 0x9e, 0x4, 0x6,

    /* U+0052 "R" */
    0xe4, 0xa5, 0xca, 0x4a, 0x40,

    /* U+0053 "S" */
    0x78, 0x86, 0x11, 0xe0,

    /* U+0054 "T" */
    0xf9, 0x8, 0x42, 0x10, 0x80,

    /* U+0055 "U" */
    0x8c, 0x63, 0x18, 0xc5, 0xc0,

    /* U+0056 "V" */
    0x85, 0x24, 0x92, 0x30, 0xc3, 0x0,

    /* U+0057 "W" */
    0x88, 0xc4, 0x55, 0x4a, 0xa5, 0x51, 0x98, 0x88,

    /* U+0058 "X" */
    0x49, 0x23, 0xc, 0x31, 0x2c, 0x80,

    /* U+0059 "Y" */
    0x8a, 0x54, 0xc2, 0x10, 0x80,

    /* U+005A "Z" */
    0xf8, 0x84, 0x44, 0x23, 0xe0,

    /* U+0061 "a" */
    0x71, 0x79, 0xf0,

    /* U+0062 "b" */
    0x84, 0x2d, 0x98, 0xc7, 0xc0,

    /* U+0063 "c" */
    0x78, 0x88, 0x70,

    /* U+0064 "d" */
    0x8, 0x5f, 0x18, 0xc5, 0xa0,

    /* U+0065 "e" */
    0x69, 0xf8, 0x60,

    /* U+0066 "f" */
    0x6b, 0xa4, 0x90,

    /* U+0067 "g" */
    0x7c, 0x63, 0x17, 0x87, 0xc0,

    /* U+0068 "h" */
    0x88, 0xf9, 0x99, 0x90,

    /* U+0069 "i" */
    0xbe,

    /* U+006A "j" */
    0x20, 0x92, 0x49, 0xc0,

    /* U+006B "k" */
    0x88, 0x9a, 0xca, 0x90,

    /* U+006C "l" */
    0xfe,

    /* U+006D "m" */
    0xef, 0x26, 0x4c, 0x99, 0x20,

    /* U+006E "n" */
    0xf9, 0x99, 0x90,

    /* U+006F "o" */
    0x74, 0x63, 0x17, 0x0,

    /* U+0070 "p" */
    0xb6, 0x63, 0x1f, 0x42, 0x0,

    /* U+0071 "q" */
    0x7c, 0x63, 0x17, 0x84, 0x20,

    /* U+0072 "r" */
    0xf2, 0x48,

    /* U+0073 "s" */
    0x74, 0x21, 0xf0,

    /* U+0074 "t" */
    0x5d, 0x24, 0xc0,

    /* U+0075 "u" */
    0x99, 0x99, 0xf0,

    /* U+0076 "v" */
    0x8a, 0x94, 0xc2, 0x0,

    /* U+0077 "w" */
    0x93, 0x65, 0xb3, 0x66, 0x40,

    /* U+0078 "x" */
    0x96, 0x26, 0x90,

    /* U+0079 "y" */
    0x8a, 0x94, 0x42, 0x23, 0x0,

    /* U+007A "z" */
    0xf2, 0x44, 0xf0
};


/*---------------------
 *  GLYPH DESCRIPTION
 *--------------------*/

static const lv_font_fmt_txt_glyph_dsc_t glyph_dsc[] = {
    {.bitmap_index = 0, .adv_w = 0, .box_w = 0, .box_h = 0, .ofs_x = 0, .ofs_y = 0} /* id = 0 reserved */,
    {.bitmap_index = 0, .adv_w = 43, .box_w = 1, .box_h = 1, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 1, .adv_w = 45, .box_w = 1, .box_h = 7, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 2, .adv_w = 62, .box_w = 2, .box_h = 1, .ofs_x = 1, .ofs_y = 2},
    {.bitmap_index = 3, .adv_w = 35, .box_w = 1, .box_h = 1, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 4, .adv_w = 84, .box_w = 5, .box_h = 7, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 9, .adv_w = 84, .box_w = 5, .box_h = 7, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 14, .adv_w = 84, .box_w = 4, .box_h = 7, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 18, .adv_w = 84, .box_w = 5, .box_h = 7, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 23, .adv_w = 84, .box_w = 4, .box_h = 7, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 27, .adv_w = 84, .box_w = 4, .box_h = 7, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 31, .adv_w = 84, .box_w = 4, .box_h = 7, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 35, .adv_w = 84, .box_w = 4, .box_h = 7, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 39, .adv_w = 84, .box_w = 4, .box_h = 7, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 43, .adv_w = 101, .box_w = 6, .box_h = 7, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 49, .adv_w = 90, .box_w = 4, .box_h = 7, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 53, .adv_w = 96, .box_w = 5, .box_h = 7, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 58, .adv_w = 110, .box_w = 6, .box_h = 7, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 64, .adv_w = 79, .box_w = 4, .box_h = 7, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 68, .adv_w = 77, .box_w = 4, .box_h = 7, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 72, .adv_w = 107, .box_w = 6, .box_h = 7, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 78, .adv_w = 111, .box_w = 5, .box_h = 7, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 83, .adv_w = 42, .box_w = 3, .box_h = 7, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 86, .adv_w = 57, .box_w = 3, .box_h = 7, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 89, .adv_w = 91, .box_w = 5, .box_h = 7, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 94, .adv_w = 74, .box_w = 4, .box_h = 7, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 98, .adv_w = 141, .box_w = 7, .box_h = 7, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 105, .adv_w = 117, .box_w = 6, .box_h = 7, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 111, .adv_w = 117, .box_w = 7, .box_h = 7, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 118, .adv_w = 88, .box_w = 4, .box_h = 7, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 122, .adv_w = 117, .box_w = 7, .box_h = 9, .ofs_x = 1, .ofs_y = -2},
    {.bitmap_index = 130, .adv_w = 94, .box_w = 5, .box_h = 7, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 135, .adv_w = 83, .box_w = 4, .box_h = 7, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 139, .adv_w = 83, .box_w = 5, .box_h = 7, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 144, .adv_w = 108, .box_w = 5, .box_h = 7, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 149, .adv_w = 97, .box_w = 6, .box_h = 7, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 155, .adv_w = 147, .box_w = 9, .box_h = 7, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 163, .adv_w = 93, .box_w = 6, .box_h = 7, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 169, .adv_w = 87, .box_w = 5, .box_h = 7, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 174, .adv_w = 89, .box_w = 5, .box_h = 7, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 179, .adv_w = 80, .box_w = 4, .box_h = 5, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 182, .adv_w = 92, .box_w = 5, .box_h = 7, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 187, .adv_w = 72, .box_w = 4, .box_h = 5, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 190, .adv_w = 92, .box_w = 5, .box_h = 7, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 195, .adv_w = 82, .box_w = 4, .box_h = 5, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 198, .adv_w = 50, .box_w = 3, .box_h = 7, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 201, .adv_w = 92, .box_w = 5, .box_h = 7, .ofs_x = 1, .ofs_y = -2},
    {.bitmap_index = 206, .adv_w = 89, .box_w = 4, .box_h = 7, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 210, .adv_w = 38, .box_w = 1, .box_h = 7, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 211, .adv_w = 38, .box_w = 3, .box_h = 9, .ofs_x = -1, .ofs_y = -2},
    {.bitmap_index = 215, .adv_w = 78, .box_w = 4, .box_h = 7, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 219, .adv_w = 38, .box_w = 1, .box_h = 7, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 220, .adv_w = 135, .box_w = 7, .box_h = 5, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 225, .adv_w = 89, .box_w = 4, .box_h = 5, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 228, .adv_w = 92, .box_w = 5, .box_h = 5, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 232, .adv_w = 92, .box_w = 5, .box_h = 7, .ofs_x = 1, .ofs_y = -2},
    {.bitmap_index = 237, .adv_w = 92, .box_w = 5, .box_h = 7, .ofs_x = 1, .ofs_y = -2},
    {.bitmap_index = 242, .adv_w = 55, .box_w = 3, .box_h = 5, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 244, .adv_w = 67, .box_w = 4, .box_h = 5, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 247, .adv_w = 54, .box_w = 3, .box_h = 6, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 250, .adv_w = 89, .box_w = 4, .box_h = 5, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 253, .adv_w = 76, .box_w = 5, .box_h = 5, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 257, .adv_w = 114, .box_w = 7, .box_h = 5, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 262, .adv_w = 73, .box_w = 4, .box_h = 5, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 265, .adv_w = 76, .box_w = 5, .box_h = 7, .ofs_x = 0, .ofs_y = -2},
    {.bitmap_index = 270, .adv_w = 71, .box_w = 4, .box_h = 5, .ofs_x = 1, .ofs_y = 0}
};

/*---------------------
 *  CHARACTER MAPPING
 *--------------------*/

static const uint16_t unicode_list_0[] = {
    0x0, 0x1, 0xd, 0xe
};

/*Collect the unicode lists and glyph_id offsets*/
static const lv_font_fmt_txt_cmap_t cmaps[] =
{
    {
        .range_start = 32, .range_length = 15, .glyph_id_start = 1,
        .unicode_list = unicode_list_0, .glyph_id_ofs_list = NULL, .list_length = 4, .type = LV_FONT_FMT_TXT_CMAP_SPARSE_TINY
    },
    {
        .range_start = 49, .range_length = 9, .glyph_id_start = 5,
        .unicode_list = NULL, .glyph_id_ofs_list = NULL, .list_length = 0, .type = LV_FONT_FMT_TXT_CMAP_FORMAT0_TINY
    },
    {
        .range_start = 65, .range_length = 26, .glyph_id_start = 14,
        .unicode_list = NULL, .glyph_id_ofs_list = NULL, .list_length = 0, .type = LV_FONT_FMT_TXT_CMAP_FORMAT0_TINY
    },
    {
        .range_start = 97, .range_length = 26, .glyph_id_start = 40,
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
    .cmap_num = 4,
    .bpp = 1,
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
const lv_font_t LR_StartFace_InfoFont = {
#else
lv_font_t LR_StartFace_InfoFont = {
#endif
    .get_glyph_dsc = lv_font_get_glyph_dsc_fmt_txt,    /*Function pointer to get glyph's data*/
    .get_glyph_bitmap = lv_font_get_bitmap_fmt_txt,    /*Function pointer to get glyph's bitmap*/
    .line_height = 9,          /*The maximum line height required by the font*/
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



#endif /*#if LR_STARTFACE_INFOFONT*/
