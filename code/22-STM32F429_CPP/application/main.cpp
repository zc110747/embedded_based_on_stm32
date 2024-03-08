//////////////////////////////////////////////////////////////////////////////
//  (c) copyright 2023-by Persional Inc.  
//  All Rights Reserved
//
//  Name:
//      main.cpp
//
//  Purpose:
//      usart driver interface process.
//
// Author:
//      @zc
//
//  Assumptions:
//
//  Revision History:
//
/////////////////////////////////////////////////////////////////////////////
#include "includes.h"
#include "gpio.hpp"
#include "ap3216.hpp"

static I2C_HandleTypeDef hi2c_ap3216;

int main(void)
{
    device_gpio led;
    device_ap3216 ap3216_dev;
    AP3216C_INFO *ap3216_info_ptr;
    
    HAL_Init();
    
    logger_module_init();
    
    led.set_dev(GPIOB, GPIO_PIN_0);
    led.set_mode(GPIO_MODE_OUTPUT_PP, GPIO_NOPULL, GPIO_SPEED_FREQ_LOW);
    led.init();
    
    ap3216_dev.set_gpio_scl(GPIOH, GPIO_PIN_4, GPIO_AF4_I2C2);
    ap3216_dev.set_gpio_sda(GPIOH, GPIO_PIN_5, GPIO_AF4_I2C2);
    
    hi2c_ap3216.Instance = I2C2;
    hi2c_ap3216.Init.ClockSpeed = 100000;
    hi2c_ap3216.Init.DutyCycle = I2C_DUTYCYCLE_2;
    hi2c_ap3216.Init.OwnAddress1 = 0;
    hi2c_ap3216.Init.AddressingMode = I2C_ADDRESSINGMODE_7BIT;
    hi2c_ap3216.Init.DualAddressMode = I2C_DUALADDRESS_DISABLE;
    hi2c_ap3216.Init.OwnAddress2 = 0;
    hi2c_ap3216.Init.GeneralCallMode = I2C_GENERALCALL_DISABLE;
    hi2c_ap3216.Init.NoStretchMode = I2C_NOSTRETCH_DISABLE;
    ap3216_dev.set_mode(&hi2c_ap3216);
    ap3216_dev.init();
    
    ap3216_info_ptr = ap3216_dev.get_info();
    
    while(1)
    {
        led.toggle();
        
        ap3216_dev.upadte_info();
        
        PRINT_LOG(LOG_INFO, HAL_GetTick(), "info:%d, %d, %d", 
            ap3216_info_ptr->als,
            ap3216_info_ptr->ir,
            ap3216_info_ptr->ps);
        
        HAL_Delay(500);
    }
}
