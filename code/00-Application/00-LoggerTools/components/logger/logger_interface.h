//////////////////////////////////////////////////////////////////////////////
//  (c) copyright 2025-by Persional Inc.  
//  All Rights Reserved
//
//  Name:
//      logger_interface.h
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
#ifndef __LOGGER_INTERFACE_H__
#define __LOGGER_INTERFACE_H__

#include "stm32f4xx_hal.h"
#include "global_def.h"
#include <string.h>

typedef enum
{
    /// @brief 日志状态正常
    LOG_STATUS_OK = 0,

    /// @brief 日志状态异常
    LOG_STATUS_ERROR,
}LOG_STATUS;

typedef enum
{
    LOG_DEVICE_USART = 0,   /// 输出到串口
    LOG_DEVICE_RTT,         /// 输出到RTT，JLINK工具，使用RTT Viewer查看
    LOG_DEVICE_ITM,         /// 输出到ITM，STM32工具，使用STM32Programmer查看 
    LOG_DEVICE_ETH,         /// 输出到Ethernet，通过socket日志查看
    LOG_DEVICE_SDCARD,      /// 输出到SD卡，通过SD卡日志查看
    LOG_DEVICE_MAX,
}LOG_DEVICE;

/// @brief 日志模块默认输出设备
#ifndef LOG_MODULE_ON
#define LOG_MODULE_ON           0
#endif

#ifndef LOG_DEFAULT_DEVICE
#define LOG_DEFAULT_DEVICE LOG_DEVICE_USART
#endif

LOG_STATUS logger_interface_init(void);
void logger_set_inteface(LOG_DEVICE device);
LOG_DEVICE logger_get_interface(void);

LOG_STATUS logger_protect_init(void);
LOG_STATUS logger_protect_entry(void);
LOG_STATUS logger_protect_exit(void);

// 实际日志的物理层发送接口
LOG_STATUS logger_uart_init(void);
LOG_STATUS logger_uart_transmit(uint8_t *buf, int len);

LOG_STATUS logger_sdcard_init(void);
LOG_STATUS logger_sdcard_transmit(uint8_t *buf, int len);

LOG_STATUS logger_eth_init(void);
LOG_STATUS logger_eth_transmit(uint8_t *buf, int len);

LOG_STATUS logger_rtt_transmit(uint8_t *buf, int len);
LOG_STATUS logger_itm_transmit(uint8_t *buf, int len);

LOG_STATUS logger_interface_write(LOG_DEVICE device, uint8_t *buf, int len);
#endif
