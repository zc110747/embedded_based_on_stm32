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

/* ----------------------- Modbus includes ----------------------------------*/
#include "mb.h"
#include "mbport.h"
#include "mbtcp.h"
#include "mbconfig.h"

#if MB_TCP_ENABLED > 0
/* ----------------------- Defines  -----------------------------------------*/
#define TCP_RX_BUFFER_SIZE      1024

BOOL
xMBTCPPortInit(USHORT usTCPPort)
{
    return TRUE;
}

void
vMBTCPPortDisable(void)
{
}

BOOL
xMBTCPPortGetRequest(UCHAR **ppucMBTCPFrame, USHORT *usTCPLength)
{
    return TRUE;
}

BOOL
xMBTCPPortSendResponse(const UCHAR *pucMBTCPFrame, USHORT usTCPLength)
{
    return TRUE;
}
#endif
