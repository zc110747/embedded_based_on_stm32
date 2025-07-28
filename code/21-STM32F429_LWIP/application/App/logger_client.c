//////////////////////////////////////////////////////////////////////////////
//  (c) copyright 2023-by Persional Inc.  
//  All Rights Reserved
//
//  Name:
//      logger_client.c
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
#include <stdbool.h>
#include "logger_client.h"
#include "lwip/sockets.h"
#include "SEGGER_RTT.h"

#define LOGGER_TX_QUEUE_LEN  					64
#define LOGGER_RECV_BUFFER_SIZE		            256

/* static internal Parameter */
static volatile LoggerSystem_t LoggerSystem = {
		.level = LOG_TRACE,
		.xTxQueue = NULL,
};
static volatile int loggerfd = -1;
static volatile int is_logger_connected = -1;

//logger memory manage
static uint8_t logger_recv_buffer[LOGGER_RECV_BUFFER_SIZE];
static osThreadId_t TcpTaskHandle, LoggerTxHandle;
static const osThreadAttr_t TcpTask_attributes = {
  .name = "tcp_task",
  .stack_size = 1024 * 4,
  .priority = (osPriority_t) osPriorityHigh,
};
static const osThreadAttr_t LoggerTxTask_attributes = {
  .name = "logger_tx_task",
  .stack_size = 1024 * 4,
  .priority = (osPriority_t) osPriorityHigh,
};

static NetInfo_t NetInfo = {
	/* Device Net Info */
	.ipaddr = {192, 168, 10, 99},
    .gateway = {192, 168, 10, 1},
    .netmask = {255, 255, 255, 0},
    .serverip = {192, 168, 10, 156},
    .logger_port = 15059, 
};
    
extern u8_t netif_link_up(void);
void logger_client_task(void *argument)
{
    struct sockaddr_in server_addr;

    ip_addr_t ipaddr;
    int bytes_received;

    IP4_ADDR(&ipaddr, NetInfo.serverip[0],
                NetInfo.serverip[1],
                NetInfo.serverip[2],
                NetInfo.serverip[3]);
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(NetInfo.logger_port);
    server_addr.sin_addr.s_addr = ipaddr.addr; //0x2801000a

	for( ;; )
	{
		//only check link up
		if(!netif_link_up())
		{
			vTaskDelay(1000);
			continue;
		}

		loggerfd = socket(AF_INET, SOCK_STREAM, 0);
		if(loggerfd== -1)
		{
			PRINT_LOG(LOG_ERROR, xTaskGetTickCount(), "logger Socket Alloc Failed!");
			vTaskDelay(1000);
			continue;
		}

		is_logger_connected = connect(loggerfd, (struct sockaddr *)&server_addr, sizeof(struct sockaddr));
		if(is_logger_connected == -1)
		{
			//PRINTLOG(LOG_WARN, xTaskGetTickCount(), "logger Socket Connect Failed");
			lwip_close(loggerfd);
			vTaskDelay(3000);
			continue;
		}
		else
		{
			int flag;
			struct timeval console_time;

			console_time.tv_sec = 6;
			console_time.tv_usec = 0;
			setsockopt(loggerfd, IPPROTO_TCP, TCP_NODELAY, (void *)&flag, sizeof(int));
			setsockopt(loggerfd, SOL_SOCKET, SO_RCVTIMEO, (char*)&console_time, sizeof(console_time));
			console_time.tv_sec = 2;
			setsockopt(loggerfd, SOL_SOCKET, SO_SNDTIMEO, (char*)&console_time, sizeof(console_time));

			for( ;; )
			{
				bytes_received = recv(loggerfd, logger_recv_buffer, LOGGER_RECV_BUFFER_SIZE, 0);
				if(bytes_received <= 0)
				{
					vTaskDelay(10);
                    
                    // timeout bytes received is -1, no connnected bytes received is 0
					if(is_logger_connected == -1 || bytes_received == 0) {
						break;
                    }
					PRINT_LOG(LOG_WARN, xTaskGetTickCount(), "no cycle send every 6 second for logger!!!");
				}
				else
				{
					logger_recv_buffer[bytes_received] = '\0';
					PRINT_LOG(LOG_WARN, xTaskGetTickCount(), "tcp receive:%s", logger_recv_buffer);
				}
			}

			is_logger_connected = -1;
			close(loggerfd);
			loggerfd= -1;
			vTaskDelay(1000);  //connect 1000ms peroid, when no link
		}
	}    
}

GlobalType_t eth_logger_write(uint8_t *ptr, uint8_t size)
{   
    if(LoggerSystem.xTxQueue != NULL)
    {
        LOGGER_MESSAGE msg;
        msg.data = (char *)ptr;
        msg.length = size;
        if(xQueueSend(LoggerSystem.xTxQueue, &msg, (portTickType)1) != pdTRUE)
        {
        }
    }
    else
    {
        SEGGER_RTT_Write(0, ptr, size);
    }
    return RT_OK;   
}

NetInfo_t *get_net_info(void)
{
    return &NetInfo;
}
    
static void logger_tx_thread(void *arg)
{
	LOGGER_MESSAGE msg;

	while(1)
	{
		if (xQueueReceive(LoggerSystem.xTxQueue, &msg, portMAX_DELAY) == pdPASS)
		{
			if(is_logger_connected >= 0)
			{
				if(send(loggerfd, msg.data, msg.length, MSG_DONTWAIT)<0)
				{
					is_logger_connected = -1;
					PRINT_LOG(LOG_ERROR, xTaskGetTickCount(), "logger Net write failed!");
				}
			}
			else
			{
				SEGGER_RTT_Write(0, msg.data, msg.length);
			}
			vTaskDelay(10);
		}
	}
}

GlobalType_t logger_client_init(void)
{
    LoggerSystem.xTxQueue = xQueueCreate(LOGGER_TX_QUEUE_LEN, sizeof(LOGGER_MESSAGE));
    
    TcpTaskHandle = osThreadNew(logger_client_task, NULL, &TcpTask_attributes);
    if(TcpTaskHandle == NULL)
    {
        return RT_FAIL;
    }
    
    LoggerTxHandle = osThreadNew(logger_tx_thread, NULL, &LoggerTxTask_attributes);
    if(LoggerTxHandle == NULL)
    {
        return RT_FAIL;
    }
  
    return RT_OK;
}
