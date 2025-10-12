//////////////////////////////////////////////////////////////////////////////
//  (c) copyright 2025-by Persional Inc.  
//  All Rights Reserved
//
//  Name:
//      drv_spi_adxl345.h
//
//  Purpose:
//      driver for spi module.
//      HW: CS-PF5, 
//          SPI5_CLK - PF7
//          SPI5_MISO - PF8
//          SPI5_MOSI - PF9
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
#ifndef _DRV_SPI_ADXl345_H
#define _DRV_SPI_ADXl345_H

#include "drv_spi.h"

GlobalType_t drv_adxl345_init(void);
int drv_adxl345_single_read(int16_t *Data_X, int16_t *Data_Y, int16_t *Data_Z);
int drv_adxl345_multi_read(int16_t *Data_X, int16_t *Data_Y, int16_t *Data_Z);

#endif
