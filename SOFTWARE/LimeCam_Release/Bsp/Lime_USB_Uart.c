#include "Lime_USB_Uart.h"
#include "usart.h"
#include "usbd_cdc_if.h"
#include <stdio.h>

#define DEBUG_PRINTF      0

USB_Uart_t USB_Uart = {NoConnect,NoConnect,0};



  /*******************************************************************************/
  /* Line Coding Structure                                                       */
  /*-----------------------------------------------------------------------------*/
  /* Offset | Field       | Size | Value  | Description                          */
  /* 0      | dwDTERate   |   4  | Number |Data terminal rate, in bits per second*/
  /* 4      | bCharFormat |   1  | Number | Stop bits                            */
  /*                                        0 - 1 Stop bit                       */
  /*                                        1 - 1.5 Stop bits                    */
  /*                                        2 - 2 Stop bits                      */
  /* 5      | bParityType |  1   | Number | Parity                               */
  /*                                        0 - None                             */
  /*                                        1 - Odd                              */
  /*                                        2 - Even                             */
  /*                                        3 - Mark                             */
  /*                                        4 - Space                            */
  /* 6      | bDataBits  |   1   | Number Data bits (5, 6, 7, 8 or 16).          */
  /*******************************************************************************/
uint8_t uart3TempRecvData = 0;
void Lime_USB_Uart_ResetVal(uint8_t* pbuf)
{
//  static uint32_t LastInTime = 0;
  USB_Uart.BaudRate = (pbuf[0] | (pbuf[1] << 8)| (pbuf[2] << 16)| (pbuf[3] << 24));
  USB_Uart.WordLength = pbuf[6];
  USB_Uart.StopBits = pbuf[4];
  USB_Uart.Parity = pbuf[5];
  
//  if(HAL_GetTick() - LastInTime < 10)
//  {
//    USB_Uart.Mode = Connect_Open;
//  }
//  else
//  {
//    USB_Uart.Mode = Connect_NoOpen;
//  }
//  LastInTime = HAL_GetTick();
  
  if(DEBUG_PRINTF)printf("rate:%d\r\n", USB_Uart.BaudRate);
//  printf("Stop:%d\r\n",USB_Uart.StopBits);
//  printf("parity:%d\r\n",USB_Uart.Parity);
//  printf("DataBits:%d\r\n",USB_Uart.WordLength);
  
  if(USB_Uart.BaudRate == 0)
  {
    USB_Uart.Mode = Connect_NoOpen;
  }
  else
  {
    USB_Uart.Mode = Connect_Open;
  }
  
  if((USB_Uart.Mode == Connect_Open) && (USB_Uart.LastMode != Connect_Open))
  {
    USB_RESET_UART_HANDLE.Init.BaudRate = USB_Uart.BaudRate;
    
    switch(USB_Uart.WordLength)
    {
//      case 7:
//        USB_RESET_UART_HANDLE.Init.WordLength = UART_WORDLENGTH_7B;
//        break;
      case 8:
        USB_RESET_UART_HANDLE.Init.WordLength = UART_WORDLENGTH_8B;
        break;
      case 9:
        USB_RESET_UART_HANDLE.Init.WordLength = UART_WORDLENGTH_9B;
        break;
      default:break;
    }
    switch(USB_RESET_UART_HANDLE.Init.StopBits)
    {
      case 0:
        USB_RESET_UART_HANDLE.Init.StopBits = UART_STOPBITS_1;
        break;
//      case 1:
//        USB_RESET_UART_HANDLE.Init.StopBits = UART_STOPBITS_1_5;
//        break;
      case 2:
        USB_RESET_UART_HANDLE.Init.StopBits = UART_STOPBITS_2;
        break;
      default:break;
    }
    switch(USB_Uart.Parity)
    {
      case 0:
        USB_RESET_UART_HANDLE.Init.Parity = UART_PARITY_NONE;
        break;
      case 1:
        USB_RESET_UART_HANDLE.Init.Parity = UART_PARITY_ODD;
        break;
      case 2:
        USB_RESET_UART_HANDLE.Init.Parity = UART_PARITY_EVEN;
        break;
      //其余硬件不支持
      default:break;
    }
    if(DEBUG_PRINTF)printf("WritingInREG\r\n");
    //memset(pbuf,0,7);
    if (HAL_UART_Init(&USB_RESET_UART_HANDLE) != HAL_OK)
    {
      Error_Handler();
    }
    HAL_UART_Receive_IT(&huart3, &uart3TempRecvData, 1);
  }
  
  USB_Uart.LastMode = USB_Uart.Mode;
}


extern USBD_HandleTypeDef hUsbDeviceFS;

inline uint8_t Lime_USB_UART_Is_Connected(void)         //如果已连接，返回1，未连接返回0
{
  if(USB_HARDWARE_DISCONNECT)return 0;
//  {
//    USB_Uart.Mode = NoConnect;
//    USB_Uart.LastMode = NoConnect;
//  }
  if(USB_Uart.Mode == Connect_Open)return 1;
  return 0;
}

void Lime_USB_UART_RetransUSB_handle(uint8_t chr)  //放串口接收中断里
{
  if(Lime_USB_UART_Is_Connected())
    CDC_Transmit_FS(&chr,1);
}
inline void Lime_USB_UART_RetransUART_handle(uint8_t* Buf, uint32_t *Len)  //放USB接收中断
{
  if(Lime_USB_UART_Is_Connected())
    HAL_UART_Transmit(&USB_RESET_UART_HANDLE,Buf,*Len,0xff);
}




