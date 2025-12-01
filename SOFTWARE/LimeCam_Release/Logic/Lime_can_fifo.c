/*
提供can报文自动保存、解析与覆盖
缓冲区大小为8，遇到相同id的报文会执行覆盖，
接收到全新id的报文，会自动覆盖掉旧报文
*/

#include "Lime_can_fifo.h"
#include "stdio.h"
#include "string.h"

canFifo_t canFifo = {0};

void canFifo_AddMessage(uint16_t stdID,uint8_t data[8])     //id，和数据
{
  uint8_t i = 0;
  uint32_t TheTimeOfLastData = 0,id = 0;                    //找那个需要覆盖的数据
  
//  printf("GetCanID:0x%x\r\n", stdID);
//  printf("Data:%d,%d\r\n",data[0],data[1]);
  
  for(i = 0; i < TOTAL_CAN_PAGE; i++)
  {
    if((canFifo.page[i].stdID == stdID) && (canFifo.page[i].isUsed == 1))
    {                                                       //之前已经接受过该数据
      memcpy(canFifo.page[i].data,data,8);
      canFifo.page[i].time = HAL_GetTick();
      canFifo.LastRecvTime = HAL_GetTick();
//      printf("Old\n");
      return;
    }
    if(canFifo.page[i].isUsed == 0)                         //之前未接受过，是新数据
    {
      canFifo.page[i].isUsed = 1;
      canFifo.page[i].stdID = stdID;
      memcpy(canFifo.page[i].data,data,8);
      canFifo.page[i].time = HAL_GetTick();
      canFifo.LastRecvTime = HAL_GetTick();
      canFifo.TotalPage ++;
//      printf("New\n");
      return;
    }
  }
                                                            //数据满了
  TheTimeOfLastData = canFifo.page[0].time;
  for(i = 1; i < TOTAL_CAN_PAGE; i++)                       //遍历，找到最旧的数据，然后覆盖掉
  {
    if(canFifo.page[i].time < TheTimeOfLastData)
    {
      TheTimeOfLastData = canFifo.page[i].time;
      id = i;
    }
  }
  printf("CanCoverNum:%d,ITs stdID:%d\r\n",id,canFifo.page[id].stdID);
  canFifo.page[id].stdID = stdID;
  memcpy(canFifo.page[id].data,data,8);

}

uint8_t canFifo_GetTotalNum(void)                           //获取收到的总数据个数
{
  return canFifo.TotalPage;
}

void canFifo_GetStdIDList(uint16_t * saveBuf)               //获取std id 列表，将列表自动填入savePin
{
  uint8_t i = 0;
  uint8_t outPin = 0;
  for(i = 0; i < canFifo.TotalPage; i++)
  {
    if(canFifo.page[i].isUsed == 1)
    {
      saveBuf[outPin] = canFifo.page[i].stdID;
      outPin++;
    }
  }
  
}

uint8_t canFifo_Recv_IsOnline(void)                         //判断can设备是否在线
{
  if(HAL_GetTick() - canFifo.LastRecvTime < 500)return 1;
  return 0;
}
uint8_t canFifo_Recv_IsEmpty(void)                          //判断接收缓冲区是不是空的，空的返回1，否则返回0
{
  if(canFifo.TotalPage == 0)return 1;
  return 0;
}


//---------------------RM电机解码---------------------------------

void canFifo_Motor_Decode(uint8_t data[8], motor_measure_t* motor)
{
  if(data == NULL)return;
  if(motor == NULL)return;
  motor->ecd = (uint16_t)((data)[0] << 8 | (data)[1]);
  motor->speed_rpm = (uint16_t)((data)[2] << 8 | (data)[3]);
  motor->given_current = (uint16_t)((data)[4] << 8 | (data)[5]);
  motor->temperate = (data)[6];
}

void canFifo_Motor_Encode(uint8_t data[8], motor_measure_t* motor)
{
  if(data == NULL)return;
  if(motor == NULL)return;
  
  data[0] = motor->ecd >> 8;
  data[1] = motor->ecd & 0xff;
  data[2] = motor->speed_rpm >> 8;
  data[3] = motor->speed_rpm & 0xff;
  data[4] = motor->given_current >> 8;
  data[5] = motor->given_current & 0xff;
  data[6] = motor->temperate;
  data[7] = 0;
}



