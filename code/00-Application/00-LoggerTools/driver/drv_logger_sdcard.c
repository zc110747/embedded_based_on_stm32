//////////////////////////////////////////////////////////////////////////////
//  (c) copyright 2025-by Persional Inc.  
//  All Rights Reserved
//
//  Name:
//      drv_logger_uart.c
//
//  Purpose:
// 
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
#include "logger.h"
#include "ff.h"
#include "drv_global.h"

typedef struct 
{
    FATFS fs;
    
    FIL fil;

    BYTE work[FF_MAX_SS];

    uint32_t last_write_tick;

    uint8_t is_ready;
}LOG_SDCARD_INFO;

static LOG_SDCARD_INFO log_sdcard_info;

LOG_STATUS logger_sdcard_init(void)
{
    FRESULT res;

    log_sdcard_info.is_ready = 0;

    res = f_mount(&log_sdcard_info.fs, "1:", 1);
    if (res != FR_OK) {
        return LOG_STATUS_ERROR;
    }

    res = f_open(&log_sdcard_info.fil, "1:log.txt", FA_READ | FA_WRITE | FA_CREATE_NEW);
    if (res != FR_OK) {
        return LOG_STATUS_ERROR;
    }

    log_sdcard_info.is_ready = 1;
    log_sdcard_info.last_write_tick = HAL_GetTick();
    return LOG_STATUS_OK;
}

LOG_STATUS logger_sdcard_transmit(uint8_t *buf, int len)
{
    if (!log_sdcard_info.is_ready) {
        return LOG_STATUS_ERROR;
    }

    FRESULT res = f_write(&log_sdcard_info.fil, buf, len, NULL);
    if (res != FR_OK) {
        return LOG_STATUS_ERROR;
    }

    if (drv_tick_difference(log_sdcard_info.last_write_tick, HAL_GetTick()) >= TICK_MINUTES(10)) {
        log_sdcard_info.last_write_tick = HAL_GetTick();
        res = f_sync(&log_sdcard_info.fil);
        if (res != FR_OK) {
            return LOG_STATUS_ERROR;
        }
    }

    return LOG_STATUS_OK;
}
