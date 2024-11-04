//////////////////////////////////////////////////////////////////////////////
//  (c) copyright 2023-by Persional Inc.  
//  All Rights Reserved
//
//  Name:
//      drv_hx711.h
//
//  Purpose:
//      drv_i2c_ds3213.c
//      hardware: 
//          use not hardware i2c interface.
//          SOFT_I2C4
//              I2C_SCL - PB4 - DCMI_SCL
//              I2C_SDA - PB3 - DCMI_SDA
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
#ifndef __DRV_HX711_H
#define __DRV_HX711_H

#include "includes.h"

#define HX711_CLK_ENABLE()      __HAL_RCC_GPIOC_CLK_ENABLE()
#define HX711_SCK_PORT          GPIOC
#define HX711_SCK_PIN           GPIO_PIN_6
#define HX711_SDA_PORT          GPIOC
#define HX711_SDA_PIN           GPIO_PIN_7

#define HX711_SCK_HIGH          HAL_GPIO_WritePin(HX711_SCK_PORT, HX711_SCK_PIN, GPIO_PIN_SET)
#define HX711_SCK_LOW           HAL_GPIO_WritePin(HX711_SCK_PORT, HX711_SCK_PIN, GPIO_PIN_RESET)

#define HX711_SDA_READ          (HAL_GPIO_ReadPin(HX711_SDA_PORT, HX711_SDA_PIN)==GPIO_PIN_SET?1:0)

GlobalType_t hx711_driver_init(void);
uint32_t HX711_Read(void);
#endif
