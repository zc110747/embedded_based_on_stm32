//////////////////////////////////////////////////////////////////////////////
//  (c) copyright 2025-by Persional Inc.  
//  All Rights Reserved
//
//  Name:
//      drv_logger_uart.h
//
//  Purpose:
// 
//
//  Author:
//      @zc
//
//  Assumptions:
//	
//
//  Revision History:
//
/////////////////////////////////////////////////////////////////////////////
#ifndef __DRV_LOGGER_UART_H__
#define __DRV_LOGGER_UART_H__

#include "logger.h"

// logger uart hw config
#define LOG_UART_TX_PIN         GPIO_PIN_9
#define LOG_UART_TX_PORT        GPIOA
#define LOG_UART_RX_PIN         GPIO_PIN_10
#define LOG_UART_RX_PORT        GPIOA
#define LOG_UART_AF             GPIO_AF7_USART1
#define LOG_UART_CLK_ENABLE()   do {    __HAL_RCC_GPIOA_CLK_ENABLE(); \
                                        __HAL_RCC_USART1_CLK_ENABLE(); \
                                } while (0)

#define LOG_UART_MODULE         USART1
#define LOG_UART_BAUDRATE       115200
#define LOG_UART_DATA_BITS      UART_WORDLENGTH_8B
#define LOG_UART_STOP_BITS      UART_STOPBITS_1
#define LOG_UART_PARITY         UART_PARITY_NONE

#define LOG_UART_IRQn           USART1_IRQn
#define LOG_UART_IRQHandler     USART1_IRQHandler

// interrupt priority
#define LOG_UART_PRE_PRIO       0
#define LOG_UART_SUB_PRIO       1
                                
void logger_uart_rx_process(void);
LOG_STATUS logger_process_callback(uint8_t *buffer, uint16_t len);
#endif
