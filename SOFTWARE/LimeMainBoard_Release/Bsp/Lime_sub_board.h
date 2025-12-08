#ifndef __LIME_SUB_BOARD_H
#define __LIME_SUB_BOARD_H

#include "main.h"

#define LIME_SUB_BOARD_BUF_SIZE	128

typedef struct
{
	uint16_t LimitMin[4];
	uint16_t DeathMin[4];
	uint16_t DeathMax[4];
	uint16_t LimitMax[4];
	bool calEn[4];
}Lime_sub_board_caliInfo_t;

extern uint8_t Lime_sub_board_rawBufA[LIME_SUB_BOARD_BUF_SIZE];
extern uint8_t Lime_sub_board_rawBufB[LIME_SUB_BOARD_BUF_SIZE];
extern Lime_sub_board_caliInfo_t Lime_sub_board_caliInfo;

void Lime_sub_board_Init(void);
void Lime_sub_board_UartHook(uint8_t *buf, uint32_t size);
void Lime_sub_board_rocker_CaluCaliVal(uint16_t raw[4], Lime_sub_board_caliInfo_t *calInfo, uint16_t res[4]);
void Lime_sub_board_rocker_AutoCalHandle(bool isInit, bool* isFinish, Lime_sub_board_caliInfo_t *calInfo, int8_t ui_channelPos[4], uint8_t ui_channelPercent[4][2]);

#endif	//__LIME_SUB_BOARD_H
