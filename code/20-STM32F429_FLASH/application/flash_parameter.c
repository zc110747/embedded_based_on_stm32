//////////////////////////////////////////////////////////////////////////////
//  (c) copyright 2023-by Persional Inc.  
//  All Rights Reserved
//
//  Name:
//      flash_parameter.c
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
#include "flash_parameter.h"
#include <string.h>

static FlashData_t g_parameter_info = {0};
static uint32_t FLASH_CACHE[64]; // 256bytes
const size_t parameter_length = sizeof(FlashData_t);

GlobalType_t flash_parameter_init(void)
{
    flash_read(FLASH_PARAMETER_ADDRESS, (uint8_t *)&g_parameter_info, parameter_length);
    
    if (g_parameter_info.head != FLASH_HEADER)
    {
        memset((uint8_t *)&g_parameter_info, 0, parameter_length);
        if (flash_parameter_write_with_compare() != RT_OK) {
            return RT_FAIL;
        }
    }
    
    return RT_OK;
}

void flash_parameter_update(void)
{
    flash_read(FLASH_PARAMETER_ADDRESS, (uint8_t *)&g_parameter_info, parameter_length);
}

GlobalType_t flash_parameter_write(void)
{
    memcpy((char *)FLASH_CACHE, (char *)&g_parameter_info, parameter_length);
    if(flash_erase_device(flash_get_sector(FLASH_PARAMETER_ADDRESS), 1) != FLASH_OK) {
        PRINT_LOG(LOG_ERROR, HAL_GetTick(), "flash erase failed!");
        return RT_FAIL;
    }

    if(flash_write_word(FLASH_PARAMETER_ADDRESS, FLASH_CACHE, parameter_length/4+1) != FLASH_OK) {
        PRINT_LOG(LOG_ERROR, HAL_GetTick(), "flash write failed!");
        return RT_FAIL;
    }

    return RT_OK;
}

// 带更新的写入
GlobalType_t flash_parameter_write_with_compare(void)
{
    flash_read_word(FLASH_PARAMETER_ADDRESS, FLASH_CACHE, parameter_length/4+1);

    if (memcmp((char *)FLASH_CACHE, (char *)&g_parameter_info, parameter_length) != 0)
    {
        g_parameter_info.head = FLASH_HEADER;
        g_parameter_info.crc = calc_crc16((uint8_t *)&g_parameter_info, parameter_length-2);
        if (flash_parameter_write() != RT_OK) {
            PRINT_LOG(LOG_ERROR, HAL_GetTick(), "flash parameter write failed!");
            return RT_FAIL;
        } else {
            PRINT_LOG(LOG_ERROR, HAL_GetTick(), "flash parameter write success!");
        }
    } else {
        PRINT_LOG(LOG_INFO, HAL_GetTick(), "data no change, not write!");
    }
     
    return RT_OK;
}

FlashData_t *flash_parameter_get(void)
{
    return &g_parameter_info;
}
