//////////////////////////////////////////////////////////////////////////////
//  (c) copyright 2023-by Persional Inc.  
//  All Rights Reserved
//
//  Name:
//      gpio.hpp
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
#ifndef _CLOCK_H
#define _CLOCK_H

#include "includes.h"

class clock_dev
{
public:
    clock_dev(){
    }
    ~clock_dev(){
    }

#ifdef HAL_GPIO_MODULE_ENABLED
    void enable_gpio_clk(GPIO_TypeDef *port_);
#endif

#ifdef HAL_I2C_MODULE_ENABLED
    void enable_i2c_clk(I2C_TypeDef *port_);
#endif
};

#ifdef HAL_GPIO_MODULE_ENABLED
void clock_dev::enable_gpio_clk(GPIO_TypeDef *port_)
{
    if(port_ == GPIOA)
    {
        __HAL_RCC_GPIOA_CLK_ENABLE();
    }
    else if(port_ == GPIOB)
    {
        __HAL_RCC_GPIOB_CLK_ENABLE();        
    }
    else if(port_ == GPIOC)
    {
        __HAL_RCC_GPIOC_CLK_ENABLE();        
    }
    else if(port_ == GPIOD)
    {
        __HAL_RCC_GPIOD_CLK_ENABLE();        
    }
    else if(port_ == GPIOE)
    {
        __HAL_RCC_GPIOE_CLK_ENABLE();        
    }
    else if(port_ == GPIOF)
    {
        __HAL_RCC_GPIOF_CLK_ENABLE();        
    }
    else if(port_ == GPIOG)
    {
        __HAL_RCC_GPIOG_CLK_ENABLE();        
    }
    else if(port_ == GPIOH)
    {
        __HAL_RCC_GPIOH_CLK_ENABLE();        
    }
}
#endif

#ifdef HAL_I2C_MODULE_ENABLED
void clock_dev::enable_i2c_clk(I2C_TypeDef *port_)
{
    if(port_ == I2C1)
    {
        __HAL_RCC_I2C1_CLK_ENABLE();
    }
    else if(port_ == I2C2)
    {
        __HAL_RCC_I2C2_CLK_ENABLE();
    }
    else if(port_ == I2C3)
    {
        __HAL_RCC_I2C3_CLK_ENABLE();
    }
}
#endif

#endif

