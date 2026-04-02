//////////////////////////////////////////////////////////////////////////////
//  (c) copyright 2023-by Persional Inc.  
//  All Rights Reserved
//
//  Name:
//      gpio.hpp
//
//  Purpose:
//      usart driver interface process.
//
// Author:
//      @公众号：<嵌入式技术总结>
//
//  Assumptions:
//
//  Revision History:
//
/////////////////////////////////////////////////////////////////////////////
#ifndef __CXX_COMMON_HPP__
#define __CXX_COMMON_HPP__

#include "stm32f4xx_hal.h"

enum class RT_CXX_TYPE
{
    RT_OK = 0,
    RT_FAIL = -1,
    RT_TIMEOUT = -2,
    RT_NOINIT = -3,
};

#endif
