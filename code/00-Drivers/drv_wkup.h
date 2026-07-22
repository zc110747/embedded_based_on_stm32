//////////////////////////////////////////////////////////////////////////////
//  (c) copyright 2023-by Persional Inc.  
//  All Rights Reserved
//
//  Name:
//      drv_alg.h
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
#ifndef _DRV_WKUP_H
#define _DRV_WKUP_H

#ifdef __cplusplus
extern "C" {
#endif

#include "includes.h"

GlobalType_t drv_wkup_init(void);
void drv_wkup_enter_sleep(uint8_t Entry);
void drv_wkup_enter_stop(uint8_t Entry);
void drv_wkup_enter_standby(void);    
    
#ifdef __cplusplus
}
#endif

#endif
