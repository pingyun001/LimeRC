#include "Lime_nrf_slave.h"
#include "Lime_nrf_video.h"
#include "agreement_hal.h"
#include "nrfPhoto.h"
#include "Lime_flash.h"

#include <stdio.h>
#include <string.h>

#include "cmsis_os.h"
#define DELAY_TIME(ms)		osDelay(ms)

#if ((0) && GLOBAL_DEBUG_LOG_EN)
	#define DEBUG_LOG(...)	printf(__VA_ARGS__)
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

static uint8_t cal_crc8(uint8_t *data, uint8_t len);
static HAL_StatusTypeDef answerPackToMaster(uint8_t txbuf[32], LimeNrfSlave_t *nrf_Slave);

static void fillSlaveInfoToBuf_IamHere(uint8_t buf[32], LimeNrfSlave_t *nrf_Slave);
static void fillSlaveInfoToBuf_ChannelHoppingAnswer(uint8_t buf[32], LimeNrfSlave_t *nrf_Slave);
static void fillSlaveInfoToBuf_SlaveTxInfoToMaster(uint8_t buf[32], LimeNrfSlave_t *nrf_Slave, float voltage, uint8_t* infoBuf, uint8_t len);

static uint8_t rdNrfInfo_AsWhoIsHere(uint8_t rdbuf[32]);
static uint8_t rdNrfInfo_AsChannelHoppingForMe(uint8_t rdbuf[32], LimeNrfSlave_t *nrf_SlaveInfo);
static uint8_t rdNrfInfo_AsNoAckPack(uint8_t rdbuf[32]);
static uint8_t rdNrfInfo_AsAckPack(uint8_t rdbuf[32]);
static uint8_t rdNrfInfo_AsSetOutputMode(uint8_t rdbuf[32]);

HAL_StatusTypeDef LimeNrfSlave_Init(LimeNrfSlave_t *nrf_Slave)
{
	/* error check for nrf_Slave */
	if(nrf_Slave == NULL)
	{
		DEBUG_LOG("%s(),nrf_Slave is NULL\n", __FUNCTION__);
		return HAL_ERROR;
	}
	
	/* error check for nrf bsp APIs */
	if(	(nrf_Slave->nrf_Init == NULL) || 
			(nrf_Slave->nrf_ChangeBspModeTo == NULL) || 
			(nrf_Slave->nrf_SendPack32 == NULL) || 
			(nrf_Slave->nrf_NewPackAvaliable == NULL) || 
			(nrf_Slave->nrf_ReadPack32 == NULL))
	{
		DEBUG_LOG("%s(),nrf bsp API is NULL\n", __FUNCTION__);
		return HAL_ERROR;
	}
	
	/* error check for send buffers */
	if((nrf_Slave->sendBuff == NULL) || nrf_Slave->sendBufTotalSize == 0)
	{
		DEBUG_LOG("%s(),nrf send buff is NULL\n", __FUNCTION__);
		return HAL_ERROR;
	}
	
	/* error check for read buffers */
	if((nrf_Slave->readBuff == NULL) || nrf_Slave->readBufTotalSize == 0)
	{
		DEBUG_LOG("%s(),nrf read buff is NULL\n", __FUNCTION__);
		return HAL_ERROR;
	}
	
	/* refresh now status */
	nrf_Slave->myStatus = LimeNrfSlave_Status_Init;
	
	/* refresh default info */
#if LIME_DEBUG
	nrf_Slave->isPairedInfoValid = false;
	nrf_Slave->LimePairedUID[0] = 0;
	nrf_Slave->LimePairedUID[1] = 0;
	nrf_Slave->LimePairedUID[2] = 0;
	nrf_Slave->myRole = LimeNrfSlave_Role_debug;
	snprintf(nrf_Slave->name, 13, "LimeDebug");
#else
	nrf_Slave->isPairedInfoValid = Lime_GlobalData.isPaired;
	nrf_Slave->LimePairedUID[0] = Lime_GlobalData.LimePairedUID[0];
	nrf_Slave->LimePairedUID[1] = Lime_GlobalData.LimePairedUID[1];
	nrf_Slave->LimePairedUID[2] = Lime_GlobalData.LimePairedUID[2];
	nrf_Slave->myRole = LimeNrfSlave_Role_CamerV3_1;
	snprintf(nrf_Slave->name, 13, "%08x", HAL_GetUIDw0());
	DEBUG_LOG("Flash Info:UID:0x%08x,0x%08x,0x%08x\n", nrf_Slave->LimePairedUID[0], nrf_Slave->LimePairedUID[1], nrf_Slave->LimePairedUID[2]);
	DEBUG_LOG("Flash Info:Paired:%d\n", nrf_Slave->isPairedInfoValid);
#endif
	nrf_Slave->NowChannel = 0;
	nrf_Slave->LimeChipUID[0] = HAL_GetUIDw0();
	nrf_Slave->LimeChipUID[1] = HAL_GetUIDw1();
	nrf_Slave->LimeChipUID[2] = HAL_GetUIDw2();
	nrf_Slave->sendBufFreeSize = nrf_Slave->sendBufFreeSize;
	nrf_Slave->readBufUsedSize = nrf_Slave->readBufTotalSize;
	nrf_Slave->magicCodeMaster = 254;
	
	/* try to init nrf bsp */
	ASSERT_ERR_RETURN(nrf_Slave->nrf_Init(nrf_Slave->NowChannel));
	ASSERT_ERR_RETURN(nrf_Slave->nrf_ChangeBspModeTo(Info_Nrf_Mode_Receive));
	
	DEBUG_LOG("My UID:0x%08x, 0x%08x, 0x%08x\n", HAL_GetUIDw0(), HAL_GetUIDw1(), HAL_GetUIDw2());
	
	/* refresh now status */
	nrf_Slave->myStatus = nrf_Slave->isPairedInfoValid ? LimeNrfSlave_Status_Paired : LimeNrfSlave_Status_WaitingPairing;
	
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

static void fillSlaveInfoToBuf_IamHere(uint8_t buf[32], LimeNrfSlave_t *nrf_Slave)
{
	/* fill head */
	buf[0] = LIME_NRF_COMM_HEAD;
	buf[1] = LIME_NRF_COMM_HEAD_SLAVE;
	
	/* fill option code */
	buf[2] = 0x10;
	
	/* fill payload: myRole */
	buf[3] = nrf_Slave->myRole;
	
	/* fill payload: UID */
	memcpy(buf + 4, nrf_Slave->LimeChipUID, 12);
	
	/* fill payload: name */
	memcpy(buf + 16, nrf_Slave->name, 13);
	
	/* fill magic code */
	buf[29] = nrf_Slave->magicCodeSlave ++;
	
	/* cal crc val */
	buf[30] = cal_crc8(buf, 30);
	
	/* fill tail */
	buf[31] = LIME_NRF_COMM_TAIL;
}

static uint8_t rdNrfInfo_AsWhoIsHere(uint8_t rdbuf[32])
{
	if((rdbuf[0] != LIME_NRF_COMM_HEAD) || (rdbuf[1] != LIME_NRF_COMM_HEAD_MASTER))
		return 0;
	
	if(rdbuf[2] != 0x01)
		return 0;
	
	if(rdbuf[31] != LIME_NRF_COMM_TAIL)
		return 0;
	
	return 1;
}

static uint8_t rdNrfInfo_AsChannelHoppingForMe(uint8_t rdbuf[32], LimeNrfSlave_t *nrf_SlaveInfo)
{
	if((rdbuf[0] != LIME_NRF_COMM_HEAD) || (rdbuf[1] != LIME_NRF_COMM_HEAD_MASTER))
		return 0;
	
	if(rdbuf[2] != 0x02)
		return 0;
	
	if(rdbuf[31] != LIME_NRF_COMM_TAIL)
		return 0;
	
	int8_t ret = memcmp(rdbuf + 4, nrf_SlaveInfo->LimeChipUID, 12);
	if(ret != 0)
	{
		return 0;
	}
	
	return 1;
}

static HAL_StatusTypeDef answerPackToMaster(uint8_t txbuf[32], LimeNrfSlave_t *nrf_Slave)
{
		DELAY_TIME(1);
		
		nrf_Slave->nrf_ChangeBspModeTo(Info_Nrf_Mode_Transmit);
		
		if(nrf_Slave->nrf_SendPack32(txbuf) != HAL_OK)
		{
			nrf_Slave->nrf_ChangeBspModeTo(Info_Nrf_Mode_Receive);
			nrf_Slave->errorCount ++;
			
			return HAL_TIMEOUT;
		}
		
		DELAY_TIME(1);
		
		nrf_Slave->nrf_ChangeBspModeTo(Info_Nrf_Mode_Receive);
		
		return HAL_OK;
}

static void fillSlaveInfoToBuf_ChannelHoppingAnswer(uint8_t buf[32], LimeNrfSlave_t *nrf_Slave)
{
	/* fill head */
	buf[0] = LIME_NRF_COMM_HEAD;
	buf[1] = LIME_NRF_COMM_HEAD_SLAVE;
	
	/* fill option code */
	buf[2] = 0x20;
	
	/* fill magic code */
	buf[29] = nrf_Slave->magicCodeSlave ++;
	
	/* cal crc val */
	buf[30] = cal_crc8(buf, 30);
	
	/* fill tail */
	buf[31] = LIME_NRF_COMM_TAIL;
}

static uint8_t rdNrfInfo_AsNoAckPack(uint8_t rdbuf[32])
{
	if(rdbuf[0] != LIME_NRF_COMM_HEAD)
		return 0;
	
	if(rdbuf[1] != 0x03)
		return 0;
	
	if(rdbuf[31] != LIME_NRF_COMM_TAIL)
		return 0;
	
	return 1;
}

static uint8_t rdNrfInfo_AsAckPack(uint8_t rdbuf[32])
{
	if(rdbuf[0] != LIME_NRF_COMM_HEAD)
		return 0;
	
	if(rdbuf[1] != 0x04)
		return 0;
	
	if(rdbuf[31] != LIME_NRF_COMM_TAIL)
		return 0;
	
	return 1;
}

static uint8_t rdNrfInfo_AsSetOutputMode(uint8_t rdbuf[32])
{
	if(rdbuf[0] != LIME_NRF_COMM_HEAD)
		return 0;
	
	if(rdbuf[1] != 0x05)
		return 0;
	
	if(rdbuf[31] != LIME_NRF_COMM_TAIL)
		return 0;
	
	return 1;
}

static void fillSlaveInfoToBuf_SlaveTxInfoToMaster(uint8_t buf[32], LimeNrfSlave_t *nrf_Slave, float voltage, uint8_t* infoBuf, uint8_t len)
{
	/* fill head */
	buf[0] = LIME_NRF_COMM_HEAD;
	
	/* fill option code */
	buf[1] = 0x40;
	
	/* fill voltage */
	uint16_t vol_16 = voltage * 100.0f;
	buf[2] = vol_16 & 0xff;
	buf[3] = (vol_16 >> 8) & 0xff;
	
	/* fill info */
	len = len > 24 ? 24 : len;
	if(infoBuf != NULL)
		memcpy(&buf[4], infoBuf, len);
	
	/* fill magic code */
	buf[29] = nrf_Slave->magicCodeSlave ++;
	
	/* cal crc val */
	buf[30] = cal_crc8(buf, 30);
	
	/* fill tail */
	buf[31] = LIME_NRF_COMM_TAIL;
}

HAL_StatusTypeDef LimeNrfSlave_AddDataToSendBuf(LimeNrfSlave_t *nrf_Slave, uint8_t* buf, uint32_t size, uint32_t* realAddedSize)
{
	if((nrf_Slave == NULL) || (buf == NULL))
	{
		DEBUG_LOG("FIFO ADD PARA ERR\n");
		return HAL_ERROR;
	}
	
	uint32_t freeSize = (	nrf_Slave->sendBufrdPin > nrf_Slave->sendBufwrPin) ?  
												nrf_Slave->sendBufrdPin - nrf_Slave->sendBufwrPin : 
												nrf_Slave->sendBufTotalSize - (nrf_Slave->sendBufwrPin - nrf_Slave->sendBufrdPin);
	
	if(freeSize == 0)
	{
		* realAddedSize = 0;
		return HAL_OK;
	}
	
	uint32_t realWrSize = (freeSize > size) ? size : freeSize;
	
	uint32_t partALen = ((nrf_Slave->sendBufTotalSize - nrf_Slave->sendBufwrPin) >= realWrSize) ? realWrSize : nrf_Slave->sendBufTotalSize - nrf_Slave->sendBufwrPin;
	uint32_t partBLen = (partALen == realWrSize) ? 0 : realWrSize - partALen;
	
	memcpy(nrf_Slave->sendBuff + nrf_Slave->sendBufwrPin, buf, partALen);
	memcpy(nrf_Slave->sendBuff + 0, buf + partALen, partBLen);
	
	nrf_Slave->sendBufwrPin = (partALen == realWrSize) ? nrf_Slave->sendBufwrPin + partALen : partBLen;
	
	* realAddedSize = realWrSize;
	
	return HAL_OK;
}

static HAL_StatusTypeDef LimeNrfSlave_catDataFromSendBuf(LimeNrfSlave_t *nrf_Slave, uint8_t* buf, uint32_t size, uint32_t* realReaddedSize)
{
	if((nrf_Slave == NULL) || (buf == NULL))
	{
		DEBUG_LOG("FIFO CAT PARA ERR\n");
		return HAL_ERROR;
	}
	
	uint32_t usedSize = (	nrf_Slave->sendBufwrPin >= nrf_Slave->sendBufrdPin) ?  
												nrf_Slave->sendBufwrPin - nrf_Slave->sendBufrdPin : 
												nrf_Slave->sendBufTotalSize - (nrf_Slave->sendBufrdPin - nrf_Slave->sendBufwrPin);
	
	uint32_t realRdSize = (usedSize > size) ? size : usedSize;
	
	uint32_t partALen = ((nrf_Slave->sendBufTotalSize - nrf_Slave->sendBufrdPin) >= realRdSize) ? realRdSize : nrf_Slave->sendBufTotalSize - nrf_Slave->sendBufrdPin;
	uint32_t partBLen = (partALen == realRdSize) ? 0 : realRdSize - partALen;
	
	memcpy(buf, nrf_Slave->sendBuff + nrf_Slave->sendBufrdPin, partALen);
	memcpy(buf + partALen, nrf_Slave->sendBuff + 0, partBLen);
	
	* realReaddedSize = realRdSize;
	
	return HAL_OK;
}

static HAL_StatusTypeDef LimeNrfSlave_delDataFromSendBuf(LimeNrfSlave_t *nrf_Slave, uint32_t size, uint32_t* realDeledSize)
{
	uint32_t usedSize = (	nrf_Slave->sendBufwrPin >= nrf_Slave->sendBufrdPin) ?  
												nrf_Slave->sendBufwrPin - nrf_Slave->sendBufrdPin : 
												nrf_Slave->sendBufTotalSize - (nrf_Slave->sendBufrdPin - nrf_Slave->sendBufwrPin);
	
	uint32_t realRdSize = (usedSize > size) ? size : usedSize;
	
	uint32_t partALen = ((nrf_Slave->sendBufTotalSize - nrf_Slave->sendBufrdPin) >= realRdSize) ? realRdSize : nrf_Slave->sendBufTotalSize - nrf_Slave->sendBufrdPin;
	uint32_t partBLen = (partALen == realRdSize) ? 0 : realRdSize - partALen;
	
	nrf_Slave->sendBufrdPin = (partALen == realRdSize) ? nrf_Slave->sendBufrdPin + partALen : partBLen;
	
	* realDeledSize = realRdSize;
	
	return HAL_OK;
}

void LimeNrf_FIFO_SelfTest(LimeNrfSlave_t *nrf_Slave)
{
	DEBUG_LOG("%s()\n", __FUNCTION__);
	
	uint8_t testBuf[10];
	for(uint8_t i = 0; i < 10; i++)testBuf[i] = i;
	
	uint32_t realWrSize = 0, readRdSize = 0, realDelSize = 0;
	LimeNrfSlave_AddDataToSendBuf(nrf_Slave, testBuf, 8, &realWrSize);
	DEBUG_LOG("add Length:%d, wr:%d, rd:%d\n", realWrSize, nrf_Slave->sendBufwrPin, nrf_Slave->sendBufrdPin);
	
	memset(testBuf, 0, 10);
	LimeNrfSlave_catDataFromSendBuf(nrf_Slave, testBuf, 10, &readRdSize);
	DEBUG_LOG("cat Length:%d wr:%d, rd:%d\n", readRdSize, nrf_Slave->sendBufwrPin, nrf_Slave->sendBufrdPin);
	printfBuf(testBuf, readRdSize);
	
	LimeNrfSlave_delDataFromSendBuf(nrf_Slave, readRdSize, &realDelSize);
	DEBUG_LOG("del Length:%d wr:%d, rd:%d\n", realDelSize, nrf_Slave->sendBufwrPin, nrf_Slave->sendBufrdPin);
	
	//再写6个进去
	for(uint8_t i = 0; i < 10; i++)testBuf[i] = i + 0x40;  
	LimeNrfSlave_AddDataToSendBuf(nrf_Slave, testBuf, 6, &realWrSize);
	DEBUG_LOG("add Length:%d, wr:%d, rd:%d\n", realWrSize, nrf_Slave->sendBufwrPin, nrf_Slave->sendBufrdPin);
	
	//尝试回读8个
	memset(testBuf, 0, 10);
	LimeNrfSlave_catDataFromSendBuf(nrf_Slave, testBuf, 8, &readRdSize);
	DEBUG_LOG("cat Length:%d wr:%d, rd:%d\n", readRdSize, nrf_Slave->sendBufwrPin, nrf_Slave->sendBufrdPin);
	printfBuf(testBuf, readRdSize);
	
	//尝试回读7个
	memset(testBuf, 0, 10);
	LimeNrfSlave_catDataFromSendBuf(nrf_Slave, testBuf, 7, &readRdSize);
	DEBUG_LOG("cat Length:%d wr:%d, rd:%d\n", readRdSize, nrf_Slave->sendBufwrPin, nrf_Slave->sendBufrdPin);
	printfBuf(testBuf, readRdSize);
	
	//删掉成功回读的值
	LimeNrfSlave_delDataFromSendBuf(nrf_Slave, readRdSize, &realDelSize);
	DEBUG_LOG("del Length:%d wr:%d, rd:%d\n", realDelSize, nrf_Slave->sendBufwrPin, nrf_Slave->sendBufrdPin);
}

HAL_StatusTypeDef LimeNrfSlave_RunHandle(LimeNrfSlave_t *nrf_Slave)
{
	if(nrf_Slave->myStatus == LimeNrfSlave_Status_Init)
	{
		DEBUG_LOG("%s(), no Init\n", __FUNCTION__);
		
		return HAL_ERROR;
	}
	
	/* timeout check and auto turn-off camera */
	if((HAL_GetTick() - nrf_Slave->lastRecvRightPackTime > 1000) && ( !nrf_Slave->IsRCOfflineAndTurnOffCamera) && (nrf_Slave->myStatus == LimeNrfSlave_Status_Paired))
	{
		nrf_Slave->IsRCOfflineAndTurnOffCamera = true;
		
		/* turn off camera */
		nrfPhotoTask_Suspend();
		
		/* reset to channel 0 and waiting re-conn */
		LimeNrfSlave_Init(nrf_Slave);
	}
	
	uint8_t rdbuf[32] = {0};
	uint8_t txbuf[32] = {0};
	if( !nrf_Slave->nrf_NewPackAvaliable())
	{
		return HAL_OK;
	}
	
	/* get rx data */
	nrf_Slave->nrf_ReadPack32(rdbuf);
	
	/* pack crc check */
	if(rdbuf[30] != cal_crc8(rdbuf, 30))
	{
		DEBUG_LOG("CRC Error\n");
		goto errEnd;
	}
	
	/* pack magic code check*/
	if(nrf_Slave->magicCodeMaster == rdbuf[29])
	{
		DEBUG_LOG("MagicCode Error:%#x\n", rdbuf[29]);
		goto errEnd;
	}
	else
		nrf_Slave->magicCodeMaster = rdbuf[29];
	
	/* all Check finish, success count++ */
	nrf_Slave->succeCount++;
	
	/*1: judge if Broadcast frames */
	if(rdNrfInfo_AsWhoIsHere(rdbuf))
	{
		/* get Lime Remote Ctrl UID */
		uint32_t RemcUID[3] = {0};
		memcpy(RemcUID, rdbuf + 4, 12);
		
		/* this Lime Camera has paired */
		if(nrf_Slave->isPairedInfoValid)
		{
			if((RemcUID[0] == nrf_Slave->LimePairedUID[0]) && (RemcUID[1] == nrf_Slave->LimePairedUID[1]) && RemcUID[2] == nrf_Slave->LimePairedUID[2])
			{
				/*2: answer info */
				fillSlaveInfoToBuf_IamHere(txbuf, nrf_Slave);
				
				/* answer to Master */
				answerPackToMaster(txbuf, nrf_Slave);
			}
		}
		/* this Lime Camera no paired */
		else
		{
			/*2: answer info */
			fillSlaveInfoToBuf_IamHere(txbuf, nrf_Slave);
			
			/* answer to Master */
			answerPackToMaster(txbuf, nrf_Slave);
		}
		
		DEBUG_LOG(">>>F:1(I'm Here)\n");
		
		goto successEnd;
	}
	
	/*3: judge if Channel hopping instructions */
	if(rdNrfInfo_AsChannelHoppingForMe(rdbuf, nrf_Slave))
	{
		uint8_t targetChannel = rdbuf[3];
		
		/* get Lime Remote Ctrl UID */
		uint32_t RemcUID[3] = {0};
		memcpy(RemcUID, rdbuf + 16, 12);
		
		/*4: answer info */
		fillSlaveInfoToBuf_ChannelHoppingAnswer(txbuf, nrf_Slave);
		
		/* answer to Master */
		answerPackToMaster(txbuf, nrf_Slave);
		
		DEBUG_LOG(">>>F:3(Channel hopping to:%d)\n", targetChannel);
		DEBUG_LOG(">>>Master UID:0x%08x,0x%08x,0x%08x\n", RemcUID[0], RemcUID[1],RemcUID[2]);
//		printfBuf(rdbuf, 32);
		
		/* then re-Init the nrf to Channel... */
		nrf_Slave->nrf_Init(targetChannel);
		nrf_Slave->nrf_ChangeBspModeTo(Info_Nrf_Mode_Receive);
		nrf_Slave->NowChannel = targetChannel;
		DELAY_TIME(1);
		
		/* refresh now status & save */
		nrf_Slave->myStatus = LimeNrfSlave_Status_Paired;
		nrf_Slave->LimePairedUID[0] = RemcUID[0];
		nrf_Slave->LimePairedUID[1] = RemcUID[1];
		nrf_Slave->LimePairedUID[2] = RemcUID[2];
		nrf_Slave->isPairedInfoValid = true;
		
#if !LIME_DEBUG
		Lime_GlobalData.isPaired = true;
		Lime_GlobalData.LimePairedUID[0] = RemcUID[0];
		Lime_GlobalData.LimePairedUID[1] = RemcUID[1];
		Lime_GlobalData.LimePairedUID[2] = RemcUID[2];
		
		Lime_FlashWriteGlobalData();
#endif
		
		goto successEnd;
	}
	
	/*5: judge if No Ack pack*/
	if(rdNrfInfo_AsNoAckPack(rdbuf))
	{
		//save data to buf
		AgreementOut_copyTo_RC_Key_V3A(rdbuf + 2);
		
		DEBUG_LOG(">>>F:5(pack no ack)\n");
		
		goto successEnd;
	}
	
	/*6: judge if Ack pack*/
	if(rdNrfInfo_AsAckPack(rdbuf))
	{
		/* get payload */
		uint8_t catBuf[24] = {0};
		uint32_t catRealSize = 0;
		LimeNrfSlave_catDataFromSendBuf(nrf_Slave, catBuf, 24, &catRealSize);
		
		fillSlaveInfoToBuf_SlaveTxInfoToMaster(txbuf, nrf_Slave, 1.23f, catBuf, catRealSize);
		
		/* answer to Master */
		if(answerPackToMaster(txbuf, nrf_Slave) == HAL_OK)
		{
			uint32_t realDelSize;
			LimeNrfSlave_delDataFromSendBuf(nrf_Slave, catRealSize, &realDelSize);
			if(realDelSize != catRealSize)
			{
				DEBUG_LOG(">>>F:6(unexpected err)\n");
				goto errEnd;
			}
		}
		
		DEBUG_LOG(">>>F:6(pack ack)\n");
		
		goto successEnd;
	}
	
	/*8: set output mode & start video stream */
	if(rdNrfInfo_AsSetOutputMode(rdbuf))
	{
		LimeNrfCatAgreement_e agreement = (LimeNrfCatAgreement_e)rdbuf[2];
		uint8_t outLevel = rdbuf[3];
		
		agreement = agreement > LimeNrfCatAgreement_MaxAndErr ? LimeNrfCatAgreement_MaxAndErr : agreement;
		
		/* fill payload */
		memcpy(txbuf, rdbuf, 32);
		txbuf[1] = 0x50;
		txbuf[30] = cal_crc8(txbuf, 30);
		
		/* answer to Master */
		answerPackToMaster(txbuf, nrf_Slave);
		
		/* Save & Change UART Hardware attribute */
		nrf_Slave->catAgreement = agreement;
		nrf_Slave->catOutputEnable = (outLevel != 0);
		nrf_Slave->catOutputTogg = (outLevel == 2);
		AgreementOut_ChangeCodecTo((agreementCodecSel_e)agreement, nrf_Slave->catOutputTogg, nrf_Slave->catOutputEnable);
		
		/* set video stream */
		bool videoEnable = rdbuf[4];
		uint8_t videoChannel = rdbuf[5];
		if(videoEnable)
			nrfPhotoTask_ReStart(videoChannel);
		else
			nrfPhotoTask_Suspend();
		
		DEBUG_LOG(">>>F:8(set output mode:agree:%d, togg:%d, video:%d, channel:%d)\n", agreement, levelTogg, videoEnable, videoChannel);
		
		goto successEnd;
	}
		
successEnd:
	nrf_Slave->lastRecvRightPackTime = HAL_GetTick();
	DEBUG_LOG("total:%d, err:%d, rate:%.4f%%\n", nrf_Slave->succeCount, nrf_Slave->errorCount, ((float)nrf_Slave->errorCount) / ((float)nrf_Slave->succeCount) * 100.0f);
	nrf_Slave->IsRCOfflineAndTurnOffCamera = false;
	return HAL_OK;
	
	
errEnd:
	DEBUG_LOG("errorOccurred\n");
	printfBuf(rdbuf, 32);
	return HAL_ERROR;
}


void LimeNrfSlave_DebugShow(LimeNrfSlave_t *nrf_Slave)
{
	uint8_t buf[32] = {0};
	
	DEBUG_LOG("%s()\n", __FUNCTION__);
	
	fillSlaveInfoToBuf_IamHere(buf, nrf_Slave);
	
//	printfBuf(buf, 32);
}

/* extern API */
char* LimeNrfSlave_GetMyNamePin(LimeNrfSlave_t *nrf_Slave)
{
	return nrf_Slave->name;
}

bool LimeNrfSlave_IsConnected(LimeNrfSlave_t *nrf_Slave)
{
	if(((HAL_GetTick() - nrf_Slave->lastRecvRightPackTime) < 500) && nrf_Slave->NowChannel != 0)
		return true;
	
	return false;
}

bool LimeNrfSlave_IsPairing(LimeNrfSlave_t *nrf_Slave)
{
	if(nrf_Slave->myStatus == LimeNrfSlave_Status_WaitingPairing)
		return true;
	
	return false;
}

uint8_t LimeNrfSlave_GetRfCh(LimeNrfSlave_t *nrf_Slave)
{
	return nrf_Slave->NowChannel;
}

LimeNrfSlave_Role_e LimeNrfSlave_GetAgreement(LimeNrfSlave_t *nrf_Slave)
{
	return nrf_Slave->catAgreement;
}

bool LimeNrfSlave_GetOutPutLevelIsTogg(LimeNrfSlave_t *nrf_Slave)
{
	return nrf_Slave->catOutputTogg;
}
bool LimeNrfSlave_GetOutPutEnable(LimeNrfSlave_t *nrf_Slave)
{
	return nrf_Slave->catOutputEnable;
}

HAL_StatusTypeDef LimeNrfSlave_PowerCtrl(LimeNrfSlave_t *nrf_Slave, bool isPowerOn)
{
	if(nrf_Slave == NULL || nrf_Slave->nrf_PowerCtrl == NULL)
		return HAL_ERROR;
	
	return nrf_Slave->nrf_PowerCtrl(isPowerOn);
}

HAL_StatusTypeDef LimeNrfSlave_ResetForPair(LimeNrfSlave_t *nrf_Slave)
{
	if(nrf_Slave == NULL)
		return HAL_ERROR;
	
	/* hardware err */
	if(nrf_Slave->myStatus == LimeNrfSlave_Status_Init)
		return HAL_ERROR;
	
	/* enter pair mode */
	nrf_Slave->myStatus = LimeNrfSlave_Status_WaitingPairing;
	nrf_Slave->isPairedInfoValid = false;
	
	/* return to channel 0 */
	nrf_Slave->NowChannel = 0;
	
	/* re-Init Hardware to channel 0 with receive mode */
	ASSERT_ERR_RETURN(nrf_Slave->nrf_Init(0));
	ASSERT_ERR_RETURN(nrf_Slave->nrf_ChangeBspModeTo(Info_Nrf_Mode_Receive));
	
	/* save data to flash */
	Lime_GlobalData.isPaired = 0;
	Lime_GlobalData.LimePairedUID[0] = 0;
	Lime_GlobalData.LimePairedUID[1] = 0;
	Lime_GlobalData.LimePairedUID[2] = 0;
	Lime_FlashWriteGlobalData();
}
