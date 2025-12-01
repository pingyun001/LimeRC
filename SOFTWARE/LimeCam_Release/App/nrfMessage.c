#include "nrfMessage.h"
#include "cmsis_os.h"
#include <stdio.h>

#include "Lime_nrf_slave_ports.h"
#include "Lime_nrf_slave.h"
#include "Lime_nrf_video.h"
#include "nrfPhoto.h"

#include "agreement_hal.h"

#if ((1) && GLOBAL_DEBUG_LOG_EN)
	#define DEBUG_LOG(...)	printf(__VA_ARGS__)
#else
	#define DEBUG_LOG(...)
#endif

void nrfMessage_main(void const * argument)
{
	static uint32_t totalSuccessCount = 0, totalErrCount = 0;
	
	LimeNrfSlave_PowerCtrl(&LimeInfoNrf, true);
	
	osDelay(500);
	
	DEBUG_LOG(">>>Task:%s, start!\n", __FUNCTION__);
	
	if(LimeNrfSlave_Init(&LimeInfoNrf) != HAL_OK)
	{
		DEBUG_LOG("Lime NRF Init Failed, Try Again\n");
		
		osDelay(10);
		
		/* re-try power on */
		LimeNrfSlave_PowerCtrl(&LimeInfoNrf, false);
		osDelay(500);
		LimeNrfSlave_PowerCtrl(&LimeInfoNrf, true);
		osDelay(300);
		if(LimeNrfSlave_Init(&LimeInfoNrf) != HAL_OK)
		{
			DEBUG_LOG("Lime NRF Init Failed Again!\n");
			
			vTaskDelete(NULL);
		}
	}
	else
	{
		DEBUG_LOG("Lime NRF Init Success\n");
	}
	
//	LimeNrf_FIFO_SelfTest(&LimeInfoNrf);
	
	AgreementOut_ChangeCodecTo(agreementCodecSel_PP, true, false);
	
	while(1)
	{
		if(LimeNrfSlave_RunHandle(&LimeInfoNrf) != HAL_OK)
		{
			DEBUG_LOG("error\n");
		}
		else
		{
//			DEBUG_LOG("running\n");
		}
		

		
		AgreementOut_RunHandle();
		
		osDelay(1);
	}
}
