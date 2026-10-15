//////////////////////////////////////////////////////////////////////////////
//  (c) copyright 2023-by Persional Inc.  
//  All Rights Reserved
//
//  Name:
//      app_semap_mutex.c
//
//  Purpose:
//      semaphore interface
//
//  Author:
//      @公众号：<嵌入式技术总结>
//
//  Assumptions:
//	
//  Revision History:
//
/////////////////////////////////////////////////////////////////////////////
#include "app_semap_mutex.h"

#define APP_QUEUE_NUMS          16
#define APP_STACK_SIZE          512
#define APP_THREAD_PRIO         9

static TX_SEMAPHORE app_tx_semaphore = {0};
static TX_MUTEX app_tx_mutex = {0};

static TX_THREAD tx_app_rx_thread = {0};
static TX_THREAD tx_app_tx_thread = {0};

static volatile int g_i = 0;

static void semaphore_rx_thread_entry(ULONG thread_input)
{
    UINT status;
    int index = 0;   

    for (index=0; index<100; index++)
    {
        tx_mutex_get(&app_tx_mutex, TX_WAIT_FOREVER);
        g_i++;
        tx_mutex_put(&app_tx_mutex);
        
        // 延时等待1ms
        tx_thread_sleep(1);
    }
   
    while(1)
    {
        status = tx_semaphore_get(&app_tx_semaphore, TX_WAIT_FOREVER);
        if(status == TX_SUCCESS)
        {
        }
        else
        {
        }
    }    
}


static void semaphore_tx_thread_entry(ULONG thread_input)
{    
    int index = 0;
    
    for (index=0; index<100; index++)
    {
        tx_mutex_get(&app_tx_mutex, TX_WAIT_FOREVER);
        g_i++;
        tx_mutex_put(&app_tx_mutex);
        
        // 延时等待1ms
        tx_thread_sleep(1);
    }

    while(1)
    {
        tx_semaphore_put(&app_tx_semaphore);
        
        tx_thread_sleep(500);
    }    
}

VOID func_semap_notify(TX_SEMAPHORE *semap)
{
    PRINT_LOG(LOG_INFO, HAL_GetTick(), "func_semap_notify id:0x%x", semap->tx_semaphore_id);
}

#if APP_SEMAPHORE_MUTEX_ON == 1
GlobalType_t app_semap_mutex_init(VOID *memory_ptr)
{
    CHAR *pointer;
    TX_BYTE_POOL *byte_pool = (TX_BYTE_POOL*)memory_ptr;
    
    if (tx_semaphore_create(&app_tx_semaphore, "tx app semaphore", 0) != TX_SUCCESS)
    {
        return RT_FAIL;
    }
    
    if (tx_semaphore_put_notify(&app_tx_semaphore, func_semap_notify) != TX_SUCCESS)
    {
        return RT_FAIL;
    }
    
    
    if (tx_mutex_create(&app_tx_mutex, "tx app mutex", TX_NO_INHERIT) != TX_SUCCESS)
    {
        return RT_FAIL;
    }
    
    if (tx_byte_allocate(byte_pool, (VOID**) &pointer, APP_STACK_SIZE, TX_NO_WAIT) != TX_SUCCESS)
    {
        return RT_FAIL;
    }
    
    /* Create tx app thread.  */
    /*
    TX_THREAD *thread_ptr: Thread pointer.
    CHAR *name_ptr:	Thread name.
    VOID (*entry_function)(ULONG id): Entry function.
    ULONG entry_input: Entry input.
    VOID *stack_start: Stack start. 
    ULONG stack_size: Stack size.
    UINT priority: Priority (0-31).
    UINT preempt_threshold: Preempt threshold.
    ULONG time_slice: Time slice.
    UINT auto_start: Auto start.
    */
    if (tx_thread_create(&tx_app_rx_thread, "tx_app_rx_thread", 
        semaphore_rx_thread_entry, 0, 
        pointer,
        APP_STACK_SIZE, 
        APP_THREAD_PRIO, 
        APP_THREAD_PRIO,
        TX_NO_TIME_SLICE, 
        TX_AUTO_START) != TX_SUCCESS)
    {
        return RT_FAIL;
    }
  
    if (tx_byte_allocate(byte_pool, (VOID**) &pointer, APP_STACK_SIZE, TX_NO_WAIT) != TX_SUCCESS)
    {
        return RT_FAIL;
    }
    
    // create tx thread
    if (tx_thread_create(&tx_app_tx_thread, "tx_app_tx_thread", 
        semaphore_tx_thread_entry, 0, 
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
GlobalType_t app_semap_mutex_init(VOID *memory_ptr)
{
    return RT_OK;
}
#endif
