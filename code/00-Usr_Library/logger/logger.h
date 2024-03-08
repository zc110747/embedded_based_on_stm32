//////////////////////////////////////////////////////////////////////////////
//  (c) copyright 2023-by Persional Inc.  
//  All Rights Reserved
//
//  Name:
//      logger.h
//
//  Purpose:
//      logger information process.
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

#ifndef _INCLUDE_LOGGER_H
#define _INCLUDE_LOGGER_H

#ifdef __cplusplus
extern "C" {
#endif
    
#include "stm32f4xx_hal.h"
#include "global_def.h"
#include "circular_buffer.h"

#define LOGGER_BUFFER_SIZE      2048

typedef enum
{
	LOG_TRACE = 0,
	LOG_DEBUG,
	LOG_INFO,
	LOG_WARN,
	LOG_ERROR,
	LOG_FATAL,
}LOG_LEVEL;

typedef enum
{
    LOG_DEVICE_USART = 0,
    LOG_DEVICE_ETH,
}LOG_DEVICE;

typedef struct
{
    //device weather ready    
    uint8_t ready;
   
    //device used to run on
    LOG_DEVICE device;
    
    //level protect tx
    LOG_LEVEL level;
    
    //logger rx buffer
    volatile CircularBuffer LoggerRxBufferInfo;
    uint8_t LoggerRxBuffer[LOGGER_BUFFER_SIZE];
    
    //logger tx buffer
    volatile CircularBuffer LoggerTxBufferInfo;
    uint8_t LoggerTxBuffer[LOGGER_BUFFER_SIZE];
}LOGGER_INFO;

GlobalType_t logger_module_init(void);
int print_log(LOG_LEVEL level, uint32_t tick, const char* fmt, ...);

#if LOGGER_MODULE_ON == 1
#define PRINT_LOG(level, ticks, fmt, ...)       print_log((LOG_LEVEL)level, ticks, fmt, ##__VA_ARGS__);
#else
#define PRINT_LOG(level, ticks, fmt, ...)          
#endif

#ifdef __cplusplus
}
#endif
#endif
