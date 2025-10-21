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
//      @zc
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

#define KEY1_CLK_ENABLE()   __HAL_RCC_GPIOH_CLK_ENABLE()
#define KEY1_PORT           GPIOH
#define KEY1_PIN            GPIO_PIN_2

#define KEY1_IRQn           EXTI2_IRQn
#define KEY1_READ_PIN()     (HAL_GPIO_ReadPin(KEY1_PORT, KEY1_PIN)==GPIO_PIN_SET?1:0)

GlobalType_t drv_exti_init(void);
uint8_t get_key1_press(void);
#ifdef __cplusplus
}
#endif

#endif
