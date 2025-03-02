//////////////////////////////////////////////////////////////////////////////
//  (c) copyright 2023-by Persional Inc.  
//  All Rights Reserved
//
//  Name:
//      drv_sdio.h
//
//  Purpose:
//      driver for sdio module.
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
#ifndef _DRV_SDIO_H
#define _DRV_SDIO_H

#ifdef __cplusplus
extern "C" {
#endif
 
#include "includes.h" 

#define SDIO_MODE_PULL                  0
#define SDIO_MODE_DMA                   1
#define SDIO_RUN_MODE                   SDIO_MODE_DMA

#define SDMMC_READ_WRITE_TIMEOUT        1000
#define SDMMC_BLOCK_SIZE                512
#define SDMMC_CLOCK_DIV                 2

#define _SDIO_DMA_SUPPORT               1

GlobalType_t sdcard_driver_init(void);
HAL_StatusTypeDef sdcard_read_disk(uint8_t *buf, uint32_t startBlocks, uint32_t NumberOfBlocks);
HAL_StatusTypeDef sdcard_write_disk(const uint8_t *buf, uint32_t startBlocks, uint32_t NumberOfBlocks);
    
#ifdef __cplusplus
}
#endif

#endif
