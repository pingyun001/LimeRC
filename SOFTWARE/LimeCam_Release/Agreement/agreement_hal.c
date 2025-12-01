#include "agreement_hal.h"
#include <string.h>
#include <stdio.h>

#include "usart.h"
#define huartOutX huart1

static RC_Key_V3A_t RC_Key_V3A = {0};

static agreementCodec_t *agreementCodec[agreementCodecSel_Length] = AGREEMENT_CODEC_LIST;
static agreementCodec_t *usingCodec = NULL;

static uint8_t outUartBuf[AGREEMENT_OUT_BUF_MAX_LENGTH] = {0};
static uint8_t outLength = 0;
static bool isOutputEn = false;


bool isNewPackLoaded = false;
bool isOffline = true;
bool isSyncLock = false;
static uint32_t lastRefreshTime = 0;

void AgreementOut_copyTo_RC_Key_V3A(uint8_t nrfData[10])
{
	isSyncLock = true;
	
	memcpy(&RC_Key_V3A, nrfData, 10);
	
	isNewPackLoaded = true;
	isOffline = false;
	lastRefreshTime = HAL_GetTick();
	
	isSyncLock = false;
}

bool AgreementOut_IsBusy(void)
{
	if(huartOutX.gState != HAL_UART_STATE_READY)
		return true;
	
	return false;
}

HAL_StatusTypeDef AgreementOut_ChangeCodecTo(agreementCodecSel_e codec, bool isToggle, bool isEnable)
{
	isSyncLock = true;
	
	if(codec >= agreementCodecSel_Length)
	{
		isSyncLock = false;
		return HAL_ERROR;
	}
	if(AgreementOut_IsBusy())
	{
		isSyncLock = false;
		return HAL_BUSY;
	}
	
	usingCodec = agreementCodec[codec];
	
	if(isToggle)
		HAL_GPIO_WritePin(NOT_CTRL_GPIO_Port, NOT_CTRL_Pin, GPIO_PIN_SET);
	else
		HAL_GPIO_WritePin(NOT_CTRL_GPIO_Port, NOT_CTRL_Pin, GPIO_PIN_RESET);
	
	isOutputEn = isEnable;
	
	huartOutX.Init.BaudRate = usingCodec->uart_baudRate;
	huartOutX.Init.WordLength = usingCodec->uart_wordLength == 9 ? UART_WORDLENGTH_9B : UART_WORDLENGTH_8B;
	huartOutX.Init.Parity = usingCodec->uart_parity == 'O' ? UART_PARITY_ODD : usingCodec->uart_parity == 'E' ? UART_PARITY_EVEN : UART_PARITY_NONE;
	huartOutX.Init.StopBits = usingCodec->uart_stopBits == 2 ? UART_STOPBITS_2 : UART_STOPBITS_1;
	HAL_StatusTypeDef ret = HAL_UART_Init(&huartOutX);
	
	isSyncLock = false;
	
	return ret;
}

HAL_StatusTypeDef AgreementOut_OutStrToUart(void)
{
	if(AgreementOut_IsBusy())
	{
		return HAL_BUSY;
	}
	if(usingCodec == NULL)
	{
		return HAL_ERROR;
	}
	if(!isOutputEn)
	{
		return HAL_OK;
	}
	
	if(usingCodec->encode == NULL)
	{
//		printf("usingCodec->encode == NULL\n");
		return HAL_ERROR;
	}
	usingCodec->encode(&RC_Key_V3A, outUartBuf, &outLength);
//	printf(">>>bus %s Out:%s\n", usingCodec->name, outUartBuf);
	return HAL_UART_Transmit(&huartOutX, outUartBuf, outLength, 1000);
}

void AgreementOut_RunHandle(void)
{
	if(isSyncLock)
		return;
	
	/* normal out data to uart */
	if(isNewPackLoaded)
	{
		/* send pack */
		if(AgreementOut_OutStrToUart() == HAL_OK)
			isNewPackLoaded = false;
	}
	
	/* timeout. send one pack */
	else if((HAL_GetTick() - lastRefreshTime > 100) && ( !isOffline))
	{
		/* reset rockers */
		RC_Key_V3A.ch[0] = 2048;
		RC_Key_V3A.ch[1] = 2048;
		RC_Key_V3A.ch[2] = 2048;
		RC_Key_V3A.ch[3] = 2048;
		
		/* send pack */
		if(AgreementOut_OutStrToUart() == HAL_OK)
			isOffline = true;
	}
}
