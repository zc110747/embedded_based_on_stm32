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
#pragma once

#include "stm32f4xx_hal.h"

namespace stm32f4
{
    namespace gpio {
        enum class GPIO_PORT {
            PA = 0,
            PB,
            PC,
            PD,
            PE,
            PF,
            PG,
            PH,
        };            
        
        struct pins_alternate_info {
            gpio::GPIO_PORT port;
            uint32_t pin;
            uint32_t alternate;
        };
     
        template<GPIO_PORT port>
        constexpr GPIO_TypeDef* GPIO_PORT_TO_ADDR(void) {
            if constexpr (port == GPIO_PORT::PA) {
                return GPIOA;
            } else if constexpr (port == GPIO_PORT::PB) {
                return GPIOB;
            } else if constexpr (port == GPIO_PORT::PC) {
                return GPIOC;
            } else if constexpr (port == GPIO_PORT::PD) {
                return GPIOD;
            } else if constexpr (port == GPIO_PORT::PE) {
                return GPIOE;
            } else if constexpr (port == GPIO_PORT::PF) {
                return GPIOF;
            } else if constexpr (port == GPIO_PORT::PG) {
                return GPIOG;
            } else if constexpr (port == GPIO_PORT::PH) {
                return GPIOH;
            } else {
                return nullptr;
            }
        }
        
        template<GPIO_PORT port>
        void clock_enable(void)
        {
            if constexpr (port == GPIO_PORT::PA) {
               __HAL_RCC_GPIOA_CLK_ENABLE();
            } else if constexpr (port == GPIO_PORT::PB) {
               __HAL_RCC_GPIOB_CLK_ENABLE();
            } else if constexpr (port == GPIO_PORT::PC) {
               __HAL_RCC_GPIOC_CLK_ENABLE();
            } else if constexpr (port == GPIO_PORT::PD) {
               __HAL_RCC_GPIOD_CLK_ENABLE();
            } else if constexpr (port == GPIO_PORT::PE) {
               __HAL_RCC_GPIOE_CLK_ENABLE();
            } else if constexpr (port == GPIO_PORT::PF) {
               __HAL_RCC_GPIOF_CLK_ENABLE();
            } else if constexpr (port == GPIO_PORT::PG) {
               __HAL_RCC_GPIOG_CLK_ENABLE();
            } else if constexpr (port == GPIO_PORT::PH) {
               __HAL_RCC_GPIOH_CLK_ENABLE();
            } else {
                // process nothing
            }
        }
                
        template <GPIO_PORT port, int pin>
        class device_gpio {
            static_assert(pin >= GPIO_PIN_0 && pin <= GPIO_PIN_15, "pin number not support");
            protected:
                GPIO_TypeDef* port_{GPIO_PORT_TO_ADDR<port>()};            
                int pin_num_{pin};
               
            public:
                int config(uint32_t mode, uint32_t pull = GPIO_NOPULL, uint32_t speed = GPIO_SPEED_FREQ_MEDIUM, uint32_t alternate = 0)
                {
                    GPIO_InitTypeDef GPIO_InitStruct = {0};
                    
                    clock_enable<port>();
                    
                    GPIO_InitStruct.Mode = mode;
                    GPIO_InitStruct.Pull = pull;
                    GPIO_InitStruct.Speed = speed;
                    GPIO_InitStruct.Pin = pin_num_;
                    GPIO_InitStruct.Alternate = alternate;
                    HAL_GPIO_Init(port_, &GPIO_InitStruct);
                    
                    return HAL_OK;
                }
        };
        
        template <GPIO_PORT port, int pin>
        class DEV_GPIO_OUTPUT : public device_gpio<port, pin> {
            public:
                void high(void) {
                    HAL_GPIO_WritePin(this->port_, this->pin_num_, GPIO_PIN_SET);
                }
                
                void low(void) {
                    HAL_GPIO_WritePin(this->port_, this->pin_num_, GPIO_PIN_RESET);
                }
                
                void toggle(void) {
                    HAL_GPIO_TogglePin(this->port_, this->pin_num_);
                }
        };

        template <GPIO_PORT port, int pin>
        class DEV_GPIO_INPUT : public device_gpio<port, pin> {
            public:
                int data(void)
                {
                    return HAL_GPIO_ReadPin(this->port_, this->pin_num_) == GPIO_PIN_SET?1:0;
                }
        };
        
        template<int pin>
        constexpr int GPIO_PIN_TO_IRQn(void)
        {
            if constexpr (pin == GPIO_PIN_0) {
                return EXTI0_IRQn;
            } else if constexpr (pin == GPIO_PIN_1) {
                return EXTI1_IRQn;
            } else if constexpr (pin == GPIO_PIN_2) {
                return EXTI2_IRQn;
            } else if constexpr (pin == GPIO_PIN_3) {
                return EXTI3_IRQn;
            } else if constexpr (pin == GPIO_PIN_4) {
                return EXTI4_IRQn;
            } else if constexpr (pin >= GPIO_PIN_5 && pin <= GPIO_PIN_9) {
                return EXTI9_5_IRQn;
            } else if constexpr (pin >= GPIO_PIN_10 && pin <= GPIO_PIN_15) {
                return EXTI15_10_IRQn;
            }      
        }
        
        template <GPIO_PORT port, int pin>
        class DEV_GPIO_EXIT : public DEV_GPIO_INPUT<port, pin> {
            public:
                void config_irq(int pre_priority = 0, int sub_priority = 0)
                {
                    IRQn_Type IRQn = GPIO_PIN_TO_IRQn<pin>();
                    
                    HAL_NVIC_SetPriority(IRQn, pre_priority, sub_priority);
                    HAL_NVIC_EnableIRQ(IRQn);
                }
        };
    }
}
