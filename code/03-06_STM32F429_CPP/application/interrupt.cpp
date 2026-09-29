//////////////////////////////////////////////////////////////////////////////
//  (c) copyright 2023-by Persional Inc.  
//  All Rights Reserved
//
//  Name:
//      main.cpp
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
#include "gpio.hpp"

class INFO
{
public:
    int a;
    int b;
};

INFO info = {0};

extern "C" int get_info_total(void)
{
    return info.a + info.b;
}