//////////////////////////////////////////////////////////////////////////////
//  (c) copyright 2023-by Persional Inc.  
//  All Rights Reserved
//
//  Name:
//      drv_bsp.h
//
//  Purpose:
//      bsp device 
//
//  Author:
//      
//
//  Assumptions:
//	
//  Revision History:
//
/////////////////////////////////////////////////////////////////////////////
#ifndef __DRV_BSP_H
#define __DRV_BSP_H

#include "drv_bsp_target.h"
#include "drv_bsp_sdio.h"

GlobalType_t drv_bsp_init(void);

uint32_t board_millis(void);
void board_delay_ms(uint32_t ms);
#endif
