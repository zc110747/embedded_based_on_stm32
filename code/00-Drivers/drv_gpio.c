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
//      @zc
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
static GlobalType_t key0_driver_init(void);

//------ static internal variable ------
static uint8_t key0_anti_shake_tick = 0;

//------ global function ------
GlobalType_t gpio_driver_init(void)
{
    GlobalType_t type;
    
    /*led driver init*/
    type = led_driver_init();
    
    type |= key0_driver_init();
    
    return type;
}

uint8_t *get_key0_ansh_tick(void)
{
    return &key0_anti_shake_tick;
}

uint8_t io_anti_shake(uint8_t *tick_ptr, uint8_t now_io, uint8_t read_io)
{
    uint8_t out_io = now_io;

    (*tick_ptr) += 1;

    if (now_io == 0)
    {
        if (read_io == 1)
        {
            if (*tick_ptr > ANTI_SHAKE_TICK)
            {
                out_io = 1;
                *tick_ptr = 0;
            }
        }
        else
        {
            *tick_ptr = 0;
        }
    }
    else if (now_io == 1)
    {
        if (read_io == 0)
        {
            if (*tick_ptr > ANTI_SHAKE_TICK)
            {
                out_io = 0;
                *tick_ptr = 0;
            }
        }
        else
        {
            *tick_ptr = 0;
        }
    }

    return out_io;
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

static GlobalType_t key0_driver_init(void)
{
    GPIO_InitTypeDef GPIO_InitStruct = {0};

    /*LED GPIO Module Clock Enable*/
    KEY0_CLK_ENABLE();
    
    /*GPIO Initialize as input*/
    GPIO_InitStruct.Pin = KEY0_PIN;
    GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
    
    //config as default to avoid error trigger
    GPIO_InitStruct.Pull = GPIO_PULLUP; 
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_MEDIUM;
    HAL_GPIO_Init(KEY0_PORT, &GPIO_InitStruct);   
    
    return RT_OK; 
}
