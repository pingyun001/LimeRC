#ifndef __KEY_H
#define __KEY_H

#include "main.h"


#define KEY_OFFSET_SW_UP			(1UL << 0)
#define KEY_OFFSET_SW_SET			(1UL << 1)
#define KEY_OFFSET_SW_LEFT			(1UL << 2)
#define KEY_OFFSET_SW_RIGHT			(1UL << 3)
#define KEY_OFFSET_SW_DOWN			(1UL << 4)
#define REV_OFFSET_1				(1UL << 5)
#define REV_OFFSET_2				(1UL << 6)
#define REV_OFFSET_3				(1UL << 7)

#define KEY_OFFSET_L_DOWN			(1UL << 8)
#define KEY_OFFSET_L_MID			(1UL << 9)
#define KEY_OFFSET_L_UP				(1UL << 10)
#define KEY_OFFSET_R_DOWN			(1UL << 11)
#define KEY_OFFSET_R_MID			(1UL << 12)
#define KEY_OFFSET_R_UP				(1UL << 13)
#define KEY_OFFSET_JOY_L			(1UL << 14)
#define KEY_OFFSET_JOY_R			(1UL << 15)

#define KEY_GET_HARDWARE_REV(data16)		(((data16) & (REV_OFFSET_3 | REV_OFFSET_2 | REV_OFFSET_1)) >> 5)

#define OFFSET_BOOK_LIST \
{\
	KEY_OFFSET_SW_UP,\
	KEY_OFFSET_SW_SET,\
	KEY_OFFSET_SW_LEFT,\
	KEY_OFFSET_SW_RIGHT,\
	KEY_OFFSET_SW_DOWN,\
	KEY_OFFSET_L_DOWN,\
	KEY_OFFSET_L_MID,\
	KEY_OFFSET_L_UP,\
	KEY_OFFSET_R_DOWN,\
	KEY_OFFSET_R_MID,\
	KEY_OFFSET_R_UP,\
	KEY_OFFSET_JOY_L,\
	KEY_OFFSET_JOY_R,\
}

//所有按键规则：每次按下和每次抬起均自增1，若为偶数，则松开，若为奇数，则持久按下
typedef struct
{
	uint8_t sw_up;		
	uint8_t sw_set;		
	uint8_t sw_left;
	uint8_t sw_right;
	uint8_t sw_down;
	
	uint8_t l_down;
	uint8_t l_mid;
	uint8_t l_up;
	uint8_t r_down;
	uint8_t r_mid;
	uint8_t r_up;
	
	uint8_t joy_l;
	uint8_t joy_r;
	
	uint8_t kr;
	
	uint8_t reversion;	//硬件版本
	
	uint32_t refreshTime;
}KeyInfo_t;

extern KeyInfo_t KeyInfo;

uint8_t key_HC165Scan(uint16_t* data);

void key_ScanHandle(void);

uint8_t key_GetHardwareReversion(KeyInfo_t * info, uint8_t* reversion);


#endif	//__KEY_H
