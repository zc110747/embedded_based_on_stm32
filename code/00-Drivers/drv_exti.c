//////////////////////////////////////////////////////////////////////////////
//  (c) copyright 2023-by Persional Inc.  
//  All Rights Reserved
//
//  Name:
//      drv_exti.c
//
//  Purpose:
//      driver for KEY exti.
//      KEY1  -   PH2  -  PIN_EXTI
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
#include "drv_exti.h"
#include "drv_target.h"

static volatile uint8_t key1_interrupt = 0;
static uint32_t key1_delay_tick = 0;

GlobalType_t drv_exti_init(void)
{
    GPIO_InitTypeDef GPIO_InitStruct = {0};

    KEY1_CLK_ENABLE();
    
    GPIO_InitStruct.Pin = KEY1_PIN;
    GPIO_InitStruct.Mode = GPIO_MODE_IT_FALLING;
    GPIO_InitStruct.Pull = GPIO_PULLUP;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
    HAL_GPIO_Init(KEY1_PORT, &GPIO_InitStruct);  

    HAL_NVIC_SetPriority(KEY1_IRQn, 0, 0);
    HAL_NVIC_EnableIRQ(KEY1_IRQn); 
    
    return RT_OK;
}

uint8_t get_key1_press(void)
{
    if (key1_interrupt == 1)
    {
        if (drv_tick_difference(key1_delay_tick, HAL_GetTick()) > 200)
        {
            key1_interrupt = 0;
            __HAL_GPIO_EXTI_CLEAR_IT(KEY1_PIN);
            HAL_NVIC_EnableIRQ(KEY1_IRQn); 
            
            if (KEY1_READ_PIN() == 0)
            {
                return 1;
            }
        }
    }
    
    return 0;
}

void EXTI2_IRQHandler(void)
{
    if (__HAL_GPIO_EXTI_GET_IT(KEY1_PIN) != RESET)
    {
        __HAL_GPIO_EXTI_CLEAR_IT(KEY1_PIN);
        
        key1_interrupt = 1;
        
        key1_delay_tick = HAL_GetTick();
        
        //avoid repeater trigger, just disable until delay
        HAL_NVIC_DisableIRQ(KEY1_IRQn); 
    }   
}
