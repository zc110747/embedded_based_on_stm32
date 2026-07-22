//////////////////////////////////////////////////////////////////////////////
//  (c) copyright 2023-by Persional Inc.  
//  All Rights Reserved
//
//  Name:
//      drv_alg.c
//
//  Purpose:
//      driver for alg
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
#include "drv_wkup.h"

//------ static internal variable ------


//------ static internal function ------

//------ global function ------
GlobalType_t drv_wkup_init(void)
{
    GlobalType_t result;
    GPIO_InitTypeDef GPIO_Initure;
    
    __HAL_RCC_GPIOA_CLK_ENABLE();
	
    GPIO_Initure.Pin = GPIO_PIN_0;
    GPIO_Initure.Mode = GPIO_MODE_IT_RISING;
    GPIO_Initure.Pull = GPIO_PULLDOWN;
    GPIO_Initure.Speed = GPIO_SPEED_FAST;
    HAL_GPIO_Init(GPIOA,&GPIO_Initure);

    HAL_NVIC_SetPriority(EXTI0_IRQn, 0x01, 0x01);
    HAL_NVIC_EnableIRQ(EXTI0_IRQn);
    
    return result;
}

void EXTI0_IRQHandler(void)
{
    HAL_GPIO_EXTI_IRQHandler(GPIO_PIN_0);
}

void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin)
{
    if(GPIO_Pin == GPIO_PIN_0) 
    {
        // 唤醒引脚, 仅触发不动作
    }    
}

void drv_wkup_enter_sleep(uint8_t Entry)
{
    HAL_SuspendTick();

    __HAL_PWR_CLEAR_FLAG(PWR_FLAG_WU);
    
    HAL_PWR_EnterSLEEPMode(PWR_MAINREGULATOR_ON, Entry);

    HAL_ResumeTick();
}

void drv_wkup_enter_stop(uint8_t Entry)
{   
    HAL_SuspendTick();
    
    __HAL_PWR_CLEAR_FLAG(PWR_FLAG_WU);

    HAL_PWR_EnterSTOPMode(PWR_MAINREGULATOR_ON, Entry);
    
    HAL_ResumeTick();
}

void drv_wkup_enter_standby(void)
{
    __HAL_PWR_CLEAR_FLAG(PWR_FLAG_WU);

    __HAL_PWR_CLEAR_FLAG(PWR_FLAG_SB);

    HAL_PWR_EnableWakeUpPin(PWR_WAKEUP_PIN1);

    HAL_PWR_EnterSTANDBYMode();
}
