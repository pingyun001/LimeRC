/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * File Name          : freertos.c
  * Description        : Code for freertos applications
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

/* Includes ------------------------------------------------------------------*/
#include "FreeRTOS.h"
#include "task.h"
#include "main.h"
#include "cmsis_os.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */

/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
/* USER CODE BEGIN Variables */

/* USER CODE END Variables */
osThreadId bspUIHandle;
osThreadId nrfMessageHandle;
osThreadId bspInfoHandle;
osThreadId nrfVideoHandle;
osThreadId watchDogHandle;

/* Private function prototypes -----------------------------------------------*/
/* USER CODE BEGIN FunctionPrototypes */

/* USER CODE END FunctionPrototypes */

void bspUI_main(void const * argument);
void nrfMessage_main(void const * argument);
void bspInfo_main(void const * argument);
void nrfVideo_main(void const * argument);
void watchDog_main(void const * argument);

void MX_FREERTOS_Init(void); /* (MISRA C 2004 rule 8.1) */

/* GetIdleTaskMemory prototype (linked to static allocation support) */
void vApplicationGetIdleTaskMemory( StaticTask_t **ppxIdleTaskTCBBuffer, StackType_t **ppxIdleTaskStackBuffer, uint32_t *pulIdleTaskStackSize );

/* USER CODE BEGIN GET_IDLE_TASK_MEMORY */
static StaticTask_t xIdleTaskTCBBuffer;
static StackType_t xIdleStack[configMINIMAL_STACK_SIZE];

void vApplicationGetIdleTaskMemory( StaticTask_t **ppxIdleTaskTCBBuffer, StackType_t **ppxIdleTaskStackBuffer, uint32_t *pulIdleTaskStackSize )
{
  *ppxIdleTaskTCBBuffer = &xIdleTaskTCBBuffer;
  *ppxIdleTaskStackBuffer = &xIdleStack[0];
  *pulIdleTaskStackSize = configMINIMAL_STACK_SIZE;
  /* place for user code */
}
/* USER CODE END GET_IDLE_TASK_MEMORY */

/**
  * @brief  FreeRTOS initialization
  * @param  None
  * @retval None
  */
void MX_FREERTOS_Init(void) {
  /* USER CODE BEGIN Init */

  /* USER CODE END Init */

  /* USER CODE BEGIN RTOS_MUTEX */
  /* add mutexes, ... */
  /* USER CODE END RTOS_MUTEX */

  /* USER CODE BEGIN RTOS_SEMAPHORES */
  /* add semaphores, ... */
  /* USER CODE END RTOS_SEMAPHORES */

  /* USER CODE BEGIN RTOS_TIMERS */
  /* start timers, add new ones, ... */
  /* USER CODE END RTOS_TIMERS */

  /* USER CODE BEGIN RTOS_QUEUES */
  /* add queues, ... */
  /* USER CODE END RTOS_QUEUES */

  /* Create the thread(s) */
  /* definition and creation of bspUI */
  osThreadDef(bspUI, bspUI_main, osPriorityBelowNormal, 0, 2048);
  bspUIHandle = osThreadCreate(osThread(bspUI), NULL);

  /* definition and creation of nrfMessage */
  osThreadDef(nrfMessage, nrfMessage_main, osPriorityNormal, 0, 256);
  nrfMessageHandle = osThreadCreate(osThread(nrfMessage), NULL);

  /* definition and creation of bspInfo */
  osThreadDef(bspInfo, bspInfo_main, osPriorityLow, 0, 512);
  bspInfoHandle = osThreadCreate(osThread(bspInfo), NULL);

  /* definition and creation of nrfVideo */
  osThreadDef(nrfVideo, nrfVideo_main, osPriorityHigh, 0, 256);
  nrfVideoHandle = osThreadCreate(osThread(nrfVideo), NULL);

  /* definition and creation of watchDog */
  osThreadDef(watchDog, watchDog_main, osPriorityBelowNormal, 0, 128);
  watchDogHandle = osThreadCreate(osThread(watchDog), NULL);

  /* USER CODE BEGIN RTOS_THREADS */
  /* add threads, ... */
  /* USER CODE END RTOS_THREADS */

}

/* USER CODE BEGIN Header_bspUI_main */
/**
  * @brief  Function implementing the bspUI thread.
  * @param  argument: Not used
  * @retval None
  */
/* USER CODE END Header_bspUI_main */
__weak void bspUI_main(void const * argument)
{
  /* USER CODE BEGIN bspUI_main */
  /* Infinite loop */
  for(;;)
  {
    osDelay(1);
  }
  /* USER CODE END bspUI_main */
}

/* USER CODE BEGIN Header_nrfMessage_main */
/**
* @brief Function implementing the nrfMessage thread.
* @param argument: Not used
* @retval None
*/
/* USER CODE END Header_nrfMessage_main */
__weak void nrfMessage_main(void const * argument)
{
  /* USER CODE BEGIN nrfMessage_main */
  /* Infinite loop */
  for(;;)
  {
    osDelay(1);
  }
  /* USER CODE END nrfMessage_main */
}

/* USER CODE BEGIN Header_bspInfo_main */
/**
* @brief Function implementing the bspInfo thread.
* @param argument: Not used
* @retval None
*/
/* USER CODE END Header_bspInfo_main */
__weak void bspInfo_main(void const * argument)
{
  /* USER CODE BEGIN bspInfo_main */
  /* Infinite loop */
  for(;;)
  {
    osDelay(1);
  }
  /* USER CODE END bspInfo_main */
}

/* USER CODE BEGIN Header_nrfVideo_main */
/**
* @brief Function implementing the nrfVideo thread.
* @param argument: Not used
* @retval None
*/
/* USER CODE END Header_nrfVideo_main */
__weak void nrfVideo_main(void const * argument)
{
  /* USER CODE BEGIN nrfVideo_main */
  /* Infinite loop */
  for(;;)
  {
    osDelay(1);
  }
  /* USER CODE END nrfVideo_main */
}

/* USER CODE BEGIN Header_watchDog_main */
/**
* @brief Function implementing the watchDog thread.
* @param argument: Not used
* @retval None
*/
/* USER CODE END Header_watchDog_main */
__weak void watchDog_main(void const * argument)
{
  /* USER CODE BEGIN watchDog_main */
  /* Infinite loop */
  for(;;)
  {
    osDelay(1);
  }
  /* USER CODE END watchDog_main */
}

/* Private application code --------------------------------------------------*/
/* USER CODE BEGIN Application */

/* USER CODE END Application */
