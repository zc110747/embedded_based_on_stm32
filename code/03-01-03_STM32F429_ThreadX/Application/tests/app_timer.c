//////////////////////////////////////////////////////////////////////////////
//  (c) copyright 2023-by Persional Inc.  
//  All Rights Reserved
//
//  Name:
//      app_timer.c
//
//  Purpose:
//      timer interface
//
//  Author:
//      @公众号：<嵌入式技术总结>
//
//  Assumptions:
//
//  Revision History:
//
/////////////////////////////////////////////////////////////////////////////
#include "app_timer.h"

static TX_TIMER one_shot_timer;      /* 一次性定时器 */
static TX_TIMER periodic_timer;      /* 周期性定时器 */
static TX_EVENT_FLAGS_GROUP timer_event_flag = {0};
static TX_THREAD tx_app_timer_get_event_thread = {0};
static volatile ULONG one_shot_count = 0;
static volatile ULONG periodic_count = 0;

#define APP_STACK_SIZE                512
#define APP_THREAD_PRIO               9
#define TIME_EVENT_FLAG_0            (0x01)
#define TIME_EVENT_FLAG_1            (0x02)

// one-shot timer callback
void one_shot_timer_callback(ULONG timer_input)
{
    tx_event_flags_set(&timer_event_flag, TIME_EVENT_FLAG_0, TX_OR);
}

// periodic timer callback
void periodic_timer_callback(ULONG timer_input)
{
    tx_event_flags_set(&timer_event_flag, TIME_EVENT_FLAG_1, TX_OR);
}

void timer_event_flag_get_thread_entry(ULONG thread_input)
{
    UINT status;
    ULONG actual_flags;
    
    while(1)
    {
        status = tx_event_flags_get(&timer_event_flag, 
                                   TIME_EVENT_FLAG_0 | TIME_EVENT_FLAG_1, 
                                   TX_OR_CLEAR, 
                                   &actual_flags, 
                                   TX_WAIT_FOREVER);
        
        if(status == TX_SUCCESS)
        {          
            if(actual_flags & TIME_EVENT_FLAG_0)
            {
                one_shot_count++;
                PRINT_LOG(LOG_INFO, HAL_GetTick(), "One-shot timer triggered! Count: %lu", one_shot_count);
            }

            if(actual_flags & TIME_EVENT_FLAG_1)
            {
                periodic_count++;
                PRINT_LOG(LOG_INFO, HAL_GetTick(), "Periodic timer triggered! Count: %lu", periodic_count);
            }
        }
        else
        {
            PRINT_LOG(LOG_INFO, HAL_GetTick(), "Failed to get event flags, status: %d", status);
        }
    }
}

#if APP_TIMER_ON == 1
GlobalType_t app_timer_init(VOID *memory_ptr)
{
    UINT status;
    CHAR *pointer;
    TX_BYTE_POOL *byte_pool = (TX_BYTE_POOL*)memory_ptr;
    
    /* create one-shot timer */
    // @timer_ptr: Timer pointer.
    // @name_ptr: Timer name.
    // @expiration_function: Timer expiration function.
    // @expiration_input: Timer expiration input.
    // @initial_ticks: Timer initial ticks.
    // @reschedule_ticks: Timer reschedule ticks, zero means no reschedule.
    // @auto_activate: Auto activate.
    status = tx_timer_create(&one_shot_timer, "one_shot_timer", 
                        one_shot_timer_callback, 0, 
                        5000, 0, TX_AUTO_ACTIVATE);
    if (status != TX_SUCCESS)
    {
        return RT_FAIL;
    }
    
    /* create periodic timer */
    status = tx_timer_create(&periodic_timer, "periodic_timer", 
                            periodic_timer_callback, 0, 
                            2000, 2000, TX_AUTO_ACTIVATE);
    if (status != TX_SUCCESS)
    {
        return RT_FAIL;
    }
    
    if (tx_event_flags_create(&timer_event_flag, "timer_event_flag") != TX_SUCCESS) {
        return RT_FAIL;
    }
    
    /* Allocate the stack for get event thread.  */
    if (tx_byte_allocate(byte_pool, (VOID **) &pointer, APP_STACK_SIZE, TX_NO_WAIT) != TX_SUCCESS)
    {
       return RT_FAIL;
    }
    
    /* Create get event thread.  */
    if (tx_thread_create(&tx_app_timer_get_event_thread, 
        "tx_app_timer_thread", 
        timer_event_flag_get_thread_entry, 0, 
        pointer,
        APP_STACK_SIZE, 
        APP_THREAD_PRIO, 
        APP_THREAD_PRIO,
        TX_NO_TIME_SLICE, 
        TX_AUTO_START) != TX_SUCCESS)
    {
        return RT_FAIL;
    }
    return RT_OK;
}

#else
GlobalType_t app_timer_init(VOID *memory_ptr)
{
    return RT_OK;
}
#endif
