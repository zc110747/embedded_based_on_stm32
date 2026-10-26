//////////////////////////////////////////////////////////////////////////////
//  (c) copyright 2023-by Persional Inc.  
//  All Rights Reserved
//
//  Name:
//      drv_bsp.c
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
#include "drv_bsp.h"

GlobalType_t drv_bsp_init(void)
{
    // sdcard drvier init
    drv_sdcard_init();
 

    
    return RT_OK;
}

uint32_t board_millis(void)
{
    return HAL_GetTick();
}

void board_delay_ms(uint32_t ms)
{
    HAL_Delay(ms);
}
    