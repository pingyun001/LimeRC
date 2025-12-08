#ifndef __LIME_JPEG_DECODE_H
#define __LIME_JPEG_DECODE_H

#include "main.h"


HAL_StatusTypeDef Lime_jpeg_decode(uint8_t *jpegFile, uint32_t size);

uint8_t *Lime_jpeg_getResBuf(void);

void rgb565_to_grayscale(uint8_t *rgb565buf, int width, int height);

#endif	//__LIME_JPEG_DECODE_H
