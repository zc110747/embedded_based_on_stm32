//////////////////////////////////////////////////////////////////////////////
//  (c) copyright 2023-by Persional Inc.  
//  All Rights Reserved
//
//  Name:
//      flash_parameter.h
//
//  Purpose:
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
#ifndef _FLASH_PARAMETER_H
#define _FLASH_PARAMETER_H

#include "drv_global.h"


#define FLASH_PARAMETER_ADDRESS     FLASH_SECTOR4_ADDRESS
#define FLASH_HEADER                0x5AA5

typedef __packed struct __flash_data
{
    uint16_t head;
    
    uint8_t parity;
    uint8_t stopbits;
    uint32_t baudrate; 
    
    uint8_t led_status;
    uint8_t beep_status;
    
    uint8_t reserved[24];
    uint16_t crc;
}FlashData_t;

FlashData_t *flash_parameter_get(void);
void flash_parameter_update(void);
GlobalType_t flash_parameter_write(void);
GlobalType_t flash_parameter_write_with_compare(void);
GlobalType_t flash_parameter_init(void);
#endif
