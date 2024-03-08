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
#ifndef _GPIO_H
#define _GPIO_H

#include "clock.hpp"

class device_gpio
{
public:
    device_gpio(){
    }
    ~device_gpio(){
    }
    
    void set_dev(GPIO_TypeDef *Port, uint16_t Pin);
    void set_mode(uint32_t Mode, uint32_t Pull, uint32_t Speed, uint32_t Alternate = 0);
    void init();
    void high();
    void lower();
    void toggle();

private:
    void clock_init();

    GPIO_TypeDef *port_;
    
    uint16_t pin_;
        
    uint32_t mode_;

    uint32_t pull_;
    
    uint32_t speed_;
    
    uint32_t alternate_;

    clock_dev clk_dev_;
};

void device_gpio::set_dev(GPIO_TypeDef *Port, uint16_t Pin)
{
    port_ = Port;  
    pin_ = Pin;
}

void device_gpio::set_mode(uint32_t Mode, uint32_t Pull, uint32_t Speed, uint32_t Alternate)
{
    mode_ = Mode;
    pull_ = Pull;
    speed_ = Speed;
    alternate_ = Alternate;
}

void device_gpio::init()
{
    GPIO_InitTypeDef GPIO_InitStruct = {0};
        
    clk_dev_.enable_gpio_clk(port_);
    
    GPIO_InitStruct.Mode = mode_;
    GPIO_InitStruct.Pull = pull_;
    GPIO_InitStruct.Speed = speed_;
    GPIO_InitStruct.Pin = pin_;
    GPIO_InitStruct.Alternate = alternate_;
    HAL_GPIO_Init(port_, &GPIO_InitStruct); 
}

void device_gpio::high()
{
    HAL_GPIO_WritePin(port_, pin_, GPIO_PIN_SET);
}

void device_gpio::lower()
{
    HAL_GPIO_WritePin(port_, pin_, GPIO_PIN_RESET);
}

void device_gpio::toggle()
{
    HAL_GPIO_TogglePin(port_, pin_);
}
#endif

