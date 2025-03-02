//////////////////////////////////////////////////////////////////////////////
//  (c) copyright 2023-by Persional Inc.  
//  All Rights Reserved
//
//  Name:
//      dac.hpp
//
//  Purpose:
//      dac interface process.
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
    namespace dac {
        //reference voltage, uint:mv
        #define DAC_REFERENCE_VOL           3300

        //dac max output value
        #define DAC_MAX_VALUE               4095

        struct dac_info {
            gpio::GPIO_PORT port_;
            uint32_t pin_;
        };
        
        template <uint32_t ch>
        constexpr dac_info ch_to_dac_info(void)
        {
            if constexpr (ch == DAC_CHANNEL_1) {
                 return {gpio::GPIO_PORT::PA, GPIO_PIN_4};
            } else {
                 return {gpio::GPIO_PORT::PA, GPIO_PIN_5};
            }
        }
           
        template<uint32_t ch> 
        class dac_deivce {
            static_assert(ch == DAC_CHANNEL_1 || ch == DAC_CHANNEL_2, "dac channel error!");
            public:
                int config(uint32_t trigger=DAC_TRIGGER_NONE, uint32_t output_buffer=DAC_OUTPUTBUFFER_ENABLE) {
                    
                    DAC_ChannelConfTypeDef sConfig = {0};
                    constexpr auto info = ch_to_dac_info<ch>();
                    
                    // int dac gpio
                    gpio::device_gpio<info.port_, info.pin_> gpio_dev;
                    gpio_dev.config(GPIO_MODE_ANALOG);
                    
                    //init dac clock
                    __HAL_RCC_DAC_CLK_ENABLE();

                    //config dac
                    hdac_.Instance = DAC;
                    if (HAL_DAC_Init(&hdac_) != HAL_OK) {
                        return HAL_ERROR;                
                    }

                    sConfig.DAC_Trigger = trigger;
                    sConfig.DAC_OutputBuffer = output_buffer;
                    if (HAL_DAC_ConfigChannel(&hdac_, &sConfig, ch) != HAL_OK) {
                       return HAL_ERROR; 
                    }

                    return HAL_OK;
                }

                void update_dac_value(uint32_t value, uint32_t mode = DAC_ALIGN_12B_R) {
                    float adc_value;
    
                    if (value > DAC_REFERENCE_VOL)
                        value = DAC_REFERENCE_VOL;
                
                    adc_value = (float)value/DAC_REFERENCE_VOL * DAC_MAX_VALUE;
                
                    HAL_DAC_Stop(&hdac_, ch);
                    HAL_DAC_SetValue(&hdac_, ch, mode, (uint32_t)adc_value);
                    HAL_DAC_Start(&hdac_, ch);                   
                }
            private:
                DAC_HandleTypeDef hdac_ = {0};
        };
    }
}