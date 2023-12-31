//////////////////////////////////////////////////////////////////////////////
//  (c) copyright 2023-by Persional Inc.  
//  All Rights Reserved
//
//  Name:
//      logger.c
//
//  Purpose:
//      logger module process.
//      example used usart.
//          interrupt usart_rx => rx cache buffer => process
//          software => tx cache buffer => interrupt usart_tx
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
#include <stdarg.h>
#include <stdio.h>
#include "logger.h"

//internal variable
static UART_HandleTypeDef huart1;
static LOGGER_INFO g_logger_info = {0};

//internal function
static GlobalType_t logger_uart_init(void);
static GlobalType_t logger_get_tx_byte(uint8_t *data);
static GlobalType_t logger_put_rx_buffer(uint8_t *ptr, uint8_t size);
static GlobalType_t logger_put_tx_buffer(uint8_t *ptr, uint8_t size);
static GlobalType_t uart_logger_write(uint8_t *ptr, uint8_t size);
static GlobalType_t logger_dev_tx_buffer(uint8_t *ptr, uint8_t size);
static void UART_SendData(UART_HandleTypeDef* huart, uint16_t Data);
static GlobalType_t logger_dev_tx_buffer(uint8_t *ptr, uint8_t size);

GlobalType_t logger_module_init(void)
{
    GlobalType_t result;
    
    CircularBufferInit(&g_logger_info.LoggerTxBufferInfo, LOGGER_BUFFER_SIZE, g_logger_info.LoggerTxBuffer);
    CircularBufferInit(&g_logger_info.LoggerRxBufferInfo, LOGGER_BUFFER_SIZE, g_logger_info.LoggerRxBuffer);

    result = logger_uart_init();
    if (result != RT_OK)
        return RT_FAIL;
    
    g_logger_info.device = LOG_DEVICE_USART;
    g_logger_info.ready = 1;
    g_logger_info.level = LOG_INFO;
    return RT_OK;
}

#define LOGGER_MAX_BUFFER_SIZE      128
static char LoggerMaxBuffer[LOGGER_MAX_BUFFER_SIZE];
int print_log(LOG_LEVEL level, uint32_t tick, const char* fmt, ...)
{
    int len, bufferlen;
    char *pbuf;
    int outlen = 0;
    
    if (g_logger_info.ready != 1)
        return -1;

    if (level < g_logger_info.level)
        return -2;

    {
        va_list	valist;
        
        len = LOGGER_MAX_BUFFER_SIZE;
        bufferlen = len - 1;
        pbuf = LoggerMaxBuffer;
        
        len = snprintf(pbuf, bufferlen, "level:%d times:%d info:", level, tick);
        if ((len<=0) || (len>=bufferlen))
        {
            return -3;
        }
        outlen += len;
        pbuf = &pbuf[len];
        bufferlen -= len;
        
        va_start(valist, fmt);
        len = vsnprintf(pbuf, bufferlen, fmt, valist);
        va_end(valist);
        if ((len<=0) || (len>=bufferlen))
        {
            return -4;
        } 
        outlen += len;
        pbuf = &pbuf[len];
        bufferlen -= len;
        
        if (bufferlen < 3)
        {
            return -4;
        }
        
        pbuf[0] = '\r';
        pbuf[1] = '\n';
        outlen += 2;
        logger_dev_tx_buffer((uint8_t *)LoggerMaxBuffer, outlen);
    }
    
    return outlen;
}

__weak GlobalType_t eth_logger_write(uint8_t *ptr, uint8_t size)
{ 
    return RT_OK;    
}

//-------------------------------------internal function-------------------------------
static GlobalType_t uart_logger_write(uint8_t *ptr, uint8_t size)
{
    GlobalType_t result;
    
    if (g_logger_info.device != LOG_DEVICE_USART)
    {
        return RT_FAIL;
    }        
    
    __disable_irq();
    result = logger_put_tx_buffer(ptr, size);  
    __enable_irq();

    if (result == RT_OK)
    {
      __HAL_UART_ENABLE_IT(&huart1, UART_IT_TXE);
    }
    return result;
}

static GlobalType_t logger_dev_tx_buffer(uint8_t *ptr, uint8_t size)
{
    GlobalType_t res = RT_FAIL;
    
    switch (g_logger_info.device)
    {
        case LOG_DEVICE_USART:
            res = uart_logger_write(ptr, size);
            break;
        case LOG_DEVICE_ETH:
            res = eth_logger_write(ptr, size);
            break;
        default:
            break;
    }
    return res;
}

static GlobalType_t logger_put_rx_buffer(uint8_t *ptr, uint8_t size)
{
    uint32_t rx_allow_size, index;
    
    //device not use as logger interface
    if (g_logger_info.ready != 1 )
        return RT_FAIL;
    
    if (size == 1)
    {
       //if size is 1, not check for efficiency because the macro support check
       CircularBufferPut(&g_logger_info.LoggerRxBufferInfo, ptr[0]);
    }
    else
    {
        rx_allow_size = LOGGER_BUFFER_SIZE - CircularBufferSize(&g_logger_info.LoggerRxBufferInfo);
        if (size > rx_allow_size)
        {
            return RT_FAIL;
        }
        
        for (index=0; index<size; index++)
        {
            CircularBufferPut(&g_logger_info.LoggerRxBufferInfo, ptr[index]);
        }
    }
    return RT_OK;
}


static GlobalType_t logger_get_tx_byte(uint8_t *data)
{
    uint8_t c;
    
    //device not use as logger interface
    if (g_logger_info.ready != 1 )
    {
        return RT_FAIL;
    }
    
    if (!CircularBufferIsEmpty(&g_logger_info.LoggerTxBufferInfo))
    {
       CircularBufferGet(&g_logger_info.LoggerTxBufferInfo, c);
       *data = c;
       return RT_OK;
    }
    
    return RT_FAIL;
}

//only logger buffer is allow send 
//can put, otherwise will discard the data
static GlobalType_t logger_put_tx_buffer(uint8_t *ptr, uint8_t size)
{
    uint32_t tx_allow_size, index;
    
    //device not use as logger interface
    if (g_logger_info.ready != 1 )
        return RT_FAIL;

    if (size == 1)
    {
       //if size is 1, not check for efficiency because the macro support check
       CircularBufferPut(&g_logger_info.LoggerTxBufferInfo, ptr[0]);
    }
    else
    {
        tx_allow_size = LOGGER_BUFFER_SIZE - CircularBufferSize(&g_logger_info.LoggerTxBufferInfo);
        if (size > tx_allow_size)
        {
            return RT_FAIL;
        }
        
        for (index=0; index<size; index++)
        {
            CircularBufferPut(&g_logger_info.LoggerTxBufferInfo, ptr[index]);
        }
				__HAL_UART_ENABLE_IT(&huart1, UART_IT_TXE);
    }
    return RT_OK;   
}

static GlobalType_t logger_uart_init(void)
{
    GPIO_InitTypeDef GPIO_InitStruct = {0};

    __HAL_RCC_USART1_CLK_ENABLE();
    __HAL_RCC_GPIOA_CLK_ENABLE();

    GPIO_InitStruct.Pin = GPIO_PIN_9|GPIO_PIN_10;
    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
    GPIO_InitStruct.Alternate = GPIO_AF7_USART1;
    HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

    huart1.Instance = USART1;
    huart1.Init.BaudRate = 115200;
    huart1.Init.WordLength = UART_WORDLENGTH_8B;
    huart1.Init.StopBits = UART_STOPBITS_1;
    huart1.Init.Parity = UART_PARITY_NONE;
    huart1.Init.Mode = UART_MODE_TX_RX;
    huart1.Init.HwFlowCtl = UART_HWCONTROL_NONE;
    huart1.Init.OverSampling = UART_OVERSAMPLING_16;
    if (HAL_UART_Init(&huart1) != HAL_OK)
    {
        return RT_FAIL;      
    }

    //start usart rx not empty interrupt.
    __HAL_UART_ENABLE_IT(&huart1, UART_IT_RXNE);

    HAL_NVIC_SetPriority(USART1_IRQn, 1, 1);	
    HAL_NVIC_EnableIRQ(USART1_IRQn);
    
    return RT_OK;
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

void USART1_IRQHandler(void)
{
    uint8_t data;
    
    if (__HAL_UART_GET_FLAG(&huart1, UART_FLAG_RXNE) != RESET)
    {
        data = UART_ReceiveData(&huart1);
        if (g_logger_info.device == LOG_DEVICE_USART)
        {
            logger_put_rx_buffer(&data, 1);
        }
    }
    
    if (__HAL_UART_GET_FLAG(&huart1, UART_FLAG_TXE) != RESET
    && __HAL_UART_GET_IT_SOURCE(&huart1, UART_IT_TXE))
    {
        if (logger_get_tx_byte(&data) == RT_FAIL
        || g_logger_info.device != LOG_DEVICE_USART)
        {
            __HAL_UART_DISABLE_IT(&huart1, UART_IT_TXE);
        }
        else
        {
            UART_SendData(&huart1, data);
        }
    }
}
