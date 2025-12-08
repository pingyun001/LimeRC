#ifndef __LIME_NRF_SLAVE_PORTS_H
#define __LIME_NRF_SLAVE_PORTS_H

#include "main.h"
#include "Lime_nrf_master.h"


/* internal use */
static HAL_StatusTypeDef Info_Nrf_Slave_Init(uint8_t channel);

static HAL_StatusTypeDef Info_Nrf_ChangeModeTo(LimeNrfMaster_BspMode_e mode);

static HAL_StatusTypeDef Info_Nrf_SendPack32(uint8_t buf[32]);

static uint8_t Info_Nrf_NewPackAvaliable(void);

static HAL_StatusTypeDef Info_Nrf_ReceivePack32(uint8_t buf[32]);

static HAL_StatusTypeDef Info_Nrf_PowerCtrl(bool isPowerOn);


/* extern use */
extern LimeNrfMaster_t LimeInfoNrf;


#endif	//__LIME_NRF_SLAVE_PORTS_H
