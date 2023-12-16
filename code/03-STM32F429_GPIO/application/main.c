//////////////////////////////////////////////////////////////////////////////
//  (c) copyright 2023-by Persional Inc.  
//  All Rights Reserved
//
//  Name:
//      main.c
//			
//  Purpose:
//			State: press key0, LED ON, not press, LED OFF.
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
#include "drv_global.h"
#include <string.h>

static GlobalType_t driver_initialize(void);
static GlobalType_t system_clock_init(void);

//main entery function
int main(void)
{  
    uint8_t key_io = KEY0_READ_PIN();
    uint32_t key_tick = 0;
    
    HAL_Init();
    
    //system clock tick init.
    system_clock_init();
    
    //driver initialize.
    driver_initialize();
    
    while(1)
    {
        //run task every 100ms
        if(drv_tick_difference(key_tick, HAL_GetTick()) > 100)
        {
            key_tick = HAL_GetTick();
                 
            key_io = io_anti_shake(get_key0_ansh_tick(), key_io, KEY0_READ_PIN());        
            if(key_io == 1)
            {
                LED_ON;
            }
            else
            {
                LED_OFF;
            }
        }
    }
}

static GlobalType_t driver_initialize(void)
{
    GlobalType_t xReturn;
    
    xReturn = gpio_driver_init();
    
    return xReturn;
}

//system clock config
static GlobalType_t system_clock_init(void)
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
    //PLLQCLK for I2S = 90M
    RCC_OscInitStruct.PLL.PLLM = 25;
    RCC_OscInitStruct.PLL.PLLN = 360;
    RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV2;
    RCC_OscInitStruct.PLL.PLLQ = 4;
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
