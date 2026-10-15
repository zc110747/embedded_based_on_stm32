//////////////////////////////////////////////////////////////////////////////
//  (c) copyright 2023-by Persional Inc.  
//  All Rights Reserved
//
//  Name:
//      app_performance.h
//
//  Purpose:
//      application performance
//
//  Author:
//      @公众号：<嵌入式技术总结>
//
//  Assumptions:
//
//  Revision History:
//
/////////////////////////////////////////////////////////////////////////////
#ifndef __APP_PERFORMANCE
#define __APP_PERFORMANCE

#include "main.h"

#define PERFORMANCE_TASK_STACK_SIZE    1024
#define PERFORMANCE_TASK_PRIO          20
#define PERFORMANCE_UPDATE_INTERVAL    2000

#define MAX_THREAD_NUM                 16

typedef struct {
    TX_THREAD *thread_ptr;
    CHAR      *thread_name;
    ULONG      run_time;           /* 线程运行时间（时钟节拍） */
    ULONG      last_run_time;      /* 上次统计时的运行时间 */
    ULONG      stack_size;         /* 堆栈大小 */
    ULONG      stack_used;         /* 已使用堆栈 */
    UINT       priority;           /* 线程优先级 */
    UINT       state;              /* 线程状态 */
} ThreadPerformance_t;

typedef struct {
    TX_TIMER *timer_ptr;
    CHAR     *timer_name;
    ULONG     expiration_count;    /* 定时器到期次数 */
    ULONG     last_expiration;     /* 上次到期次数 */
    UINT      active;              /* 是否激活 */
} TimerPerformance_t;

/* 性能统计函数声明 */
GlobalType_t app_performance_init(VOID *memory_ptr);
void performance_report(void);

#endif
