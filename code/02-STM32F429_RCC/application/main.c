//////////////////////////////////////////////////////////////////////////////
//  (c) copyright 2023-by Persional Inc.  
//  All Rights Reserved
//
//  Name:
//      main.c
//
//  Purpose:
//      ch02.rcc_system_clock    
//
//  Author:
//      @zc
//
//  Assumptions:
//	
//  Revision History:
//
/////////////////////////////////////////////////////////////////////////////
#include <string.h>
#include "includes.h"
#include "SEGGER_RTT.h"

static void ITM_RunTest(void);
static GlobalType_t SystemClock_Config(void);

//main entery function
int main(void)
{  
    HAL_Init();
    
    SystemClock_Config();
    
    while (1)
    {
        ITM_RunTest();
        
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

//system clock config
static GlobalType_t SystemClock_Config(void)
{
    RCC_OscInitTypeDef RCC_OscInitStruct = {0};
    RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

    __HAL_RCC_PWR_CLK_ENABLE();
    __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE1);
    

    RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_LSI|RCC_OSCILLATORTYPE_HSE
                                    |RCC_OSCILLATORTYPE_LSE;
    
    //RCC_HSE_BYPASS: OSC with source, one line out clock
    //RCC_HSE_ON: OSC without source, two line by microchip
    RCC_OscInitStruct.HSEState = RCC_HSE_ON;    
    RCC_OscInitStruct.LSEState = RCC_LSE_ON;
    RCC_OscInitStruct.LSIState = RCC_LSI_ON;
    RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
    RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
    
    //SYSCLK HSECLK/PLLM*PLLN/PLLP=25Mhz/25*360/2=180Mhz
    //PLLQCLK for I2S = 45M(MAX 48M)
    RCC_OscInitStruct.PLL.PLLM = 25;
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

    //AHBCLK    = SYSCLK    =180Mhz
    //APB1CLK   = AHBCLK/4  =45M 
    //APB1TIME  = APB1CLK*2 =90M 
    //APB2CLK   = AHBCLK/2  =90M
    //APB2TIME  =APB2*2     =180M   
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
