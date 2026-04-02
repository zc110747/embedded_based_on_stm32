//////////////////////////////////////////////////////////////////////////////
//  (c) copyright 2023-by Persional Inc.  
//  All Rights Reserved
//
//  Name:
//      drv_flash.h
//
//  Purpose:
//      
//
//  Author:
//      @公众号：<嵌入式技术总结>
//
//  Assumptions:
//	
//
//  Revision History:
//
/////////////////////////////////////////////////////////////////////////////
#ifndef __DRV_FLASH_H
#define __DRV_FLASH_H

#include "includes.h"

#define FLASH_SECTOR0_ADDRESS       0x08000000
#define FLASH_SECTOR1_ADDRESS       0x08004000
#define FLASH_SECTOR2_ADDRESS       0x08008000
#define FLASH_SECTOR3_ADDRESS       0x0800C000
#define FLASH_SECTOR4_ADDRESS       0x08010000
#define FLASH_SECTOR5_ADDRESS       0x08020000
#define FLASH_SECTOR6_ADDRESS       0x08040000
#define FLASH_SECTOR7_ADDRESS       0x08060000
#define FLASH_SECTOR8_ADDRESS       0x08080000
#define FLASH_SECTOR9_ADDRESS       0x080A0000
#define FLASH_SECTOR10_ADDRESS      0x080C0000
#define FLASH_SECTOR11_ADDRESS      0x080E0000

typedef enum
{
    FLASH_OK = 0,
    FLASH_ERROR = 1,
}FlashType_t;

uint16_t flash_get_sector(uint32_t address);
FlashType_t flash_erase_device(uint32_t sector, uint32_t nb_sectors);
FlashType_t flash_write_word(uint32_t address, uint32_t *pbuffer, uint32_t size);
FlashType_t flash_read_word(uint32_t address, uint32_t *pbuffer, uint32_t size);
FlashType_t flash_write(uint32_t address, uint8_t *pbuffer, uint32_t size);
FlashType_t flash_read(uint32_t address, uint8_t *pbuffer, uint32_t size);
#endif
