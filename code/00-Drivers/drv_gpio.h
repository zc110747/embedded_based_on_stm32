//////////////////////////////////////////////////////////////////////////////
//  (c) copyright 2023-by Persional Inc.  
//  All Rights Reserved
//
//  Name:
//      drv_gpio.h
//
//  Purpose:
//      driver for gpio module.
//      LED     -   PB0 - PIN_OUTPUT
//      KEY0    -   PH3 - PIN_INPUT
//      KEY1    -   PH2 - PIN_INPUT
//      KEY2    -   PC13 - PIN_INPUT
//
//  Author:
//      @公众号：<嵌入式技术总结>
//
//  Assumptions:
//	
//
//  Revision History:
//
/////////////////////////////////////////////////////////////////////////////
#ifndef _DRV_GPIO_H
#define _DRV_GPIO_H

#ifdef __cplusplus
extern "C" {
#endif

#include "includes.h"

// i/o anti-shake
#define ANTI_SHAKE_TICK     3

// LED Information
#define LED_CLK_ENABLE()    __HAL_RCC_GPIOB_CLK_ENABLE()
#define LED_PORT            GPIOB
#define LED_PIN             GPIO_PIN_0    

#define LED_ON              HAL_GPIO_WritePin(LED_PORT, LED_PIN, GPIO_PIN_RESET)
#define LED_OFF             HAL_GPIO_WritePin(LED_PORT, LED_PIN, GPIO_PIN_SET)
#define LED_TOGGLE          HAL_GPIO_TogglePin(LED_PORT, LED_PIN)

// Key0 Information
#define KEY0                0
#define KEY1                1
#define KEY2                2    
#define KEY_NUMS            3

    
#define KEY_GPIO_CLK_ENABLE()    do {   \
                                    __HAL_RCC_GPIOH_CLK_ENABLE();    \
                                    __HAL_RCC_GPIOC_CLK_ENABLE();    \
                                 }while(0);
#define KEY0_GPIO_PORT           GPIOH
#define KEY0_GPIO_PIN            GPIO_PIN_3
#define KEY1_GPIO_PORT           GPIOH
#define KEY1_GPIO_PIN            GPIO_PIN_2
#define KEY2_GPIO_PORT           GPIOC
#define KEY2_GPIO_PIN            GPIO_PIN_13

#define KEY_GPIO_READ_PIN(port, pin)     (HAL_GPIO_ReadPin(port, pin)==GPIO_PIN_SET?1:0)

GlobalType_t drv_gpio_init(void);
uint8_t get_key_value(uint8_t num);
uint8_t io_anti_shake(uint8_t key_num, uint8_t stable_state);
                                 
#ifdef __cplusplus
}
#endif

#endif
