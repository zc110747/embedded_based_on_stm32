//////////////////////////////////////////////////////////////////////////////
//  (c) copyright 2023-by Persional Inc.  
//  All Rights Reserved
//
//  Name:
//      can_control_task.c
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
#include <string.h>
#include "can_control_task.h"
#include "cmsis_os.h"

//------ extern variable ------
extern CAN_HandleTypeDef hcan1;

// internal variable
static CAN_CONTROL_INFO can_info;
static osThreadId_t canTaskHandle;
const osThreadAttr_t canTask_attributes = {
  .name = "canTask",
  .stack_size = 128 * 4,
  .priority = (osPriority_t) osPriorityNormal,
};

//internal function
static void canControlTask(void *argument);
static GlobalType_t can_driver_send(uint8_t *data, uint8_t len);

//------ global function ------
GlobalType_t can_control_task_init(void)
{
    memset((char *)&can_info, 0, sizeof(CAN_CONTROL_INFO));
    
    can_info.xMessageBuffer = xMessageBufferCreate(MESSAGE_BUFFER_SIZE); 
   
    canTaskHandle = osThreadNew(canControlTask, NULL, &canTask_attributes);
    
    if (can_info.xMessageBuffer == NULL
    || canTaskHandle == NULL)
    {
        return RT_FAIL;
    }
    
    return RT_OK;
}

static void canControlTask(void *argument)
{
    uint8_t rx_buffer[16];
    size_t xReceivedBytes;
    const TickType_t xBlockTime = pdMS_TO_TICKS( 100 );
    
    for(;;)
    {
        xReceivedBytes = xMessageBufferReceive( can_info.xMessageBuffer,
                                            ( void * ) rx_buffer,
                                            sizeof( rx_buffer ),
                                            xBlockTime );
        if (xReceivedBytes > 0)
        {
            PRINT_LOG(LOG_INFO, HAL_GetTick(), "rx size:%d", xReceivedBytes);
        }
    }
}

void HAL_CAN_RxFifo0MsgPendingCallback(CAN_HandleTypeDef* hcan)
{
    uint8_t Data[8];
    CAN_RxHeaderTypeDef RxMessage;
    BaseType_t xHigherPriorityTaskWoken = pdFALSE; 
 
    if (HAL_CAN_GetState(&hcan1) != RESET)
    {
        HAL_CAN_GetRxMessage(&hcan1, CAN_RX_FIFO0, &RxMessage, Data);
        
        xMessageBufferSendFromISR( can_info.xMessageBuffer,
                                  ( void * ) Data,
                                  RxMessage.DLC,
                                  &xHigherPriorityTaskWoken );
    }
                                            
    portYIELD_FROM_ISR( xHigherPriorityTaskWoken );
}

GlobalType_t can_write_buffer(uint8_t *data, uint16_t len)
{
    uint16_t nums = len/MAX_CAN_DLC;
    uint8_t offset = len%MAX_CAN_DLC;
    uint8_t *ptr;
    GlobalType_t ret = RT_OK;
    
    ptr = data;
    
    if (nums > 0)
    {
        uint16_t index;
        
        for (index=0; index<nums; index++)
        {
            ret = can_driver_send(ptr, MAX_CAN_DLC); 
            if (ret != RT_OK) 
            {
                return ret;
            }
            ptr += 8;
        }
    }
    
    if (offset > 0)
    {
        ret = can_driver_send(ptr, offset);  
    }
    return ret;
}

static GlobalType_t can_driver_send(uint8_t *data, uint8_t len)
{	
    CAN_TxHeaderTypeDef txHeader;
    uint32_t mailbox;
    uint8_t index;
    
    if (len > MAX_CAN_DLC) {
        return RT_FAIL;
    }
    
    txHeader.StdId = 0x12;
    txHeader.IDE = CAN_ID_STD;
    txHeader.RTR = CAN_RTR_DATA;
    txHeader.DLC = len;
    
    while(HAL_CAN_AddTxMessage(&hcan1, &txHeader, data, &mailbox) != HAL_OK)
    {
        index++;
        if (index > 50)
        {
            return RT_FAIL;
        }
        vTaskDelay(1);
    }
   
    return RT_OK;		
}
