//////////////////////////////////////////////////////////////////////////////
//  (c) copyright 2023-by Persional Inc.  
//  All Rights Reserved
//
//  Name:
//      global_def.h
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
#ifndef _GLOBAL_DEF_H
#define _GLOBAL_DEF_H

typedef enum
{
    RT_OK = 0,
    RT_FAIL,
}GlobalType_t;

#ifndef UINT_MAX
#define UINT_MAX    0xFFFFFFFF
#endif

#define DRIVER_GPIO_ENABLE      1
#define DRIVER_EXTI_ENABLE      1

#endif
