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
#ifndef _DRV_CAN_H
#define _DRV_CAN_H

#ifdef __cplusplus
extern "C" {
#endif

#include "includes.h"

GlobalType_t can_driver_init(void);
GlobalType_t can_write_std(uint8_t* data, uint8_t len);
int can_driver_receive(uint8_t *data, uint8_t len);

#ifdef __cplusplus
}
#endif
    
#endif
