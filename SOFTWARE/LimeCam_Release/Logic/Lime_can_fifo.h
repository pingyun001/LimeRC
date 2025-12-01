#ifndef __CAN_FIFO_H
#define __CAN_FIFO_H
#include "main.h"

#define TOTAL_CAN_PAGE    8       //总PAGE个数

typedef struct
{
  uint8_t isUsed;                 //(auto) 标记该数据是否被使用
  uint16_t stdID;                 //(auto) 此包can数据id
  uint8_t data[8];                //(auto) 8字节数据
  uint32_t time;                  //(auto) 接收时间
}canPage_t;

typedef struct
{
  canPage_t page[TOTAL_CAN_PAGE]; //(auto) 由canFifo_AddMessage更新
  uint8_t TotalPage;              //(auto) 总stdID个数
  uint32_t LastRecvTime;          //(auto) 最新接收时间
}canFifo_t;

void canFifo_AddMessage(uint16_t stdID,uint8_t data[8]);    //id，和数据
uint8_t canFifo_GetTotalNum(void);                          //获取收到的总数据个数
void canFifo_GetStdIDList(uint16_t * saveBuf);              //获取std id 列表，将列表自动填入savePin

uint8_t canFifo_Recv_IsOnline(void);                        //判断can设备是否在线
uint8_t canFifo_Recv_IsEmpty(void);                         //判断接收缓冲区是不是空的，空的返回1，否则返回0


//rm motor data
typedef struct
{
  uint16_t ecd;
  int16_t speed_rpm;
  int16_t given_current;
  uint8_t temperate;
} motor_measure_t;

void canFifo_Motor_Decode(uint8_t data[8], motor_measure_t* motor);
void canFifo_Motor_Encode(uint8_t data[8], motor_measure_t* motor);


#endif    //__CAN_FIFO_H

