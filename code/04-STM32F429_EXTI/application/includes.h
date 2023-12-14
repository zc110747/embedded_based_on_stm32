//////////////////////////////////////////////////////////////////////////////
//  (c) copyright 2023-by Persional Inc.  
//  All Rights Reserved
//
//  Name:
//      includes.h
//
//  Purpose:
//
//  Author:
//      @zc
//
//  Assumptions:
//	
//  Revision History:
//
/////////////////////////////////////////////////////////////////////////////
#ifndef _INCLUDES_H
#define _INCLUDES_H

#include "stm32f4xx_hal.h"
#include "global_def.h"

static uint32_t drv_tick_difference(uint32_t value, uint32_t now_tick)
{
    return (now_tick >= value) ? now_tick - value : UINT_MAX - value + now_tick + 1;
}

#endif
