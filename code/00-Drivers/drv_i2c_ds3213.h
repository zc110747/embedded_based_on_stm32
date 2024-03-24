//////////////////////////////////////////////////////////////////////////////
//  (c) copyright 2023-by Persional Inc.  
//  All Rights Reserved
//
//  Name:
//      drv_i2c.h
//
//  Purpose:
//      i2c driver.
//
// Author:
//      @zc
//
//  Assumptions:
//
//  Revision History:
//
/////////////////////////////////////////////////////////////////////////////
#ifndef _DRV_I2C_DS3213_H
#define _DRV_I2C_DS3213_H

#include "includes.h"

#define DS3213_ADDR 	            0x68
#define DS3213_I2C_TIMEOUT          10

#define DS3213_REG_SECONDS          0x00
#define DS3213_REG_MINUTES          0x01
#define DS3213_REG_HOURS            0x02
#define DS3213_REG_DAY              0x03
#define DS3213_REG_DATE             0x04
#define DS3213_REG_MONTH            0x05
#define DS3213_REG_YEAR             0x06
#define DS3213_REG_ALARM1_SECONDS   0x07
#define DS3213_REG_ALARM1_MINUTES   0x08
#define DS3213_REG_ALARM1_HOURS     0x09
#define DS3213_REG_ALARM1_DAY_DATE  0x0A
#define DS3213_REG_ALARM2_MINUTES   0x0B
#define DS3213_REG_ALARM2_HOURS     0x0C
#define DS3213_REG_ALARM2_DAY_DATE  0x0D
#define DS3213_REG_CONTROL1         0x0E
#define DS3213_REG_CONTROL2_STATUS  0x0F
#define DS3213_REG_AGING_OFFSET     0x10
#define DS3213_REG_TEMPMSB          0x11
#define DS3213_REG_TEMPLSB          0x12

typedef struct
{
    uint8_t year;
    uint8_t month;
    uint8_t date;
    uint8_t weekday;
    uint8_t hour;
    uint8_t minutes;
    uint8_t seconds;
}RTC_DS3213_Info;

typedef struct
{
    uint8_t seconds;
    uint8_t minutes;
    uint8_t hour;
    uint8_t weekday;
}RTC_DS3213_ALARM_Info;

typedef enum
{
    DS3213_ALARM1 = 0,
    DS3213_ALARM2 = 0,   
}DS3213_ALARM;

#ifdef __cplusplus
extern "C" {
#endif

//rtc read information
GlobalType_t ds3213_driver_init(void);
GlobalType_t ds3213_update_timer(void);
GlobalType_t ds3213_read_timer(RTC_DS3213_Info *info);
GlobalType_t ds3213_set_timer(RTC_DS3213_Info *info);
void get_rtc_ds3213_info(RTC_DS3213_Info *rtc_info);

//rtc alarm information
uint8_t get_clear_alarm(void);
GlobalType_t ds3213_clear_alarm(DS3213_ALARM alarm);
GlobalType_t ds3213_control_alarm(DS3213_ALARM alarm, FunctionalState mode);
GlobalType_t ds3213_set_alarm(DS3213_ALARM alarm, RTC_DS3213_ALARM_Info *Info);

#ifdef __cplusplus
}
#endif

#endif
