//////////////////////////////////////////////////////////////////////////////
//  (c) copyright 2025-by Persional Inc.  
//  All Rights Reserved
//
//  Name:
//      logger_interface.c
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
#include "logger_interface.h"

#if LOG_MODULE_ON == 1

// @brief 日志接口信息结构体
typedef struct 
{
    LOG_DEVICE device;
}LOG_INTEFACE_INFO;

/// inner struct info
static LOG_INTEFACE_INFO logger_interface_info;

/// global function
LOG_STATUS logger_interface_init(void)
{
    LOG_STATUS ret = LOG_STATUS_OK;

    memset((char *)&logger_interface_info, 0, sizeof(logger_interface_info));
    
    logger_interface_info.device = LOG_DEFAULT_DEVICE;

    ret |= logger_protect_init();

    if (ret != LOG_STATUS_OK) {
        return LOG_STATUS_ERROR;
    }

    // 需要保证至少一个接口成功，才能运行工作
    ret = logger_uart_init();
    ret &= logger_sdcard_init();
    ret &= logger_eth_init();
    
    if (ret != LOG_STATUS_OK) {
        return LOG_STATUS_ERROR;
    }
    
    return LOG_STATUS_OK;
}

void logger_set_inteface(LOG_DEVICE device)
{
    if (device < LOG_DEVICE_MAX)
    {
        logger_interface_info.device = device;
    }
}

LOG_DEVICE logger_get_interface(void)
{
    return logger_interface_info.device;
}

LOG_STATUS logger_interface_write(LOG_DEVICE device, uint8_t *buf, int len)
{
    LOG_STATUS ret = LOG_STATUS_OK;

    switch (device)
    {
    case LOG_DEVICE_USART:
        ret = logger_uart_transmit(buf, len);
        break;
    case LOG_DEVICE_RTT:
        ret = logger_rtt_transmit(buf, len);
        break;
    case LOG_DEVICE_ITM:
        ret = logger_itm_transmit(buf, len);
        break;
    case LOG_DEVICE_ETH:
        ret = logger_eth_transmit(buf, len);
        break;
    case LOG_DEVICE_SDCARD:
        ret = logger_sdcard_transmit(buf, len);
        break;
    default:
        ret = LOG_STATUS_ERROR;
        break;
    }
    return ret;
}

/// interface for logger modules
__weak LOG_STATUS logger_protect_init(void)
{
    LOG_STATUS ret = LOG_STATUS_OK;

    return ret;
}

__weak LOG_STATUS logger_protect_entry(void)
{
    LOG_STATUS ret = LOG_STATUS_OK;

    return ret;
}

__weak LOG_STATUS logger_protect_exit(void)
{
    LOG_STATUS ret = LOG_STATUS_OK;

    return ret;
}

__weak LOG_STATUS logger_uart_init(void)
{
    LOG_STATUS ret = LOG_STATUS_OK;
    
    return ret;
}

__weak LOG_STATUS logger_sdcard_init(void)
{
    LOG_STATUS ret = LOG_STATUS_OK;
    
    return ret;
}

__weak LOG_STATUS logger_eth_init(void)
{
    LOG_STATUS ret = LOG_STATUS_OK;
    
    return ret;
}

__weak LOG_STATUS logger_uart_transmit(uint8_t *buf, int len)
{
    LOG_STATUS ret = LOG_STATUS_OK;
    
    return ret;
}

__weak LOG_STATUS logger_rtt_transmit(uint8_t *buf, int len)
{
    LOG_STATUS ret = LOG_STATUS_OK;
    
    return ret;
}

__weak LOG_STATUS logger_itm_transmit(uint8_t *buf, int len)
{
    LOG_STATUS ret = LOG_STATUS_OK;
    
    return ret;
}

__weak LOG_STATUS logger_sdcard_transmit(uint8_t *buf, int len)
{
    LOG_STATUS ret = LOG_STATUS_OK;
    
    return ret;
}

__weak LOG_STATUS logger_eth_transmit(uint8_t *buf, int len)
{
    LOG_STATUS ret = LOG_STATUS_OK;
    
    return ret;
}
#endif
