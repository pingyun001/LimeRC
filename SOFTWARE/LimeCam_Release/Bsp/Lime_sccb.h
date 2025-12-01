#ifndef __LIME_SCCB_H
#define __LIME_SCCB_H

#include "main.h"


//SCCB引脚接口定义
#define SCCB_SCL_CLK_ENABLE       __HAL_RCC_GPIOB_CLK_ENABLE()		// SCL 引脚时钟
#define SCCB_SCL_PORT   		   GPIOB                 				      // SCL 引脚端口
#define SCCB_SCL_PIN     		   GPIO_PIN_8 								        // SCL 引脚
        
#define SCCB_SDA_CLK_ENABLE       __HAL_RCC_GPIOB_CLK_ENABLE() 	  // SDA 引脚时钟
#define SCCB_SDA_PORT   			 GPIOB                   			      // SDA 引脚端口
#define SCCB_SDA_PIN    			 GPIO_PIN_9              			      // SDA 引脚

#define SCCB_DelayVaule  8

#define LIME_SCCB_ACK_OK  	1  			                              // 响应正常
#define LIME_SCCB_ACK_ERR 	0				                              // 响应错误

#define OV2640_DEVICE_ADDRESS     0x60                            // OV2640地址

#define SCCB_SCL(a) \
        do { \
            if (a) \
                HAL_GPIO_WritePin(SCCB_SCL_PORT, SCCB_SCL_PIN, GPIO_PIN_SET); \
            else \
                HAL_GPIO_WritePin(SCCB_SCL_PORT, SCCB_SCL_PIN, GPIO_PIN_RESET); \
        } while (0)
#define SCCB_SDA(a) \
        do { \
            if (a) \
                HAL_GPIO_WritePin(SCCB_SDA_PORT, SCCB_SDA_PIN, GPIO_PIN_SET); \
            else \
                HAL_GPIO_WritePin(SCCB_SDA_PORT, SCCB_SDA_PIN, GPIO_PIN_RESET); \
        } while (0)

                    
//--------------------------------------以下为对外接口------------------------------------------------
                    
void Lime_sccb_gpio_Init(void);                                   //初始化GPIO
uint8_t Lime_sccb_WriteReg (uint8_t addr,uint8_t value);          //写寄存器
uint8_t Lime_sccb_ReadReg (uint8_t addr);                         //读寄存器
                    
#endif
