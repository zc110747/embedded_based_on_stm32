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
//      @公众号：<嵌入式技术总结>
//
//  Assumptions:
//	
//
//  Revision History:
//
/////////////////////////////////////////////////////////////////////////////
#include "drv_exti.h"
#include "drv_target.h"

static volatile KEY_INFO key_info[KEY_NUM];

GlobalType_t drv_exti_init(void)
{
    GPIO_InitTypeDef GPIO_InitStruct = {0};

    // 初始化时钟
    KEY0_CLK_ENABLE();
    KEY1_CLK_ENABLE();     
    
    // KEY0硬件初始化
    GPIO_InitStruct.Pin = KEY0_PIN;
    GPIO_InitStruct.Mode = GPIO_MODE_IT_FALLING;        // 下降沿触发中断
    GPIO_InitStruct.Pull = GPIO_PULLUP;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
    HAL_GPIO_Init(KEY0_PORT, &GPIO_InitStruct);  

    HAL_NVIC_SetPriority(KEY0_IRQn, 0, 0);
    HAL_NVIC_EnableIRQ(KEY0_IRQn);
   
    // KEY1硬件初始化
    GPIO_InitStruct.Pin = KEY1_PIN;
    HAL_GPIO_Init(KEY1_PORT, &GPIO_InitStruct); 

    HAL_NVIC_SetPriority(KEY1_IRQn, 0, 0);
    HAL_NVIC_EnableIRQ(KEY1_IRQn);  
    
    memset((char *)&key_info, 0, sizeof(key_info));
    
    return RT_OK;
}

// 硬件获取按键状态
static uint8_t key_hw_press(uint8_t num)
{
    uint8_t key_press = KEY_UNPRESSED;
    
    switch(num)
    {
        case 0:
            __HAL_GPIO_EXTI_CLEAR_IT(KEY0_PIN);
            HAL_NVIC_EnableIRQ(KEY0_IRQn);
            if (KEY0_READ_PIN() == 0)
            {
                key_press = KEY_PRESSED;
            }
            break;
         case 1:
            __HAL_GPIO_EXTI_CLEAR_IT(KEY1_PIN);
            HAL_NVIC_EnableIRQ(KEY1_IRQn);
            if (KEY1_READ_PIN() == 0)
            {
                key_press = KEY_PRESSED;
            }
            break;
         default:
             break;
    }
    return key_press;
}

uint8_t get_key_press(uint8_t num)
{
    if (key_info[num].int_flag == 1)
    {
        if (drv_tick_difference(key_info[num].delay_ticks, HAL_GetTick()) > 200)
        {
            key_info[num].int_flag = 0;
            
            return key_hw_press(num);
        }
    }
    
    // 未检测到按键时, 不进行处理
    return KEY_UNPRESSED;
}

void EXTI2_IRQHandler(void)
{
    if (__HAL_GPIO_EXTI_GET_IT(KEY0_PIN) != RESET)
    {
        __HAL_GPIO_EXTI_CLEAR_IT(KEY0_PIN);
        
        key_info[0].int_flag = 1;
        
        key_info[0].delay_ticks = HAL_GetTick();
        
        // avoid repeater trigger, just disable until delay
        HAL_NVIC_DisableIRQ(KEY0_IRQn); 
    }   
}

void EXTI3_IRQHandler(void)
{
    if (__HAL_GPIO_EXTI_GET_IT(KEY1_PIN) != RESET)
    {
        __HAL_GPIO_EXTI_CLEAR_IT(KEY1_PIN);
        
        key_info[1].int_flag = 1;
        
        key_info[1].delay_ticks = HAL_GetTick();
        
        // avoid repeater trigger, just disable until delay
        HAL_NVIC_DisableIRQ(KEY1_IRQn); 
    }   
}
