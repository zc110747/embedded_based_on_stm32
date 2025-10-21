//////////////////////////////////////////////////////////////////////////////
//  (c) copyright 2025-by Persional Inc.  
//  All Rights Reserved
//
//  Name:
//      drv_logger_uart.c
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
#include "drv_logger_uart.h"
#include "circular_buffer.h"
#include "logger.h"

#define LOGGER_RX_BUFFER_SIZE      (512)
#define LOGGER_TX_BUFFER_SIZE      (1024)

typedef struct 
{
    uint8_t is_ready;

    volatile CircularBuffer rx_buffer_info;

    uint8_t rx_buffer[LOGGER_RX_BUFFER_SIZE];

    volatile CircularBuffer tx_buffer_info;

    uint8_t tx_buffer[LOGGER_TX_BUFFER_SIZE];

    UART_HandleTypeDef uart_handle;
}LOG_UART_INFO;

static LOG_UART_INFO logger_uart_info = {0};

static LOG_STATUS logger_uart_hardware_init(void);
static LOG_STATUS circular_put_tx_buffer(uint8_t *ptr, uint8_t size);

LOG_STATUS logger_uart_init(void)
{
    LOG_STATUS ret = LOG_STATUS_OK;
    
    logger_uart_info.is_ready = 0;

    ret = logger_uart_hardware_init();
    if(ret != LOG_STATUS_OK) {
        return ret;
    }

    CircularBufferInit(&(logger_uart_info.tx_buffer_info), LOGGER_TX_BUFFER_SIZE, logger_uart_info.tx_buffer);
    CircularBufferInit(&(logger_uart_info.rx_buffer_info), LOGGER_RX_BUFFER_SIZE, logger_uart_info.rx_buffer);

    logger_uart_info.is_ready = 1;
    
    return ret;
}

#define RX_MAX_SIZE     64
static uint8_t rx_buffer[RX_MAX_SIZE];

__weak LOG_STATUS logger_process_callback(uint8_t *buffer, uint16_t len)
{
    return LOG_STATUS_OK;
}

void logger_uart_rx_process(void)
{
    char c;
    static uint16_t rx_bytes = 0;
    
    if (!CircularBufferIsEmpty(&logger_uart_info.rx_buffer_info)) {
        CircularBufferGet(&logger_uart_info.rx_buffer_info, c);
        
        if (c == '\n' || c == '\r' || rx_bytes > RX_MAX_SIZE - 1) {
            rx_buffer[rx_bytes] = '\0';
            logger_process_callback(rx_buffer, rx_bytes);
            rx_bytes = 0;
        } else {
            rx_buffer[rx_bytes++] = c;
        }
    }
}

LOG_STATUS logger_uart_transmit(uint8_t *buf, int len)
{
    LOG_STATUS ret = LOG_STATUS_OK;

    if (!logger_uart_info.is_ready) {
        return LOG_STATUS_ERROR;
    }

    ret = circular_put_tx_buffer(buf, len);
    
    return ret;
}

/// internal function
static LOG_STATUS logger_uart_hardware_init(void)
{
    LOG_STATUS ret = LOG_STATUS_OK;
    
    GPIO_InitTypeDef GPIO_InitStruct = {0};

    LOG_UART_CLK_ENABLE();

    GPIO_InitStruct.Pin = LOG_UART_TX_PIN;
    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
    GPIO_InitStruct.Alternate = LOG_UART_AF;
    HAL_GPIO_Init(LOG_UART_TX_PORT, &GPIO_InitStruct);

    GPIO_InitStruct.Pin = LOG_UART_RX_PIN;
    HAL_GPIO_Init(LOG_UART_RX_PORT, &GPIO_InitStruct);

    logger_uart_info.uart_handle.Instance = LOG_UART_MODULE;
    logger_uart_info.uart_handle.Init.BaudRate = LOG_UART_BAUDRATE;
    logger_uart_info.uart_handle.Init.WordLength = LOG_UART_DATA_BITS;
    logger_uart_info.uart_handle.Init.StopBits = LOG_UART_STOP_BITS;
    logger_uart_info.uart_handle.Init.Parity = LOG_UART_PARITY;
    logger_uart_info.uart_handle.Init.Mode = UART_MODE_TX_RX;
    logger_uart_info.uart_handle.Init.HwFlowCtl = UART_HWCONTROL_NONE;
    logger_uart_info.uart_handle.Init.OverSampling = UART_OVERSAMPLING_16;
    if (HAL_UART_Init(&logger_uart_info.uart_handle) != HAL_OK) {
        return LOG_STATUS_ERROR;      
    }

    //start usart rx not empty interrupt.
    __HAL_UART_ENABLE_IT(&logger_uart_info.uart_handle, UART_IT_RXNE);

    HAL_NVIC_SetPriority(LOG_UART_IRQn, LOG_UART_PRE_PRIO, LOG_UART_SUB_PRIO);	
    HAL_NVIC_EnableIRQ(LOG_UART_IRQn);

    return ret;
}

static LOG_STATUS circular_put_rx_buffer(uint8_t *ptr, uint8_t size)
{
    uint32_t rx_allow_size, index;

    if (logger_uart_info.is_ready != 1) {
        return LOG_STATUS_ERROR;
    }
    
    if (size == 1) {
       //if size is 1, not check for efficiency because the macro support check
       CircularBufferPut(&logger_uart_info.rx_buffer_info, ptr[0]);
    } else {
        rx_allow_size = LOGGER_RX_BUFFER_SIZE - CircularBufferSize(&logger_uart_info.rx_buffer_info);
        if (size > rx_allow_size) {
            return LOG_STATUS_ERROR;
        }
        
        for (index=0; index<size; index++) {
            CircularBufferPut(&logger_uart_info.rx_buffer_info, ptr[index]);
        }
    }
    return LOG_STATUS_OK;
}

static LOG_STATUS circular_get_tx_byte(uint8_t *data)
{
    uint8_t c = 0;
    
    if (logger_uart_info.is_ready != 1) {
        return LOG_STATUS_ERROR;
    }
    
    if (!CircularBufferIsEmpty(&logger_uart_info.tx_buffer_info)) {
       CircularBufferGet(&logger_uart_info.tx_buffer_info, c);
       *data = c;
       return LOG_STATUS_OK;
    }
    
    return LOG_STATUS_ERROR;
}

static LOG_STATUS circular_put_tx_buffer(uint8_t *ptr, uint8_t size)
{
    uint32_t tx_allow_size, index;

    if (logger_uart_info.is_ready != 1) {
        return LOG_STATUS_ERROR;
    }

    if (size == 1) {
        //if size is 1, not check for efficiency because the macro support check
       CircularBufferPut(&logger_uart_info.tx_buffer_info, ptr[0]);
    } else {
        tx_allow_size = LOGGER_TX_BUFFER_SIZE - CircularBufferSize(&logger_uart_info.tx_buffer_info);
        if (size > tx_allow_size) {
            return LOG_STATUS_ERROR;
        }
        
        for (index=0; index<size; index++) {
            CircularBufferPut(&logger_uart_info.tx_buffer_info, ptr[index]);
        }
		
        __HAL_UART_ENABLE_IT(&logger_uart_info.uart_handle, UART_IT_TXE);
    }
    return LOG_STATUS_OK;  
}

static void UART_SendData(UART_HandleTypeDef* huart, uint16_t Data)
{
    huart->Instance->DR = Data&0x1FF;
}

static uint16_t UART_ReceiveData(UART_HandleTypeDef* huart)
{
    uint16_t rx_data;

    rx_data = huart->Instance->DR&0x1FF;

    return rx_data;
}

void LOG_UART_IRQHandler(void)
{
    uint8_t data;
    
    if (__HAL_UART_GET_FLAG(&logger_uart_info.uart_handle, UART_FLAG_RXNE) != RESET) 
    {
        data = UART_ReceiveData(&logger_uart_info.uart_handle);
        circular_put_rx_buffer(&data, 1);
    }
    
    if (__HAL_UART_GET_FLAG(&logger_uart_info.uart_handle, UART_FLAG_TXE) != RESET
    && __HAL_UART_GET_IT_SOURCE(&logger_uart_info.uart_handle, UART_IT_TXE))
    {
        if (circular_get_tx_byte(&data) == RT_FAIL) 
        {
            __HAL_UART_DISABLE_IT(&logger_uart_info.uart_handle, UART_IT_TXE);
        } 
        else 
        {
            UART_SendData(&logger_uart_info.uart_handle, data);
        }
    }
}
