#ifndef __LIME_SCREEN_SHOW_H
#define __LIME_SCREEN_SHOW_H

#include "main.h"

#include "Lime_uart_fifo.h"

typedef enum
{
  Mode_PowerOff = 0,            //关闭oled显示
  Mode_Auto = 1,                //自动切换数据显示（哪个端口的数据先发过来，显示哪个）
  Mode_ShowUART2,               //展示串口2
  Mode_ShowUART3,               //展示串口3
  Mode_ShowCAN,                 //展示CAN数据
  Mode_ShowAbout,               //展示关于界面
  Mode_ShowRcInfo,              //展示遥控器信息
  Mode_CountEnd,                //仅计数，无该模式
  
  SubMode_Can_ShowInfo,         //子模式，展示can 详细信息
  
}oledCtrlMode_e;


#define UART2_IS_ONLINE   (HAL_GetTick() - oledCtrl.uart2Fifo.lastRecvTime < 1000)
#define UART3_IS_ONLINE   (HAL_GetTick() - oledCtrl.uart3Fifo.lastRecvTime < 1000)
typedef struct
{
  uartFifo_t uart2Fifo;         //(auto)串口2接收Fifo
  uartFifo_t uart3Fifo;         //(auto)串口3接收Fifo
  
  oledCtrlMode_e CtrlMode;      //(user)控制模式切换
  oledCtrlMode_e LastMode;      //(auto)上次的模式
  
  uint8_t canPagePin;           //(auto)仅在SubMode_Can_ShowInfo下生效，用于指示显示哪个page的can的详细信息
}oledCtrl_t;

extern oledCtrl_t oledCtrl;





void Lime_screen_show_Init(void);
void Lime_screen_show_RunHandle(void);
void keyScan_RunHandle(void);
void Lime_led_show_RunHandle(void);


//3.2版本新增接收机模拟功能，故，显示分为正常模式和模拟模式，在模拟模式下需要切换一套全新的界面。
void oledCtrl_NormalRun(void);  //旧的默认功能界面
void oledCtrl_SimModeRun(void); //模拟模式下的全新界面。


#endif	//__LIME_SCREEN_SHOW_H
