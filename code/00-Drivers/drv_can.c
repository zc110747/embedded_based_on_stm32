//////////////////////////////////////////////////////////////////////////////
//  (c) copyright 2023-by Persional Inc.  
//  All Rights Reserved
//
//  Name:
//      drv_can.c
//
//  Purpose:
//      driver for can module.
//
//  Author:
//      @zc
//
//  Assumptions:
//	
//
//  Revision History:
//
/////////////////////////////////////////////////////////////////////////////
#include "drv_can.h"
#include "circular_buffer.h"

#define CAN_RX_BUFFER_SIZE 255

static uint8_t CanRxBuffer[CAN_RX_BUFFER_SIZE];
static volatile CircularBuffer CanCircularBuffer;

//------ static internal function ------

//------ static internal variable ------
static CAN_HandleTypeDef hcan1;
static CAN_FilterTypeDef  hcan1_filter;

//------ global function ------
GlobalType_t can_driver_init(void)
{
    GPIO_InitTypeDef GPIO_InitStruct = {0};
    
    __HAL_RCC_CAN1_CLK_ENABLE();
    __HAL_RCC_GPIOA_CLK_ENABLE();
    
    /* CAN GPIO Init */
    GPIO_InitStruct.Pin = GPIO_PIN_11|GPIO_PIN_12;
    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
    GPIO_InitStruct.Alternate = GPIO_AF9_CAN1;
    HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);
    
    /* CAN Module Init */
    hcan1.Instance = CAN1;
    hcan1.Init.Prescaler = 6;
    hcan1.Init.Mode = CAN_MODE_LOOPBACK;
    hcan1.Init.SyncJumpWidth = CAN_SJW_1TQ;
    hcan1.Init.TimeSeg1 = CAN_BS1_8TQ;
    hcan1.Init.TimeSeg2 = CAN_BS2_6TQ;
    hcan1.Init.TimeTriggeredMode = DISABLE;
    hcan1.Init.AutoBusOff = ENABLE;
    hcan1.Init.AutoWakeUp = DISABLE;
    hcan1.Init.AutoRetransmission = ENABLE;
    hcan1.Init.ReceiveFifoLocked = DISABLE;
    hcan1.Init.TransmitFifoPriority = DISABLE;
    if (HAL_CAN_Init(&hcan1) != HAL_OK)
    {
        return RT_FAIL;
    }
 
    hcan1_filter.FilterIdHigh = 0x0000;                     //32位ID
    hcan1_filter.FilterIdLow = 0x0000;
    hcan1_filter.FilterMaskIdHigh  = 0x0000;                //32位MASK
    hcan1_filter.FilterMaskIdLow = 0x0000;  
    hcan1_filter.FilterFIFOAssignment = CAN_FILTER_FIFO0;   //过滤器0关联到FIFO0
    hcan1_filter.FilterBank = 0;                            //过滤器0
    hcan1_filter.FilterMode = CAN_FILTERMODE_IDMASK;
    hcan1_filter.FilterScale = CAN_FILTERSCALE_32BIT;
    hcan1_filter.FilterActivation = ENABLE;                 //激活滤波器0
    hcan1_filter.SlaveStartFilterBank = 14;
	
    if(HAL_CAN_ConfigFilter(&hcan1, &hcan1_filter) != HAL_OK)
    {
        return RT_FAIL;    
    }

    HAL_NVIC_SetPriority(CAN1_RX0_IRQn, 0, 0);
    HAL_NVIC_EnableIRQ(CAN1_RX0_IRQn);
    
    /* start can run */
    HAL_CAN_Start(&hcan1);
    HAL_CAN_ActivateNotification(&hcan1, CAN_IT_RX_FIFO0_MSG_PENDING);
    
    CircularBufferInit(&CanCircularBuffer, LOGGER_BUFFER_SIZE, CanRxBuffer);
    
    return RT_OK;
}    

void CAN1_RX0_IRQHandler(void)
{
    HAL_CAN_IRQHandler(&hcan1);
}

void HAL_CAN_RxFifo0MsgPendingCallback(CAN_HandleTypeDef* hcan)
{
    CAN_RxHeaderTypeDef RxMessage;
    uint8_t Data[8];
    
    if (HAL_CAN_GetState(&hcan1) != RESET)
    {
        uint8_t index;
        HAL_CAN_GetRxMessage(&hcan1, CAN_RX_FIFO0, &RxMessage, Data);
        
        for(index=0; index<RxMessage.DLC; index++)
        {
            CircularBufferPut(&CanCircularBuffer, Data[index]);
        }
    }
}

GlobalType_t can_driver_send(uint8_t* data, uint8_t len)
{	
    CAN_TxHeaderTypeDef txHeader;
    uint32_t mailbox;
    
    txHeader.StdId = 0x12;
    txHeader.IDE = CAN_ID_STD;
    txHeader.RTR = CAN_RTR_DATA;
    txHeader.DLC = len;
    
    if(HAL_CAN_AddTxMessage(&hcan1, &txHeader, data, &mailbox) == HAL_OK)
    {
        return RT_OK;
    }
    return RT_FAIL;		
}

int can_driver_receive(uint8_t *data, uint8_t len)
{
    if(CircularBufferHasData(&CanCircularBuffer))
    {
        uint8_t size, index;
        
        size = CircularBufferSize(&CanCircularBuffer);
        size = len>size?size:len;
        
        for(index=0; index<size; index++)
        {
            CircularBufferGet(&CanCircularBuffer, data[index]);
        }
        return size;
    }
    
    return -1;
}
