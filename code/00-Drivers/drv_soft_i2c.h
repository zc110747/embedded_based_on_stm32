//////////////////////////////////////////////////////////////////////////////
//  (c) copyright 2023-by Persional Inc.  
//  All Rights Reserved
//
//  Name:
//      drv_soft_i2c.h
//
//  Purpose:
//      1.driver run with stm32 HAL library.
//      other device need update function:
//          I2C_SCL_H(), I2C_SCL_L(), I2C_SDA_H(), I2C_SDA_L(), I2C_SDA_INPUT()
//          i2c_soft_init(), i2c_sda_config_in(), i2c_sda_config_out().
//      2.system systick change, can update 'I2C_DELAY_COUNT' to optimal performance.
//      3.soft i2c interface not protect the time sequence, if used, need disable irq
//        to ensure proper operation, use soft-i2c with multi-thread, must enter
//        critical zone.
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
#ifndef _DRV_SOFT_I2C_H
#define _DRV_SOFT_I2C_H

#include "includes.h"

//modify when different sysclk, too small will leader i2c error.
#define I2C_DELAY_COUNT     5

//can add for support more i2c interface, default is 3
#define SOFT_I2C1           0
#define SOFT_I2C2           1
#define SOFT_I2C3           2
#define SOFT_I2C4           3
#define SOFT_I2C5           4
#define SOFT_I2C_NUM        5

#define I2C_OK              0
#define I2C_ERROR           1

typedef struct
{
    uint32_t scl_pin;
  
    GPIO_TypeDef *scl_port;
    
    uint32_t sda_pin;
    
    GPIO_TypeDef *sda_port;
}SOFT_I2C_INFO;
                                
#ifdef __cplusplus
extern "C" {
#endif

void i2c_error_handle(void);
uint8_t i2c_soft_deinit(uint8_t soft_i2c_num);
uint8_t i2c_soft_init(uint8_t soft_i2c_num, SOFT_I2C_INFO *info_ptr);   
uint8_t i2c_write_device(uint8_t soft_i2c_num, uint8_t addr, uint8_t *data, uint16_t size);
uint8_t i2c_read_device(uint8_t soft_i2c_num, uint8_t addr, uint8_t *rdata, uint16_t size);   
uint8_t i2c_write_memory(uint8_t soft_i2c_num, uint8_t addr, uint32_t mem_addr, uint8_t mem_size, uint8_t *data, uint16_t size);
uint8_t i2c_read_memory(uint8_t soft_i2c_num, uint8_t addr, uint32_t mem_addr, uint8_t mem_size, uint8_t *rdata, uint16_t size);

#ifdef __cplusplus
}
#endif   
    
#endif
