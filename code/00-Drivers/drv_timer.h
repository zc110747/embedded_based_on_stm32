//////////////////////////////////////////////////////////////////////////////
//  (c) copyright 2023-by Persional Inc.  
//  All Rights Reserved
//
//  Name:
//      drv_timer.h
//
//  Purpose:
//      driver for timer.
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
#ifndef _DRV_TIMER_H
#define _DRV_TIMER_H

#ifdef __cplusplus
extern "C" {
#endif

#include "includes.h"

GlobalType_t timer_driver_init(void);
uint32_t ger_timer_tick(void);
uint8_t tpad_scan_key(void);
void pwm_set_percent(float percent);
uint32_t get_cnt_per_second(void);

#ifdef __cplusplus
}
#endif
    
#endif
