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

#define MAX_CAN_DLC             8
#define MESSAGE_BUFFER_SIZE     768

typedef struct
{
    MessageBufferHandle_t xMessageBuffer;
}CAN_CONTROL_INFO;

GlobalType_t can_control_task_init(void);
GlobalType_t can_write_buffer(uint8_t *data, uint16_t len);
#endif
