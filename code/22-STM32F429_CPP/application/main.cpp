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
#include "i2c.hpp"
#include "ap3216.hpp"
#include "dac.hpp"

using namespace stm32f4::gpio;
using namespace stm32f4::i2c;
using namespace stm32f4::dac;

#define DAC_PERIOD          100

static DEV_GPIO_OUTPUT<GPIO_PORT::PB, GPIO_PIN_0> led_dev;
static DEV_GPIO_INPUT<GPIO_PORT::PH, GPIO_PIN_3> key_dev;
static device_ap3216<I2C_MODE::I2C2_PH4_PH5> ap3216_dev;
static dac_deivce<DAC_CHANNEL_1> dac_dev;

int main(void)
{   
    uint16_t voltage = 500;
    uint8_t increase = 1;
    
    HAL_Init();
   
    led_dev.config(GPIO_MODE_OUTPUT_PP);
    key_dev.config(GPIO_MODE_INPUT);   
    ap3216_dev.config(100000, I2C_DUTYCYCLE_2);
    ap3216_dev.config_deivce();
    AP3216C_INFO *ap3216_info_ptr = ap3216_dev.get_info();
    dac_dev.config();
    
    logger_module_init();
    
    while(1)
    {
        led_dev.toggle();
        
        ap3216_dev.update_info();
        
        PRINT_LOG(LOG_INFO, HAL_GetTick(), "info:%d, %d, %d, key:%d", 
            ap3216_info_ptr->als,
            ap3216_info_ptr->ir,
            ap3216_info_ptr->ps,
            key_dev.data());
        
        if(increase == 1)
        {
            voltage += DAC_PERIOD;
            if (voltage > DAC_REFERENCE_VOL)
            {
                increase = 0;
                voltage = DAC_REFERENCE_VOL;
            }
        }
        else
        {
            voltage -= DAC_PERIOD;
            if(voltage <= DAC_PERIOD)
            {
                increase = 1;
                voltage = DAC_PERIOD;
            }
        }
        dac_dev.update_dac_value(voltage);
        
        HAL_Delay(500);
    }
}
