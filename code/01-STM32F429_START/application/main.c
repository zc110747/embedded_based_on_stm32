//////////////////////////////////////////////////////////////////////////////
//  (c) copyright 2023-by Persional Inc.  
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
//  Revision History:
//
/////////////////////////////////////////////////////////////////////////////
#include "includes.h"
#include "SEGGER_RTT.h"
#include <string.h>

#define DEBUG_JTAG          0
#define DEBUG_STLINK        1

#define DEBUG_INTEFACE      DEBUG_STLINK

static void ITM_RunTest(void);
static void RTT_RunTest(void);
static GlobalType_t SystemClock_Config(void);

//main entery function
int main(void)
{  
    HAL_Init();
    
    SystemClock_Config();
    
    while (1)
    {
        #if DEBUG_INTEFACE == DEBUG_JTAG
        RTT_RunTest();
        #else
        ITM_RunTest();
        #endif
        
        HAL_Delay(1000);
    }
}

//with stlink can use
volatile int32_t ITM_RxBuffer = ITM_RXBUFFER_EMPTY;
static void ITM_RunTest(void)
{
    char *pbuffer = "ITM Send Test!\r\n";
    uint8_t index;
    
    for (index=0; index<strlen(pbuffer); index++)
    {
        ITM_SendChar((uint32_t)(pbuffer[index]));
    }
}

//with jlink can use
static void RTT_RunTest(void)
{
    char *pbuffer = "RTT Send Test!\r\n";
    
    SEGGER_RTT_printf(0, pbuffer);
}

//system clock config
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
