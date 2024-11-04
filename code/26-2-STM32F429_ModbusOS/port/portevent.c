/*
  * FreeModbus Libary: LPC214X Port
  * Copyright (C) 2007 Tiago Prado Lone <tiago@maxwellbohr.com.br>
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
 * File: $Id$
 */

/* ----------------------- Modbus includes ----------------------------------*/
#include "mb.h"
#include "port.h"
#include "mbport.h"
#include "cmsis_os.h"
#include "event_groups.h"
#include "stm32f4xx_hal.h"

/* ----------------------- Variables ----------------------------------------*/
#define USE_OS_EVENT   1

/* ----------------------- Start implementation -----------------------------*/
#if USE_OS_EVENT == 1
static EventGroupHandle_t xCreatedEventGroup;

BOOL
xMBPortEventInit( void )
{
    xCreatedEventGroup = xEventGroupCreate();
    
    if (xCreatedEventGroup == NULL) {
        return FALSE;
    }
    return TRUE;
}

BOOL
xMBPortEventPost( eMBEventType eEvent )
{
    BaseType_t xHigherPriorityTaskWoken = pdFALSE;
    
    if(IS_IRQ()) {
        xEventGroupSetBitsFromISR(xCreatedEventGroup, eEvent, &xHigherPriorityTaskWoken);
    }
    else {
        xEventGroupSetBits(xCreatedEventGroup, eEvent);
    }
    
    return TRUE;
}

BOOL
xMBPortEventGet( eMBEventType * eEvent )
{
    EventBits_t uxBits;
    
    uxBits = xEventGroupWaitBits(
               xCreatedEventGroup,
               EV_READY | EV_FRAME_RECEIVED | EV_EXECUTE | EV_FRAME_SENT, 
               pdTRUE,
               pdFALSE,
               portMAX_DELAY);
    
    switch(uxBits) {
        case EV_READY:
            *eEvent = EV_READY;
            break;
        case EV_FRAME_RECEIVED:
            *eEvent = EV_FRAME_RECEIVED;
            break;
        case EV_EXECUTE:
            *eEvent = EV_EXECUTE;            
            break;
        case EV_FRAME_SENT:
            *eEvent = EV_FRAME_SENT; 
            break;
    }
    return TRUE;
}
#else
static eMBEventType eQueuedEvent;
static BOOL     xEventInQueue;

BOOL
xMBPortEventInit( void )
{
    xEventInQueue = FALSE;
    return TRUE;
}

BOOL
xMBPortEventPost( eMBEventType eEvent )
{
    xEventInQueue = TRUE;
    eQueuedEvent = eEvent;
    return TRUE;
}

BOOL
xMBPortEventGet( eMBEventType * eEvent )
{
    BOOL            xEventHappened = FALSE;

    if( xEventInQueue )
    {
        *eEvent = eQueuedEvent;
        xEventInQueue = FALSE;
        xEventHappened = TRUE;
    }
    return xEventHappened;
}
#endif
