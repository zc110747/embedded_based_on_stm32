//////////////////////////////////////////////////////////////////////////////
//  (c) copyright 2023-by Persional Inc.  
//  All Rights Reserved
//
//  Name:
//      drv_gpio.h
//
//  Purpose:
//      driver for adc module.
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
#ifndef _DRV_ADC_H
#define _DRV_ADC_H

#ifdef __cplusplus
extern "C" {
#endif

#include "includes.h"

#define ADC_RUN_MODE            RUN_MODE_DMA

GlobalType_t adc_driver_init(void);
uint32_t get_vref_val(void);
uint16_t adc_avg_value_read(uint32_t channel);

#ifdef __cplusplus
}
#endif

#endif
