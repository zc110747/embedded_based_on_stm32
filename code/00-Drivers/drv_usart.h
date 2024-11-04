//////////////////////////////////////////////////////////////////////////////
//  (c) copyright 2023-by Persional Inc.  
//  All Rights Reserved
//
//  Name:
//      drv_usart.h
//
//  Purpose:
//      driver for usart.
//      usart hw: PA2, PA3 USART2
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
#ifndef _DRV_USART_H
#define _DRV_USART_H

#ifdef __cplusplus
extern "C" {
#endif

#include "includes.h"

#define USART_RUN_MODE  RUN_MODE_NULL

GlobalType_t usart_driver_init(void);
void usart_loop_run(void);

#ifdef __cplusplus
}
#endif
#endif
