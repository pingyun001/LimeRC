#ifndef __LIME_NRF_MASTER_H
#define __LIME_NRF_MASTER_H

#include "main.h"
#include "Lime_App_Hal.h"

#define LIME_NRF_COMM_HEAD				0x70
#define LIME_NRF_COMM_HEAD_MASTER	0xAA
#define LIME_NRF_COMM_HEAD_SLAVE	0xBB
#define LIME_NRF_COMM_TAIL				'\n'

#define LIME_NRF_SLAVE_INFO_NUM		4

typedef enum
{
	Info_Nrf_Mode_Transmit,
	Info_Nrf_Mode_Receive,
}LimeNrfMaster_BspMode_e;

typedef enum
{
	LimeNrfMaster_Status_Init = 0,
	LimeNrfMaster_Status_WaitingPairing,
	LimeNrfMaster_Status_Paired,
	LimeNrfMaster_Status_NoPaired,
}LimeNrfMaster_Status_e;

typedef enum
{
	LimeNrfMaster_Role_debug = 0,
	LimeNrfMaster_Role_CamerV3_1 = 1,
	LimeNrfMaster_Role_BasicV3_1 = 2,
	//...
}LimeNrfMaster_Role_e;

typedef enum
{
	LimeNrfCatAgreement_None = 0,
	LimeNrfCatAgreement_DBUS = 1,
	LimeNrfCatAgreement_SBUS = 2,
	LimeNrfCatAgreement_PP = 3,
	//...
	LimeNrfCatAgreement_MaxAndErr,
}LimeNrfCatAgreement_e;

typedef struct
{
	/* public use */
	uint8_t NowChannel;
	uint32_t lastSuccRunTime;
	LimeNrfCatAgreement_e catAgreement;
	bool catOutputTogg;
	
	LimeNrfMaster_Status_e myStatus;
	LimeNrfMaster_Role_e myRole;
	uint32_t LimeChipUID[3];
	
	uint32_t sendBufFreeSize;
	uint32_t readBufUsedSize;
	
	/* private use */
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
	
	HAL_StatusTypeDef (*nrf_ChangeBspModeTo)(LimeNrfMaster_BspMode_e mode);
	
	HAL_StatusTypeDef (*nrf_SendPack32)(uint8_t buf[32]);
	
	uint8_t (*nrf_NewPackAvaliable)(void);
	HAL_StatusTypeDef (*nrf_ReadPack32)(uint8_t buf[32]);
	
	HAL_StatusTypeDef (*nrf_PowerCtrl)(bool isPowerOn);
	
}LimeNrfMaster_t;


HAL_StatusTypeDef LimeNrfMaster_Init(LimeNrfMaster_t *nrf_Master);

typedef struct
{
	bool isEnable;
	bool isConnected;
	uint32_t ackTime;
	
	uint8_t slClass;				//0:debug, 1:Adv, 2:Std(not support cam)
	uint32_t UID[3];
	char name[14];					//only used 13byte
	
}LimeNrfSlaveInfo_t;
extern LimeNrfSlaveInfo_t LimeNrfSlaveInfo[LIME_NRF_SLAVE_INFO_NUM];

HAL_StatusTypeDef LimeNrfMaster_ScanSlave(LimeNrfMaster_t *nrf_Master, LimeNrfSlaveInfo_t* info);

HAL_StatusTypeDef LimeNrfMaster_JumpToPrivateChannel(LimeNrfMaster_t *nrf_Master, LimeNrfSlaveInfo_t* info, uint8_t channel);

HAL_StatusTypeDef LimeNrfMaster_SendNoAckOutput(LimeNrfMaster_t *nrf_Master, uint8_t payload[27]);
HAL_StatusTypeDef LimeNrfMaster_SendRockerInfo(LimeNrfMaster_t *nrf_Master, LimeHal_KeyInfo_t *keyInfo);

HAL_StatusTypeDef LimeNrfMaster_SetReceiverOutputMode(LimeNrfMaster_t *nrf_Master, uint8_t agreement, uint8_t outLevel, bool openVideo, uint8_t videoChannel);

HAL_StatusTypeDef LimeNrfMaster_CheckChannelIsClear(LimeNrfMaster_t *nrf_Master, uint8_t channel);

/* extern API */
HAL_StatusTypeDef LimeNrfMaster_PowerCtrl(LimeNrfMaster_t *nrf_Master, bool isPowerOn);
uint32_t LimeNrfMaster_GetLastSuccessRunTime(LimeNrfMaster_t *nrf_Master);


#endif	//__LIME_NRF_MASTER_H
