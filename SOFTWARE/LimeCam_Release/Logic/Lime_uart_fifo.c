/*
本程序用来实现串口FIFO接收，并提供与屏幕显示对接的接口
最终实现屏幕显示串口数据的效果

特殊字符：下文结束字符均指\r或\n或\r\n三者的组合
  
如果输入数据超过了1行可显示的最大数据，则自动换行，同时在第一行末尾添加字符 - 。
如果输入数据超过了2行可显示的最大数据，且迟迟没有调用显示接口。则自动丢弃剩余的数据。

显示逻辑：
先根据FIFO里有几对结束字符和结束字符与字符串的组合，来判断有几句文本需要显示。

何时可以显示：（这里的可以显示是需要轮询的，轮询是否可以显示）
遇到结束符立刻可以进行显示。
未遇到结束符，但是50ms内未接到新字符，可以进行显示。

轮询显示后，标记最前一段字符串可以删除，接收新字符并进行覆盖

2023 LimeRC 青柠 数字图传遥控器系统
*/

#include "Lime_uart_fifo.h"
#include "string.h"
#include "stdio.h"

#define IS_NEXT_LINE_CHAR(chr)     ((chr == '\r') || (chr == '\n'))

//uartFifo_t uartFifo = {0};

void uartFifo_UartHandle(uartFifo_t *pin,uint8_t chr)    //放到串口接收中断，一次接收一个字节
{
  uint8_t illegalChar = 1;
  if(pin == NULL)return;
  
  if((chr>=' ')&&(chr<='~'))illegalChar = 0;
  else if(chr == '\r')illegalChar = 0;
  else if(chr == '\n')illegalChar = 0;
  if(illegalChar)return;                                  //接收字符合法性检查。非法字符直接丢弃
  if(IS_NEXT_LINE_CHAR(chr))                              //遇到结尾符号，且上一个符号不是结尾符号，换buf
  {
    if(! IS_NEXT_LINE_CHAR(pin->lastRecvChr))
    {                                                     
      pin->ActiveLine ++;                                 //换buf
      if(pin->ActiveLine >= UART_LCD_TOTAL_LINE_NUM)      //buf超限，从头开始循环
        pin->ActiveLine = 0;
      
      pin->inputPin = 0;                                  //复位保存指针
    }      
  }
  
  pin->lastRecvChr = chr;                                 //更新上个字符
                                                          //跳过切完行的首个\r\n
  if((pin->inputPin == 0) && IS_NEXT_LINE_CHAR(chr))return;
  
  if(pin->inputPin == 0)                                  //清空接收buf
     memset(pin->line[pin->ActiveLine].lineBuf,0,UART_LCD_ONE_LINE_NUM);
#if(UART_ONE_LINE_FULL_DISCARD)
  if(pin->inputPin >= UART_LCD_ONE_LINE_NUM)return;     //如果这个buf满了，直接退出
#endif
#if(UART_ONE_LINE_FULL_GO_NEXT)
  if(pin->inputPin >= UART_LCD_ONE_LINE_NUM)              //如果这个buf满了
  {
    pin->ActiveLine ++;                                   //换buf
    if(pin->ActiveLine >= UART_LCD_TOTAL_LINE_NUM)        //buf超限，从头开始循环
      pin->ActiveLine = 0;
      
    pin->inputPin = 0;                                    //复位保存指针
    memset(pin->line[pin->ActiveLine].lineBuf,0,UART_LCD_ONE_LINE_NUM);
  }
#endif
  
#if(!UART_ONE_LINE_FULL_DISCARD)
#if(!UART_ONE_LINE_FULL_GO_NEXT)
  #error: "UART_ONE_LINE_FULL_DISCARD and UART_ONE_LINE_FULL_DISCARD must choose one"
#endif
#endif
                                                          //保存字符
  pin->line[pin->ActiveLine].lineBuf[ pin->inputPin ] = chr;
  pin->line[pin->ActiveLine].RecvTime = HAL_GetTick();    //记录时间
  pin->inputPin ++;
  pin->line[pin->ActiveLine].lineLength = pin->inputPin;  //记录长度
  
  pin->lastRecvTime = HAL_GetTick();
  
  pin->DataIsNew = 1;
}

uint8_t uartShouldShowData(uartFifo_t *pin)               //判断是否是新数据
{
  if(pin == NULL)return 0;                                //参数错误
  
  if(pin->DataIsNew)
  {
    pin->DataIsNew = 0;
    return 1;
  }
  return 0;
}

uint8_t uartBufferIsEmpty(uartFifo_t *pin)                //判断缓冲区是否为空
{
  uint8_t i = 0;
  uint8_t IsEmpty = 1;
  for(i = 0; i < UART_LCD_TOTAL_LINE_NUM; i++)
  {
    if(pin->line[i].lineLength != 0)IsEmpty = 0;
  }
  return IsEmpty;
}

                                                          //获取fifo数据，返回值为Buff指针
uint8_t* uartFifo_GetDataPin(uartFifo_t *pin,uint8_t line)
{
  uint8_t *returnPin = NULL;
  int16_t offset = 0;                                     //地址偏移量
  if(pin == NULL)return 0;                                //参数错误
  
  if(!IS_NEXT_LINE_CHAR(pin->lastRecvChr))
    offset = pin->ActiveLine - line;                      //计算地址偏移量
  else 
    offset = pin->ActiveLine - line - 1;
  if(offset < 0)
    offset += UART_LCD_TOTAL_LINE_NUM;
  returnPin = pin->line[offset].lineBuf;
  return returnPin;
}


