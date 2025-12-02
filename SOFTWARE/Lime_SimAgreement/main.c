#include "main.h"
#include <stdio.h>
#include "agreement_hal.h"

RC_Key_V3A_t RC_Key_V3A;

/*

typedef struct
{
	uint16_t ch[4];				//0 ~ 2048 ~ 4095
	uint8_t l_down	: 1;
	uint8_t l_mid		: 1;
	uint8_t l_up		: 1;
	uint8_t r_down	: 1;
	uint8_t r_mid		: 1;
	uint8_t r_up		: 1;
	uint8_t joy_l		: 1;
	uint8_t joy_r		: 1;
}RC_Key_V3A_t;

*/

void printBufAdv(uint8_t* buf, uint32_t length, uint32_t showAddr)
{
	uint32_t addr = showAddr;
	size_t size = length;
	uint8_t *data = buf;
	uint32_t i = 0;
	printf("Offset (h) 00 01 02 03 04 05 06 07 08 09 0A 0B 0C 0D 0E 0F\r\n");
	for (i = 0; i < size; i++)
	{
		if (i % 16 == 0)
		{
			printf("[%08X] ", addr + i);
		}
		if(data[i] != 0)
			printf("%02X ", data[i]);
		else
			printf("-- ");
		if (((i + 1) % 16 == 0) || i == size - 1)
		{
			printf("\r\n");
		}
	}
}

void printRC_KEY_V3A(RC_Key_V3A_t* rc_key)
{
    printf("ch[0]:%d, ch[1]:%d, ch[2]:%d, ch[3]:%d, l_down:%d, l_mid:%d, l_up:%d, r_down:%d, r_mid:%d, r_up:%d, joy_l:%d, joy_r:%d\n", 
        rc_key->ch[0], rc_key->ch[1], rc_key->ch[2], rc_key->ch[3], rc_key->l_down, rc_key->l_mid, rc_key->l_up, rc_key->r_down, rc_key->r_mid, rc_key->r_up, rc_key->joy_l, rc_key->joy_r);
}


int main()
{
    printf("\n\n\n\n\n\n\n\n\n\n\n\n\nAgreement Demo Start!\n");

    uint8_t outBuf[256] = {0};
    char outRes[128] = {0};
    uint8_t len = {0};

    // RC_Key_V3A.ch[0] = 4095;
    // RC_Key_V3A.ch[1] = 0;
    // RC_Key_V3A.ch[2] = 2048;
    // RC_Key_V3A.ch[3] = 0;
    // RC_Key_V3A.l_down = 0;
    // RC_Key_V3A.l_mid = 1;
    // RC_Key_V3A.l_up = 0;
    // RC_Key_V3A.r_down = 0;
    // RC_Key_V3A.r_mid = 0;
    // RC_Key_V3A.r_up = 1;
    // RC_Key_V3A.joy_l = 1;
    // RC_Key_V3A.joy_r = 0;

    // printRC_KEY_V3A(&RC_Key_V3A);

#if 0
    printf("SBUS EnDecode Test\n");
    agrCodec_SBUS.encode(&RC_Key_V3A, (uint8_t*)outBuf, &len);
    printf("len:%d, outRes:%s\n", len, outBuf);
    printBufAdv((uint8_t*)outBuf, len, 0);

    agrCodec_SBUS.debugOutRes((uint8_t*)outBuf, (char*)outRes);
    printf("debugOutRes:%s\n", outRes);

    agrCodec_SBUS.decode((uint8_t*)outBuf, &RC_Key_V3A);
    printRC_KEY_V3A(&RC_Key_V3A);
#endif

#if 0
    printf("PP EnDecode Test\n");
    agrCodec_PP.encode(&RC_Key_V3A, (uint8_t*)outBuf, &len);
    printf("len:%d, outRes:%s\n", len, outBuf);
    printBufAdv((uint8_t*)outBuf, len, 0);

    agrCodec_PP.debugOutRes((uint8_t*)outBuf, (char*)outRes);
    printf("debugOutRes:%s\n", outRes);

    // agrCodec_PP.decode((uint8_t*)outBuf, &RC_Key_V3A);
    // printRC_KEY_V3A(&RC_Key_V3A);
#endif

#if 0
    printf("DBUS EnDecode Test\n");
    agrCodec_DBUS.encode(&RC_Key_V3A, (uint8_t*)outBuf, &len);
    printf("len:%d, outRes:%s\n", len, outRes);
    printBufAdv((uint8_t*)outBuf, len, 0);

    agrCodec_DBUS.debugOutRes((uint8_t*)outBuf, (char*)outRes);
    printf("debugOutRes:%s\n", outRes);

    agrCodec_DBUS.decode((uint8_t*)outBuf, &RC_Key_V3A);
    printRC_KEY_V3A(&RC_Key_V3A);
#endif

    // uint8_t dbusBuf[18] = {0x00, 0x04, 0x20, 0x00, 0x01, 0x78, 0x00};//全中
    // uint8_t dbusBuf[18] = {0x6C, 0x01, 0x20, 0x5B, 0x00, 0x78, 0x00};//双上
    // uint8_t dbusBuf[18] = {0x6C, 0x01, 0x20, 0x5B, 0x00, 0x78, 0x00};//双上
    // uint8_t dbusBuf[18] = {0x7C, 0x63, 0x8B, 0x0F, 0xD9, 0x72, 0x00};//双下
    // uint8_t dbusBuf[18] = {0x6C, 0x01, 0x20, 0x5B, 0x00, 0x78, 0x00};//双左
    // uint8_t dbusBuf[18] = {0x00, 0x04, 0x20, 0x00, 0x01, 0x78, 0x00};//LimeRC全中
    // uint8_t dbusBuf[18] = {0x00, 0x64, 0x0B, 0x00, 0x01, 0x78, 0x00};//LimeRC右上
    // uint8_t dbusBuf[18] = {0x00, 0x04, 0x20, 0x00, 0x29, 0x7D, 0x00};//LimeRC左下
    // uint8_t dbusBuf[18] = {0x00, 0x04, 0xA0, 0x5B, 0x00, 0x78, 0x00};//LimeRC左左
    uint8_t dbusBuf[18] = {0x94, 0x06, 0x20, 0x00, 0x01, 0x78, 0x00};//LimeRC右右
    agrCodec_DBUS.debugOutRes((uint8_t*)dbusBuf, outRes);
    printf("debugOutRes:%s\n", outRes);

    printf("Agreement Demo End!\n");

    return 0;
}

