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
static GlobalType_t low_power_entry(void);
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

static GlobalType_t low_power_hw_process(void)
{     
    GPIO_InitTypeDef GPIO_InitStruct = {0};
        
    //模块时钟复位
    __HAL_RCC_GPIOA_FORCE_RESET();
    __HAL_RCC_GPIOB_FORCE_RESET();
    __HAL_RCC_GPIOC_FORCE_RESET();
    __HAL_RCC_GPIOD_FORCE_RESET();
    __HAL_RCC_GPIOE_FORCE_RESET();
    //...
    __HAL_RCC_USART1_FORCE_RESET();
    __HAL_RCC_I2C1_FORCE_RESET();
    
    //模块释放复位
    __HAL_RCC_GPIOA_RELEASE_RESET();
    __HAL_RCC_GPIOB_RELEASE_RESET();
    __HAL_RCC_GPIOC_RELEASE_RESET();
    __HAL_RCC_GPIOD_RELEASE_RESET();
    __HAL_RCC_GPIOE_RELEASE_RESET();
    //...
    __HAL_RCC_USART1_RELEASE_RESET();
    __HAL_RCC_I2C1_RELEASE_RESET();
    
    //除使用中的模块, 其它模块时钟关闭
    __HAL_RCC_GPIOA_CLK_DISABLE();
    __HAL_RCC_GPIOC_CLK_DISABLE();
    __HAL_RCC_GPIOD_CLK_DISABLE();
    __HAL_RCC_GPIOE_CLK_DISABLE();
    //...
    __HAL_RCC_USART1_CLK_DISABLE();
    __HAL_RCC_I2C1_CLK_DISABLE();

    //重新初始化必要的I/O
    /*GPIO Initialize as input*/
    GPIO_InitStruct.Pin = GPIO_PIN_1;
    GPIO_InitStruct.Mode = GPIO_MODE_INPUT;

    //config as default to avoid error trigger
    GPIO_InitStruct.Pull = GPIO_PULLUP; 
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_MEDIUM;
    HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);
    
    return RT_OK;
}

static GlobalType_t low_power_entry(void)
{
    RCC_OscInitTypeDef RCC_OscInitStruct = {0};
    RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

    low_power_hw_process();
    
    //将时钟切换到HSE
    RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                            |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
    RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_HSE;
    RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
    RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV1;
    RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

    if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_5) != HAL_OK)
    {
        return RT_FAIL;
    }
  
    //关闭LSE和PLL
    RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_LSI|RCC_OSCILLATORTYPE_HSE
                                    |RCC_OSCILLATORTYPE_LSE;
    RCC_OscInitStruct.HSEState = RCC_HSE_ON;    
    RCC_OscInitStruct.LSEState = RCC_LSE_OFF;
    RCC_OscInitStruct.LSIState = RCC_LSI_ON;
    RCC_OscInitStruct.PLL.PLLState = RCC_PLL_OFF;
    if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
    {
        return RT_FAIL;
    }
    
    HAL_PWREx_DisableOverDrive();
    return RT_OK;
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
