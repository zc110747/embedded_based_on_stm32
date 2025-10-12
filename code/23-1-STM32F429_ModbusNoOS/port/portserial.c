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

#include "stm32f4xx_hal.h"
#include "port.h"
#include "drv_usart.h"

/* ----------------------- Modbus includes ----------------------------------*/
#include "mb.h"
#include "mbport.h"

/* ----------------------- static parameter ---------------------------------*/
static UART_HandleTypeDef huart2;

/* ----------------------- static functions ---------------------------------*/
static void prvvUARTTxReadyISR( void );
static void prvvUARTRxISR( void );

/* ----------------------- Start implementation -----------------------------*/
void vMBPortSerialEnable( BOOL xRxEnable, BOOL xTxEnable )
{
    if (xRxEnable)
    {
        __HAL_UART_ENABLE_IT(&huart2, UART_IT_RXNE);    
    }
    else
    {
        __HAL_UART_DISABLE_IT(&huart2, UART_IT_RXNE);         
    }
    
    if (xTxEnable)
    {
        __HAL_UART_ENABLE_IT(&huart2, UART_IT_TXE);
        prvvUARTTxReadyISR(  );        
    }
    else
    {
        __HAL_UART_DISABLE_IT(&huart2, UART_IT_TXE);      
    }
}

void
vMBPortClose( void )
{
    __HAL_UART_DISABLE(&huart2);
}

BOOL
xMBPortSerialInit( UCHAR ucPORT, ULONG ulBaudRate, UCHAR ucDataBits, eMBParity eParity )
{
    BOOL bInitialized = TRUE;
    GPIO_InitTypeDef GPIO_InitStruct = {0};
    uint32_t parity = UART_PARITY_NONE;

    if(eParity == MB_PAR_NONE)
    {
       parity = UART_PARITY_NONE; 
    }
    else if(eParity == MB_PAR_ODD)
    {
       parity = UART_PARITY_ODD;
    }
    else
    {
       parity = UART_PARITY_EVEN;
    }
    
    __HAL_RCC_USART2_CLK_ENABLE();
    __HAL_RCC_GPIOA_CLK_ENABLE();
    
    GPIO_InitStruct.Pin = GPIO_PIN_2 | GPIO_PIN_3;
    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
    GPIO_InitStruct.Alternate = GPIO_AF7_USART2;
    HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);
    
    huart2.Instance = USART2;
    huart2.Init.BaudRate = ulBaudRate;
    huart2.Init.WordLength = UART_WORDLENGTH_8B;
    huart2.Init.StopBits = UART_STOPBITS_1;
    huart2.Init.Parity = parity;
    huart2.Init.Mode = UART_MODE_TX_RX;
    huart2.Init.HwFlowCtl = UART_HWCONTROL_NONE;
    huart2.Init.OverSampling = UART_OVERSAMPLING_16;
    if (HAL_UART_Init(&huart2) != HAL_OK)
    {
        bInitialized = FALSE;
    }

    HAL_NVIC_SetPriority(USART2_IRQn, 0, 0);	
    HAL_NVIC_EnableIRQ(USART2_IRQn);
    
    return bInitialized;
}

BOOL xMBPortSerialPutByte( CHAR ucByte )
{
    while (__HAL_UART_GET_FLAG(&huart2, UART_FLAG_TXE) == RESET)
    {}
    huart2.Instance->DR = ucByte;
    return TRUE;
}

BOOL xMBPortSerialGetByte( CHAR * pucByte )
{
    while (__HAL_UART_GET_FLAG(&huart2, UART_FLAG_RXNE) == RESET)
    {}
    *pucByte = huart2.Instance->DR;

    return TRUE;
}


void USART2_IRQHandler( void )
{
    if (__HAL_UART_GET_FLAG(&huart2, UART_FLAG_RXNE) != RESET)
    {
        prvvUARTRxISR();
    }
    
    if(__HAL_UART_GET_FLAG(&huart2, UART_FLAG_TXE) != RESET
    && __HAL_UART_GET_IT_SOURCE(&huart2, UART_IT_TXE))
    {
         prvvUARTTxReadyISR();
    }
    
    if(__HAL_UART_GET_FLAG(&huart2,  UART_FLAG_ORE | UART_FLAG_PE | UART_FLAG_NE | UART_FLAG_FE) != RESET)
    {
        __HAL_UART_CLEAR_PEFLAG(&huart2);
    }
}


/* 
 * Create an interrupt handler for the transmit buffer empty interrupt
 * (or an equivalent) for your target processor. This function should then
 * call pxMBFrameCBTransmitterEmpty( ) which tells the protocol stack that
 * a new character can be sent. The protocol stack will then call 
 * xMBPortSerialPutByte( ) to send the character.
 */
static void
prvvUARTTxReadyISR( void )
{
    pxMBFrameCBTransmitterEmpty(  );
}

/* 
 * Create an interrupt handler for the receive interrupt for your target
 * processor. This function should then call pxMBFrameCBByteReceived( ). The
 * protocol stack will then call xMBPortSerialGetByte( ) to retrieve the
 * character.
 */
static void
prvvUARTRxISR( void )
{
    pxMBFrameCBByteReceived(  );
}
