//////////////////////////////////////////////////////////////////////////////
//  (c) copyright 2023-by Persional Inc.  
//  All Rights Reserved
//
//  Name:
//      app_event_flag.c
//
//  Purpose:
//      event flag interface
//
//  Author:
//      @公众号：<嵌入式技术总结>
//
//  Assumptions:
//
//  Revision History:
//
/////////////////////////////////////////////////////////////////////////////
#include "app_event_flag.h"

#define APP_STACK_SIZE          512
#define APP_THREAD_PRIO         9

static TX_EVENT_FLAGS_GROUP event_flag = {0};
static TX_THREAD tx_app_set_event_thread = {0};
static TX_THREAD tx_app_get_event_thread = {0};

/* 事件标志定义 */
#define EVENT_FLAG_0            (0x01)
#define EVENT_FLAG_1            (0x02)
#define EVENT_FLAG_2            (0x04)
#define EVENT_FLAG_3            (0x08)

void event_flag_set_thread_entry(ULONG thread_input)
{
    UINT status;
    UINT flag_index = 0;
    
    while(1)
    {
        /* 轮流设置不同的事件标志 */
        switch(flag_index % 4)
        {
            case 0:
                status = tx_event_flags_set(&event_flag, EVENT_FLAG_0 | EVENT_FLAG_1, TX_OR);
                break;
            case 1:
                status = tx_event_flags_set(&event_flag, EVENT_FLAG_1 | EVENT_FLAG_2, TX_OR);
                break;
            case 2:
                status = tx_event_flags_set(&event_flag, EVENT_FLAG_2, TX_OR);
                break;
            case 3:
                status = tx_event_flags_set(&event_flag, EVENT_FLAG_3, TX_OR);
                break;
        }
        
        flag_index++;
        
        tx_thread_sleep(1000);
    }
}

void event_flag_get_thread_entry(ULONG thread_input)
{
    UINT status;
    ULONG actual_flags;
    
    while(1)
    {
        status = tx_event_flags_get(&event_flag, 
                                   EVENT_FLAG_0 | EVENT_FLAG_1 | EVENT_FLAG_2 | EVENT_FLAG_3, 
                                   TX_OR_CLEAR, 
                                   &actual_flags, 
                                   TX_WAIT_FOREVER);
        
        if(status == TX_SUCCESS)
        {          
            if(actual_flags & EVENT_FLAG_0)
            {
                PRINT_LOG(LOG_INFO, HAL_GetTick(), "Event flag 0 received");
            }

            if(actual_flags & EVENT_FLAG_1)
            {
                PRINT_LOG(LOG_INFO, HAL_GetTick(), "Event flag 1 received");
            }

            if(actual_flags & EVENT_FLAG_2)
            {
                PRINT_LOG(LOG_INFO, HAL_GetTick(), "Event flag 2 received");
            }

            if(actual_flags & EVENT_FLAG_3)
            {
                PRINT_LOG(LOG_INFO, HAL_GetTick(), "Event flag 3 received");
            }
        }
        else
        {
            PRINT_LOG(LOG_INFO, HAL_GetTick(), "Failed to get event flags, status: %d", status);
        }
    }
}

VOID func_event_notify(TX_EVENT_FLAGS_GROUP *group_ptr)
{
    PRINT_LOG(LOG_INFO, HAL_GetTick(), "func_event_notify id:0x%x", group_ptr->tx_event_flags_group_id);
}

#if APP_EVENT_FLAG_ON == 1
GlobalType_t app_event_flag_init(VOID *memory_ptr)
{
    CHAR *pointer;
    TX_BYTE_POOL *byte_pool = (TX_BYTE_POOL*)memory_ptr;

    /* Create event flags group.  */
    if (tx_event_flags_create(&event_flag, "app_event_flag") != TX_SUCCESS) {
        return RT_FAIL;
    }
    
    // 设置事件标志组发送完成的回调函数
    if (tx_event_flags_set_notify(&event_flag, func_event_notify) != TX_SUCCESS) {
        return RT_FAIL;
    }

    /* Allocate the stack for set event thread.  */
    if (tx_byte_allocate(byte_pool, (VOID **) &pointer, APP_STACK_SIZE*2, TX_NO_WAIT) != TX_SUCCESS)
    {
       return RT_FAIL;
    }
    
    /* Create set event thread.  */
    if (tx_thread_create(&tx_app_set_event_thread, 
        "tx_app_set_event_thread", 
        event_flag_set_thread_entry, 0, 
        pointer,
        APP_STACK_SIZE*2, 
        APP_THREAD_PRIO, 
        APP_THREAD_PRIO,
        TX_NO_TIME_SLICE, 
        TX_AUTO_START) != TX_SUCCESS)
    {
        return RT_FAIL;
    }
    
    /* Allocate the stack for get event thread.  */
    if (tx_byte_allocate(byte_pool, (VOID **) &pointer, APP_STACK_SIZE, TX_NO_WAIT) != TX_SUCCESS)
    {
       return RT_FAIL;
    }
    
    /* Create get event thread.  */
    if (tx_thread_create(&tx_app_get_event_thread, 
        "tx_app_get_event_thread", 
        event_flag_get_thread_entry, 0, 
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
GlobalType_t app_event_flag_init(VOID *memory_ptr)
{
    return RT_OK;
}
#endif
