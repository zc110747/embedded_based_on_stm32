//////////////////////////////////////////////////////////////////////////////
//  (c) copyright 2023-by Persional Inc.  
//  All Rights Reserved
//
//  Name:
//      drv_target.h
//
//  Purpose:
//      driver for target module.
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
#ifndef _DRV_TARGET_H
#define _DRV_TARGET_H

#ifdef __cplusplus
extern "C" {
#endif

#include "includes.h"

void set_os_on(void);
uint8_t get_os_on(void);
void hal_delay_ms(uint16_t time);
uint32_t drv_tick_difference(uint32_t value, uint32_t now_tick);

uint8_t bcdToDec(uint8_t val);
uint8_t decToBcd(uint8_t val);

#ifdef __cplusplus
}
#endif

#endif
