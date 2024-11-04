//////////////////////////////////////////////////////////////////////////////
//  (c) copyright 2023-by Persional Inc.  
//  All Rights Reserved
//
//  Name:
//      dac.h
//
//  Purpose:
//      dac interface init and set_voltage.
//
// Author:
//      @zc
//
//  Assumptions:
//
//  Revision History:
//
/////////////////////////////////////////////////////////////////////////////
#ifndef __DRV_DAC_H
#define __DRV_DAC_H

#ifdef __cplusplus
extern "C" {
#endif

#include "includes.h"

#define RUN_MODE_VOICE              4

#define DAC_RUN_MODE                RUN_MODE_VOICE

//reference voltage, uint:mv
#define DAC_REFERENCE_VOL           3300

//dac max output value
#define DAC_MAX_VALUE               4095

void dac_set(uint16_t mv);   
GlobalType_t dac_driver_init(void);

#ifdef __cplusplus
}
#endif   
#endif
