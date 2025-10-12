//////////////////////////////////////////////////////////////////////////////
//  (c) copyright 2023-by Persional Inc.  
//  All Rights Reserved
//
//  Name:
//      drv_i2c.c
//
//  Purpose:
//      1.driver for i2c internal modules£¬ software and hardware.
//      
//
// Author:
//      @zc
//
//  Assumptions:
//
//  Revision History:
//
/////////////////////////////////////////////////////////////////////////////
#include "drv_i2c.h"
#include "drv_soft_i2c.h"

static I2C_HandleTypeDef hi2c1 = {0};
static I2C_HandleTypeDef hi2c2 = {0};
static I2C_HandleTypeDef hi2c3 = {0};

I2C_HandleTypeDef *get_i2c1_handle(void)
{
    return &hi2c1; 
}

I2C_HandleTypeDef *get_i2c2_handle(void)
{
    return &hi2c2; 
}

I2C_HandleTypeDef *get_i2c3_handle(void)
{
    return &hi2c3; 
}

// global function
GlobalType_t drv_i2c_init(void)
{
#if I2C_RUN_MODE == I2C_USE_HARDWARE
    if (drv_i2c2_init(0) != RT_OK) {
        return RT_FAIL;
    }
#else
    if (i2c2_soft_driver_init() != RT_OK) {
        return RT_FAIL;
    }
#endif
    
#if DRIVER_DS3213_ENABLE == 1 || DRIVER_PCF8563_ENABLE == 1
    if (i2c4_soft_driver_init() != RT_OK) {
        return RT_FAIL;
    }
#endif
    
    return RT_OK;
}

GlobalType_t i2c4_soft_driver_init(void)
{
    SOFT_I2C_INFO I2C_Info = {0};
    
    //clock need enable before software i2c Init
    __HAL_RCC_GPIOB_CLK_ENABLE(); 

    I2C_Info.scl_pin = GPIO_PIN_4;
    I2C_Info.scl_port = GPIOB;
    I2C_Info.sda_pin = GPIO_PIN_3;
    I2C_Info.sda_port = GPIOB;
    
    if(i2c_soft_init(SOFT_I2C4, &I2C_Info) != I2C_OK)
    {
        return RT_FAIL;
    }
    
    return RT_OK;
}

GlobalType_t i2c2_soft_driver_init(void)
{
    SOFT_I2C_INFO I2C_Info = {0};

    //clock need enable before software i2c Init
    __HAL_RCC_GPIOH_CLK_ENABLE(); 

    I2C_Info.scl_pin = GPIO_PIN_4;
    I2C_Info.scl_port = GPIOH;
    I2C_Info.sda_pin = GPIO_PIN_5;
    I2C_Info.sda_port = GPIOH;
    
    if(i2c_soft_init(SOFT_I2C2, &I2C_Info) != I2C_OK)
    {
        return RT_FAIL;
    }
    
    return RT_OK;
}

GlobalType_t drv_i2c2_init(int must_init)
{
    GPIO_InitTypeDef GPIO_InitStruct = {0};

    if (must_init == 1) {
        hi2c1.State = HAL_I2C_STATE_RESET; 
    }
    
    if (hi2c1.State == HAL_I2C_STATE_RESET) 
    {
        __HAL_RCC_I2C2_CLK_ENABLE();
        __HAL_RCC_GPIOH_CLK_ENABLE();

        __HAL_RCC_I2C2_FORCE_RESET();
        HAL_Delay(1);
        __HAL_RCC_I2C2_RELEASE_RESET();   
        HAL_Delay(1);
        
        GPIO_InitStruct.Pin = GPIO_PIN_4|GPIO_PIN_5;
        GPIO_InitStruct.Mode = GPIO_MODE_AF_OD;
        GPIO_InitStruct.Pull = GPIO_NOPULL;
        GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
        GPIO_InitStruct.Alternate = GPIO_AF4_I2C2;
        HAL_GPIO_Init(GPIOH, &GPIO_InitStruct);

        hi2c2.Instance = I2C2;
        hi2c2.Init.ClockSpeed = 100000;
        hi2c2.Init.DutyCycle = I2C_DUTYCYCLE_2;
        hi2c2.Init.OwnAddress1 = 0;
        hi2c2.Init.AddressingMode = I2C_ADDRESSINGMODE_7BIT;
        hi2c2.Init.DualAddressMode = I2C_DUALADDRESS_DISABLE;
        hi2c2.Init.OwnAddress2 = 0;
        hi2c2.Init.GeneralCallMode = I2C_GENERALCALL_DISABLE;
        hi2c2.Init.NoStretchMode = I2C_NOSTRETCH_DISABLE;
        if (HAL_I2C_Init(&hi2c2) != HAL_OK) {
            return RT_FAIL;
        }

        if (HAL_I2CEx_ConfigAnalogFilter(&hi2c2, I2C_ANALOGFILTER_ENABLE) != HAL_OK) {
            return RT_FAIL;
        }

        if (HAL_I2CEx_ConfigDigitalFilter(&hi2c2, 0) != HAL_OK) {
            return RT_FAIL;
        }
    }
    
    return RT_OK;
}

