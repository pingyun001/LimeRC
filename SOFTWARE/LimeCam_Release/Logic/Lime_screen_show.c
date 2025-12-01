#include "Lime_screen_show.h"

#include "Lime_oled12832.h"
#include "Lime_ws2812.h"

#include <stdio.h>
#include <string.h>

#include "Lime_uart_fifo.h"
#include "Lime_USB_Uart.h"
#include "Lime_canFliter.h"
#include "Lime_can_fifo.h"
#include "string.h"
#include "stdio.h"

#include "Lime_nrf_slave_ports.h"
#include "Lime_nrf_slave.h"

#if LIME_DEBUG
#define ABOUT_ME_VERSION  "Version:DEBUG"
#else
#define ABOUT_ME_VERSION  "Version:3.1.0"
#endif

#if ((1) && GLOBAL_DEBUG_LOG_EN)
	#define DEBUG_LOG(...)	printf(__VA_ARGS__)
#else
	#define DEBUG_LOG(...)
#endif


oledCtrl_t oledCtrl = {0};

void Lime_screen_show_Init(void)
{
  oledCtrl.CtrlMode = Mode_Auto;
}


static void oledShowTitle(void)
{
  if(LimeNrfSlave_IsConnected(&LimeInfoNrf))
  {
    Lime_12832OLED_ShowString(0,0,"rc",8,1);
  }
  if(UART2_IS_ONLINE)
  {
    Lime_12832OLED_ShowString(38,0,"s2",8,1);
  }
  if(UART3_IS_ONLINE)
  {
    Lime_12832OLED_ShowString(51,0,"s3",8,1);
  }
  if(Lime_USB_UART_Is_Connected())
  {
    Lime_12832OLED_ShowString(96,0,"usb",8,1);
  }
  if(Lime_CanIsOnline())
  {
    Lime_12832OLED_ShowString(64,0,"can",8,1);
  }
  switch(oledCtrl.CtrlMode)
  {
    case Mode_Auto:
      Lime_12832OLED_ShowString(13,0,"auto",8,0);
      break;
    case Mode_ShowUART2:
      Lime_12832OLED_ShowString(38,0,"s2",8,0);
      break;
    case Mode_ShowUART3:
      Lime_12832OLED_ShowString(51,0,"s3",8,0);
      break;
    case Mode_ShowCAN:
      Lime_12832OLED_ShowString(64,0,"can",8,0);
      break;
    case Mode_ShowAbout:
      Lime_12832OLED_ShowString(83,0,"me",8,0);
      break;
    case Mode_ShowRcInfo:
      Lime_12832OLED_ShowString(0,0,"rc",8,0);
      break;
    default:break;
  }
}

static void oledShowUartString(uartFifo_t* Pin)     //展示串口信息到屏幕
{
  Lime_12832OLED_ShowString(0,24,(char*)uartFifo_GetDataPin(Pin,0),8,1);
  Lime_12832OLED_ShowString(0,16,(char*)uartFifo_GetDataPin(Pin,1),8,1);
  Lime_12832OLED_ShowString(0,8 ,(char*)uartFifo_GetDataPin(Pin,2),8,1);
}
static void oledShowCanString(void)
{
  char showBuf[32] = {0};
  char showSubBuf[6] = {0};
  uint8_t showLine = 8;
  uint8_t i = 0;
  uint16_t StdID_List[8] = {0};
  
  if(canFifo_GetTotalNum() > 8)return ;             //防止下面的getList数组越界
  canFifo_GetStdIDList(StdID_List);
  
  
  for(i = 0; i < canFifo_GetTotalNum(); i++)
  {
    snprintf(showSubBuf,7,"0x%x ",StdID_List[i]);
    strcat(showBuf,showSubBuf);                     //填入新数据并刷新显存
    Lime_12832OLED_ShowString(0,showLine,showBuf,8,1);
    if(i % 3 == 2)                                  //换行
    {
      showLine += 8;
      memset(showBuf,0,sizeof(showBuf));            //清空缓存
    }
  }
}

extern canFifo_t canFifo;
static void oledShowCan_MotorString(uint8_t canPage)
{
  motor_measure_t motor_measure;
  char showBuf[32] = {0};
  if(canPage > 8)return;
  
  canFifo_Motor_Decode(canFifo.page[canPage].data ,&motor_measure);
  memset(showBuf,0,sizeof(showBuf));
  snprintf(showBuf,20,"CAN(%d/%d) ID:0x%x",canPage + 1, canFifo_GetTotalNum(),canFifo.page[canPage].stdID);
  Lime_12832OLED_ShowString(0,0,showBuf,8,1);
  
  memset(showBuf,0,sizeof(showBuf));
  snprintf(showBuf,20,"Ecd:%4d  Spd:%4d",motor_measure.ecd,motor_measure.speed_rpm);
  Lime_12832OLED_ShowString(0,8,showBuf,8,1);
  
  memset(showBuf,0,sizeof(showBuf));
  snprintf(showBuf,20,"Cur:%4d  Tem:%4d",motor_measure.given_current,motor_measure.temperate);
  Lime_12832OLED_ShowString(0,16,showBuf,8,1);
  
  memset(showBuf,0,sizeof(showBuf));
  snprintf(showBuf,30,"%02x%02x %02x%02x %02x%02x %02x%02x",  
                                                      canFifo.page[canPage].data[0],canFifo.page[canPage].data[1],
                                                      canFifo.page[canPage].data[2],canFifo.page[canPage].data[3],
                                                      canFifo.page[canPage].data[4],canFifo.page[canPage].data[5],
                                                      canFifo.page[canPage].data[6],canFifo.page[canPage].data[7]);
  Lime_12832OLED_ShowString(0,24,showBuf,8,1);
}

static void oledShow_rcInfo(void)                     //展示遥控器信息，主要展示输出电平和协议
{
  char prtBuf[32] = {0};
  
  memset(prtBuf,0,32);
  snprintf(prtBuf,32,"RC online! CH:%d", LimeNrfSlave_GetRfCh(&LimeInfoNrf));
  Lime_12832OLED_ShowString(0,13,prtBuf,8,1);
  
  memset(prtBuf,0,32);
  strcat(prtBuf,"Mode:");
	
	if( !LimeNrfSlave_GetOutPutEnable(&LimeInfoNrf))
		strcat(prtBuf,"NoOut");
	else if(LimeNrfSlave_GetOutPutLevelIsTogg(&LimeInfoNrf))
		strcat(prtBuf,"Togg");
	else
		strcat(prtBuf,"Norm");
	
  strcat(prtBuf," Agr:");
  switch(LimeNrfSlave_GetAgreement(&LimeInfoNrf))
  {
    case LimeNrfCatAgreement_PP:
      strcat(prtBuf,"PP");
      break;
		case LimeNrfCatAgreement_DBUS:
      strcat(prtBuf,"DBUS");
      break;
		case LimeNrfCatAgreement_SBUS:
      strcat(prtBuf,"SBUS");
      break;
		case LimeNrfCatAgreement_DEBUG:
      strcat(prtBuf,"DEBUG");
      break;
    default:
      strcat(prtBuf,"Err");
      break;
  }
  Lime_12832OLED_ShowString(0,21,prtBuf,8,1);
}

void oledCtrl_NormalRun(void)
{
  char prtBuf[32] = {0};                          
                                                    //初入 串口2 
  if((oledCtrl.CtrlMode == Mode_ShowUART2) && (oledCtrl.LastMode != Mode_ShowUART2))
  {
    if(!uartBufferIsEmpty(&oledCtrl.uart2Fifo))     //如果串口2不为空，立刻显示
    {
      oledCtrl.uart2Fifo.DataIsNew = 1;
    }
  }
  
  if(oledCtrl.CtrlMode == Mode_ShowUART2)           //常驻 串口2 
  {
    if(uartShouldShowData(&oledCtrl.uart2Fifo))     //接收到新数据
    {
      OLED_Clear();
      oledShowTitle();
      oledShowUartString(&oledCtrl.uart2Fifo);
      OLED_Refresh();
    }
    else if(uartBufferIsEmpty(&oledCtrl.uart2Fifo))
    {
      OLED_Clear();
      oledShowTitle();
      Lime_12832OLED_ShowString(20,16,"UART2 No Data",8,1);
      OLED_Refresh();
    }
  }
  
  if((oledCtrl.CtrlMode == Mode_ShowUART3) && (oledCtrl.LastMode != Mode_ShowUART3))
  {
    if(!uartBufferIsEmpty(&oledCtrl.uart3Fifo))     //如果串口3不为空，立刻显示
    {
      oledCtrl.uart3Fifo.DataIsNew = 1;
    }
  }
  
  if(oledCtrl.CtrlMode == Mode_ShowUART3)           //常驻 串口3
  {
    if(uartShouldShowData(&oledCtrl.uart3Fifo))     //接收buf内有数据
    {
      OLED_Clear();                                 //清空缓冲区
      oledShowTitle();                              //展示头
      oledShowUartString(&oledCtrl.uart3Fifo);      //展示文
      OLED_Refresh();                               //刷新
    }
    else if(uartBufferIsEmpty(&oledCtrl.uart3Fifo))
    {
      OLED_Clear();
      oledShowTitle();
      Lime_12832OLED_ShowString(20,16,"UART3 No Data",8,1);
      OLED_Refresh();
    }
  }
  
  if(oledCtrl.CtrlMode == Mode_ShowCAN)             //常驻 CAN
  {
    if(!canFifo_Recv_IsEmpty())                     //CAN非空
    {
      OLED_Clear();
      oledShowTitle();
      oledShowCanString();
      OLED_Refresh();
    }
    else                                            //CAN未收到数据
    {
      OLED_Clear();
      oledShowTitle();
      Lime_12832OLED_ShowString(20,16,"CAN No Data",8,1);
      OLED_Refresh();
    }
  }
  if(oledCtrl.CtrlMode == SubMode_Can_ShowInfo)     //常驻 CAN 子模式，展示CAN详细信息
  {
    OLED_Clear();
    oledShowTitle();
    oledShowCan_MotorString(oledCtrl.canPagePin);
    OLED_Refresh();
  }
  
  
  
  if(oledCtrl.CtrlMode == Mode_ShowAbout)           //常驻 About
  {
    OLED_Clear();
    oledShowTitle();
    Lime_12832OLED_ShowString(20,12,ABOUT_ME_VERSION,8,1);
    Lime_12832OLED_ShowString(20,24,"bilibili PingYun",8,1);
    OLED_Refresh();
  }
  
  if(oledCtrl.CtrlMode == Mode_Auto)
  {
    if(oledCtrl.uart3Fifo.lastRecvTime > oledCtrl.uart2Fifo.lastRecvTime)
    {
      OLED_Clear();                                 //清空缓冲区
      oledShowTitle();                              //展示头
      oledShowUartString(&oledCtrl.uart3Fifo);      //展示文
      OLED_Refresh();                               //刷新
    }
    else
    {
      OLED_Clear();                                 //清空缓冲区
      oledShowTitle();                              //展示头
      oledShowUartString(&oledCtrl.uart2Fifo);      //展示文
      OLED_Refresh();                               //刷新
    }
  }
  
  if(oledCtrl.CtrlMode == Mode_ShowRcInfo)          //展示遥控器信息
  {
    if(LimeNrfSlave_IsConnected(&LimeInfoNrf))
    {
      OLED_Clear();
      oledShowTitle();
      oledShow_rcInfo();
      OLED_Refresh();
    }
		else if(LimeNrfSlave_IsPairing(&LimeInfoNrf))
		{
			OLED_Clear();
      oledShowTitle();
      Lime_12832OLED_ShowString(20,12,"Pairing...",8,1);
      memset(prtBuf,0,32);
      snprintf(prtBuf,32,"I'm:%s", LimeNrfSlave_GetMyNamePin(&LimeInfoNrf));
      Lime_12832OLED_ShowString(20,22,prtBuf,8,1);
      OLED_Refresh();
		}
    else
    {
      OLED_Clear();
      oledShowTitle();
      Lime_12832OLED_ShowString(20,12,"RC offline",8,1);
      memset(prtBuf,0,32);
      snprintf(prtBuf,32,"I'm:%s", LimeNrfSlave_GetMyNamePin(&LimeInfoNrf));
      Lime_12832OLED_ShowString(20,22,prtBuf,8,1);
      OLED_Refresh();
    }
    
  }
  
  oledCtrl.LastMode = oledCtrl.CtrlMode;
}

void keyScan_RunHandle(void)
{
  static uint32_t keyLastPressTime = 0;
  static uint32_t keyHasReleased = 0;
  static uint8_t lastKeyPress = 0;
  if((KEY_PRESSED) && (!lastKeyPress))//按下瞬间
  {
    keyLastPressTime = HAL_GetTick();
    
    if(oledCtrl.CtrlMode == SubMode_Can_ShowInfo)
    {                                 //在CAN子菜单里，循环展示每个ID
      oledCtrl.canPagePin ++;
      if(oledCtrl.canPagePin >= canFifo_GetTotalNum())oledCtrl.canPagePin = 0;
    }
    else if(oledCtrl.CtrlMode != Mode_ShowCAN)
    {
      keyHasReleased = 0;
      oledCtrl.CtrlMode ++;
      if(oledCtrl.CtrlMode >= Mode_CountEnd)
        oledCtrl.CtrlMode = Mode_Auto;
    }
  }
  if((!KEY_PRESSED) && (lastKeyPress))//松开瞬间
  {
    if((oledCtrl.CtrlMode == Mode_ShowCAN) && (keyHasReleased))
    {
      keyHasReleased = 0;
      oledCtrl.CtrlMode  = (oledCtrlMode_e)(Mode_ShowCAN + 1);
      if(oledCtrl.CtrlMode >= Mode_CountEnd)
        oledCtrl.CtrlMode = Mode_Auto;
    }
  }
  if(!KEY_PRESSED)                    //持续松开
  {
    keyHasReleased = 1;
  }
  
  if((KEY_PRESSED) && (lastKeyPress)) //长按
  {
    if((oledCtrl.CtrlMode == Mode_ShowCAN) && (HAL_GetTick() - keyLastPressTime > 500) 
         && (!canFifo_Recv_IsEmpty()) && (keyHasReleased))
    {                                 //从CAN进入CAN子菜单
      keyHasReleased = 0;
      oledCtrl.CtrlMode = SubMode_Can_ShowInfo;
    }
    if((oledCtrl.CtrlMode == SubMode_Can_ShowInfo) && (HAL_GetTick() - keyLastPressTime > 500)
         && (keyHasReleased))
    {                                 //从CAN子菜单退出
      keyHasReleased = 0;
      oledCtrl.CtrlMode  = Mode_ShowCAN;
      if(oledCtrl.CtrlMode >= Mode_CountEnd)
        oledCtrl.CtrlMode = Mode_Auto;
    }
		if((oledCtrl.CtrlMode == Mode_Auto) && (HAL_GetTick() - keyLastPressTime > 500))
		{																	//遥控器信息界面长按进入配对模式
			oledCtrl.CtrlMode = Mode_ShowRcInfo;
			DEBUG_LOG("%s():Enter Pair Mode\n", __FUNCTION__);
			
			LimeNrfSlave_ResetForPair(&LimeInfoNrf);
		}
  }
  
  
  lastKeyPress = KEY_PRESSED;
}

void oledCtrl_SimModeRun(void)  //模拟模式下的全新界面。
{
  OLED_Clear();
  Lime_12832OLED_ShowString(0,0,"RC simulation Mode",8,1);
  
//  if(JudgeSimMode() & 0x01)     //can模拟
//    Lime_12832OLED_ShowString(10,10,"CanTX Enable",8,1);
//  if(JudgeSimMode() & 0x02)     //RC模拟
//    Lime_12832OLED_ShowString(10,20,"RemcSim Enable",8,1);
  
  OLED_Refresh();
}

void Lime_screen_show_RunHandle(void)         //屏幕刷新逻辑
{
  if((oledCtrl.CtrlMode == Mode_PowerOff) && (oledCtrl.LastMode != Mode_PowerOff))
  {
    OLED_DisPlay_Off();
  }
  if((oledCtrl.CtrlMode != Mode_PowerOff) && (oledCtrl.LastMode == Mode_PowerOff))
  {
    OLED_DisPlay_On();
  }
  
//  if(JudgeSimMode())                  //接收机正在模拟其它设备
//    oledCtrl_SimModeRun();
//  else                                //接收机作为接收者                 
    oledCtrl_NormalRun();
}

void Lime_led_show_RunHandle(void)
{
	//成功连接，亮绿灯
	if(LimeNrfSlave_IsConnected(&LimeInfoNrf))
	{
		ws2812_SetColor(0, 20, 0);
		
		return;
	}
	//配对阶段，亮绿灯
	else if(LimeNrfSlave_IsPairing(&LimeInfoNrf))
	{
		ws2812_SetColor(0, 0, 20);
	}
	//断开连接，亮红灯
	else
	{
		ws2812_SetColor(20, 0, 0);
	}
}


