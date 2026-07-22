//////////////////////////////////////////////////////////////////////////////
//  (c) copyright 2023-by Persional Inc.  
//  All Rights Reserved
//
//  Name:
//      drv_exti.h
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
#ifndef _DRV_EXTI_H
#define _DRV_EXTI_H

#ifdef __cplusplus
extern "C" {
#endif

#include "includes.h"

typedef struct
{
    uint8_t int_flag;
    uint32_t delay_ticks;
}KEY_INFO;

#define KEY_NUM             2

#define KEY_PRESSED         0
#define KEY_UNPRESSED       1

// KEY0的硬件
#define KEY0_CLK_ENABLE()   __HAL_RCC_GPIOH_CLK_ENABLE()
#define KEY0_PORT           GPIOH
#define KEY0_PIN            GPIO_PIN_2
#define KEY0_IRQn           EXTI2_IRQn
#define KEY0_READ_PIN()     (HAL_GPIO_ReadPin(KEY0_PORT, KEY0_PIN)==GPIO_PIN_SET?1:0)

#define KEY1_CLK_ENABLE()   __HAL_RCC_GPIOH_CLK_ENABLE()
#define KEY1_PORT           GPIOH
#define KEY1_PIN            GPIO_PIN_3
#define KEY1_IRQn           EXTI3_IRQn
#define KEY1_READ_PIN()     (HAL_GPIO_ReadPin(KEY1_PORT, KEY1_PIN)==GPIO_PIN_SET?1:0)

GlobalType_t drv_exti_init(void);
uint8_t get_key_press(uint8_t key_num);
#ifdef __cplusplus
}
#endif

#endif
