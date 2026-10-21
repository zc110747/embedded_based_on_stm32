/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file    app_threadx.c
  * @author  MCD Application Team
  * @brief   ThreadX applicative file
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2021 STMicroelectronics.
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
#include "app_threadx.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "main.h"
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */
#define MS_TO_TICK(ms)  ((TX_TIMER_TICKS_PER_SECOND * (ms)) / 1000U)
/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
#define TX_APP_STACK_SIZE                       512
#define TX_APP_THREAD_PRIO                      10
#define TX_APP_THREAD_PREEMPTION_THRESHOLD      TX_APP_THREAD_PRIO
#define TX_APP_THREAD_TIME_SLICE                TX_NO_TIME_SLICE
#define TX_APP_THREAD_AUTO_START                TX_AUTO_START
/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
/* USER CODE BEGIN PV */

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
/* USER CODE BEGIN PFP */
TX_THREAD tx_app_thread;
static UINT app_threadx_init(VOID *memory_ptr);
static void tx_app_thread_idle(ULONG thread_input);
/* USER CODE END PFP */

/**
  * @brief  Application ThreadX Initialization.
  * @param memory_ptr: memory pointer
  * @retval int
  */
UINT App_ThreadX_Init(VOID *memory_ptr)
{
  UINT ret = TX_SUCCESS;
  TX_BYTE_POOL *byte_pool = (TX_BYTE_POOL*)memory_ptr;

  /* USER CODE BEGIN App_ThreadX_Init */
    (void)byte_pool;

    // threadx init
    app_threadx_init(memory_ptr);
  /* USER CODE END App_ThreadX_Init */

  return ret;
}

/**
  * @brief  MX_ThreadX_Init
  * @param  None
  * @retval None
  */
void MX_ThreadX_Init(void)
{
  /* USER CODE BEGIN  Before_Kernel_Start */

  /* USER CODE END  Before_Kernel_Start */

  tx_kernel_enter();

  /* USER CODE BEGIN  Kernel_Start_Error */

  /* USER CODE END  Kernel_Start_Error */
}

/* USER CODE BEGIN 1 */
#include "application.h"

static UINT app_threadx_init(VOID *memory_ptr)
{
    UINT ret = TX_SUCCESS;
    TX_BYTE_POOL *byte_pool = (TX_BYTE_POOL*)memory_ptr;

    CHAR *pointer;

    if (tx_byte_allocate(byte_pool, 
                       (VOID**) &pointer,
                       TX_APP_STACK_SIZE, TX_NO_WAIT) != TX_SUCCESS)
    {
        return TX_POOL_ERROR;
    }

    if (tx_thread_create(&tx_app_thread, 
                        "tx app thread", tx_app_thread_idle, 0, pointer,
                        TX_APP_STACK_SIZE, TX_APP_THREAD_PRIO, 
                        TX_APP_THREAD_PREEMPTION_THRESHOLD,
                        TX_APP_THREAD_TIME_SLICE, 
                        TX_APP_THREAD_AUTO_START) != TX_SUCCESS)
    {
        return TX_THREAD_ERROR;
    }

    if (application_init(memory_ptr) != RT_OK)
    {
        return TX_THREAD_ERROR;
    }        
    return ret;
}

void tx_app_thread_idle(ULONG thread_input)
{
    while(1)
    {
        LED_ON;

        tx_thread_sleep(1000);

        LED_OFF;

        tx_thread_sleep(1000);
    }
}

static TX_MUTEX logger_tx_mutex = {0};

LOG_STATUS logger_protect_init(void)
{
    LOG_STATUS ret = LOG_STATUS_OK;
    
    if (tx_mutex_create(&logger_tx_mutex, "logger tx mutex", TX_NO_INHERIT) != TX_SUCCESS)
    {
        ret = LOG_STATUS_ERROR;
    }
    
    return ret;
}

LOG_STATUS logger_protect_entry(void)
{
    LOG_STATUS ret = LOG_STATUS_OK;

    if(tx_mutex_get(&logger_tx_mutex, MS_TO_TICK(100)) != TX_SUCCESS)
    {
        ret =  LOG_STATUS_ERROR;
    }
    
    return ret;
}

LOG_STATUS logger_protect_exit(void)
{
    LOG_STATUS ret = LOG_STATUS_OK;

    tx_mutex_put(&logger_tx_mutex);
    
    return ret;
}
/* USER CODE END 1 */
