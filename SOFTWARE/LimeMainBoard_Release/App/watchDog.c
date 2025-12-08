#include "watchDog.h"
#include "iwdg.h"
#include "cmsis_os.h"

void watchDog_main(void const * argument)
{
	
	while(1)
	{
		/* KR Long Press, stop feed watchdog, then, restart */
		if(IS_KR_PRESSED())
		{
			osDelay(200);
			continue;
		}
		
		/* feed watchdog */
		HAL_IWDG_Refresh(&hiwdg1);
		osDelay(500);
	}
}



