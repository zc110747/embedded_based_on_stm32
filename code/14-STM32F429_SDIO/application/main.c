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
#include "drv_global.h"
#include <stdio.h>
#include "ff.h"

#define DEBUG_JTAG          0
#define DEBUG_STLINK        1
#define DEBUG_INTEFACE      DEBUG_STLINK

static GlobalType_t driver_initialize(void);
static GlobalType_t system_clock_init(void);

BYTE work[FF_MAX_SS];

void fatfs_app(void)
{
    FATFS fs;
    FIL fil;
    UINT bw;  
    FRESULT res;
    
    res = f_mount(&fs, "1:", 1);
    if(res == FR_OK)
    {
       goto __mount;
    }
    else
    {
        res = f_mkfs("1:", 0, work, FF_MAX_SS);
        if(res == FR_OK)
        {
            res = f_mount(&fs, "1:", 1);

            if(res == FR_OK)
            {
 __mount:
                res = f_open(&fil, "1:hello.txt", FA_READ | FA_WRITE);
                if(res != FR_OK)
                {
                   PRINT_LOG(LOG_DEBUG, HAL_GetTick(), "f_mount open failed!");
                }
                else
                {
                    f_read(&fil, work, 64, &bw);
                    if(bw != 0)
                    {
                        work[bw] = 0;             
                        PRINT_LOG(LOG_DEBUG, HAL_GetTick(), "%s", work); 
                    }
                    f_close(&fil);
                }
                f_mount(0, "1:", 0);
            }
            else
            {
               PRINT_LOG(LOG_DEBUG, HAL_GetTick(), "f_mount failed:%d", res); 
            }
        }
        else
        {
            PRINT_LOG(LOG_DEBUG, HAL_GetTick(),  "f_mkfs failed:%d", res); 
        }
    }
}

//main entery function
int main(void)
{  
    uint32_t tick = HAL_GetTick();
    HAL_Init();
    
    //system clock tick init.
    system_clock_init();
    
    //logger module init
    logger_module_init();
	
    //driver initialize.
    driver_initialize();
    
    //fatfs run test.
    fatfs_app();
    
    while (1)
    {
        if(drv_tick_difference(tick, HAL_GetTick()) > 1000)
        {
            tick = HAL_GetTick();
            
            LED_TOGGLE;
        }      
    }
}

static GlobalType_t driver_initialize(void)
{
    GlobalType_t xReturn;
    
    xReturn = gpio_driver_init();
    
    //sdcard init
    xReturn |= sdcard_driver_init();
    
    if (xReturn == RT_OK)
    {
        PRINT_LOG(LOG_INFO, HAL_GetTick(), "device driver init success!");
    }                                                                                                                             

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
    //PLLQCLK for I2S = 45M
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
