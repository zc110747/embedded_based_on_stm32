//////////////////////////////////////////////////////////////////////////////
//  (c) copyright 2025-by Persional Inc.  
//  All Rights Reserved
//
//  Name:
//      logger.c
//
//  Purpose:
//      logger module process.
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

#if LOG_MODULE_ON == 1
/// local parameter
static LOGGER_INFO g_logger_info = {0};

/// local function statement
static char *LogGetMemoryBuffer(uint16_t size);

/// global function
LOG_STATUS logger_module_init(void)
{
    LOG_STATUS ret;
    
    ret = logger_interface_init();
    if (ret != LOG_STATUS_OK) {
        return ret;
    }

    g_logger_info.ready = 1;
    g_logger_info.level = LOG_DEFAULT_LEVEL;
    
    return ret;
}

void logger_set_level(LOG_LEVEL level)
{
    if (level < LOG_LEVEL_MAX)
    {
        g_logger_info.level = level;
    }
}

static char MemoryBuffer[MEMORY_BUFFER_TOTAL_SIZE + 1];
static char *nextBufferPointer = MemoryBuffer;
static char *MemoryBufferEndPoint = &MemoryBuffer[MEMORY_BUFFER_TOTAL_SIZE];

static char *LogGetMemoryBuffer(uint16_t size)
{
	char *textBufferPointer;

	textBufferPointer = nextBufferPointer;
	if((nextBufferPointer = textBufferPointer + size) >  MemoryBufferEndPoint)
	{
		textBufferPointer = MemoryBuffer;
		nextBufferPointer = textBufferPointer + size;
	}
	return(textBufferPointer);
}

int print_log(LOG_LEVEL level, uint32_t tick, const char* fmt, ...)
{
    int len, bufferlen;
    char *pbuf, *buf;
    int outlen = 0;
    va_list	valist;
    LOG_DEVICE device;
    
    if (g_logger_info.ready != 1) {
        return -1;
    }

    if (level < g_logger_info.level) {
        return -2;
    }
    
    if (logger_protect_entry() == LOG_STATUS_OK) {
        
        device = logger_get_interface();
        buf = LogGetMemoryBuffer(LOGGER_MAX_BUFFER_SIZE);
        len = LOGGER_MAX_BUFFER_SIZE;
        bufferlen = len - 1;
        pbuf = buf;
        
        len = snprintf(pbuf, bufferlen, "level:%d times:%d info:", level, tick);
        if ((len <= 0) || (len>=bufferlen)) {
            logger_protect_exit();
            return -3;
        }
        outlen += len;
        pbuf = &pbuf[len];
        bufferlen -= len;
        
        va_start(valist, fmt);
        len = vsnprintf(pbuf, bufferlen, fmt, valist);
        va_end(valist);
        if ((len<=0) || (len>=bufferlen)) {
            logger_protect_exit();
            return -4;
        } 
        outlen += len;
        pbuf = &pbuf[len];
        bufferlen -= len;
        
        if (bufferlen < 3) {
            logger_protect_exit();
            return -4;
        }
        
        pbuf[0] = '\r';
        pbuf[1] = '\n';
        outlen += 2;
    
        if(logger_interface_write(device, (uint8_t *)buf, outlen) != LOG_STATUS_OK) {
            logger_protect_exit();
            return -5;
        }
        
        logger_protect_exit();
    }
   
    return outlen;
}

#endif
