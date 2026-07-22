//////////////////////////////////////////////////////////////////////////////
//  (c) copyright 2023-by Persional Inc.  
//  All Rights Reserved
//
//  Name:
//      drv_gpio.c
//
//  Purpose:
//      driver for gpio.
//      LED     -   PB0 - PIN_OUTPUT
//      KEY0    -   PH3 - PIN_INPUT
//
//      step1: unable gpio module clock.
//      step2: config the gpio mode.
//      step3: use HAL_GPIO_WritePin to config gpio.
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
#include "drv_gpio.h"

//------ static internal function ------
static GlobalType_t led_driver_init(void);
static GlobalType_t key_gpio_driver_init(void);

//------ static internal variable ------
static uint8_t anti_shake_tick[KEY_NUMS] = {0};

//------ global function ------
GlobalType_t drv_gpio_init(void)
{
    GlobalType_t type;
    
    /*led driver init*/
    type = led_driver_init();
    
    type |= key_gpio_driver_init();
    
    return type;
}

uint8_t io_anti_shake(uint8_t key_num, uint8_t stable_state)
{
    uint8_t stable = stable_state;
    uint8_t read_io;
    
    if (key_num >= KEY_NUMS)
        return stable;
    
    read_io = get_key_value(key_num);
    
    if (stable_state == read_io)
    {
        anti_shake_tick[key_num] = 0;
    }
    else 
    {
        anti_shake_tick[key_num]++;
        if (anti_shake_tick[key_num] >= ANTI_SHAKE_TICK)
        {
            stable = read_io;
            anti_shake_tick[key_num] = 0;
        }
    }

    return stable;
}

uint8_t get_key_value(uint8_t num)
{
    uint8_t key_value = 0;
    
    if (num == 0) 
    {
        key_value = KEY_GPIO_READ_PIN(KEY0_GPIO_PORT, KEY0_GPIO_PIN);
    } 
    else if (num == 1)
    {
        key_value = KEY_GPIO_READ_PIN(KEY1_GPIO_PORT, KEY1_GPIO_PIN);
    }
    else
    {
        key_value = KEY_GPIO_READ_PIN(KEY2_GPIO_PORT, KEY2_GPIO_PIN);
    }
    
    return key_value;
}

//------ internal function ------
static GlobalType_t led_driver_init(void)
{
    GPIO_InitTypeDef GPIO_InitStruct = {0};

    /*LED GPIO Module Clock Enable*/
    LED_CLK_ENABLE();
    
    /*Shutdown LED to avoid blink*/
    LED_OFF;
    
    /*GPIO Initialize as output*/
    GPIO_InitStruct.Pin = LED_PIN;
    GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
    HAL_GPIO_Init(LED_PORT, &GPIO_InitStruct);   
    
    return RT_OK;
}

static GlobalType_t key_gpio_driver_init(void)
{
    GPIO_InitTypeDef GPIO_InitStruct = {0};

    /*LED GPIO Module Clock Enable*/
    KEY_GPIO_CLK_ENABLE();
    
    //config as default to avoid error trigger
    GPIO_InitStruct.Pull = GPIO_PULLUP; 
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_MEDIUM;
    GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
    
    /*GPIO Initialize as input*/
    GPIO_InitStruct.Pin = KEY0_GPIO_PIN;   
    HAL_GPIO_Init(KEY0_GPIO_PORT, &GPIO_InitStruct);   

    GPIO_InitStruct.Pin = KEY1_GPIO_PIN;   
    HAL_GPIO_Init(KEY1_GPIO_PORT, &GPIO_InitStruct);

    GPIO_InitStruct.Pin = KEY2_GPIO_PIN;   
    HAL_GPIO_Init(KEY2_GPIO_PORT, &GPIO_InitStruct);    
    return RT_OK; 
}
