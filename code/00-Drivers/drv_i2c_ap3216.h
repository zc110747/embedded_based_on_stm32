//////////////////////////////////////////////////////////////////////////////
//  (c) copyright 2023-by Persional Inc.  
//  All Rights Reserved
//
//  Name:
//      drv_i2c.h
//
//  Purpose:
//      driver for i2c module.
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
#ifndef __I2C_INCLUDE
#define __I2C_INCLUDE

#ifdef __cplusplus
extern "C" {
#endif

#include "includes.h"

/*address*/
#define AP3216C_ADDR    	    0x1e	
#define AP3216C_TIMEOUT         10

/*register*/
#define AP3216C_SYSTEMCONG	    0x00	
#define AP3216C_INTSTATUS	    0X01	
#define AP3216C_INTCLEAR	    0X02	
#define AP3216C_IRDATALOW	    0x0A	
#define AP3216C_IRDATAHIGH	    0x0B
#define AP3216C_ALSDATALOW	    0x0C	
#define AP3216C_ALSDATAHIGH	    0X0D	
#define AP3216C_PSDATALOW	    0X0E	
#define AP3216C_PSDATAHIGH	    0X0F	

typedef struct
{
    uint16_t ir;
    uint16_t als;
    uint16_t ps;
}AP3216C_INFO;

void ap3216_loop_run_test(void);
GlobalType_t ap3216_driver_init(void);
GlobalType_t ap3216_reg_read(uint8_t reg, uint8_t data);
GlobalType_t ap3216_i2c_multi_read(uint8_t reg, uint8_t *rdata, uint8_t size);    
GlobalType_t ap3216_reg_write(uint8_t reg, uint8_t data);
GlobalType_t ap3216_i2c_multi_write(uint8_t reg, uint8_t *data, uint8_t size);    

#ifdef __cplusplus
}
#endif

#endif
