#ifndef __LIME_NRF_SLAVE_H
#define __LIME_NRF_SLAVE_H


#include "main.h"

#define LIME_NRF_COMM_HEAD				0x70
#define LIME_NRF_COMM_HEAD_MASTER	0xAA
#define LIME_NRF_COMM_HEAD_SLAVE	0xBB
#define LIME_NRF_COMM_TAIL				'\n'

typedef enum
{
	Info_Nrf_Mode_Transmit,
	Info_Nrf_Mode_Receive,
}LimeNrfSlave_BspMode_e;

typedef enum
{
	LimeNrfSlave_Status_Init = 0,
	LimeNrfSlave_Status_WaitingPairing,
	LimeNrfSlave_Status_Paired,
	LimeNrfSlave_Status_NoPaired,
}LimeNrfSlave_Status_e;

typedef enum
{
	LimeNrfSlave_Role_debug = 0,
	LimeNrfSlave_Role_CamerV3_1 = 1,
	LimeNrfSlave_Role_BasicV3_1 = 2,
	//...
}LimeNrfSlave_Role_e;

typedef enum
{
	LimeNrfCatAgreement_DBUS = 0,
	LimeNrfCatAgreement_SBUS = 1,
	LimeNrfCatAgreement_PP = 2,
	LimeNrfCatAgreement_DEBUG = 3,
	//...
	LimeNrfCatAgreement_MaxAndErr,
}LimeNrfCatAgreement_e;

typedef struct
{
	/* public use */
	uint8_t NowChannel;
	uint32_t lastRecvRightPackTime;
	LimeNrfCatAgreement_e catAgreement;
	bool catOutputEnable;
	bool catOutputTogg;
	
	char name[13];
	
	LimeNrfSlave_Status_e myStatus;
	LimeNrfSlave_Role_e myRole;
	uint32_t LimeChipUID[3];
	
	/* flash save info */
	uint32_t LimePairedUID[3];
	bool isPairedInfoValid;
	
	uint32_t sendBufFreeSize;
	uint32_t readBufUsedSize;
	
	/* private use */
	bool IsRCOfflineAndTurnOffCamera;
	uint32_t succeCount;
	uint32_t errorCount;
	uint8_t magicCodeSlave;
	uint8_t magicCodeMaster;
	uint32_t sendBufrdPin;		//USED for tx FIFO
	uint32_t sendBufwrPin;		//USED for tx FIFO
	
	/* extern malloc (need be init extern)*/
	uint8_t *sendBuff;				//USED for tx FIFO
	uint32_t sendBufTotalSize;//USED for tx FIFO
	
	uint8_t *readBuff;
	uint32_t readBufTotalSize;
	
	/* API (in blocking mode) (need be init extern)*/
	HAL_StatusTypeDef (*nrf_Init)(uint8_t channel);
	
	HAL_StatusTypeDef (*nrf_ChangeBspModeTo)(LimeNrfSlave_BspMode_e mode);
	
	HAL_StatusTypeDef (*nrf_SendPack32)(uint8_t buf[32]);
	
	uint8_t (*nrf_NewPackAvaliable)(void);
	HAL_StatusTypeDef (*nrf_ReadPack32)(uint8_t buf[32]);
	
	HAL_StatusTypeDef (*nrf_PowerCtrl)(bool isPowerOn);
	
}LimeNrfSlave_t;

HAL_StatusTypeDef LimeNrfSlave_Init(LimeNrfSlave_t *nrf_Slave);

HAL_StatusTypeDef LimeNrfSlave_RunHandle(LimeNrfSlave_t *nrf_Slave);

/* tx Fifo */
HAL_StatusTypeDef LimeNrfSlave_AddDataToSendBuf(LimeNrfSlave_t *nrf_Slave, uint8_t* buf, uint32_t size, uint32_t* realAddedSize);
static HAL_StatusTypeDef LimeNrfSlave_catDataFromSendBuf(LimeNrfSlave_t *nrf_Slave, uint8_t* buf, uint32_t size, uint32_t* realReaddedSize);
static HAL_StatusTypeDef LimeNrfSlave_delDataFromSendBuf(LimeNrfSlave_t *nrf_Slave, uint32_t size, uint32_t* realDeledSize);
void LimeNrf_FIFO_SelfTest(LimeNrfSlave_t *nrf_Slave);

void LimeNrfSlave_DebugShow(LimeNrfSlave_t *nrf_Slave);

/* extern API */
char* LimeNrfSlave_GetMyNamePin(LimeNrfSlave_t *nrf_Slave);
bool LimeNrfSlave_IsConnected(LimeNrfSlave_t *nrf_Slave);
bool LimeNrfSlave_IsPairing(LimeNrfSlave_t *nrf_Slave);
uint8_t LimeNrfSlave_GetRfCh(LimeNrfSlave_t *nrf_Slave);
LimeNrfSlave_Role_e LimeNrfSlave_GetAgreement(LimeNrfSlave_t *nrf_Slave);
bool LimeNrfSlave_GetOutPutLevelIsTogg(LimeNrfSlave_t *nrf_Slave);
bool LimeNrfSlave_GetOutPutEnable(LimeNrfSlave_t *nrf_Slave);
HAL_StatusTypeDef LimeNrfSlave_PowerCtrl(LimeNrfSlave_t *nrf_Slave, bool isPowerOn);

HAL_StatusTypeDef LimeNrfSlave_ResetForPair(LimeNrfSlave_t *nrf_Slave);

#endif	//__LIME_NRF_SLAVE_H
