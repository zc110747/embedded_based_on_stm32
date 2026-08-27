//////////////////////////////////////////////////////////////////////////////
//  (c) copyright 2025-by Persional Inc.  
//  All Rights Reserved
//
//  Name:
//      app_queue.c
//
//  Purpose:
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
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"

#if TASK_QUEUE_ON == 1
static TaskHandle_t xQrHandle = NULL;
static TaskHandle_t xQwHandle = NULL;
QueueHandle_t xQueue = NULL;

static void queue_reader_task(void *argument)
{
    BaseType_t xReturned;
    int value;

    while(1)
    {
        // 查看队列头部数据(不读取，仅检查头部数据)
        xReturned = xQueuePeek(xQueue, &value, portMAX_DELAY);
        if (xReturned == pdPASS) {
            PRINT_LOG(LOG_INFO, HAL_GetTick(), "xQueuePeek: %d", value);
        }

        // 从队列接收数据
        xReturned = xQueueReceive(xQueue, &value, portMAX_DELAY);
        if (xReturned == pdPASS) {
            PRINT_LOG(LOG_INFO, HAL_GetTick(), "xQueueReceive: %d", value);
        }
    }
}

static void queue_writer_task(void *argument)
{
    BaseType_t xReturned;
    uint32_t value = 0;

    PRINT_LOG(LOG_INFO, HAL_GetTick(), "queue %s write start", pcQueueGetName(xQueue));  

    while(1)
    {
        // 发送数据到队列
        xReturned = xQueueSend(xQueue, &value, portMAX_DELAY);
        if (xReturned == pdPASS) {
            PRINT_LOG(LOG_INFO, HAL_GetTick(), "xQueueSend: %d", value);
            value++;
        }

        // 向队列尾部发送数据
        xReturned = xQueueSendToBack(xQueue, &value, portMAX_DELAY);
        if (xReturned == pdPASS) {
            PRINT_LOG(LOG_INFO, HAL_GetTick(), "xQueueSendToBack: %d", value);
            value++;
        }

        // 从队列头部接收数据
        xReturned = xQueueSendToFront(xQueue, &value, portMAX_DELAY);
        if (xReturned == pdPASS) {
            PRINT_LOG(LOG_INFO, HAL_GetTick(), "xQueueSendToFront: %d", value);
            value++;
        }
         
        // 覆盖队列中的数据
        xReturned = xQueueOverwrite(xQueue, &value);
        if (xReturned == pdPASS) {
            PRINT_LOG(LOG_INFO, HAL_GetTick(), "xQueueOverwrite: %d", value);
            value++;
        }

    
        vTaskDelay(1000);
    }
}

GlobalType_t app_queue_init(void)
{    
    BaseType_t xReturned;

    // task reader create
    xReturned = xTaskCreate(queue_reader_task, 
            "queue_reader_task", 
            256, 
            NULL, 
            tskIDLE_PRIORITY + 2, 
            &xQrHandle);

    xReturned &= xTaskCreate(queue_writer_task, 
            "queue_writer_task", 
            256, 
            NULL, 
            tskIDLE_PRIORITY + 2, 
            &xQwHandle);

    // queue create
    xQueue = xQueueCreate(10, sizeof(int));

    vQueueAddToRegistry(xQueue, "testQueue");

    if (xReturned != pdPASS
    || xQueue == NULL) {
        return RT_FAIL;
    }
    return RT_OK; 
}
#else
GlobalType_t app_queue_init(void)
{
    return RT_OK;
}
#endif
