#ifndef __UART_FIFO_H
#define __UART_FIFO_H

#include "main.h"

#define UART_ONE_LINE_FULL_DISCARD  0                     //一行满了，丢弃其余行
#define UART_ONE_LINE_FULL_GO_NEXT  1                     //一行满了，跳下一行


#define UART_LCD_ONE_LINE_NUM     20                      //LCD一行可以显示的字符数
#define UART_LCD_TOTAL_LINE_NUM   3                       //LCD一共的行数


typedef struct
{
  uint8_t lineBuf[UART_LCD_ONE_LINE_NUM + 1];             //buf
  uint8_t lineLength;                                     //存的字符长度
  uint32_t RecvTime;                                      //更新时间
}uart_line_t;

typedef struct
{
  uart_line_t line[UART_LCD_TOTAL_LINE_NUM];              //(auto)一共有3行buf
  uint8_t lastRecvChr;                                    //(auto)接收的上一个字符
  uint16_t inputPin;                                      //(auto)输入指针
  uint8_t ActiveLine;                                     //(auto)输入到哪个Line缓存
  uint16_t outputPin;                                     //(auto)输出指针，指向屏幕显示数据在buf中的开始位置（即前面的都丢弃）
  uint32_t lastRecvTime;                                  //(auto)上次接收中断的时间
  uint8_t DataIsNew;                                      //(auto)(user)记录数据是新的，进行一次屏幕判断后，该变量清0。接收一次数据置1
}uartFifo_t;

extern uartFifo_t uartFifo;

void uartFifo_UartHandle(uartFifo_t *pin,uint8_t chr);    //(移植)放到串口接收中断，一次接收一个字节
                  
uint8_t uartShouldShowData(uartFifo_t *pin);              //(移植)判断屏幕是否该显示数据
uint8_t uartBufferIsEmpty(uartFifo_t *pin);               //(移植)判断缓冲区是否为空

                                                          //(移植)获取fifo数据，返回值为Buff指针
uint8_t* uartFifo_GetDataPin(uartFifo_t *pin,uint8_t line);




#endif    //__UART_FIFO_H



