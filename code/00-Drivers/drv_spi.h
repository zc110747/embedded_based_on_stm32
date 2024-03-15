//////////////////////////////////////////////////////////////////////////////
//  (c) copyright 2023-by Persional Inc.  
//  All Rights Reserved
//
//  Name:
//      drv_spi.h
//
//  Purpose:
//      driver for spi module.
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
#ifndef _DRV_SPI_H
#define _DRV_SPI_H

#ifdef __cplusplus
extern "C" {
#endif

#include "includes.h"

#define SPI_RUN_MODE        RUN_MODE_NORMAL


GlobalType_t spi_driver_init(void);

//dma interface
uint16_t wq_read_chipid(void);
uint8_t spi_write_check_ok(void);
uint8_t spi_read_check_ok(void);
uint8_t spi_write_dma(uint8_t *data, uint16_t size);
uint8_t spi_read_dma(uint8_t *tx_buffer, uint8_t *rx_buffer, uint16_t size);
uint8_t spi_rw_byte_nocheck(uint8_t data);
uint8_t spi_rw_byte(uint8_t data, HAL_StatusTypeDef *err);

#ifdef __cplusplus
}
#endif

#endif
