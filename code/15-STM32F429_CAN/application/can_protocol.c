//////////////////////////////////////////////////////////////////////////////
//  (c) copyright 2023-by Persional Inc.  
//  All Rights Reserved
//
//  Name:
//      can_protocol.c
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
#include "can_protocol.h"
#include "can_control_task.h"

extern CAN_CONTROL_INFO can_info;

static uint16_t crc16_modbus(uint8_t *data, uint16_t length);
static void can_reqeust_show_info(uint8_t *pbuffer, uint16_t len);

/*
协议格式
head(1) | version(1) |  cmd(1) | size(2) | data(size) | crc16(2) |
*/
void protocol_rx_frame(uint8_t data)
{
    if(can_info.state == CAN_FRAME_EMPTY)
    {
        if (data == CAN_UI_HEADR)
        {
            can_info.rx_framebuffer[0] = data;
            can_info.rx_framesize = 1;
            can_info.state = CAN_FRAME_RECEIVED;
            can_info.last_timer = HAL_GetTick();
        }
    }
    else if(can_info.state == CAN_FRAME_RECEIVED)
    {
        can_info.rx_framebuffer[can_info.rx_framesize++] = data;
        if (can_info.rx_framesize > CAN_HEAD_SIZE)
        {
            uint16_t frame_size = (((uint16_t)can_info.rx_framebuffer[3]<<8) | can_info.rx_framebuffer[4]) + 7;
            if (frame_size > MAX_FRAME_BUFFER)
            {
                PRINT_LOG(LOG_ERROR, HAL_GetTick(), "can frame too large!");
				can_info.state = CAN_FRAME_EMPTY;
            }
            else 
            {
                if (can_info.rx_framesize == frame_size)
                {
                    uint16_t checksum = crc16_modbus(&can_info.rx_framebuffer[0], frame_size - 2);
					if(checksum == (can_info.rx_framebuffer[frame_size-2]<<8 | can_info.rx_framebuffer[frame_size-1]))
					{
						can_info.state = CAN_FRAME_COMPLETE;
                        
                        //去除crc校验位
                        can_info.rx_framesize -= 2;
					}
					else
					{
						can_info.state = CAN_FRAME_EMPTY;
						PRINT_LOG(LOG_INFO, HAL_GetTick(), "can frame crc error!");
					}
                }
            }
        }
    }
    else
    {
        // error process
    }
}

uint16_t create_can_tx_buffer(uint8_t cmd, uint8_t *pbuffer, uint16_t len, uint8_t *out_buffer)
{
    uint16_t size = 0;
    uint16_t crc_value;

    out_buffer[size++] = CAN_UI_HEADR;
    out_buffer[size++] = CAN_PROTOCOL_VERSION;
    out_buffer[size++] = cmd;
    out_buffer[size++] = len >> 8;
    out_buffer[size++] = len&0xff;

    if (len > 0)
    {
        memcpy(&out_buffer[size], pbuffer, len);
        size += len;
    }   
    
    crc_value = crc16_modbus(out_buffer, size);
    out_buffer[size++] = crc_value >> 8;
    out_buffer[size++] = crc_value&0xff;

    return size;
}

GlobalType_t can_rx_protocol_process(void)
{
    uint8_t version, cmd;
    
    version = can_info.rx_framebuffer[1];
    cmd = can_info.rx_framebuffer[2];
    
    if (version > CAN_PROTOCOL_VERSION)
    {
        PRINT_LOG(LOG_ERROR, HAL_GetTick(), "can frame version not support!");
        return RT_FAIL;
    }

    switch(cmd)
    {
        case CAN_REQUEST_SHOW_INFO:
            can_reqeust_show_info(&can_info.rx_framebuffer[5], can_info.rx_framesize-5);
            break;
        default:
            break;
    }

    return RT_OK;
}

static void can_reqeust_show_info(uint8_t *pbuffer, uint16_t len)
{
    PRINT_LOG(LOG_INFO, HAL_GetTick(), "rx_data:0x%x, size:%d", pbuffer[0], len);
}

static uint16_t crc16_modbus(uint8_t *data, uint16_t length)
{
    uint8_t i;
    uint16_t crc = 0xffff;        // Initial value
    while(length--)
    {
        crc ^= *data++;            // crc ^= *data; data++;
        for (i = 0; i < 8; ++i)
        {
            if (crc & 1)
                crc = (crc >> 1) ^ 0xA001;        // 0xA001 = reverse 0x8005
            else
                crc = (crc >> 1);
        }
    }
    return crc;
}
