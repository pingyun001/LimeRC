#ifndef __AGREEMENTS_HAL_H
#define __AGREEMENTS_HAL_H

#include "main.h"

#define AGREEMENT_OUT_BUF_MAX_LENGTH	128

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
	uint8_t s_l			: 2;	//1:up, 2:down, 3:mid
	uint8_t s_r			: 2;	//1:up, 2:down, 3:mid
}RC_Key_V3A_t;

typedef struct
{
	char name[8];
	
	uint32_t uart_baudRate;
	uint8_t  uart_wordLength;		//8:8bit, 9:9bit, others:reserved
	uint8_t  uart_stopBits;			//1:1bit, 2:2bit, others:reserved
	uint8_t  uart_parity;				//N:none, E:even, O:odd
	
	void (*encode)(RC_Key_V3A_t* scr, uint8_t* tar, uint8_t* tarLen);
	void (*decode)(uint8_t* scr, RC_Key_V3A_t* tar);
	void (*debugOutRes)(uint8_t* scr, char resStr[64]);
}agreementCodec_t;

#include "agreement_pp.h"
#include "agreement_dbus.h"
#include "agreement_sbus.h"
#include "agreement_debug.h"

typedef enum
{
	agreementCodecSel_DBUS = 0,
	agreementCodecSel_SBUS = 1,
	agreementCodecSel_PP = 2,
	agreementCodecSel_DEBUG = 3,
	//...
	agreementCodecSel_Length,
}agreementCodecSel_e;

#define AGREEMENT_CODEC_LIST	\
{\
	&agrCodec_DBUS,\
	&agrCodec_SBUS,\
	&agrCodec_PP,\
	&agrCodec_DEBUG,\
}\

bool AgreementOut_IsBusy(void);
void AgreementOut_copyTo_RC_Key_V3A(uint8_t nrfData[10]);
HAL_StatusTypeDef AgreementOut_ChangeCodecTo(agreementCodecSel_e codec, bool isToggle, bool isEnable);
void AgreementOut_RunHandle(void);

#endif	//__AGREEMENTS_HAL_H
