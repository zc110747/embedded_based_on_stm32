//////////////////////////////////////////////////////////////////////////////
//  (c) copyright 2023-by Persional Inc.  
//  All Rights Reserved
//
//  Name:
//      application.c
//
//  Purpose:
//      application interface.
//
//  Author:
//      @公众号：<嵌入式技术总结>
//
//  Assumptions:
//	
//  Revision History:
//
/////////////////////////////////////////////////////////////////////////////
#include "application.h"
#include "app_queue.h"
#include "app_semap_mutex.h"
#include "app_event_flag.h"
#include "app_timer.h"
#include "app_performance.h"

GlobalType_t application_init(VOID *memory_ptr)
{
    GlobalType_t result = RT_OK;

    // queue application
    result = app_queue_init(memory_ptr);
    
    // semaphore_application
    result |= app_semap_mutex_init(memory_ptr);
    
    // event flag
    result |= app_event_flag_init(memory_ptr);

    // timer application
    result |= app_timer_init(memory_ptr);
    
    // performance application
    result |= app_performance_init(memory_ptr);
    
    return result;
}
