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
//      @公众号：<嵌入式技术总结>
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

static DEV_GPIO_OUTPUT<GPIO_PORT::PB, GPIO_PIN_0> led;
static DEV_GPIO_INPUT<GPIO_PORT::PH, GPIO_PIN_3> key;
static device_ap3216<I2C_MODE::I2C2_PH4_PH5> ap3216c;
static dac_deivce<DAC_CHANNEL_1> dac1;

RT_CXX_TYPE driver_init(void)
{
    RT_CXX_TYPE type;
    
    type = led.init(GPIO_MODE_OUTPUT_PP);
    if (type != RT_CXX_TYPE::RT_OK) {
        return type;
    }
    
    type = key.init(GPIO_MODE_INPUT);
    if (type != RT_CXX_TYPE::RT_OK) {
        return type;
    }
    
    type = ap3216c.init(100000, I2C_DUTYCYCLE_2, 0x03);
    if (type != RT_CXX_TYPE::RT_OK) {
        return type;
    }

    type = dac1.init();
    if (type != RT_CXX_TYPE::RT_OK) {
        return type;
    }
    return type;
}

int main(void)
{   
    uint16_t voltage = 500;
    uint8_t increase = 1;
    
    HAL_Init();
   
    logger_module_init();
    
    driver_init();
    
    auto ap3216_info_ptr = ap3216c.get_info();

    while(1)
    {   
        if (ap3216c.read_block() == RT_CXX_TYPE::RT_OK) {     
            PRINT_LOG(LOG_INFO, HAL_GetTick(), "info:%d, %d, %d, key:%d", 
                ap3216_info_ptr->als,
                ap3216_info_ptr->ir,
                ap3216_info_ptr->ps,
                key.data());
        } else {
            PRINT_LOG(LOG_INFO, HAL_GetTick(), "i2c read failed!");
        }
   
        if(increase == 1) {
            voltage += DAC_PERIOD;
            if (voltage > DAC_REFERENCE_VOL)
            {
                increase = 0;
                voltage = DAC_REFERENCE_VOL;
            }
        } else {
            voltage -= DAC_PERIOD;
            if(voltage <= DAC_PERIOD)
            {
                increase = 1;
                voltage = DAC_PERIOD;
            }
        }
        dac1.set_value(voltage);
        
        led.toggle();
        HAL_Delay(500);
    }
}
