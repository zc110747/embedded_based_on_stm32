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
//      @zc
//
//  Assumptions:
//	
//
//  Revision History:
//
/////////////////////////////////////////////////////////////////////////////
#ifndef _DRV_ALG_H
#define _DRV_ALG_H

#ifdef __cplusplus
extern "C" {
#endif

#include "includes.h"

GlobalType_t drv_alg_init(void);
HAL_StatusTypeDef rng_get_value(uint32_t *val);
int rng_sf_get_value(void);
uint32_t calc_hw_crc32(uint32_t *pbuffer, uint32_t size);
uint8_t calc_crc8(uint8_t *ptr, uint32_t len);
uint16_t calc_crc16(uint8_t *ptr, uint32_t len);
uint32_t calc_crc32(uint8_t *data, size_t length);

#ifdef __cplusplus
}
#endif

#endif
