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
#include <stdio.h>
#include "arm_const_structs.h"
#include "drv_global.h"

#define DEBUG_JTAG          0
#define DEBUG_STLINK        1
#define DEBUG_INTEFACE      DEBUG_STLINK

static void alg_app_run(void);
static GlobalType_t driver_initialize(void);
static GlobalType_t system_clock_init(void);
static GlobalType_t dsp_app(void);

//main entery function
int main(void)
{  
    uint32_t tick;
    
    HAL_Init();
    
    //system clock tick init.
    system_clock_init();
    
    //logger module init
    logger_module_init();
	
    //driver initialize.
    driver_initialize();
    
    dsp_app();
    
    tick  = HAL_GetTick();
    
    while (1)
    {
        if(drv_tick_difference(tick, HAL_GetTick()) > 1000)
        {
            tick = HAL_GetTick();
            
            LED_TOGGLE;
            
            PRINT_LOG(LOG_INFO, HAL_GetTick(), "random:0x%x", rng_get_value());
            
            alg_app_run();
        }
    }
}

static void alg_app_run(void)
{
    uint32_t buffer[4];
    uint32_t crc_hw, crc_soft;
    
    buffer[0] = rng_get_value();
    buffer[1] = rng_get_value();
    buffer[2] = rng_get_value();
    buffer[3] = rng_get_value();
    
    crc_hw = calc_hw_crc32(buffer, 4);
    crc_soft = calc_crc32(buffer, 4);
    if(crc_hw == crc_soft)
    {
        PRINT_LOG(LOG_INFO, HAL_GetTick(), "CRC Equal, 0x%x", crc_hw);
    }
    else
    {
        PRINT_LOG(LOG_INFO, HAL_GetTick(), "CRC Not Equal, 0x%x, 0x%x", crc_hw, crc_soft);
    }
}

static GlobalType_t dsp_app(void)
{ 
    //float sqrt
    {
        float32_t in, out;
        
        in = 3.15;   
        if(arm_sqrt_f32(in, &out) == ARM_MATH_SUCCESS)
        {
            PRINT_LOG(LOG_INFO, HAL_GetTick(), "dfu sqrt:%f, %f.", in, out);
        }
    }   
    
    //float cos, sin
    {
        float32_t angle, outSin, outCos;
        
        angle = PI/4;
        outSin = arm_sin_f32(angle);
        outCos = arm_cos_f32(angle);
        PRINT_LOG(LOG_INFO, HAL_GetTick(), "dfu sin:%f, cons:%f.", outSin, outCos);
    }
    

    {
        float32_t vector[] = {1.2, 2.5, 3.5, 11.2, 23.4, 12.5};
        float32_t sum_quare = 0.0;
        float32_t mean_val = 0.0;
        float32_t var_val = 0.0;
        float32_t rms_val = 0.0;
        float32_t std_val = 0.0;
        float32_t min_val = 0.0;
        uint32_t min_index = 0;
        float32_t max_val = 0.0;
        uint32_t max_index = 0;
        
        uint16_t size = sizeof(vector)/sizeof(float32_t);
        
        //sum of square
        arm_power_f32(vector, size, &sum_quare);
        
        //mean
        arm_mean_f32(vector, size, &mean_val);
        
        //variance
        arm_var_f32(vector, size, &var_val);
        
        //root mean square
        arm_rms_f32(vector, size, &rms_val);
        
        //standard deviation
        arm_std_f32(vector, size, &std_val);        
        
        //min
        arm_min_f32(vector, size, &min_val, &min_index);

        //max
        arm_max_f32(vector, size, &max_val, &max_index);
        
        PRINT_LOG(LOG_INFO, HAL_GetTick(), "quare:%f, mean:%f, variance:%f, rms:%f.", sum_quare, mean_val, var_val, rms_val);     
        PRINT_LOG(LOG_INFO, HAL_GetTick(), "std:%f, min:%d, %f, max:%d, %f", std_val, min_index, min_val, max_index, max_val);          
    }
    
    {
        //mult float vector
        float32_t vector1[] = {1.5, 2.5, 6.2};
        float32_t vector2[] = {2.1, 3.5, 5.4};
        float32_t vector3[] = {0.0, 0.0, 0.0};
        arm_mult_f32(vector1, vector2, vector3, 3);
        PRINT_LOG(LOG_INFO, HAL_GetTick(), "vector:%f, %f, %f", vector3[0], vector3[1], vector3[2]);   
    }
    
    //fft
    {
        #define FFT_SRC_SIZE 256
        static float32_t fft_buffer[FFT_SRC_SIZE*2];
        static float32_t fft_outbuffer[FFT_SRC_SIZE*2];
        uint16_t i;
        
        for(i=0; i<FFT_SRC_SIZE; i++)
        {
            //50Hz正弦波,起始相位60°
            fft_buffer[i*2] = 1+arm_cos_f32(2*PI*50*i/FFT_SRC_SIZE+PI/3);
            fft_buffer[i*2+1] = 0;
        }
        
        //cfft变换
        arm_cfft_f32(&arm_cfft_sR_f32_len256, fft_buffer, 0, 1);
        
        //求解模值
        arm_cmplx_mag_f32(fft_buffer, fft_outbuffer, FFT_SRC_SIZE);
    }
    
    return RT_OK;
}

static GlobalType_t driver_initialize(void)
{
    GlobalType_t xReturn;
    
    xReturn = gpio_driver_init();
    
    xReturn |= alg_driver_init();
    
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
