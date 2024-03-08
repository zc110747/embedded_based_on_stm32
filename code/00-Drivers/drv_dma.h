//////////////////////////////////////////////////////////////////////////////
//  (c) copyright 2023-by Persional Inc.  
//  All Rights Reserved
//
//  Name:
//      drv_dma.h
//
//  Purpose:
//      driver for dma module.
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
#ifndef _DRV_DMA_H
#define _DRV_DMA_H

#ifdef __cplusplus
extern "C" {
#endif

#include "includes.h"

GlobalType_t dma_driver_init(void);
GlobalType_t dma_memory_run(void);

#ifdef __cplusplus
}
#endif

#endif
