//////////////////////////////////////////////////////////////////////////////
//  (c) copyright 2023-by Persional Inc.  
//  All Rights Reserved
//
//  Name:
//      app_queue.c
//
//  Purpose:
//      queue interface
//
//  Author:
//      @公众号：<嵌入式技术总结>
//
//  Assumptions:
//	
//  Revision History:
//
/////////////////////////////////////////////////////////////////////////////
#include "app_queue.h"

#define APP_QUEUE_NUMS          16
#define APP_STACK_SIZE          512
#define APP_THREAD_PRIO         9

static TX_QUEUE app_tx_queue = {0};
static TX_THREAD tx_app_rx_thread = {0};
static TX_THREAD tx_app_tx_thread = {0};

typedef struct
{
    int id;
    int nums;
}MSGS;

void queue_rx_thread_entry(ULONG thread_input)
{
    UINT status;
    MSGS rx_msg = {0};
    
    while(1)
    {
        status = tx_queue_receive(&app_tx_queue, &rx_msg, TX_WAIT_FOREVER);
        
        if(status == TX_SUCCESS)
        {
            PRINT_LOG(LOG_INFO, HAL_GetTick(), "Received ID %d, Data is %d", rx_msg.id, rx_msg.nums);
        }
        else
        {
            PRINT_LOG(LOG_INFO, HAL_GetTick(), "Failed to receive message, status: %d", status);
        }
    }    
}

void queue_tx_thread_entry(ULONG thread_input)
{
    UINT status;
    MSGS rx_msg = {0};
    
    while(1)
    {
        // 使用tx_queue_send发送到队列
        rx_msg.id = 0;
        rx_msg.nums = rand()%1000;
        status = tx_queue_send(&app_tx_queue, &rx_msg, TX_WAIT_FOREVER);        
        if(status != TX_SUCCESS)
        {
            PRINT_LOG(LOG_INFO, HAL_GetTick(), "Failed to send message, status: %d", status);
        }
        
        // 使用tx_queue_front_send发送到队列前端
        rx_msg.id = 1;
        rx_msg.nums = rand()%1000;
        status = tx_queue_front_send(&app_tx_queue, &rx_msg, TX_WAIT_FOREVER);        
        if(status != TX_SUCCESS)
        {
            PRINT_LOG(LOG_INFO, HAL_GetTick(), "Failed to send message, status: %d", status);
        }
        
        tx_thread_sleep(1000);
    }    
}

VOID func_queue_notify(TX_QUEUE *queue_ptr)
{
    PRINT_LOG(LOG_INFO, HAL_GetTick(), "func_queue_notify");
}

#if APP_QUEUE_ON == 1
GlobalType_t app_queue_init(VOID *memory_ptr)
{
    CHAR *pointer;
    TX_BYTE_POOL *byte_pool = (TX_BYTE_POOL*)memory_ptr;
   
    srand(HAL_GetTick()%1000);
    
    /* Allocate the stack for tx app queue.  */
    if (tx_byte_allocate(byte_pool, (VOID **) &pointer, APP_QUEUE_NUMS*sizeof(MSGS), TX_NO_WAIT) != TX_SUCCESS)
    {
       return RT_FAIL;
    }
    
    /* Create tx app queue.  */
    if (tx_queue_create(&app_tx_queue, "app_tx_queue", 
                      sizeof(MSGS),
                      pointer, 
                      APP_QUEUE_NUMS*sizeof(MSGS)) != TX_SUCCESS)
    {
        return RT_FAIL;
    }
  
    // 设置队列发送完成的回调函数
    if (tx_queue_send_notify(&app_tx_queue, func_queue_notify) != TX_SUCCESS)
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
        queue_rx_thread_entry, 0, 
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

    if (tx_thread_create(&tx_app_tx_thread, "tx_app_tx_thread", 
        queue_tx_thread_entry, 0, 
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
GlobalType_t app_queue_init(VOID *memory_ptr)
{
    return RT_OK;
}
#endif
