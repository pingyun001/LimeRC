#ifndef __LIME_USB_UART_H
#define __LIME_USB_UART_H

#include "main.h"

#define USB_RESET_UART_HANDLE    huart3       //这里添加重设波特率的串口是哪个

#define USB_HARDWARE_DISCONNECT  (hUsbDeviceFS.dev_state == 4)//硬件断开连接


typedef enum
{
  NoConnect = 0,            //USB未连接（线都没插上或者线断开了）
  Connect_NoOpen,           //USB未打开
  Connect_Open,             //已连接且打开
}USB_UART_Mode_e;

typedef struct
{
  USB_UART_Mode_e Mode;     //(auto)指示USB状态，更新不及时，不要直接调用。请调用Lime_USB_UART_Is_Connected();
  USB_UART_Mode_e LastMode; //(auto)
  uint32_t BaudRate;        //(auto)从USB报文解析出的波特率
  uint32_t WordLength;      //(auto)从USB报文解析出的字长
  uint32_t StopBits;        //(auto)从USB报文解析出的停止位宽
  uint32_t Parity;          //(auto)从USB报文解析出的校验位
}USB_Uart_t;

extern USB_Uart_t USB_Uart;

void Lime_USB_Uart_ResetVal(uint8_t* pbuf);      //放到usbd_cdc_if.c中，用来重设串口硬件参数
uint8_t Lime_USB_UART_Is_Connected(void);        //如果已连接，返回1，未连接返回0

void Lime_USB_UART_RetransUART_handle(uint8_t* Buf, uint32_t *Len);  //放USB接收中断
void Lime_USB_UART_RetransUSB_handle(uint8_t chr);  //放串口接收中断里

#endif    //__LIME_USB_UART_H

