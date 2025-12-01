/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.h
  * @brief          : Header for main.c file.
  *                   This file contains the common defines of the application.
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2025 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */
/* USER CODE END Header */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __MAIN_H
#define __MAIN_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "stm32f4xx_hal.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */

/* USER CODE END Includes */

/* Exported types ------------------------------------------------------------*/
/* USER CODE BEGIN ET */

/* USER CODE END ET */

/* Exported constants --------------------------------------------------------*/
/* USER CODE BEGIN EC */

/* USER CODE END EC */

/* Exported macro ------------------------------------------------------------*/
/* USER CODE BEGIN EM */

/* USER CODE END EM */

/* Exported functions prototypes ---------------------------------------------*/
void Error_Handler(void);

/* USER CODE BEGIN EFP */

/* USER CODE END EFP */

/* Private defines -----------------------------------------------------------*/
#define KEY_Pin GPIO_PIN_13
#define KEY_GPIO_Port GPIOC
#define WS2812_Pin GPIO_PIN_1
#define WS2812_GPIO_Port GPIOA
#define DEBUGA_Pin GPIO_PIN_11
#define DEBUGA_GPIO_Port GPIOE
#define DEBUGB_Pin GPIO_PIN_12
#define DEBUGB_GPIO_Port GPIOE
#define OLED_I2C2SCL_Pin GPIO_PIN_10
#define OLED_I2C2SCL_GPIO_Port GPIOB
#define OLED_I2C2SDA_Pin GPIO_PIN_11
#define OLED_I2C2SDA_GPIO_Port GPIOB
#define NRF2_SCK_Pin GPIO_PIN_13
#define NRF2_SCK_GPIO_Port GPIOB
#define NRF2_MISO_Pin GPIO_PIN_14
#define NRF2_MISO_GPIO_Port GPIOB
#define NRF2_MOSI_Pin GPIO_PIN_15
#define NRF2_MOSI_GPIO_Port GPIOB
#define NRF2_CE_Pin GPIO_PIN_10
#define NRF2_CE_GPIO_Port GPIOD
#define NRF2_CSN_Pin GPIO_PIN_11
#define NRF2_CSN_GPIO_Port GPIOD
#define NRF2_IRQ_Pin GPIO_PIN_12
#define NRF2_IRQ_GPIO_Port GPIOD
#define NRF2_IRQ_EXTI_IRQn EXTI15_10_IRQn
#define PWDN_Pin GPIO_PIN_13
#define PWDN_GPIO_Port GPIOD
#define NRF2_EN_Pin GPIO_PIN_14
#define NRF2_EN_GPIO_Port GPIOD
#define NOT_CTRL_Pin GPIO_PIN_8
#define NOT_CTRL_GPIO_Port GPIOA
#define NRF1_EN_Pin GPIO_PIN_4
#define NRF1_EN_GPIO_Port GPIOD
#define NRF1_CE_Pin GPIO_PIN_5
#define NRF1_CE_GPIO_Port GPIOD
#define NRF1_CSN_Pin GPIO_PIN_6
#define NRF1_CSN_GPIO_Port GPIOD
#define NRF1_IRQ_Pin GPIO_PIN_7
#define NRF1_IRQ_GPIO_Port GPIOD
#define NRF1_IRQ_EXTI_IRQn EXTI9_5_IRQn
#define NRF1_SCK_Pin GPIO_PIN_3
#define NRF1_SCK_GPIO_Port GPIOB
#define NRF1_MISO_Pin GPIO_PIN_4
#define NRF1_MISO_GPIO_Port GPIOB
#define NRF1_MOSI_Pin GPIO_PIN_5
#define NRF1_MOSI_GPIO_Port GPIOB
#define SCCB_I2C1SCL_Pin GPIO_PIN_8
#define SCCB_I2C1SCL_GPIO_Port GPIOB
#define SCCB_I2C1SDA_Pin GPIO_PIN_9
#define SCCB_I2C1SDA_GPIO_Port GPIOB

/* USER CODE BEGIN Private defines */
#define NRF_IRQ_GPIO_Port NRF2_IRQ_GPIO_Port
#define NRF_IRQ_Pin	NRF2_IRQ_Pin

#define NRF_CE_Pin NRF1_CE_Pin
#define NRF_CE_GPIO_Port NRF1_CE_GPIO_Port

#define KEY_PRESSED   (HAL_GPIO_ReadPin(KEY_GPIO_Port, KEY_Pin) == GPIO_PIN_SET)

#define GLOBAL_DEBUG_LOG_EN		1

#include <stdbool.h>
/* USER CODE END Private defines */

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */
