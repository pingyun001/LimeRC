#include "agreement_pp.h"
#include <string.h>
#include <stdio.h>


static void encode(RC_Key_V3A_t* scr, uint8_t* tar, uint8_t* tarLen);
static void decode(uint8_t* scr, RC_Key_V3A_t* tar);
static void debugOutRes(uint8_t* scr, char resStr[128]);

agreementCodec_t agrCodec_PP = 
{
	.name = "PP",
	
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
	char tempBuf[6] = {0};
	
	memset(tar, 0, AGREEMENT_OUT_BUF_MAX_LENGTH);
	
	/* add head */
	sprintf(tar, "pp");
	
	/* add ch 1 - 4 */
	for(uint8_t i = 0; i < 4; i++)
	{
		memset(tempBuf, 0, sizeof(tempBuf));
		
		snprintf(tempBuf, sizeof(tempBuf), "%d,", scr->ch[i]);
		
		strcat(tar, tempBuf);
	}
	
	/* add key */
	uint8_t keyLVal = 0, keyRVal = 0;
	keyLVal = scr->s_l;
	keyRVal = scr->s_r;
	memset(tempBuf, 0, sizeof(tempBuf));
	snprintf(tempBuf, sizeof(tempBuf), "%d,", keyLVal);
	strcat(tar, tempBuf);
	
	memset(tempBuf, 0, sizeof(tempBuf));
	snprintf(tempBuf, sizeof(tempBuf), "%d,", keyRVal);
	strcat(tar, tempBuf);
	
	/* add tail */
	memset(tempBuf, 0, sizeof(tempBuf));
	snprintf(tempBuf, sizeof(tempBuf), "\r\n");
	strcat(tar, tempBuf);
	
	/* calculate tarLen */
	*tarLen = strlen(tar);
}

static void decode(uint8_t* scr, RC_Key_V3A_t* tar)
{
	//Not supported
}

static void debugOutRes(uint8_t* scr, char resStr[128])
{
	int32_t len = 0;
	len += snprintf(resStr, 128, "DEBUS Res:");

	uint16_t ch[4] = {0};
	uint8_t keyLVal = 0, keyRVal = 0;
	sscanf(scr + 2, "%d,%d,%d,%d,%d,%d\r\n", &ch[0], &ch[1], &ch[2], &ch[3], &keyLVal, &keyRVal);

	len += snprintf(resStr + len, 128 - len, "ch1:%d ", ch[0]);
	len += snprintf(resStr + len, 128 - len, "ch2:%d ", ch[1]);
	len += snprintf(resStr + len, 128 - len, "ch3:%d ", ch[2]);
	len += snprintf(resStr + len, 128 - len, "ch4:%d ", ch[3]);
	len += snprintf(resStr + len, 128 - len, "keyL:%d ", keyLVal);
	len += snprintf(resStr + len, 128 - len, "keyR:%d ", keyRVal);
	len += snprintf(resStr + len, 128 - len, "\n");
	
	resStr[127] = '\0';
}
