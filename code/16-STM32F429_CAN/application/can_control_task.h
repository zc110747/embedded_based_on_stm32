//////////////////////////////////////////////////////////////////////////////
//  (c) copyright 2023-by Persional Inc.  
//  All Rights Reserved
//
//  Name:
//      can_control_task.h
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
#ifndef __CAN_CONTROL_TASK_H
#define __CAN_CONTROL_TASK_H

#include "drv_global.h"
#include "FreeRTOS.h"
#include "message_buffer.h"
#include "semphr.h"
#include "cmsis_os.h"

#define CAN_WRITE_STD               0

#define MAX_CAN_DLC                 8
#define MAX_FRAME_BUFFER            548         //允许的最大包格式

#define MESSAGE_RX_BUFFER_SIZE      768
#define MESSAGE_TX_BUFFER_SIZE      1280

#define CAN_STD_ID_DATA             0x12
#define CAN_EXT_ID_DATA             0x1234

typedef struct
{
    osThreadId_t canRxTaskHandle;
    osThreadId_t canTxTaskHandle;

    MessageBufferHandle_t xRxMessageBuffer;
    MessageBufferHandle_t xTxMessageBuffer;
    SemaphoreHandle_t xSemaphore;
    
    // can protocol process
	uint16_t state;
	uint32_t last_timer;
    uint8_t rx_framebuffer[MAX_FRAME_BUFFER];
    uint16_t rx_framesize;
}CAN_CONTROL_INFO;

GlobalType_t can_control_task_init(void);
GlobalType_t can_write_ext(uint8_t *data, uint8_t len);
GlobalType_t can_write_std(uint8_t *data, uint8_t len);
GlobalType_t can_message_send(uint8_t *pdate, uint16_t size);
#endif
