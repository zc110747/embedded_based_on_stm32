//////////////////////////////////////////////////////////////////////////////
//  (c) copyright 2023-by Persional Inc.  
//  All Rights Reserved
//
//  Name:
//      app_performance.c
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
#include "app_performance.h"

// global information
extern TX_THREAD * _tx_thread_created_ptr;
extern TX_TIMER  * _tx_timer_created_ptr;
extern ULONG     _tx_thread_created_count;
extern ULONG     _tx_timer_created_count;

// internal information
static TX_THREAD performance_task;
static TX_TIMER performance_timer;
static TX_SEMAPHORE app_performance_semaphore = {0};

static ThreadPerformance_t thread_perf[MAX_THREAD_NUM];
static ULONG thread_count = 0;
static TimerPerformance_t timer_perf[MAX_THREAD_NUM];
static ULONG timer_count = 0;
static ULONG total_system_time = 0;
static ULONG last_total_time = 0;

static const CHAR* get_thread_state(UINT state)
{
    switch(state)
    {
        case TX_READY:
            return "READY";
        case TX_COMPLETED:
            return "COMPLETED";
        case TX_TERMINATED:
            return "TERMINATED";
        case TX_SUSPENDED:
            return "SUSPENDED";
        case TX_SLEEP:
            return "SLEEP";
        case TX_QUEUE_SUSP:
            return "WAIT_QUEUE";
        case TX_SEMAPHORE_SUSP:
            return "WAIT_SEM";
        case TX_EVENT_FLAG:
            return "WAIT_EVENT";
        case TX_BLOCK_MEMORY:
            return "WAIT_BLOCK_MEM";
        case TX_BYTE_MEMORY:
            return "WAIT_BYTE_MEM";
        case TX_IO_DRIVER:
            return "WAIT_IO";
        case TX_FILE:
            return "WAIT_FILE";
        case TX_TCP_IP:
            return "WAIT_TCPIP";
        case TX_MUTEX_SUSP:
            return "WAIT_MUTEX";
        case TX_PRIORITY_CHANGE:
            return "PRIORITY_CHG";
        default:
            return "UNKNOWN";
    }
}


static void get_thread_performance(void)
{
    TX_THREAD *thread_ptr;
    ULONG index = 0;
    UINT preempt_threshold;
    ULONG time_slice;
    TX_THREAD *next_thread;
    TX_THREAD *next_suspended_thread;
    
    thread_count = 0;
    
    // 遍历所有线程, 获取线程信息
    thread_ptr = _tx_thread_created_ptr;
    while(index < _tx_thread_created_count)
    {
        tx_thread_info_get(thread_ptr,
                          &thread_perf[index].thread_name,
                          &thread_perf[index].state,
                          &thread_perf[index].run_time,
                          &thread_perf[index].priority,
                          &preempt_threshold,
                          &time_slice,
                          &next_thread,
                          &next_suspended_thread);
        
        thread_perf[index].thread_ptr = thread_ptr;
        
        thread_perf[index].stack_size = thread_ptr->tx_thread_stack_size;
        thread_perf[index].stack_used = (ULONG)((CHAR*)thread_ptr->tx_thread_stack_ptr -
                                              (CHAR*)thread_ptr->tx_thread_stack_start + 1);
        
        index++;
        thread_ptr = thread_ptr->tx_thread_created_next;
    }
    
    thread_count = index;
}

/* 获取定时器性能信息 */
static void get_timer_performance(void)
{
    TX_TIMER *timer_ptr;
    ULONG index = 0;
    
    timer_count = 0;
    
    // 遍历所有定时器, 获取定时器信息
    timer_ptr = _tx_timer_created_ptr;
    while(index < _tx_timer_created_count)
    {
        tx_timer_info_get(timer_ptr,
                         &timer_perf[index].timer_name,
                         &timer_perf[index].active,
                         &timer_perf[index].expiration_count,
                         TX_NULL, TX_NULL);
        
        timer_perf[index].timer_ptr = timer_ptr;
        
        index++;
        timer_ptr = timer_ptr->tx_timer_created_next;
    }
    
    timer_count = index;
}

// 输出性能报告
void performance_report(void)
{
    ULONG i;
    ULONG delta_time;
    ULONG cpu_usage;
    
    total_system_time = tx_time_get();
    delta_time = total_system_time - last_total_time;
    
    if(delta_time == 0)
        delta_time = 1;
    
    PRINT_LOG(LOG_TRANS, HAL_GetTick(), "\r\n========================================\r\n");
    PRINT_LOG(LOG_TRANS, HAL_GetTick(), "     ThreadX Performance Report\r\n");
    PRINT_LOG(LOG_TRANS, HAL_GetTick(), "========================================\r\n");
    PRINT_LOG(LOG_TRANS, HAL_GetTick(), "System Time: %lu ticks\r\n", total_system_time);
    PRINT_LOG(LOG_TRANS, HAL_GetTick(), "Report Interval: %lu ticks\r\n", delta_time);
    PRINT_LOG(LOG_TRANS, HAL_GetTick(), "========================================\r\n");
    
    PRINT_LOG(LOG_TRANS, HAL_GetTick(), "\r\n--- Thread Performance ---\r\n");
    PRINT_LOG(LOG_TRANS, HAL_GetTick(), "%2s %-20s %4s %-12s %6s %6s %6s\r\n",
           "No", "Name", "Prio", "State", "Stack", "Used", "CPU%%");
    PRINT_LOG(LOG_TRANS, HAL_GetTick(), "-------------------------------------------------------------------\r\n");
    
    // 输出线程信息
    for(i = 0; i < thread_count; i++)
    {
        cpu_usage = ((thread_perf[i].run_time - thread_perf[i].last_run_time) * 100) / delta_time;
        
        PRINT_LOG(LOG_TRANS, HAL_GetTick(), "%2lu %-20s %4u %-12s %6lu %6lu %6lu%%\r\n",
               i,
               thread_perf[i].thread_name,
               thread_perf[i].priority,
               get_thread_state(thread_perf[i].state),
               thread_perf[i].stack_size,
               thread_perf[i].stack_used,
               cpu_usage);
        
        thread_perf[i].last_run_time = thread_perf[i].run_time;
    }
    
    // 输出定时器信息
    PRINT_LOG(LOG_TRANS, HAL_GetTick(), "\r\n--- Timer Performance ---\r\n");
    PRINT_LOG(LOG_TRANS, HAL_GetTick(), "%2s %-20s %6s %12s\r\n",
           "No", "Name", "Active", "Expirations");
    PRINT_LOG(LOG_TRANS, HAL_GetTick(), "--------------------------------------------\r\n");
    
    for(i = 0; i < timer_count; i++)
    {
        ULONG delta_exp = timer_perf[i].expiration_count - timer_perf[i].last_expiration;
        
        PRINT_LOG(LOG_TRANS, HAL_GetTick(), "%2lu %-20s %6s %12lu (+%lu)\r\n",
               i,
               timer_perf[i].timer_name,
               timer_perf[i].active ? "Yes" : "No",
               timer_perf[i].expiration_count,
               delta_exp);
        
        // 更新上次到期
        timer_perf[i].last_expiration = timer_perf[i].expiration_count;
    }
    
    PRINT_LOG(LOG_TRANS, HAL_GetTick(), "\r\n========================================\r\n\r\n");

    
    last_total_time = total_system_time;
}

// 性能检测任务入口函数
static void performance_task_entry(ULONG thread_input)
{
    UINT status;
    
    last_total_time = tx_time_get();

    get_thread_performance();
    for(ULONG i = 0; i < thread_count; i++)
    {
        thread_perf[i].last_run_time = thread_perf[i].run_time;
    }
    
    get_timer_performance();
    for(ULONG i = 0; i < timer_count; i++)
    {
        timer_perf[i].last_expiration = timer_perf[i].expiration_count;
    }
    
    while(1)
    {
        status = tx_semaphore_get(&app_performance_semaphore, TX_WAIT_FOREVER);
        if (status == TX_SUCCESS)
        {
            // 更新线程信息
            get_thread_performance();
            
            // 更新定时器信息
            get_timer_performance();
            
            // 打印输出
            performance_report();
        }
        else
        {
            PRINT_LOG(LOG_TRANS, HAL_GetTick(), "tx semaphore get failed: %d\r\n", status);
        }
    }
}

void performance_timer_timer_callback(ULONG timer_input)
{
    tx_semaphore_put(&app_performance_semaphore);
}

#if APP_PREFORMANCE_ON == 1
GlobalType_t app_performance_init(VOID *memory_ptr)
{
    CHAR *pointer;
    TX_BYTE_POOL *byte_pool = (TX_BYTE_POOL*)memory_ptr;
    
    if(tx_byte_allocate(byte_pool, (VOID**)&pointer, PERFORMANCE_TASK_STACK_SIZE, TX_NO_WAIT) != TX_SUCCESS)
    {
        PRINT_LOG(LOG_TRANS, HAL_GetTick(), "Failed to allocate memory for performance task stack\r\n");
        return RT_FAIL;
    }
    
    if(tx_thread_create(&performance_task, 
                        "performance_task",
                        performance_task_entry, 
                        0,
                        pointer, 
                        PERFORMANCE_TASK_STACK_SIZE,
                        PERFORMANCE_TASK_PRIO, 
                        PERFORMANCE_TASK_PRIO,
                        TX_NO_TIME_SLICE, 
                        TX_AUTO_START) != TX_SUCCESS)
    {
        PRINT_LOG(LOG_TRANS, HAL_GetTick(), "Failed to create performance task\r\n");
        return RT_FAIL;
    }
    
    // create semaphore
    if (tx_semaphore_create(&app_performance_semaphore, 
                            "app_performance_semaphore", 
                            0) != TX_SUCCESS)
    {
        return RT_FAIL;
    }
    
    /* create periodic timer */
    if(tx_timer_create(&performance_timer, 
                        "performance_timer", 
                        performance_timer_timer_callback, 
                        0, 
                        2000, 2000, TX_AUTO_ACTIVATE) != TX_SUCCESS)
    {
        return RT_FAIL;
    }
    
    PRINT_LOG(LOG_TRANS, HAL_GetTick(), "Performance monitoring initialized successfully\r\n");
    return RT_OK;
}
#else
GlobalType_t app_performance_init(VOID *memory_ptr)
{
    return RT_OK;
}
#endif