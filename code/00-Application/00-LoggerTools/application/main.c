//////////////////////////////////////////////////////////////////////////////
//  (c) copyright 2025-by Persional Inc.  
//  All Rights Reserved
//
//  Name:
//      main.c
//
//  Purpose:
//
//  Author:
//      @zc
//
//  Assumptions:
//	
//
//  Revision History:
//
/////////////////////////////////////////////////////////////////////////////
#include "includes.h"
#include "SEGGER_RTT.h"

static GlobalType_t SystemClock_Config(void);
static GlobalType_t application_init(void);

int main(void)
{
    HAL_Init();
    
    SystemClock_Config();
   
    application_init();
    
    // 设置通过串口打印
    logger_set_level(LOG_DEBUG);
    logger_set_inteface(LOG_DEVICE_USART);

    PRINT_LOG(LOG_INFO, HAL_GetTick(), "application start!");
    
    while(1)
    {
        logger_uart_rx_process();
    }
}

static GlobalType_t driver_init(void)
{
    GlobalType_t ret = RT_OK;
    
    ret = drv_sdcard_init();
    
    return ret;
}

static GlobalType_t application_init(void)
{
    GlobalType_t ret = RT_OK;
    
    if (logger_module_init() != LOG_STATUS_OK) {
        ret = RT_FAIL;
    }
    
	ret = driver_init();
	
    return ret;
}

static GlobalType_t SystemClock_Config(void)
{
    RCC_OscInitTypeDef RCC_OscInitStruct = {0};
    RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

    __HAL_RCC_PWR_CLK_ENABLE();
    __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE1);
    
    RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_LSI|RCC_OSCILLATORTYPE_HSE
                          |RCC_OSCILLATORTYPE_LSE;
    RCC_OscInitStruct.HSEState = RCC_HSE_OFF;
    RCC_OscInitStruct.HSIState = RCC_HSI_ON;
    RCC_OscInitStruct.LSEState = RCC_LSE_OFF;
    RCC_OscInitStruct.LSIState = RCC_LSI_ON;
    RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
    RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSI;
    
    RCC_OscInitStruct.PLL.PLLM = 16;
    RCC_OscInitStruct.PLL.PLLN = 360;
    RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV2;
    RCC_OscInitStruct.PLL.PLLQ = 8;
    if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
    {
        return RT_FAIL;
    }

    if (HAL_PWREx_EnableOverDrive() != HAL_OK)
    {
        return RT_FAIL;
    }

    RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                            |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
    RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
    RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
    RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV4;
    RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV2;

    if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_5) != HAL_OK)
    {
        return RT_FAIL;
    }
    return RT_OK;    
}
