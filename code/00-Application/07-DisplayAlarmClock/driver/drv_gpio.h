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
#ifndef _DRV_GPIO_H
#define _DRV_GPIO_H

#ifdef __cplusplus
extern "C" {
#endif

#include "includes.h"

//i/o anti-shake
#define ANTI_SHAKE_TICK     2

//LED Information
#define LED_CLK_ENABLE()    __HAL_RCC_GPIOB_CLK_ENABLE()
#define LED_PORT            GPIOB
#define LED_PIN             GPIO_PIN_0    

#define LED_ON              HAL_GPIO_WritePin(LED_PORT, LED_PIN, GPIO_PIN_RESET)
#define LED_OFF             HAL_GPIO_WritePin(LED_PORT, LED_PIN, GPIO_PIN_SET)
#define LED_TOGGLE          HAL_GPIO_TogglePin(LED_PORT, LED_PIN)

//Key0 Information
#define KEY0_CLK_ENABLE()   __HAL_RCC_GPIOH_CLK_ENABLE()
#define KEY0_PORT           GPIOH
#define KEY0_PIN            GPIO_PIN_3

#define KEY0_READ_PIN()     (HAL_GPIO_ReadPin(KEY0_PORT, KEY0_PIN)==GPIO_PIN_SET?1:0)

GlobalType_t drv_gpio_init(void);

uint8_t *get_key0_ansh_tick(void);
uint8_t io_anti_shake(uint8_t *tick_ptr, uint8_t nowIoStatus, uint8_t readIoStatus);

#ifdef __cplusplus
}
#endif

#endif
