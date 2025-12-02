#include "agreement_debug.h"
#include <stdio.h>
#include <string.h>

static void encode(RC_Key_V3A_t* scr, uint8_t* tar, uint8_t* tarLen);
static void decode(uint8_t* scr, RC_Key_V3A_t* tar);
static void debugOutRes(uint8_t* scr, char resStr[128]);

agreementCodec_t agrCodec_DEBUG = 
{
	.name = "DEBUG",
	
	.uart_baudRate = 115200,
	.uart_wordLength = 8,
	.uart_parity = 'N',
	.uart_stopBits = 1,
	
	.encode = encode,
	.decode = decode,
	.debugOutRes = debugOutRes,
};

static void encode(RC_Key_V3A_t* scr, uint8_t* tar, uint8_t* tarLen)
{
    memcpy(tar, scr, sizeof(RC_Key_V3A_t));
    *tarLen = sizeof(RC_Key_V3A_t);
}
static void decode(uint8_t* scr, RC_Key_V3A_t* tar)
{
    memcpy(tar, scr, sizeof(RC_Key_V3A_t));
}
static void debugOutRes(uint8_t* scr, char resStr[128])
{
    int32_t len = 0;
    len += sprintf(resStr + len, "DEBUG Res:raw buf:");
    for(uint16_t i = 0; i < sizeof(RC_Key_V3A_t); i++)
    {
        len += sprintf(resStr + len, " %02X", scr[i]);
    }
    len += sprintf(resStr + len, "DEBUG Res:raw buf:");

    RC_Key_V3A_t temp_Key_V3A = {0};
    memcpy(&temp_Key_V3A, scr, sizeof(RC_Key_V3A_t));
    len += sprintf(resStr + len, "ch[0]:%d", temp_Key_V3A.ch[0]);
    len += sprintf(resStr + len, "ch[1]:%d", temp_Key_V3A.ch[1]);
    len += sprintf(resStr + len, "ch[2]:%d", temp_Key_V3A.ch[2]);
    len += sprintf(resStr + len, "ch[3]:%d", temp_Key_V3A.ch[3]);
    len += sprintf(resStr + len, "l_down:%d", temp_Key_V3A.l_down);
    len += sprintf(resStr + len, "l_mid:%d", temp_Key_V3A.l_mid);
    len += sprintf(resStr + len, "l_up:%d", temp_Key_V3A.l_up);
    len += sprintf(resStr + len, "r_down:%d", temp_Key_V3A.r_down);
    len += sprintf(resStr + len, "r_mid:%d", temp_Key_V3A.r_mid);
    len += sprintf(resStr + len, "r_up:%d", temp_Key_V3A.r_up);
}