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
#include "can_protocol.h"

// global info
CAN_CONTROL_INFO can_info;

//------ extern variable ------
extern CAN_HandleTypeDef hcan1;

// internal variable
const osThreadAttr_t canRxTask_attributes = {
  .name = "canRxTask",
  .stack_size = 128 * 4,
  .priority = (osPriority_t) osPriorityNormal,
};
const osThreadAttr_t canTxTask_attributes = {
  .name = "canTxTask",
  .stack_size = 256,
  .priority = (osPriority_t) osPriorityHigh,
};

//internal function
static void canRxTask(void *argument);
static void canTxTask(void *argument);
static GlobalType_t can_buffer_send(uint8_t *data, uint16_t len);

//------ global function ------
GlobalType_t can_control_task_init(void)
{
    memset((char *)&can_info, 0, sizeof(CAN_CONTROL_INFO));
    
    can_info.xRxMessageBuffer = xMessageBufferCreate(MESSAGE_RX_BUFFER_SIZE);
    can_info.xTxMessageBuffer = xMessageBufferCreate(MESSAGE_TX_BUFFER_SIZE);
    can_info.xSemaphore = xSemaphoreCreateMutex();

    can_info.canRxTaskHandle = osThreadNew(canRxTask, NULL, &canRxTask_attributes);
    can_info.canTxTaskHandle = osThreadNew(canTxTask, NULL, &canTxTask_attributes);
    
    if (can_info.xRxMessageBuffer == NULL
    || can_info.canRxTaskHandle == NULL
    || can_info.canTxTaskHandle == NULL
    || can_info.xSemaphore == NULL)
    {
        return RT_FAIL;
    }
    
    return RT_OK;
}

void HAL_CAN_RxFifo0MsgPendingCallback(CAN_HandleTypeDef* hcan)
{
    uint8_t Data[8];
    CAN_RxHeaderTypeDef RxMessage;
    BaseType_t xHigherPriorityTaskWoken = pdFALSE; 
 
    if (HAL_CAN_GetState(&hcan1) != RESET)
    {
        HAL_CAN_GetRxMessage(&hcan1, CAN_RX_FIFO0, &RxMessage, Data);
        
        if (RxMessage.DLC > 0)
        {
            xMessageBufferSendFromISR( can_info.xRxMessageBuffer,
                                      ( void * ) Data,
                                      RxMessage.DLC,
                                      &xHigherPriorityTaskWoken );
        }
    }
                                            
    portYIELD_FROM_ISR( xHigherPriorityTaskWoken );
}

GlobalType_t can_message_send(uint8_t *pdate, uint16_t size)
{
    if (size > MAX_FRAME_BUFFER) {
        return RT_FAIL;
    }
    
    if( xSemaphoreTake(can_info.xSemaphore, ( TickType_t ) 20) == pdTRUE )
    {
        xMessageBufferSend(can_info.xTxMessageBuffer, pdate, size, pdMS_TO_TICKS(100));
        xSemaphoreGive(can_info.xSemaphore);
    }
    else
    {
        PRINT_LOG(LOG_ERROR, HAL_GetTick(), "message send timeout!");
        return RT_FAIL;
    }

    return RT_OK;
}

// internal function
static void canTxTask(void *argument)
{
    static uint8_t tx_buffer[MAX_FRAME_BUFFER];
    size_t byte_size;
    
    for(;;)
    {
        byte_size =  xMessageBufferReceive(can_info.xTxMessageBuffer,
                                            ( void * ) tx_buffer,
                                            MAX_FRAME_BUFFER,
                                            portMAX_DELAY );
        if (byte_size > 0)
        {
            if(can_buffer_send(tx_buffer, byte_size) != RT_OK)
            {
                PRINT_LOG(LOG_ERROR, HAL_GetTick(), "write failed, size:%d\n", byte_size);
            }
        }
        else
        {
            PRINT_LOG(LOG_ERROR, HAL_GetTick(), "canTxTask Message failed\n");
        }
    }
}

static void canRxTask(void *argument)
{
    uint8_t rx_buffer[16];
    uint8_t index;
    size_t xReceivedBytes;
    const TickType_t xBlockTime = pdMS_TO_TICKS( 100 );
    
    for(;;)
    {
        xReceivedBytes = xMessageBufferReceive( can_info.xRxMessageBuffer,
                                            ( void * ) rx_buffer,
                                            sizeof( rx_buffer ),
                                            xBlockTime );
        if (xReceivedBytes > 0)
        {
            for (index=0; index<xReceivedBytes; index++)
            {
                protocol_rx_frame(rx_buffer[index]);
                
                if (can_info.state == CAN_FRAME_COMPLETE)
                {
                    can_info.state = CAN_FRAME_EMPTY;
                    can_rx_protocol_process();
                }
                else if(can_info.state == CAN_FRAME_EMPTY)
                {
                    can_info.rx_framesize = 0;
                }
                else
                {
                    // in receive
                }
            }
        }
    }
}

static GlobalType_t can_buffer_send(uint8_t *data, uint16_t len)
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
#if CAN_WRITE_STD == 1
            ret = can_write_std(ptr, MAX_CAN_DLC);
#else
            ret = can_write_ext(ptr, MAX_CAN_DLC);
#endif
            if (ret != RT_OK) 
            {
                return ret;
            }
            ptr += 8;
        }
    }
    
    if (offset > 0)
    {
#if CAN_WRITE_STD == 1
        ret = can_write_std(ptr, offset);
#else
        ret = can_write_ext(ptr, offset);
#endif 
    }
    return ret;
}

GlobalType_t can_write_std(uint8_t *data, uint8_t len)
{	
    CAN_TxHeaderTypeDef txHeader;
    uint32_t mailbox;
    uint8_t index;
    
    if (len > MAX_CAN_DLC) {
        return RT_FAIL;
    }
    
    txHeader.StdId = CAN_STD_ID_DATA;
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

GlobalType_t can_write_ext(uint8_t *data, uint8_t len)
{	
    CAN_TxHeaderTypeDef txHeader;
    uint32_t mailbox;
    uint8_t index;
    
    if (len > MAX_CAN_DLC) {
        return RT_FAIL;
    }
    
    txHeader.ExtId = CAN_EXT_ID_DATA;
    txHeader.IDE = CAN_ID_EXT;
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
