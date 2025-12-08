#include "Lime_nrf_master.h"
#include <stdio.h>
#include <string.h>

#include "cmsis_os.h"
#define DELAY_TIME(ms)		osDelay(ms)

#if 0
	#define DEBUG_LOG(...)	LEprintf(__VA_ARGS__)
#else
	#define DEBUG_LOG(...)
#endif

#define ASSERT_ERR_RETURN(fun)	\
do\
{\
	HAL_StatusTypeDef ret = fun;\
	if(ret != HAL_OK)\
	{\
		DEBUG_LOG("%s(),line:%d,error\n", __FUNCTION__, __LINE__);\
		return ret;\
	}\
}while(0)

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

static void fillMasterInfoToBuf_WhoIsHere(uint8_t buf[32], LimeNrfMaster_t *nrf_Master);
static void fillMasterInfoToBuf_JumpToPrivateChannel(uint8_t buf[32], LimeNrfMaster_t *nrf_Master, LimeNrfSlaveInfo_t* info, uint8_t channel);
static void fillMasterInfoToBuf_NoAckOutput(uint8_t buf[32], LimeNrfMaster_t *nrf_Master, uint8_t payload[27]);
static void fillMasterInfoToBuf_AckOutput(uint8_t buf[32], LimeNrfMaster_t *nrf_Master, uint8_t payload[27]);
static void fillMasterInfoToBuf_SetSlaveOutAgreement(uint8_t buf[32], LimeNrfMaster_t *nrf_Master, uint8_t agreement, uint8_t outLevel, bool openVideo, uint8_t videoChannel);
static void fillMasterInfoToBuf_SetRCKeyV3A(uint8_t payload[27], LimeHal_KeyInfo_t *keyInfo);

static HAL_StatusTypeDef askAndWaitAnswerFromSlave(uint8_t txbuf[32], uint8_t rxbuf[32], LimeNrfMaster_t *nrf_Master);


HAL_StatusTypeDef LimeNrfMaster_Init(LimeNrfMaster_t *nrf_Master)
{
	/* error check for nrf_Master */
	if(nrf_Master == NULL)
	{
		DEBUG_LOG("%s(),nrf_Master is NULL\n", __FUNCTION__);
		return HAL_ERROR;
	}
	
	/* error check for nrf bsp APIs */
	if(	(nrf_Master->nrf_Init == NULL) || 
			(nrf_Master->nrf_ChangeBspModeTo == NULL) || 
			(nrf_Master->nrf_SendPack32 == NULL) || 
			(nrf_Master->nrf_NewPackAvaliable == NULL) || 
			(nrf_Master->nrf_ReadPack32 == NULL))
	{
		DEBUG_LOG("%s(),nrf bsp API is NULL\n", __FUNCTION__);
		return HAL_ERROR;
	}
	
	/* error check for send buffers */
	if((nrf_Master->sendBuff == NULL) || nrf_Master->sendBufTotalSize == 0)
	{
		DEBUG_LOG("%s(),nrf send buff is NULL\n", __FUNCTION__);
		return HAL_ERROR;
	}
	
	/* error check for read buffers */
	if((nrf_Master->readBuff == NULL) || nrf_Master->readBufTotalSize == 0)
	{
		DEBUG_LOG("%s(),nrf read buff is NULL\n", __FUNCTION__);
		return HAL_ERROR;
	}
	
	/* power on */
	ASSERT_ERR_RETURN(LimeNrfMaster_PowerCtrl(nrf_Master, false));
	DELAY_TIME(200);
	ASSERT_ERR_RETURN(LimeNrfMaster_PowerCtrl(nrf_Master, true));
	DELAY_TIME(200);
	
	/* refresh now status */
	nrf_Master->myStatus = LimeNrfMaster_Status_Init;
	
	/* try to init nrf bsp */
	ASSERT_ERR_RETURN(nrf_Master->nrf_Init(0));
	ASSERT_ERR_RETURN(nrf_Master->nrf_ChangeBspModeTo(Info_Nrf_Mode_Transmit));
	
	/* refresh default info */
	nrf_Master->NowChannel = 0;
	nrf_Master->myRole = LimeNrfMaster_Role_debug;
	nrf_Master->LimeChipUID[0] = HAL_GetUIDw0();
	nrf_Master->LimeChipUID[1] = HAL_GetUIDw1();
	nrf_Master->LimeChipUID[2] = HAL_GetUIDw2();
	nrf_Master->sendBufFreeSize = nrf_Master->sendBufFreeSize;
	nrf_Master->readBufUsedSize = nrf_Master->readBufTotalSize;
	
	DEBUG_LOG("My UID:0x%08x, 0x%08x, 0x%08x\n", HAL_GetUIDw0(), HAL_GetUIDw1(), HAL_GetUIDw2());
	
	/* refresh now status */
	nrf_Master->myStatus = LimeNrfMaster_Status_NoPaired;
	
	return HAL_OK;
}

#define CRC8_POLYNOMIAL 0x31 //x^8 + x^5 + x^4 + 1
static uint8_t cal_crc8(uint8_t *data, uint8_t len)
{
	uint8_t crc = 0x00;
	for (uint8_t i = 0; i < len; i++) 
	{
		crc ^= data[i];
		for (uint8_t j = 0; j < 8; j++) 
		{
			if (crc & 0x80) 
				crc = (crc << 1) ^ CRC8_POLYNOMIAL;
			else
				crc <<= 1;
		}
	}
	return crc;
}

static void fillMasterInfoToBuf_WhoIsHere(uint8_t buf[32], LimeNrfMaster_t *nrf_Master)
{
	/* fill Head */
	buf[0] = LIME_NRF_COMM_HEAD;
	buf[1] = LIME_NRF_COMM_HEAD_MASTER;
	
	/* fill option code */
	buf[2] = 0x01;
	
	/* fill payload: UID */
	memcpy(buf + 4, nrf_Master->LimeChipUID, 12);
	
	/* fill Magic Code & Magic Code added 1 */
	buf[29] = nrf_Master->magicCodeMaster++;
	
	/* cal crc val */
	buf[30] = cal_crc8(buf, 30);
	
	/* fill tail */
	buf[31] = LIME_NRF_COMM_TAIL;
}

static void fillMasterInfoToBuf_JumpToPrivateChannel(uint8_t buf[32], LimeNrfMaster_t *nrf_Master, LimeNrfSlaveInfo_t* info, uint8_t channel)
{
	/* fill Head */
	buf[0] = LIME_NRF_COMM_HEAD;
	buf[1] = LIME_NRF_COMM_HEAD_MASTER;
	
	/* fill option code */
	buf[2] = 0x02;
	
	/* fill payload */
	buf[3] = channel;
	memcpy(buf +  4, info->UID, 12);
	memcpy(buf + 16, nrf_Master->LimeChipUID, 12);
	
	/* fill Magic Code & Magic Code added 1 */
	buf[29] = nrf_Master->magicCodeMaster++;
	
	/* cal crc val */
	buf[30] = cal_crc8(buf, 30);
	
	/* fill tail */
	buf[31] = LIME_NRF_COMM_TAIL;
}

static void fillMasterInfoToBuf_NoAckOutput(uint8_t buf[32], LimeNrfMaster_t *nrf_Master, uint8_t payload[27])
{
	/* fill Head */
	buf[0] = LIME_NRF_COMM_HEAD;
	
	/* fill option code */
	buf[1] = 0x03;
	
	/* fill payload */
	memcpy(buf + 2, payload, 27);
	
	/* fill Magic Code & Magic Code added 1 */
	buf[29] = nrf_Master->magicCodeMaster++;
	
	/* cal crc val */
	buf[30] = cal_crc8(buf, 30);
	
	/* fill tail */
	buf[31] = LIME_NRF_COMM_TAIL;
}

static void fillMasterInfoToBuf_AckOutput(uint8_t buf[32], LimeNrfMaster_t *nrf_Master, uint8_t payload[27])
{
	/* fill Head */
	buf[0] = LIME_NRF_COMM_HEAD;
	
	/* fill option code */
	buf[1] = 0x04;
	
	/* fill payload */
	memcpy(buf + 2, payload, 27);
	
	/* fill Magic Code & Magic Code added 1 */
	buf[29] = nrf_Master->magicCodeMaster++;
	
	/* cal crc val */
	buf[30] = cal_crc8(buf, 30);
	
	/* fill tail */
	buf[31] = LIME_NRF_COMM_TAIL;
}

static void fillMasterInfoToBuf_SetSlaveOutAgreement(uint8_t buf[32], LimeNrfMaster_t *nrf_Master, uint8_t agreement, uint8_t outLevel, bool openVideo, uint8_t videoChannel)
{
	/* fill Head */
	buf[0] = LIME_NRF_COMM_HEAD;
	
	/* fill option code */
	buf[1] = 0x05;
	
	/* fill payload */
	buf[2] = agreement;
	buf[3] = outLevel;
	buf[4] = openVideo;
	buf[5] = videoChannel;
	
	/* fill Magic Code & Magic Code added 1 */
	buf[29] = nrf_Master->magicCodeMaster++;
	
	/* cal crc val */
	buf[30] = cal_crc8(buf, 30);
	
	/* fill tail */
	buf[31] = LIME_NRF_COMM_TAIL;
}

static void fillMasterInfoToBuf_SetRCKeyV3A(uint8_t payload[27], LimeHal_KeyInfo_t *keyInfo)
{
	RC_Key_V3A_t RC_Key_V3A = {0};
	
	RC_Key_V3A.ch[0] = keyInfo->rocker[0];
	RC_Key_V3A.ch[1] = keyInfo->rocker[1];
	RC_Key_V3A.ch[2] = keyInfo->rocker[2];
	RC_Key_V3A.ch[3] = keyInfo->rocker[3];
	RC_Key_V3A.l_down = keyInfo->l_down	% 2;
	RC_Key_V3A.l_mid	= keyInfo->l_mid	% 2;
	RC_Key_V3A.l_up	 	= keyInfo->l_up		% 2;
	RC_Key_V3A.r_down = keyInfo->r_down	% 2;
	RC_Key_V3A.r_mid	= keyInfo->r_mid	% 2;
	RC_Key_V3A.r_up	 	= keyInfo->r_up		% 2;
	RC_Key_V3A.joy_l	= keyInfo->joy_l	% 2;
	RC_Key_V3A.joy_r	= keyInfo->joy_r	% 2;
	RC_Key_V3A.s_l		= keyInfo->s_l;
	RC_Key_V3A.s_r		= keyInfo->s_r;
	
	memcpy(payload, (uint8_t*)&RC_Key_V3A, 10);
}

static HAL_StatusTypeDef askAndWaitAnswerFromSlave(uint8_t txbuf[32], uint8_t rxbuf[32], LimeNrfMaster_t *nrf_Master)
{
	HAL_StatusTypeDef ret = HAL_ERROR;
	
	/* change to sender */
	ASSERT_ERR_RETURN(nrf_Master->nrf_ChangeBspModeTo(Info_Nrf_Mode_Transmit));
	DELAY_TIME(2);
	
	/* try to send */
	ASSERT_ERR_RETURN(nrf_Master->nrf_SendPack32(txbuf));
	
	/* change to receiver */
	ret = nrf_Master->nrf_ChangeBspModeTo(Info_Nrf_Mode_Receive);
	if(ret != HAL_OK)
	{
		/* change to sender */
		ASSERT_ERR_RETURN(nrf_Master->nrf_ChangeBspModeTo(Info_Nrf_Mode_Transmit));
		
		return ret;
	}
	
	/* waiting slave answer info */
	uint32_t startTime = HAL_GetTick();
	while( !nrf_Master->nrf_NewPackAvaliable())
	{
		if(HAL_GetTick() - startTime > 100)
		{
			DEBUG_LOG("Timeout\n");
			return HAL_TIMEOUT;
		}
	}
	
	/* get rx data */
	ASSERT_ERR_RETURN(nrf_Master->nrf_ReadPack32(rxbuf));
	
	/* change to sender */
	ASSERT_ERR_RETURN(nrf_Master->nrf_ChangeBspModeTo(Info_Nrf_Mode_Transmit));
	
	return HAL_OK;
}

HAL_StatusTypeDef LimeNrfMaster_PowerCtrl(LimeNrfMaster_t *nrf_Master, bool isPowerOn)
{
	if(nrf_Master == NULL || nrf_Master->nrf_PowerCtrl == NULL)
		return HAL_ERROR;
	
	return nrf_Master->nrf_PowerCtrl(isPowerOn);
}

uint32_t LimeNrfMaster_GetLastSuccessRunTime(LimeNrfMaster_t *nrf_Master)
{
	if(nrf_Master == NULL)
		return 0;
	
	return nrf_Master->lastSuccRunTime;
}

//------------------------------Slave Info-----------------------------------//
LimeNrfSlaveInfo_t LimeNrfSlaveInfo[LIME_NRF_SLAVE_INFO_NUM] = {0};

HAL_StatusTypeDef LimeNrfMaster_ScanSlave(LimeNrfMaster_t *nrf_Master, LimeNrfSlaveInfo_t* info)
{
	uint8_t txbuf[32] = {0};
	uint8_t rxbuf[32] = {0};
	
	/* fill cmd 1: Who is Here */
	fillMasterInfoToBuf_WhoIsHere(txbuf, nrf_Master);
	
	/* send and wait ack */
	ASSERT_ERR_RETURN(askAndWaitAnswerFromSlave(txbuf, rxbuf, nrf_Master));
	
	/* check pack crc */
	if(rxbuf[30] != cal_crc8(rxbuf, 30))
	{
		DEBUG_LOG("CRC Error\n");
		return HAL_ERROR;
	}
	
	/* check option code */
	if(rxbuf[2] != 0x10)
	{
		DEBUG_LOG("OptionCode Error\n");
		return HAL_ERROR;
	}
	
	/* Get UID */
	uint32_t UID[3] = {0};
	memcpy(UID, rxbuf + 4, 12);
	
	/* Find target memory */
	uint16_t infoOffset = 0;
	for(uint16_t infoOffset = 0; infoOffset < LIME_NRF_SLAVE_INFO_NUM; infoOffset++)
	{
		/* free memory */
		if((info + infoOffset)->isEnable == false)
		{
			break;
		}
		
		/* used memory */
		if(memcmp((info + infoOffset)->UID, UID, 12) == 0)
		{
			break;
		}
	}
	
	/* check memory is full */
	if(infoOffset == LIME_NRF_SLAVE_INFO_NUM)
	{
		DEBUG_LOG("%s():No More Free Memory\n", __FUNCTION__);
		return HAL_ERROR;
	}
	
	/* save info */
	(info + infoOffset)->isEnable = true;
	(info + infoOffset)->ackTime = HAL_GetTick();
	(info + infoOffset)->slClass = rxbuf[3];
	memcpy((info + infoOffset)->UID, UID, 12);
	memcpy((info + infoOffset)->name, rxbuf + 16, 13);
	
	DEBUG_LOG("GetSlaveInfo:\nackTime:%d\nslClass:%d\nUID:0x%08x,0x%08x,0x%08x\nname:%s\n", (info + infoOffset)->ackTime,
																																													(info + infoOffset)->slClass,
																																													(info + infoOffset)->UID[0],
																																													(info + infoOffset)->UID[1],
																																													(info + infoOffset)->UID[2],
																																													(info + infoOffset)->name);
	
	return HAL_OK;
}

HAL_StatusTypeDef LimeNrfMaster_JumpToPrivateChannel(LimeNrfMaster_t *nrf_Master, LimeNrfSlaveInfo_t* info, uint8_t channel)
{
	uint8_t txbuf[32] = {0};
	uint8_t rxbuf[32] = {0};
	
	/* fill cmd 2: jump to private channel */
	fillMasterInfoToBuf_JumpToPrivateChannel(txbuf, nrf_Master, info, channel);
	
	/* send and wait ack */
	ASSERT_ERR_RETURN(askAndWaitAnswerFromSlave(txbuf, rxbuf, nrf_Master));
	
	/* check pack crc */
	if(rxbuf[30] != cal_crc8(rxbuf, 30))
	{
		DEBUG_LOG("CRC Error\n");
		return HAL_ERROR;
	}
	
	/* check option code */
	if(rxbuf[2] != 0x20)
	{
		DEBUG_LOG("OptionCode Error\n");
		return HAL_ERROR;
	}
	
	/* hardware: jump to private channel */
	ASSERT_ERR_RETURN(nrf_Master->nrf_Init(channel));
	
	/* save channel info */
	nrf_Master->NowChannel = channel;
	
	DEBUG_LOG("Jump to private channel%d, success!\n", channel);
	
	return HAL_OK;
}

HAL_StatusTypeDef LimeNrfMaster_SendNoAckOutput(LimeNrfMaster_t *nrf_Master, uint8_t payload[27])
{
	uint8_t txbuf[32] = {0};
	
	/* fill cmd 5: no ack send */
	fillMasterInfoToBuf_NoAckOutput(txbuf, nrf_Master, payload);
	
	/* change to sender */
	ASSERT_ERR_RETURN(nrf_Master->nrf_ChangeBspModeTo(Info_Nrf_Mode_Transmit));
	
	/* send pack */
	ASSERT_ERR_RETURN(nrf_Master->nrf_SendPack32(txbuf));
	
	DEBUG_LOG("Send no ack pack, success!\n");
	
	return HAL_OK;
}

HAL_StatusTypeDef LimeNrfMaster_SendRockerInfo(LimeNrfMaster_t *nrf_Master, LimeHal_KeyInfo_t *keyInfo)
{
	uint8_t payload[27] = {1,2,3,4,5};
	fillMasterInfoToBuf_SetRCKeyV3A(payload, keyInfo);
	return LimeNrfMaster_SendNoAckOutput(nrf_Master, payload);
}

HAL_StatusTypeDef LimeNrfMaster_SetReceiverOutputMode(LimeNrfMaster_t *nrf_Master, uint8_t agreement, uint8_t outLevel, bool openVideo, uint8_t videoChannel)
{
	uint8_t txbuf[32] = {0};
	uint8_t rxbuf[32] = {0};
	
	/* fill cmd 8: jump to private channel */
	fillMasterInfoToBuf_SetSlaveOutAgreement(txbuf, nrf_Master, agreement, outLevel, openVideo, videoChannel);
	
	/* send and wait ack */
	ASSERT_ERR_RETURN(askAndWaitAnswerFromSlave(txbuf, rxbuf, nrf_Master));
	
	/* check pack crc */
	if(rxbuf[30] != cal_crc8(rxbuf, 30))
	{
		DEBUG_LOG("CRC Error\n");
		return HAL_ERROR;
	}
	
	/* check option code */
	if(rxbuf[1] != 0x50)
	{
		DEBUG_LOG("OptionCode Error\n");
		return HAL_ERROR;
	}
	
	DEBUG_LOG("Set receiver output mode, success!\n");
	
	return HAL_OK;
}

HAL_StatusTypeDef LimeNrfMaster_CheckChannelIsClear(LimeNrfMaster_t *nrf_Master, uint8_t channel)
{
	/* jump channel */
	ASSERT_ERR_RETURN(nrf_Master->nrf_Init(channel));
	
	/* change mode to receiver */
	HAL_StatusTypeDef ret = nrf_Master->nrf_ChangeBspModeTo(Info_Nrf_Mode_Receive);
	if(ret != HAL_OK)
	{
		ASSERT_ERR_RETURN(nrf_Master->nrf_ChangeBspModeTo(Info_Nrf_Mode_Transmit));
		return HAL_ERROR;
	}
	DELAY_TIME(2);
	
	/* try to receive packs */
	uint32_t startTime = HAL_GetTick();
	while( !nrf_Master->nrf_NewPackAvaliable())
	{
		/* no sender, this channel can use */
		if(HAL_GetTick() - startTime > 400)
		{
			DEBUG_LOG("Channel %d Free\n", channel);
			
			/* reinit to channel 0 & transmit mode */
			ASSERT_ERR_RETURN(nrf_Master->nrf_Init(0));
			ASSERT_ERR_RETURN(nrf_Master->nrf_ChangeBspModeTo(Info_Nrf_Mode_Transmit));
			return HAL_OK;
		}
	}
	
	DEBUG_LOG("Channel %d Busy\n", channel);
	
	/* channel has packs, read */
	uint8_t rxBuf[32];
	ASSERT_ERR_RETURN(nrf_Master->nrf_ReadPack32(rxBuf));
	
	/* reinit to channel 0 & transmit mode */
	ASSERT_ERR_RETURN(nrf_Master->nrf_Init(0));
	ASSERT_ERR_RETURN(nrf_Master->nrf_ChangeBspModeTo(Info_Nrf_Mode_Transmit));
	return HAL_BUSY;
}
