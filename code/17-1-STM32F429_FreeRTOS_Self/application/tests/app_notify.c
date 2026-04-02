//////////////////////////////////////////////////////////////////////////////
//  (c) copyright 2025-by Persional Inc.  
//  All Rights Reserved
//
//  Name:
//      notify.c
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
#include "app_notify.h"
#include "FreeRTOS.h"
#include "task.h"

#if TASK_NOTIFY_ON == 1

#define ULONG_MAX  0xFFFFFFFF

static TaskHandle_t xNrHandle = NULL;
static TaskHandle_t xNwHandle = NULL;

// xNrHandle
static void notify_reader_task(void *argument)
{
    uint32_t notify_value = 0;

    while(1)
    {
        // take notify
        notify_value = ulTaskNotifyTake(pdTRUE, portMAX_DELAY);
        PRINT_LOG(LOG_INFO, HAL_GetTick(), "notify take:%d", notify_value);

        // take notify index notify
        notify_value = ulTaskNotifyTakeIndexed(1, pdTRUE, portMAX_DELAY);
        PRINT_LOG(LOG_INFO, HAL_GetTick(), "notify take index:%d", notify_value);
        
        // take notify
        if (xTaskNotifyWait(0x00, ULONG_MAX, &notify_value, portMAX_DELAY ) == pdPASS)
        {
            PRINT_LOG(LOG_INFO, HAL_GetTick(), "notify wait:%d", notify_value);
        }
        
        // take index notify
        if (xTaskNotifyWaitIndexed(0x00, 0x01, ULONG_MAX, &notify_value, portMAX_DELAY ) == pdPASS)
        {
            PRINT_LOG(LOG_INFO, HAL_GetTick(), "notify wait:%d", notify_value);
        }

        // take notify
        if (xTaskNotifyWait(0x00, ULONG_MAX, &notify_value, portMAX_DELAY ) == pdPASS)
        {
            PRINT_LOG(LOG_INFO, HAL_GetTick(), "notify wait:%d", notify_value);
        }
        
        // take index notify
        if (xTaskNotifyWaitIndexed(0x00, 0x01, ULONG_MAX, &notify_value, portMAX_DELAY ) == pdPASS)
        {
            PRINT_LOG(LOG_INFO, HAL_GetTick(), "notify wait:%d", notify_value);
        }        
    }
}

// xNwHandle
static void notify_writer_task(void *argument)
{
    uint32_t notify_value = 0;
    
    while(1)
    {
        // notify task by default index 0
        xTaskNotifyGive(xNrHandle);
        vTaskDelay(1);
        
        // notify index of task
        xTaskNotifyGiveIndexed(xNrHandle, 1);
        vTaskDelay(1);
        
        // notify value
        xTaskNotify(xNrHandle, 100, eSetValueWithOverwrite);
        vTaskDelay(1);
        
        // notify index value
        xTaskNotifyIndexed(xNrHandle, 1, 200, eSetValueWithOverwrite);
        vTaskDelay(1);
        
        // notify value
        xTaskNotifyAndQuery(xNrHandle, 300, eSetValueWithOverwrite, &notify_value);
        PRINT_LOG(LOG_INFO, HAL_GetTick(), "NotifyAndQuery:%d", notify_value);
        vTaskDelay(1);
        
        // notify index value
        xTaskNotifyAndQueryIndexed(xNrHandle, 1, 400, eSetValueWithOverwrite, &notify_value);
        PRINT_LOG(LOG_INFO, HAL_GetTick(), "NotifyAndQuery index:%d", notify_value);
        vTaskDelay(500);
    }
}

GlobalType_t app_notify_init(void)
{   
    BaseType_t xReturned;

    // task reader create
    xReturned = xTaskCreate(notify_reader_task, 
            "notify_reader_task", 
            256, 
            NULL, 
            tskIDLE_PRIORITY + 2, 
            &xNrHandle);

    xReturned &= xTaskCreate(notify_writer_task, 
            "notify_writer_task", 
            256, 
            NULL, 
            tskIDLE_PRIORITY + 2, 
            &xNwHandle);

    if (xReturned != pdPASS) {
        return RT_FAIL;
    }
    return RT_OK;
}
#else
GlobalType_t app_notify_init(void)
{
    return RT_OK;
}
#endif
