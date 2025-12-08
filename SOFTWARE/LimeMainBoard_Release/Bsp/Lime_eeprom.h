#ifndef __LIME_EEPROM_H
#define __LIME_EEPROM_H

#include "main.h"

#define EEP_TOTAL_SIZE	256
#define EEP_HEAD		0x70
#define EEP_TAIL		0x76

typedef enum
{
	Lime_eep_status_NoInit,
	Lime_eep_status_ApiErr,
	Lime_eep_status_Offline,
	Lime_eep_status_Initing,
	Lime_eep_status_Idle,
	Lime_eep_status_Busy,
}Lime_eep_status_e;

typedef struct
{
	/* screen */
	uint16_t screen_auto_reduce_light_time;
	uint16_t screen_auto_off_time;
	
	/* rocker cali */
	uint16_t LimitMin[4];
	uint16_t DeathMin[4];
	uint16_t DeathMax[4];
	uint16_t LimitMax[4];
	char calEn[4];
	
	/* last saved receiver info */
	bool autoConnEn;
	uint32_t autoConnUID[3];
	
	/* receiver agreement */
	uint8_t receiver_Agreement;		//0: DBUS, 1:SBUS, 2:PP, 3:DEBUG
	uint8_t receiver_OutToggle;		//0: silent, 1: normal, 2: toggle
	
	
	//......
}Lime_eep_data_t;

typedef struct
{
	uint16_t writeWrapSize;
	uint16_t readWrapSize;
	uint8_t  latency;
	
	Lime_eep_status_e status;
	Lime_eep_data_t eep_saved_data;
	Lime_eep_data_t eep_cache_data;
	uint8_t eep_raw_data[EEP_TOTAL_SIZE];
	
	HAL_StatusTypeDef (*isDeviceOnline)(void);
	HAL_StatusTypeDef (*write)(uint32_t address, uint8_t* buf, uint32_t length);
	HAL_StatusTypeDef (*read) (uint32_t address, uint8_t* buf, uint32_t length);
	
}Lime_eep_t;

HAL_StatusTypeDef Lime_eep_Init(Lime_eep_t *info, Lime_eep_data_t *defaultData, Lime_eep_data_t *syncData);

HAL_StatusTypeDef Lime_eep_HardwareTest(Lime_eep_t *info);

HAL_StatusTypeDef Lime_eep_SyncData(Lime_eep_t *info, Lime_eep_data_t *syncData);

Lime_eep_status_e Lime_eep_GetStatus(Lime_eep_t *info);

void Lime_eep_printBufAdv(uint8_t* buf, uint32_t length, uint32_t showAddr);

/*----------------------------------------ports & users----------------------------------------*/
extern Lime_eep_t LimeEEP;
extern Lime_eep_data_t defaultData;
extern Lime_eep_data_t hotData;


#endif	//__LIME_EEPROM_H
