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

//static internal variable
static CAN_FilterTypeDef  hcan1_filter;

// global variable
CAN_HandleTypeDef hcan1;

//static internal function

//global function
GlobalType_t can_driver_init(void)
{
    GPIO_InitTypeDef GPIO_InitStruct = {0};
    
    __HAL_RCC_CAN1_CLK_ENABLE();
    __HAL_RCC_GPIOA_CLK_ENABLE();
    
    /* CAN GPIO Init */
    GPIO_InitStruct.Pin = GPIO_PIN_11 | GPIO_PIN_12;
    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
    GPIO_InitStruct.Alternate = GPIO_AF9_CAN1;
    HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);
    
    /* CAN Module Init */
    hcan1.Instance = CAN1;
    hcan1.Init.Prescaler = 6;
    //hcan1.Init.Mode = CAN_MODE_LOOPBACK;                 //环回模式, 测试CAN接口
    hcan1.Init.Mode = CAN_MODE_NORMAL;                     //normal mode
    hcan1.Init.SyncJumpWidth = CAN_SJW_1TQ;
    hcan1.Init.TimeSeg1 = CAN_BS1_8TQ;
    hcan1.Init.TimeSeg2 = CAN_BS2_6TQ;
    hcan1.Init.TimeTriggeredMode = DISABLE;
    hcan1.Init.AutoBusOff = ENABLE;                         //自动的总线关闭管理，一旦监测到128次连续11个隐性位，即通过硬件自动退出总线关闭状态
    hcan1.Init.AutoWakeUp = DISABLE;
    hcan1.Init.AutoRetransmission = DISABLE;
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

    HAL_NVIC_SetPriority(CAN1_RX0_IRQn, 5, 0);
    HAL_NVIC_EnableIRQ(CAN1_RX0_IRQn);
    
    /* start can run */
    HAL_CAN_Start(&hcan1);
    HAL_CAN_ActivateNotification(&hcan1, CAN_IT_RX_FIFO0_MSG_PENDING);
    
    return RT_OK;
}    

void CAN1_RX0_IRQHandler(void)
{
    HAL_CAN_IRQHandler(&hcan1);
}
