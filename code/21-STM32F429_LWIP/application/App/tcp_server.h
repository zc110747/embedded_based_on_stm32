//////////////////////////////////////////////////////////////////////////////
//  (c) copyright 2023-by Persional Inc.  
//  All Rights Reserved
//
//  Name:
//      tcp_server.h
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
#ifndef _INCLUDE_TCP_SERVER_H
#define _INCLUDE_TCP_SERVER_H

#include "drv_global.h"
#include "cmsis_os.h"
#include "lwip.h"
#include "semphr.h"

typedef struct
{
	uint8_t length;
	char *data;
}LOGGER_MESSAGE;

typedef struct
{
	LOG_LEVEL level;

	QueueHandle_t xTxQueue;
}LoggerSystem_t;

typedef struct
{
	/* Device Net Info */
	uint8_t ipaddr[4];
	uint8_t gateway[4];
	uint8_t netmask[4];
    uint8_t serverip[4];
	uint16_t logger_port;
}NetInfo_t;

GlobalType_t tcp_server_init(void);
NetInfo_t *get_net_info(void);
#endif
