//////////////////////////////////////////////////////////////////////////////
//  (c) copyright 2023-by Persional Inc.  
//  All Rights Reserved
//
//  Name:
//      drv_i2c.h
//
//  Purpose:
//      i2c driver.
//
// Author:
//      @zc
//
//  Assumptions:
//
//  Revision History:
//
/////////////////////////////////////////////////////////////////////////////
#ifndef _DRV_I2C_PCF8574_H
#define _DRV_I2C_PCF8574_H

#include "includes.h"

#define PCF8574_ADDR 	        0x40
#define PCF8574_I2C_TIMEOUT     10

#ifdef __cplusplus
extern "C" {
#endif

#define BEEP_IO         0		//蜂鸣器控制引脚  	P0
#define AP_INT_IO       1   	//AP3216C中断引脚	P1
#define DCMI_PWDN_IO    2    	//DCMI的电源控制引脚	P2
#define USB_PWR_IO      3    	//USB电源控制引脚	P3
#define EX_IO      		4    	//扩展IO,自定义使用 	P4
#define MPU_INT_IO      5   	//MPU9250中断引脚	P5
#define RS485_RE_IO     6    	//RS485_RE引脚		P6
#define ETH_RESET_IO    7    	//以太网复位引脚		P7

GlobalType_t pcf8574_driver_init(void);
GlobalType_t pcf8574_i2c_write(uint8_t data);
GlobalType_t pcf8574_i2c_read(uint8_t *pdata);

#ifdef __cplusplus
}
#endif

#endif
