#include "bspUI.h"
#include "cmsis_os.h"
#include <stdio.h>
#include <string.h>



#include "oled_st7789.h"
#include "Lime_sub_board.h"
#include "Lime_jpeg_decode.h"

#include "lvgl.h"
#include "lv_port_disp.h"
#include "Lime_LvMainFace.h"
#include "Lime_App_Hal.h"

void bspUI_main(void const * argument)
{
	LEprintf(">>>Task:%s, start!\n", __FUNCTION__);
	
	Lime_sub_board_Init();
	
	/* Init screen */
	oled_st7789_Init();
	osDelay(10);
	
	/* Init lvgl */
	lv_init();
	lv_port_disp_init();
	Lime_LvMainFace_Init();
	
	while(1)
	{
		lv_timer_handler();
		osDelay(1);
	}
}


