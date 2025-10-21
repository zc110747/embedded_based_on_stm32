//////////////////////////////////////////////////////////////////////////////
//  (c) copyright 2025-by Persional Inc.  
//  All Rights Reserved
//
//  Name:
//      application.c
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
#include "application.h"
#include "FreeRTOS.h"
#include "task.h"
#include "semphr.h"
#include "queue.h"
#include "timers.h"
#include "stream_buffer.h"
#include "message_buffer.h"
#include "event_groups.h"
#include <string.h>

#define BIT_0	( 1 << 0 )
#define BIT_4	( 1 << 4 )

typedef struct 
{
    TaskHandle_t handle_reader;

    TaskHandle_t handle_writer;

    // semaphore mutex
    SemaphoreHandle_t semaphore_0;

    // semaphore binary
    SemaphoreHandle_t semaphore_1;

    // semaphore count
    SemaphoreHandle_t semaphore_2;
    
    // queue
    QueueHandle_t queue_0;

    // stream
    StreamBufferHandle_t stream_0;
    
    // message_buffer
    MessageBufferHandle_t message_buffer_0;

    // Timer
    TimerHandle_t timer_0;

    // EventGroup
    EventGroupHandle_t event_group_0;
    int data[10];
}OS_INFO;

static int rx_buffer[128];
static OS_INFO info_;

#if configSUPPORT_STATIC_ALLOCATION == 1
StackType_t xIdleTaskStack[100];
StaticTask_t xIdleTaskTCB;

void vApplicationGetIdleTaskMemory( StaticTask_t ** ppxIdleTaskTCBBuffer,
                                    StackType_t ** ppxIdleTaskStackBuffer,
                                    configSTACK_DEPTH_TYPE * puxIdleTaskStackSize )
{
    *ppxIdleTaskTCBBuffer = &xIdleTaskTCB;
    *ppxIdleTaskStackBuffer = xIdleTaskStack;
    *puxIdleTaskStackSize = 100;
}

StackType_t xIimerTaskStack[256];
StaticTask_t xTimerTaskTCB;
void vApplicationGetTimerTaskMemory( StaticTask_t ** ppxTimerTaskTCBBuffer,
                                     StackType_t ** ppxTimerTaskStackBuffer,
                                     configSTACK_DEPTH_TYPE * puxTimerTaskStackSize )
{
    *ppxTimerTaskTCBBuffer = &xTimerTaskTCB;
    *ppxTimerTaskStackBuffer = xIimerTaskStack;
    *puxTimerTaskStackSize = 256;
}
#endif

void reader_task(void *argument)
{
    int index;
    EventBits_t uxBits;

    // semaphore mutex wait
    if( info_.semaphore_0 != NULL ) {
        if (xSemaphoreTake(info_.semaphore_0, portMAX_DELAY) == pdTRUE ) {
            PRINT_LOG(LOG_INFO, HAL_GetTick(), "reader_task: semaphore_0 take success");
            xSemaphoreGive(info_.semaphore_0);
        }
    }

    /// semaphore binary wait
    if (info_.semaphore_1 != NULL) {
        if (xSemaphoreTake(info_.semaphore_1, portMAX_DELAY) == pdTRUE ) {
            PRINT_LOG(LOG_INFO, HAL_GetTick(), "reader_task: semaphore_1 take success");
            xSemaphoreGive(info_.semaphore_1);
        }
    }
   
    // 给write_task发送通知，通知它可以处理
    xTaskNotifyGive(info_.handle_writer);

    if (info_.semaphore_2 != NULL) {
        if (xSemaphoreTake(info_.semaphore_2, portMAX_DELAY) == pdTRUE ) {
            PRINT_LOG(LOG_INFO, HAL_GetTick(), "reader_task: semaphore_2 take success");
        }
    }

    // queue wait
    if (info_.queue_0 != NULL) {
        for (index = 0; index < 10; index++) {
            if (xQueueReceive(info_.queue_0, &info_.data[index], portMAX_DELAY) == pdTRUE) {
                PRINT_LOG(LOG_INFO, HAL_GetTick(), "reader_task: queue_0 receive %d", info_.data[index]);
            }
        }
    }

    // stream wait
    // 如果是单个线程读取，不需要加锁，多线程读取时需要加锁
    if (info_.stream_0 != NULL) {
        for (index = 0; index < 10; index++) {
            if (xStreamBufferReceive(info_.stream_0, &info_.data[index], sizeof(int), portMAX_DELAY) == sizeof(int)) {
                PRINT_LOG(LOG_INFO, HAL_GetTick(), "reader_task: stream_0 receive %d", info_.data[index]);
            }
        }
    }

    // message_buffer wait
    // 如果是单个线程读取，不需要加锁，多线程读取时需要加锁
    if (info_.message_buffer_0 != NULL) {
        size_t rx_size;
        for (index = 0; index < 10; index++) {
            rx_size = xMessageBufferReceive(info_.message_buffer_0, rx_buffer, 128, portMAX_DELAY);
            if (rx_size > 0) {
                PRINT_LOG(LOG_INFO, HAL_GetTick(), "reader_task: message_buffer_0 size:%d", rx_size);
            }
        }
    }

    // event_group wait
    if (info_.event_group_0 != NULL) {
        uxBits = xEventGroupWaitBits(info_.event_group_0, 
            BIT_0 | BIT_4, 
            pdTRUE,     // 读取后清除bit
            pdFALSE,    // 不用等待both置位
            portMAX_DELAY);
        PRINT_LOG(LOG_INFO, HAL_GetTick(), "reader_task: event_group_0 wait %d", uxBits);
    }

    while(1) {
        vTaskDelay(10);
    }
}

void writer_task(void *argument)
{
    int index = 0;
   
    // 等待直到收到通知
    ulTaskNotifyTake( pdTRUE, portMAX_DELAY );  

    // semaphore count give
    if (info_.semaphore_2 != NULL) {
        xSemaphoreGive(info_.semaphore_2);
    }
    vTaskDelay(100);

    /// queue send
    if (info_.queue_0 != NULL) {
        for (index = 0; index < 10; index++) {
            xQueueSend(info_.queue_0, &info_.data[index], portMAX_DELAY);
            vTaskDelay(10);
        }
    }
    vTaskDelay(100);

    // stream send
    // 如果是单个线程发送，不需要加锁，多线程发送时需要加锁
    if (info_.stream_0 != NULL) {
        for (index = 0; index < 10; index++) {
            info_.data[index] = 10 + index;
            if (xSemaphoreTake(info_.semaphore_0, portMAX_DELAY) == pdTRUE) { 
                xStreamBufferSend(info_.stream_0, &info_.data[index], sizeof(int), portMAX_DELAY);
                xSemaphoreGive(info_.semaphore_0);
            }
            vTaskDelay(10);
        }
    }
    vTaskDelay(100);

    // message_buffer send
    // 如果是单个线程发送，不需要加锁，多线程发送时需要加锁
    if (info_.message_buffer_0 != NULL) {
        for (index = 0; index < 10; index++) {
            if (xSemaphoreTake(info_.semaphore_0, portMAX_DELAY) == pdTRUE) {
                xMessageBufferSend(info_.message_buffer_0, info_.data, 10, portMAX_DELAY);
                xSemaphoreGive(info_.semaphore_0);
            }
            vTaskDelay(10);
        }
    }
    vTaskDelay(100);

    // event_group set bits
    if (info_.event_group_0 != NULL) {
        xEventGroupSetBits(info_.event_group_0, BIT_0 | BIT_4);
    }

    while(1) {
        vTaskDelay(10);
    }
}

void vTimerCallback( TimerHandle_t xTimer )
{
    uint32_t count;

    count = ( uint32_t )pvTimerGetTimerID( xTimer ); // 获取timer id
    count++;

    if( count >= 10 ) {
        xTimerStop( xTimer, 0 );
        PRINT_LOG(LOG_INFO, HAL_GetTick(), "vTimerCallback: timer stop");
    } else {
        vTimerSetTimerID( xTimer, ( void * ) count);  // 设置timer id
    }
}

GlobalType_t application_init(void)
{
    BaseType_t xReturned;
    int index = 0;

    memset(&info_, 0, sizeof(info_));

    info_.semaphore_0 = xSemaphoreCreateMutex();
    info_.semaphore_1 = xSemaphoreCreateBinary();
    info_.semaphore_2 = xSemaphoreCreateCounting(10, 0);
    info_.queue_0 = xQueueCreate(10, sizeof(int));
    info_.stream_0 = xStreamBufferCreate(256, sizeof(int));
    info_.message_buffer_0 = xMessageBufferCreate(256);
    info_.event_group_0 = xEventGroupCreate();
    for (index = 0; index < 10; index++) {
        info_.data[index] = index;
    }

    // task reader create
    xReturned = xTaskCreate(reader_task, 
        "reader_task", 
        512, 
        NULL, 
        tskIDLE_PRIORITY + 2, 
        &info_.handle_reader);
    
    // task writer create
    xReturned &= xTaskCreate(writer_task, 
        "writer_task", 
        512, 
        NULL, 
        tskIDLE_PRIORITY + 1, 
        &info_.handle_writer);
    
    // timer create
    info_.timer_0 = xTimerCreate("timer_0", 
        pdMS_TO_TICKS(100), 
        pdTRUE, 
        ( void * ) 0, 
        vTimerCallback);
    xTimerStart(info_.timer_0, 0);

    if (xReturned == pdPASS
    && info_.semaphore_0 != NULL && info_.semaphore_1 != NULL && info_.semaphore_2 != NULL
    && info_.queue_0 != NULL
    && info_.stream_0 != NULL
    && info_.message_buffer_0 != NULL
    && info_.timer_0 != NULL
    && info_.event_group_0 != NULL
    ) {
        return RT_OK;
    }
    
    return RT_FAIL;
}
