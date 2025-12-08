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
#include "stm32h7xx_hal.h"

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
#define CHR_STDBY_Pin GPIO_PIN_2
#define CHR_STDBY_GPIO_Port GPIOC
#define CHR_CHAR_Pin GPIO_PIN_3
#define CHR_CHAR_GPIO_Port GPIOC
#define KR_Pin GPIO_PIN_0
#define KR_GPIO_Port GPIOA
#define MAIN_PWR_Pin GPIO_PIN_1
#define MAIN_PWR_GPIO_Port GPIOA
#define SUB_OLED_BLK_PWM_Pin GPIO_PIN_2
#define SUB_OLED_BLK_PWM_GPIO_Port GPIOA
#define BAT_ADC_EN_Pin GPIO_PIN_6
#define BAT_ADC_EN_GPIO_Port GPIOA
#define OLED_RES_Pin GPIO_PIN_4
#define OLED_RES_GPIO_Port GPIOC
#define OLED_DC_Pin GPIO_PIN_5
#define OLED_DC_GPIO_Port GPIOC
#define NRF1_EN_Pin GPIO_PIN_9
#define NRF1_EN_GPIO_Port GPIOE
#define NRF1_CE_Pin GPIO_PIN_10
#define NRF1_CE_GPIO_Port GPIOE
#define NRF1_IRQ_Pin GPIO_PIN_15
#define NRF1_IRQ_GPIO_Port GPIOE
#define NRF1_IRQ_EXTI_IRQn EXTI15_10_IRQn
#define NRF2_IRQ_Pin GPIO_PIN_12
#define NRF2_IRQ_GPIO_Port GPIOB
#define NRF2_CSN_Pin GPIO_PIN_8
#define NRF2_CSN_GPIO_Port GPIOD
#define NRF2_CE_Pin GPIO_PIN_9
#define NRF2_CE_GPIO_Port GPIOD
#define NRF2_EN_Pin GPIO_PIN_10
#define NRF2_EN_GPIO_Port GPIOD
#define TF_DETECH_Pin GPIO_PIN_8
#define TF_DETECH_GPIO_Port GPIOA
#define OLED_BLK_Pin GPIO_PIN_5
#define OLED_BLK_GPIO_Port GPIOD
#define DEBUG_R_Pin GPIO_PIN_6
#define DEBUG_R_GPIO_Port GPIOD
#define DEBUG_L_Pin GPIO_PIN_7
#define DEBUG_L_GPIO_Port GPIOD

/* USER CODE BEGIN Private defines */
#include <stdbool.h>
extern int LEprintf(const char *format, ...);

#define IS_KR_PRESSED()					(HAL_GPIO_ReadPin(KR_GPIO_Port, KR_Pin) == GPIO_PIN_RESET)

#define IS_TF_CARD_INSERTED()		(HAL_GPIO_ReadPin(TF_DETECH_GPIO_Port, TF_DETECH_Pin) == GPIO_PIN_RESET)

#define DEBUG_L_PIN_0()		HAL_GPIO_WritePin(DEBUG_L_GPIO_Port, DEBUG_L_Pin, GPIO_PIN_RESET)
#define DEBUG_L_PIN_1()	 	HAL_GPIO_WritePin(DEBUG_L_GPIO_Port, DEBUG_L_Pin, GPIO_PIN_SET)
#define DEBUG_R_PIN_0()	 	HAL_GPIO_WritePin(DEBUG_R_GPIO_Port, DEBUG_R_Pin, GPIO_PIN_RESET)
#define DEBUG_R_PIN_1()	 	HAL_GPIO_WritePin(DEBUG_R_GPIO_Port, DEBUG_R_Pin, GPIO_PIN_SET)

/* software version */
#define GLOBAL_SOFTWARE_VERSION		"V3.1.0"

/* USER CODE END Private defines */

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */
