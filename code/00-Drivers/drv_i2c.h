//////////////////////////////////////////////////////////////////////////////
//  (c) copyright 2023-by Persional Inc.  
//  All Rights Reserved
//
//  Name:
//      drv_i2c.h
//
//  Purpose:
//      1.driver for i2c internal modules£¬ software and hardware.
//      
//
// Author:
//      @zc
//
//  Assumptions:
//
//  Revision History:
//
/////////////////////////////////////////////////////////////////////////////
#ifndef __DRV_I2C_H
#define __DRV_I2C_H

#ifdef __cplusplus
extern "C" {
#endif

#include "includes.h"

typedef enum
{
    HW_I2C1 = 0,
    HW_I2C2,
    HW_I2C3,
}I2C_HW_HANDLE;

I2C_HandleTypeDef *get_i2c1_handle(void);
I2C_HandleTypeDef *get_i2c2_handle(void);
I2C_HandleTypeDef *get_i2c3_handle(void);

GlobalType_t drv_i2c_init(void);
GlobalType_t i2c4_soft_driver_init(void);
GlobalType_t i2c2_soft_driver_init(void);
GlobalType_t i2c2_driver_init(int must_init);   
#ifdef __cplusplus
}
#endif

#endif
