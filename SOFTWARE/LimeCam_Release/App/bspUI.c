#include "bspUI.h"
#include "cmsis_os.h"
#include <stdio.h>

#include "Lime_oled12832.h"
#include "Lime_ws2812.h"
#include "LimePVD.h"
#include "Lime_flash.h"
#include "Lime_canFliter.h"
#include "usart.h"

#include "Lime_screen_show.h"
#include "nrfPhoto.h"
#include "agreement_hal.h"
#include "nrfPhoto.h"

#if ((1) && GLOBAL_DEBUG_LOG_EN)
	#define DEBUG_LOG(...)	printf(__VA_ARGS__)
#else
	#define DEBUG_LOG(...)
#endif

extern void MX_USB_DEVICE_Init(void);

volatile uint8_t uart3RecvData = 0;

void bspUI_main(void const * argument)
{
	DEBUG_LOG(">>>Task:%s, start!\n", __FUNCTION__);
	
	/* init internal flash & read global datas */
	Lime_FlashInit();
	
	/* init led */
	ws2812_Init();
	
	/* screen test */
	Lime_12832OLED_Init();
	OLED_Clear();
	char compileTimeChar[30] = {0};
	snprintf(compileTimeChar, 30, "Compile:%s", __TIME__);
	Lime_12832OLED_ShowString(0,0,"LimeCamV3.1",8,1);
	Lime_12832OLED_ShowString(0,16, compileTimeChar,8,1);
	OLED_Refresh();
	DEBUG_LOG("OLED_Refresh Finish\n");
	
	/* init uart */
	HAL_UART_Receive_IT(&huart3, &uart3RecvData, 1);
	
	/* init PVD */
	Lime_PVD_Init();
	
	/* init CAN */
	Lime_can_filter_init();
	
	/* init USB */
	MX_USB_DEVICE_Init();
	
	/* init screen interface */
	Lime_screen_show_Init();
	
	/* start camera Task */
	nrfPhoto_taskYIELD_exHook(0);
	
	while(1)
	{
		keyScan_RunHandle();
		
		Lime_screen_show_RunHandle();
		
		Lime_led_show_RunHandle();
		
		osDelay(5);
	}
}

