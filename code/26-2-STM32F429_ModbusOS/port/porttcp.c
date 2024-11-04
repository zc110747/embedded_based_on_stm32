/*
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
 *
 * File: $Id: portserial.c,v 1.60 2019/07/11 17:04:32 flybreak $
 */

#include "port.h"
#include <string.h>
#include "includes.h"
#include "cmsis_os.h"

/* ----------------------- Modbus includes ----------------------------------*/
#include "mb.h"
#include "mbport.h"
#include "mbtcp.h"
#include "mbconfig.h"
#include "lwip/err.h"
#include "lwip/sockets.h"
#include "lwip/sys.h"


/* mb_tcp 格式 */
/* 
1. 读线圈状态(0x01)
命令: 00 00 00 00 00 06 01 01 00 00 00 20
| 事务处理标识(2byte) | 协议标识(2byte) | 长度(2byte) | 单元标识(1byte) | 功能码(1byte) | 起始地址(2byte) | 数据长度(2byte) |
| 0x00 0x00          | 0x00 0x00      | 0x00 0x06   | 0x01           | 0x01          | 0x00 0x00      | 00 20          |
应答:
| 事务处理标识(2byte) | 协议标识(2byte) | 长度(2byte) | 单元标识(1byte) | 功能码(1byte) | 字节计数(1byte) |  数据          |
| 0x00 0x00          | 0x00 0x00      | 0x00 0x07   | 0x01           | 0x01          | 0x04           | 10 01 01 00    |

2. 写单个线圈(0x05)
命令: 00 00 00 00 00 06 01 05 00 00 FF 00
| 事务处理标识(2byte) | 协议标识(2byte) | 长度(2byte) | 单元标识(1byte) | 功能码(1byte) | 起始地址(2byte) | 写入值 |
| 0x00 0x00          | 0x00 0x00      | 0x00 0x06   | 0x01           | 0x05          | 0x00 0x00      | FF 00 |
应答:
| 事务处理标识(2byte) | 协议标识(2byte) | 长度(2byte) | 单元标识(1byte) | 功能码(1byte) | 起始地址(2byte) | 写入值 |
| 0x00 0x00          | 0x00 0x00      | 0x00 0x06  | 0x01           | 0x05          | 0x00 0x00      | FF 00 |

3. 写多个线圈(0x0F)
命令: 00 00 00 00 00 09 01 0F 00 00 00 10 02 1C A0
| 事务处理标识(2byte) | 协议标识(2byte) | 长度(2byte) | 单元标识(1byte) | 功能码(1byte) | 起始地址(2byte) | 线圈数量(2byte) | 字节计数(1byte) | 写入值 |
| 0x00 0x00          | 0x00 0x00      | 0x00 0x09   | 0x01           | 0x0F          | 0x00 0x00      | 0x00 0x10      |  0x02          | 1C A0 |
应答:
| 事务处理标识(2byte) | 协议标识(2byte) | 长度(2byte) | 单元标识(1byte) | 功能码(1byte) | 起始地址(2byte) | 线圈数量(2byte) |
| 0x00 0x00          | 0x00 0x00      | 0x00 0x06  | 0x01            | 0x0F          | 0x00 0x00       | 00 10         |
*/

#if MB_TCP_ENABLED > 0
/* ----------------------- Defines  -----------------------------------------*/
#define TCP_RX_BUFFER_SIZE      1024

static int server_fd, client_fd;
static UCHAR rx_buffer[TCP_RX_BUFFER_SIZE];
static int rx_size;
static osThreadId_t mbTcpHandle;
static const osThreadAttr_t mbTcpTask_attributes = {
  .name = "mbtcp_task",
  .stack_size = 1024 * 1,
  .priority = (osPriority_t) osPriorityHigh,
};

void mb_tcp_process_task(void *argument)
{
    struct sockaddr_in server_addr;
    struct sockaddr_in client_addr;
    socklen_t client_addr_len;
    int opt = 1;
    USHORT port = *(USHORT *)argument;
    
    server_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (server_fd == -1) 
    {
        PRINT_LOG(LOG_ERROR, xTaskGetTickCount(), "无法创建socket\n");
        return;
    }

    setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));

    // 定义服务器地址结构
    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    server_addr.sin_port = htons(port);

    // 绑定socket到服务器地址
    if (bind(server_fd, (struct sockaddr*)&server_addr, sizeof(server_addr)) < 0) {
        PRINT_LOG(LOG_ERROR, xTaskGetTickCount(), "can't bind to socket %d\n", port);
        return;
    }

    // 监听socket
    listen(server_fd, 3);

    PRINT_LOG(LOG_INFO, xTaskGetTickCount(), "server listen success, fd:%d, port:%d", server_fd, port);

    while (1) 
    {
        //接收客户端连接
        client_addr_len = sizeof(client_addr);
        client_fd = accept(server_fd, (struct sockaddr*)&client_addr, &client_addr_len);
        if (client_fd < 0) {
            PRINT_LOG(LOG_ERROR, xTaskGetTickCount(), "client accept failed!");
            continue;
        }

        PRINT_LOG(LOG_INFO, xTaskGetTickCount(), "client connect success!");

        while (1) 
        {
            rx_size = recv(client_fd, rx_buffer, TCP_RX_BUFFER_SIZE, 0);
            if (rx_size < 0) 
            {
                PRINT_LOG(LOG_ERROR, xTaskGetTickCount(), "recv data failed");
                break;
            } 
            else if (rx_size == 0) 
            {
                PRINT_LOG(LOG_ERROR, xTaskGetTickCount(), "client client failed");
                break;
            }
            else
            {
                //接收到数据，通知应用层处理
                xMBPortEventPost(EV_FRAME_RECEIVED);
            }
        }
        
        //检测失败，断开现有连接
        close(client_fd);
        client_fd = -1;
    }
}

BOOL
xMBTCPPortInit(USHORT usTCPPort)
{
    mbTcpHandle = osThreadNew(mb_tcp_process_task, &usTCPPort, &mbTcpTask_attributes);
    if(mbTcpHandle == NULL)
    {
        return FALSE;
    }

    return TRUE;
}

void
vMBTCPPortDisable(void)
{
    close(client_fd);
    client_fd = -1;
}

BOOL
xMBTCPPortGetRequest(UCHAR **ppucMBTCPFrame, USHORT *usTCPLength)
{
    *ppucMBTCPFrame = &rx_buffer[0];
    *usTCPLength = rx_size;
    return TRUE;
}

BOOL
xMBTCPPortSendResponse(const UCHAR *pucMBTCPFrame, USHORT usTCPLength)
{
    int ret;
    BOOL bFrameSent = FALSE;

    if (client_fd)
    {
        ret = send(client_fd, (void *)pucMBTCPFrame, usTCPLength, 0);
        if (ret == usTCPLength)
        {
            bFrameSent = TRUE;
        }
    }
    return bFrameSent;
}
#endif
