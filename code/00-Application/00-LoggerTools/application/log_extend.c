//////////////////////////////////////////////////////////////////////////////
//  (c) copyright 2025-by Persional Inc.  
//  All Rights Reserved
//
//  Name:
//      log_extend.c
//
//  Purpose:
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
#include "includes.h"
#include "SEGGER_RTT.h"

typedef LOG_STATUS (*cmd_handler_t)(uint8_t *pbuffer, uint16_t len);
typedef struct
{
    char *cmd_tag;
    cmd_handler_t cmd_handler;
} cmd_attr_t;

static LOG_STATUS cmd_send_log(LOG_LEVEL level, uint8_t *pbuffer, uint16_t len);\

static LOG_STATUS cmd_set_log_level(uint8_t *pbuffer, uint16_t len);
static LOG_STATUS cmd_set_dev(uint8_t *pbuffer, uint16_t len);
static LOG_STATUS cmd_send_debug(uint8_t *pbuffer, uint16_t len);
static LOG_STATUS cmd_send_info(uint8_t *pbuffer, uint16_t len);

/*
命令说明:
!setlevel [0~5], 值越大要求输出等级越高
!setdev [0~4], 对应不同的硬件接口
*/
#define CMD_TABLE_SIZE          4
static const cmd_attr_t  cmd_table[CMD_TABLE_SIZE] = {
    {"!setlevel ",  cmd_set_log_level},    
    {"!setdev ",  cmd_set_dev},
    {"!debug ", cmd_send_debug},
    {"!info ", cmd_send_info}
};

const char* convert_level_str(LOG_LEVEL level)
{
    switch(level)
    {
        case LOG_TRACE:
            return "LOG_TRACE";
        case LOG_DEBUG:
            return "LOG_DEBUG";
        case LOG_INFO:
            return "LOG_INFO";
        case LOG_WARN:
            return "LOG_WARN";
        case LOG_ERROR:
            return "LOG_ERROR";
        case LOG_FATAL:
            return "LOG_FATAL";
        default:
            return "INVALID";
    }
}

static LOG_STATUS cmd_set_log_level(uint8_t *pbuffer, uint16_t len)
{
    int level;
    LOG_STATUS status = LOG_STATUS_OK;
    
    sscanf((char *)pbuffer, "%d", &level);
    PRINT_LOG(LOG_FATAL, HAL_GetTick(), "set level: %s[%d]", convert_level_str((LOG_LEVEL)level), level);
    
    logger_set_level((LOG_LEVEL)level);
    
    return status;
}

const char* convert_dev_str(LOG_DEVICE level)
{
    switch(level)
    {
        case LOG_DEVICE_USART:
            return "LOG_DEVICE_USART";
        case LOG_DEVICE_RTT:
            return "LOG_DEVICE_RTT";
        case LOG_DEVICE_ITM:
            return "LOG_DEVICE_ITM";
        case LOG_DEVICE_ETH:
            return "LOG_DEVICE_ETH";
        case LOG_DEVICE_SDCARD:
            return "LOG_DEVICE_SDCARD";
        default:
            return "INVALID";
    }
}

static LOG_STATUS cmd_set_dev(uint8_t *pbuffer, uint16_t len)
{
    int dev;
    LOG_STATUS status = LOG_STATUS_OK;

    sscanf((char *)pbuffer, "%d", &dev);
    PRINT_LOG(LOG_FATAL, HAL_GetTick(), "set dev: %s[%d]", convert_dev_str((LOG_DEVICE)dev), dev);

    logger_set_inteface((LOG_DEVICE)dev);
    return status;
}

static LOG_STATUS cmd_send_log(LOG_LEVEL level, uint8_t *pbuffer, uint16_t len)
{
    LOG_STATUS status = LOG_STATUS_OK;

    // 转发数据到相应调试接口
    PRINT_LOG(level, HAL_GetTick(), "%s", pbuffer);

    return status;    
}

static LOG_STATUS cmd_send_debug(uint8_t *pbuffer, uint16_t len)
{
    return cmd_send_log(LOG_DEBUG, pbuffer, len);    
}

static LOG_STATUS cmd_send_info(uint8_t *pbuffer, uint16_t len)
{
    return cmd_send_log(LOG_INFO, pbuffer, len);     
}

LOG_STATUS logger_process_callback(uint8_t *buffer, uint16_t len)
{
    uint8_t index;
    uint8_t *pbuffer;
    LOG_STATUS status = LOG_STATUS_ERROR;
    
    if (buffer[0] != '!') {
        return LOG_STATUS_ERROR;
    }
    
    pbuffer = buffer;
    for (index=0; index<CMD_TABLE_SIZE; index++) {
        int cmd_len = strlen(cmd_table[index].cmd_tag);
        if (memcmp(buffer, cmd_table[index].cmd_tag, cmd_len) == 0)
        {
            pbuffer += cmd_len;
            len -= cmd_len;
            status = cmd_table[index].cmd_handler(pbuffer, len);
        }
    }
    
    if(status == LOG_STATUS_ERROR) {
        PRINT_LOG(LOG_ERROR, HAL_GetTick(), "cmd run error!");
    }
    
    return status;
}

// 定义rtt打印接口
LOG_STATUS logger_rtt_transmit(uint8_t *buf, int len)
{
    LOG_STATUS ret = LOG_STATUS_OK;
    
    if (SEGGER_RTT_Write(0, buf, len) == 0) {
        ret = LOG_STATUS_ERROR;
    }
    
    return ret;
}

// 定义itm打印接口
volatile int32_t ITM_RxBuffer = ITM_RXBUFFER_EMPTY;
LOG_STATUS logger_itm_transmit(uint8_t *buf, int len)
{
    LOG_STATUS ret = LOG_STATUS_OK;
    int index;
    
    for (index=0; index<len; index++) {
        ITM_SendChar((uint32_t)(buf[index]));
    }
    return ret;
}
