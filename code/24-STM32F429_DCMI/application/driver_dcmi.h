//////////////////////////////////////////////////////////////////////////////
//  (c) copyright 2023-by Persional Inc.  
//  All Rights Reserved
//
//  Name:
//      driver_dcmi.h
//
//  Purpose:
//
//  Author:
//      @公众号：<嵌入式技术总结>
//
//  Assumptions:
//	
//  Revision History:
//
/////////////////////////////////////////////////////////////////////////////
#ifndef _DRV_DCMI_H
#define _DRV_DCMI_H

#include "includes.h"

GlobalType_t dcmi_driver_init(void);
void dcmi_start(void);
void dcmi_stop(void);
#endif
