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
    int init();
    
protected:
    GPIO_TypeDef *port_;
    
    uint16_t pin_;

    uint32_t mode_;

private:
    uint32_t pull_;
    
    uint32_t speed_;
    
    uint32_t alternate_;

    clock_dev clk_dev_;

    void clock_init();
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

int device_gpio::init()
{
    GPIO_InitTypeDef GPIO_InitStruct = {0};
        
    clk_dev_.enable_gpio_clk(port_);
    
    GPIO_InitStruct.Mode = mode_;
    GPIO_InitStruct.Pull = pull_;
    GPIO_InitStruct.Speed = speed_;
    GPIO_InitStruct.Pin = pin_;
    GPIO_InitStruct.Alternate = alternate_;
    HAL_GPIO_Init(port_, &GPIO_InitStruct);
    
    return HAL_OK;
}

class device_gpio_output: public device_gpio
{
public:
   device_gpio_output() {}
   ~device_gpio_output() {}
   
    void high();
    void lower();
    void toggle();   
};

void device_gpio_output::high()
{
    HAL_GPIO_WritePin(port_, pin_, GPIO_PIN_SET);
}

void device_gpio_output::lower()
{
    HAL_GPIO_WritePin(port_, pin_, GPIO_PIN_RESET);
}

void device_gpio_output::toggle()
{
    HAL_GPIO_TogglePin(port_, pin_);
}

class device_gpio_input: public device_gpio
{
public:
    device_gpio_input() {
        pre_priority_ = 0;
        sub_priority_ = 0;
    }
    ~device_gpio_input() {}

    uint8_t data()
    {
        return (HAL_GPIO_ReadPin(port_, pin_) == GPIO_PIN_SET)?1:0;
    }

    int get_irqn_by_pin(uint16_t pin)
    {
        int irqn = 0x100;
        
        switch(pin)
        {
            case GPIO_PIN_0:
                irqn = EXTI0_IRQn;
                break;
            case GPIO_PIN_1:
                irqn = EXTI1_IRQn;
                break;
            case GPIO_PIN_2:
                irqn = EXTI2_IRQn;
                break;
            case GPIO_PIN_3:
                irqn = EXTI3_IRQn;
                break;
            case GPIO_PIN_4:
                irqn = EXTI4_IRQn;
                break;
            case GPIO_PIN_5:
            case GPIO_PIN_6:
            case GPIO_PIN_7:
            case GPIO_PIN_8:
            case GPIO_PIN_9:
                irqn = EXTI9_5_IRQn;
            case GPIO_PIN_10:
            case GPIO_PIN_11:
            case GPIO_PIN_12:
            case GPIO_PIN_13:
            case GPIO_PIN_14:
            case GPIO_PIN_15:
                irqn = EXTI15_10_IRQn;                
                break;
            default:
                break;
        }
        return irqn;
    }
    
    void set_intterrupt_parity(int pre, int sub)
    {
        pre_priority_ = pre;
        sub_priority_ = sub;
    }
   
    int init()
    {
        if (device_gpio::init() != HAL_OK)
        {
            return HAL_ERROR;
        }

        if((mode_&EXTI_IT) != 0)
        {
            int irqn;
            
            irqn = get_irqn_by_pin(pin_);
            if(irqn >= 0x100)
            {
                 return HAL_ERROR;
            }
            HAL_NVIC_SetPriority((IRQn_Type)irqn, pre_priority_, sub_priority_);
            HAL_NVIC_EnableIRQ((IRQn_Type)irqn); 
        }
        return HAL_OK;  
    }
    
private:
    uint32_t pre_priority_;

    uint32_t sub_priority_;
};

#endif

