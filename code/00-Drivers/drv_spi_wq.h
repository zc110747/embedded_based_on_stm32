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
#ifndef _DRV_SPI_WQ_H
#define _DRV_SPI_WQ_H

#include "drv_spi.h"

#define WQ_TEST_RW_ENABLE       1

#define ID_W25Q80 	            0xEF13 	    //容量1M
#define ID_W25Q16               0xEF14      //容量2M
#define ID_W25Q32 	            0xEF15      //容量4M
#define ID_W25Q64 	            0xEF16      //容量8M
#define ID_W25Q128	            0xEF17      //容量16M
#define ID_W25Q256              0xEF18      //容量32M

#define WQ_PAGE_SIZE            256
#define WQ_SECTOR_SIZE          (4*1024)
#define WQ_BLOCK_SIZE           (64*1024)

#define WQ25_CS_ON()            HAL_GPIO_WritePin(GPIOF, GPIO_PIN_6, GPIO_PIN_RESET);
#define WQ25_CS_OFF()           HAL_GPIO_WritePin(GPIOF, GPIO_PIN_6, GPIO_PIN_SET);

#define W25X_WriteEnable		0x06 
#define W25X_WriteDisable		0x04 
#define W25X_ReadStatusReg1		0x05 
#define W25X_ReadStatusReg2		0x35 
#define W25X_ReadStatusReg3		0x15 
#define W25X_WriteStatusReg1    0x01 
#define W25X_WriteStatusReg2    0x31 
#define W25X_WriteStatusReg3    0x11 
#define W25X_ReadData			0x03 
#define W25X_FastReadData		0x0B 
#define W25X_FastReadDual		0x3B 
#define W25X_PageProgram		0x02 
#define W25X_BlockErase			0xD8 
#define W25X_SectorErase		0x20 
#define W25X_ChipErase			0xC7 
#define W25X_PowerDown			0xB9 
#define W25X_ReleasePowerDown	0xAB 
#define W25X_DeviceID			0xAB 
#define W25X_ManufactDeviceID	0x90 
#define W25X_JedecDeviceID		0x9F 
#define W25X_Enable4ByteAddr    0xB7
#define W25X_Exit4ByteAddr      0xE9

uint16_t wq_get_id(void);
void wq_rw_test(void);
GlobalType_t wq_driver_init(void);
GlobalType_t wq_erase_chip(void);
GlobalType_t wq_read(uint32_t start_addr, uint8_t *pbuffer, uint32_t num);
GlobalType_t wq_write(uint32_t start_addr, uint8_t* pbuffer, uint16_t num);
#endif
