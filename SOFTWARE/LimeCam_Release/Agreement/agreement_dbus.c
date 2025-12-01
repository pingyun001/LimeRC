#include "agreement_dbus.h"
#include <stdio.h>

static void encode(RC_Key_V3A_t* scr, uint8_t* tar, uint8_t* tarLen);
static void decode(uint8_t* scr, RC_Key_V3A_t* tar);
static void debugOutRes(uint8_t* scr, char resStr[128]);

agreementCodec_t agrCodec_DBUS = 
{
	.name = "DBUS",
	
	.uart_baudRate = 100000,
	.uart_wordLength = 9,
	.uart_parity = 'E',
	.uart_stopBits = 1,
	
	.encode = encode,
	.decode = decode,
	.debugOutRes = debugOutRes,
};

#define RC_CH_VALUE_MIN         ((uint16_t)364)
#define RC_CH_VALUE_OFFSET      ((uint16_t)1024)
#define RC_CH_VALUE_MAX         ((uint16_t)1684)

/* ----------------------- RC Switch Definition----------------------------- */
#define RC_SW_UP                ((uint16_t)1)
#define RC_SW_MID               ((uint16_t)3)
#define RC_SW_DOWN              ((uint16_t)2)
#define SWITCH_IS_DOWN(s)       (s == RC_SW_DOWN)
#define SWITCH_IS_MID(s)        (s == RC_SW_MID)
#define SWITCH_IS_UP(s)         (s == RC_SW_UP)


enum{
	Switch_Up = 1,
	Switch_Middle = 3,
	Switch_Down = 2,
};

typedef struct {
	int16_t ch1;	            //(user)each ch value from -660 -- +660
	int16_t ch2;	            //(user)
	int16_t ch3;	            //(user)
	int16_t ch4;	            //(user)
	
	uint8_t switch_left;	    //(user)3 value
	uint8_t switch_right;     //(user)
	
	struct {
		int16_t x;              //(user)
		int16_t y;              //(user)
		int16_t z;              //(user)
	
		uint8_t press_left;     //(user)
		uint8_t press_right;    //(user)
	}mouse;
	
	struct {
		uint16_t key_code;      //(user)
/**********************************************************************************
   * keyboard:15   14   13   12   11   10   9   8   7   6     5     4   3   2   1
   *           V    C    X	  Z    G    F   R   E   Q  CTRL  SHIFT  D   A   S   W
************************************************************************************/

	}keyBoard;
}Lime_DBUS_t;

static Lime_DBUS_t LimeDBUS = {0};

void Lime_DBUS_encode(Lime_DBUS_t * dbus, uint8_t outBuf[18])
{
	if(outBuf == NULL)return;
	
	outBuf[0]  = ((uint16_t)dbus->ch1)&0xff;																					//8(ch1)
	outBuf[1]  = (((uint16_t)dbus->ch1>>8)&0x07) | (((uint16_t)dbus->ch2<<3)&0xf8);		//3(ch1)+5(ch2)bit
	outBuf[2]  = (((uint16_t)dbus->ch2>>5)&0x3f) | (((uint16_t)dbus->ch3<<2)&0xc0);		//6(ch2)+2(ch3)bit
	outBuf[3]  = (((uint16_t)dbus->ch3>>2)&0xff);																			//8(ch3)
	outBuf[4]  = (((uint16_t)dbus->ch3>>10)&0x01) | (((uint16_t)dbus->ch4<<1)&0xfe);	//1(ch3)+7(ch4)bit
	outBuf[5]  = (((uint16_t)dbus->ch4>>7)&0x0f) | ((dbus->switch_left<<4)&0x30) | 		//4(ch4)+2(s1)+2(s2)bit
													((dbus->switch_right<<6)&0xc0);	
	
	outBuf[6]	 = 	dbus->mouse.x;
	outBuf[7]	 = 	dbus->mouse.x >> 8;
	outBuf[8]	 = 	dbus->mouse.y;
	outBuf[9]	 = 	dbus->mouse.y >> 8;
	outBuf[10] = 	dbus->mouse.z;
	outBuf[11] = 	dbus->mouse.z >> 8;
	outBuf[12] = 	dbus->mouse.press_left;
	outBuf[13] = 	dbus->mouse.press_right;
	outBuf[14] = 	dbus->keyBoard.key_code & 0xff;
	outBuf[15] = 	dbus->keyBoard.key_code >> 8;
	outBuf[16] = 0;		//reserved
	outBuf[17] = 0;		//reserved
}

static int32_t map(int32_t x, int32_t in_min, int32_t in_max, int32_t out_min, int32_t out_max)
{
    return (x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
}

static void encode(RC_Key_V3A_t* scr, uint8_t* tar, uint8_t* tarLen)
{
	LimeDBUS.ch1 = map(scr->ch[0], 0, 4095, 364, 1684);
	LimeDBUS.ch2 = map(scr->ch[1], 0, 4095, 364, 1684);
	LimeDBUS.ch3 = map(scr->ch[2], 0, 4095, 364, 1684);
	LimeDBUS.ch4 = map(scr->ch[3], 0, 4095, 364, 1684);
	
	LimeDBUS.switch_left  = scr->s_l;
	LimeDBUS.switch_right = scr->s_r;
	
	Lime_DBUS_encode(&LimeDBUS, tar);

	* tarLen = 18;
}


static void decode(uint8_t* scr, RC_Key_V3A_t* tar)
{
	if (scr == NULL || tar == NULL)
	{
		return;
	}

	int16_t ch[4] = {0};

	ch[0] = (scr[0] | (scr[1] << 8)) & 0x07ff;
	ch[1] = ((scr[1] >> 3) | (scr[2] << 5)) & 0x07ff;
	ch[2] = ((scr[2] >> 6) | (scr[3] << 2) |
									(scr[4] << 10)) &0x07ff;
	ch[3] = ((scr[4] >> 1) | (scr[5] << 7)) & 0x07ff;

	uint8_t s[2] = {0};
	s[0] = ((scr[5] >> 4) & 0x0003);
	s[1] = ((scr[5] >> 4) & 0x000C) >> 2;
		
	tar->l_down = SWITCH_IS_DOWN(s[0]);
	tar->l_mid  = SWITCH_IS_MID(s[0]);
	tar->l_up   = SWITCH_IS_UP(s[0]);
			
	tar->r_down = SWITCH_IS_DOWN(s[1]);
	tar->r_mid  = SWITCH_IS_MID(s[1]);
	tar->r_up   = SWITCH_IS_UP(s[1]);
	
	tar->s_l = s[0];
	tar->s_r = s[1];

	tar->ch[0] = map(ch[0], 364, 1684, 0, 4095);
	tar->ch[1] = map(ch[1], 364, 1684, 0, 4095);
	tar->ch[2] = map(ch[2], 364, 1684, 0, 4095);
	tar->ch[3] = map(ch[3], 364, 1684, 0, 4095);
}



static void debugOutRes(uint8_t* scr, char resStr[128])
{
	int32_t len = 0;
	len += snprintf(resStr, 128, "DBUS Res:");

	LimeDBUS.ch1 = (scr[0] | (scr[1] << 8)) & 0x07ff;
	LimeDBUS.ch2 = ((scr[1] >> 3) | (scr[2] << 5)) & 0x07ff;
	LimeDBUS.ch3 = ((scr[2] >> 6) | (scr[3] << 2) |
											 (scr[4] << 10)) &0x07ff;
	LimeDBUS.ch4 = ((scr[4] >> 1) | (scr[5] << 7)) & 0x07ff;

	LimeDBUS.ch1 -= RC_CH_VALUE_OFFSET;
	LimeDBUS.ch2 -= RC_CH_VALUE_OFFSET;
	LimeDBUS.ch3 -= RC_CH_VALUE_OFFSET;
	LimeDBUS.ch4 -= RC_CH_VALUE_OFFSET;

	LimeDBUS.switch_left = ((scr[5] >> 4) & 0x0003);
	LimeDBUS.switch_right = ((scr[5] >> 4) & 0x000C) >> 2;

	len += snprintf(resStr + len, 128 - len, "ch1:%d ", LimeDBUS.ch1);
	len += snprintf(resStr + len, 128 - len, "ch2:%d ", LimeDBUS.ch2);
	len += snprintf(resStr + len, 128 - len, "ch3:%d ", LimeDBUS.ch3);
	len += snprintf(resStr + len, 128 - len, "ch4:%d ", LimeDBUS.ch4);

	switch(LimeDBUS.switch_left)
	{
		case Switch_Up:
			len += snprintf(resStr + len, 128 - len, "switch_left:%s", "Up");
			break;
		case Switch_Middle:
			len += snprintf(resStr + len, 128 - len, "switch_left:%s", "Middle");
			break;
		case Switch_Down:
			len += snprintf(resStr + len, 128 - len, "switch_left:%s", "Down");
			break;
	}

	switch(LimeDBUS.switch_right)
	{
		case Switch_Up:
			len += snprintf(resStr + len, 128 - len, " switch_right:%s", "Up");
			break;
		case Switch_Middle:
			len += snprintf(resStr + len, 128 - len, " switch_right:%s", "Middle");
			break;
		case Switch_Down:
			len += snprintf(resStr + len, 128 - len, " switch_right:%s", "Down");
			break;
	}

	len += snprintf(resStr + len, 128 - len, "\n");
	
	resStr[127] = '\0';
}
