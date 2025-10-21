//////////////////////////////////////////////////////////////////////////////
//  (c) copyright 2025-by Persional Inc.  
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
    
#include "logger_interface.h"

typedef enum
{
	LOG_TRACE = 0,
	LOG_DEBUG,
	LOG_INFO,
	LOG_WARN,
	LOG_ERROR,
	LOG_FATAL,
	LOG_LEVEL_MAX,
}LOG_LEVEL;

typedef struct
{
    //device weather ready    
    uint8_t ready;
    
    //level protect tx
    LOG_LEVEL level;
}LOGGER_INFO;

LOG_STATUS logger_module_init(void);
void logger_set_level(LOG_LEVEL level);

/// global define
#define LOGGER_MAX_BUFFER_SIZE      256			// max buffer size for each log
#define MEMORY_BUFFER_TOTAL_SIZE 	8192		// total memory buffer size

#ifndef LOG_DEFAULT_LEVEL
#define LOG_DEFAULT_LEVEL LOG_INFO
#endif

#if LOG_MODULE_ON == 1
int print_log(LOG_LEVEL level, uint32_t tick, const char* fmt, ...);
#define PRINT_LOG(level, ticks, fmt, ...)       print_log((LOG_LEVEL)level, ticks, fmt, ##__VA_ARGS__);
#else
#define PRINT_LOG(level, ticks, fmt, ...)
#endif

#ifdef __cplusplus
}
#endif
#endif
