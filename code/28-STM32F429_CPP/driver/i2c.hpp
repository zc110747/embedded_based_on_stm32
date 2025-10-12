//////////////////////////////////////////////////////////////////////////////
//  (c) copyright 2023-by Persional Inc.  
//  All Rights Reserved
//
//  Name:
//      i2c.hpp
//
//  Purpose:
//      i2c interface process.
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
#include "gpio.hpp"

namespace stm32f4
{
    namespace i2c {      
        enum class I2C_METHOD {
            MASTER = 0,
            SLAVE,
        };
        
        enum class I2C_MODE{
            I2C1_PB6_PB7 = 0,
            I2C1_PB8_PB9,
            I2C2_PB10_PB11,
            I2C2_PF1_PF0,
            I2C2_PH4_PH5,
            I2C3_PH7_PH8,
        };
                    
        struct i2c_information {
            gpio::pins_alternate_info scl_gpio_;
            gpio::pins_alternate_info sda_gpio_;
        };
        
        template<I2C_MODE i2c_mode>
        constexpr i2c_information convert_to_i2c_info(void)
        {
            if constexpr (i2c_mode == I2C_MODE::I2C1_PB6_PB7) {
                return  {
                            {gpio::GPIO_PORT::PB, GPIO_PIN_6, GPIO_AF4_I2C1}, 
                            {gpio::GPIO_PORT::PB, GPIO_PIN_7, GPIO_AF4_I2C1}
                        }; 
            }  else if constexpr (i2c_mode == I2C_MODE::I2C1_PB8_PB9) {
                return  {
                            {gpio::GPIO_PORT::PB, GPIO_PIN_8, GPIO_AF4_I2C1}, 
                            {gpio::GPIO_PORT::PB, GPIO_PIN_9, GPIO_AF4_I2C1}
                        };  
            } else if constexpr (i2c_mode == I2C_MODE::I2C2_PB10_PB11) {
                return  {
                            {gpio::GPIO_PORT::PB, GPIO_PIN_10, GPIO_AF4_I2C2}, 
                            {gpio::GPIO_PORT::PB, GPIO_PIN_11, GPIO_AF4_I2C2}
                        };
                
            } else if constexpr (i2c_mode == I2C_MODE::I2C2_PF1_PF0) {
                return  {   
                            {gpio::GPIO_PORT::PF, GPIO_PIN_1, GPIO_AF4_I2C2}, 
                            {gpio::GPIO_PORT::PF, GPIO_PIN_0, GPIO_AF4_I2C2}
                        };
                
            } else if constexpr (i2c_mode == I2C_MODE::I2C2_PH4_PH5) {
                return  {
                            {gpio::GPIO_PORT::PH, GPIO_PIN_4, GPIO_AF4_I2C2}, 
                            {gpio::GPIO_PORT::PH, GPIO_PIN_5, GPIO_AF4_I2C2}
                        };
            } else if constexpr (i2c_mode == I2C_MODE::I2C3_PH7_PH8) {
                return {
                            {gpio::GPIO_PORT::PH, GPIO_PIN_7, GPIO_AF4_I2C3}, 
                            {gpio::GPIO_PORT::PH, GPIO_PIN_8, GPIO_AF4_I2C3}
                       };       
            }
        }
        
        template<I2C_MODE i2c_mode>
        I2C_TypeDef *convert_to_i2c(void)
        {
            if constexpr(i2c_mode == I2C_MODE::I2C1_PB8_PB9 
                        || i2c_mode == I2C_MODE::I2C1_PB6_PB7 ) {
                return I2C1;
            } else if constexpr (i2c_mode == I2C_MODE::I2C2_PB10_PB11 
                        || i2c_mode == I2C_MODE::I2C2_PF1_PF0
                        || i2c_mode == I2C_MODE::I2C2_PH4_PH5)  {
                return I2C2;
            } else if constexpr (i2c_mode == I2C_MODE::I2C3_PH7_PH8) {
                return I2C3;
            }
        }

        // enable i2c clock
        template<I2C_MODE i2c_mode>
        void i2c_clock_enable(void)
        {
            if constexpr(i2c_mode == I2C_MODE::I2C1_PB8_PB9 
                        || i2c_mode == I2C_MODE::I2C1_PB6_PB7 ) {
                __HAL_RCC_I2C1_CLK_ENABLE();
            } else if constexpr (i2c_mode == I2C_MODE::I2C2_PB10_PB11 
                        || i2c_mode == I2C_MODE::I2C2_PF1_PF0
                        || i2c_mode == I2C_MODE::I2C2_PH4_PH5) {
                __HAL_RCC_I2C2_CLK_ENABLE();
            }else if constexpr (i2c_mode == I2C_MODE::I2C3_PH7_PH8) {
                __HAL_RCC_I2C3_CLK_ENABLE();
            }
        }

        template<I2C_MODE i2c_mode, I2C_METHOD method = I2C_METHOD::MASTER>
        class device_i2c {
            public:
                int config(uint32_t clock_speed, uint32_t dutycycle = I2C_DUTYCYCLE_2)
                {
                    constexpr auto info = convert_to_i2c_info<i2c_mode>();

                    //init i2c gpio
                    gpio::device_gpio<info.scl_gpio_.port, info.scl_gpio_.pin> scl_gpio_dev;
                    scl_gpio_dev.config(GPIO_MODE_AF_OD, GPIO_NOPULL, GPIO_SPEED_FREQ_VERY_HIGH, info.scl_gpio_.alternate);

                    gpio::device_gpio<info.sda_gpio_.port, info.sda_gpio_.pin> sda_gpio_dev;
                    sda_gpio_dev.config(GPIO_MODE_AF_OD, GPIO_NOPULL, GPIO_SPEED_FREQ_VERY_HIGH, info.sda_gpio_.alternate);
                    
                    //init i2c clock
                    i2c_clock_enable<i2c_mode>();
               
                    //init i2c modules
                    i2c_struct_.Instance = i2c_port_;
                    i2c_struct_.Init.ClockSpeed = clock_speed;
                    i2c_struct_.Init.DutyCycle = dutycycle;
                    if constexpr (method == I2C_METHOD::SLAVE) {
                        i2c_struct_.Init.AddressingMode = I2C_ADDRESSINGMODE_7BIT;
                        i2c_struct_.Init.DualAddressMode = I2C_DUALADDRESS_DISABLE;
                        i2c_struct_.Init.OwnAddress2 = 0;
                        i2c_struct_.Init.GeneralCallMode = I2C_GENERALCALL_DISABLE;
                        i2c_struct_.Init.NoStretchMode = I2C_NOSTRETCH_DISABLE;                      
                    }
                    
                    if (HAL_I2C_Init(&i2c_struct_) != HAL_OK) {
                        return HAL_ERROR;
                    }
       
                    if (HAL_I2CEx_ConfigAnalogFilter(&i2c_struct_, I2C_ANALOGFILTER_ENABLE) != HAL_OK) {
                        return HAL_ERROR;
                    }
                    
                    if (HAL_I2CEx_ConfigDigitalFilter(&i2c_struct_, 0) != HAL_OK) {
                        return HAL_ERROR;
                    }

                    is_init = true;

                    return HAL_OK;
                }
                
                int multi_write(uint16_t DevAddress, uint16_t MemAddress, uint16_t MemAddSize, uint8_t *pData, uint16_t Size, uint32_t Timeout)
                {
                   if (!is_init)
                        return HAL_ERROR;
                   
                    return HAL_I2C_Mem_Write(&i2c_struct_, DevAddress, MemAddress, MemAddSize, pData, Size, Timeout);
                }
                
                int multi_read(uint16_t DevAddress, uint16_t MemAddress, uint16_t MemAddSize, uint8_t *pData, uint16_t Size, uint32_t Timeout)
                {
                    if (!is_init)
                        return HAL_ERROR;
                    
                    return HAL_I2C_Mem_Read(&i2c_struct_, DevAddress, MemAddress, MemAddSize, pData, Size, Timeout); 
                }
                
                int signal_write(uint16_t DevAddress, uint8_t *pData, uint16_t Size, uint32_t Timeout)
                {
                    if (!is_init)
                        return HAL_ERROR;

                    return HAL_I2C_Master_Transmit(&i2c_struct_, DevAddress, pData, Size, Timeout);
                }
                
                int signal_read(uint16_t DevAddress, uint8_t *pData, uint16_t Size, uint32_t Timeout)
                {
                    if (!is_init)
                        return HAL_ERROR;
                    
                    return HAL_I2C_Master_Receive(&i2c_struct_, DevAddress, pData, Size, Timeout);
                }
            private:
                bool is_init{false};
                
                I2C_TypeDef *i2c_port_{convert_to_i2c<i2c_mode>()};
                
                I2C_HandleTypeDef i2c_struct_{0};
        };
    }
}
