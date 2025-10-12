//////////////////////////////////////////////////////////////////////////////
//  (c) copyright 2023-by Persional Inc.  
//  All Rights Reserved
//
//  Name:
//      drv_wdg.h
//
//  Purpose:
//      driver for watchdog, include iwdg and wwdg.
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
#ifndef _DRV_WDG_H
#define _DRV_WDG_H

#ifdef __cplusplus
extern "C" {
#endif

#include "includes.h"

void iwdg_reload(void);
GlobalType_t drv_wdg_init(void);

#ifdef __cplusplus
}
#endif
    
#endif
