//////////////////////////////////////////////////////////////////////////////
//  (c) copyright 2023-by Persional Inc.  
//  All Rights Reserved
//
//  Name:
//      can_protocol.h
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
#ifndef __CAN_PROTOCOL_H
#define __CAN_PROTOCOL_H

#include "drv_global.h"

#define CAN_FRAME_EMPTY			    0
#define CAN_FRAME_RECEIVED			1
#define CAN_FRAME_COMPLETE			2

#define CAN_UI_HEADR				0x3C

#define CAN_HEAD_SIZE               5

#define CAN_PROTOCOL_VERSION        0x01

#define CAN_REQUEST_SHOW_INFO       0x01
#define CAN_REQUEST_NULL            0x02


void protocol_rx_frame(uint8_t data);
GlobalType_t can_rx_protocol_process(void);
uint16_t create_can_tx_buffer(uint8_t cmd, uint8_t *pbuffer, uint16_t len, uint8_t *out_buffer);
#endif
