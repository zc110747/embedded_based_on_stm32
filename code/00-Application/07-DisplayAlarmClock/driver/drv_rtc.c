//////////////////////////////////////////////////////////////////////////////
//  (c) copyright 2023-by Persional Inc.  
//  All Rights Reserved
//
//  Name:
//      drv_rtc.c
//
//  Purpose:
//      driver for rtc and rtc alarm.
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
#include "drv_rtc.h"
#include <string.h>

//internal defined
#define RTC_SET_FLAGS       0x5B5B
#define RTC_FORMAT_MODE     RTC_FORMAT_BIN

//internal variable
static const RTC_TIME_DEF rtc_init_def = {
    .year = 23,
    .month = 12,
    .day = 12,
    .weekday = 2,
    .hour = 20,
    .minute = 15,
    .second = 0,
};
static RTC_HandleTypeDef rtc_handler_;
static RTC_AlarmTypeDef rtc_arm_handler_;
static RTC_INFO rtc_info_;
static uint8_t is_alarm_ = 0;

//internal fucntion
static GlobalType_t rtc_hardware_init(void);
static void rtc_set_alarm(uint8_t week, uint8_t hour, uint8_t min, uint8_t sec);

//global function
GlobalType_t drv_rtc_init(void)
{
    GlobalType_t result;
    
    //unlock backup register update.
    HAL_PWR_EnableBkUpAccess();

    result = rtc_hardware_init();
    if (result == RT_OK)
    {
        rtc_alarm_register(0, 0, 0, 5);
    }
    
    return result;
}

void rtc_get_info(RTC_INFO *pGetInfo)
{
    __disable_irq();
    memcpy(pGetInfo, &rtc_info_, sizeof(RTC_INFO));
    __enable_irq();        
}

void rtc_update(void)
{
    __disable_irq();
    HAL_RTC_GetTime(&rtc_handler_, &rtc_info_.time, RTC_FORMAT_MODE);
    HAL_RTC_GetDate(&rtc_handler_, &rtc_info_.date, RTC_FORMAT_MODE);
    __enable_irq();
}

void rtc_alarm_register(uint8_t day, uint8_t hour, uint8_t min, uint8_t sec)
{
    uint8_t alarm_week = 0;
    uint8_t alarm_hour = 0;
    uint8_t alarm_min = 0;
    uint8_t alarm_sec = 0;

    rtc_update();
    
    alarm_sec += rtc_info_.time.Seconds + sec;
    if (alarm_sec >= 60)
    {
        alarm_sec -= 60;
        alarm_min++;
    }
    
    alarm_min += rtc_info_.time.Minutes + min;
    if (alarm_min >= 60)
    {
        alarm_min -= 60;
        alarm_hour++;
    }
    
    alarm_hour += rtc_info_.time.Hours + hour;
    if (alarm_hour >= 24)
    {
        alarm_hour -= 24;
        alarm_week++;
    }
    
    alarm_week += rtc_info_.date.WeekDay + day;
    if (alarm_week > 7)
    {
        alarm_week -= 7;
    }
    
    rtc_set_alarm(alarm_week, alarm_hour, alarm_min, alarm_sec);
}

uint8_t get_rtc_alarm(void)
{
    return is_alarm_;
}

void clear_rtc_alarm(void)
{
    is_alarm_ = 0;
}

void RTC_Alarm_IRQHandler(void)
{
    RTC_HandleTypeDef *prtc_handler = &rtc_handler_;

    if (__HAL_RTC_ALARM_GET_IT(prtc_handler, RTC_IT_ALRA))
    {
        if ((uint32_t)(prtc_handler->Instance->CR & RTC_IT_ALRA) != (uint32_t)RESET)
        {
            /* Clear the Alarm interrupt pending bit */
            __HAL_RTC_ALARM_CLEAR_FLAG(prtc_handler, RTC_FLAG_ALRAF);                   
            HAL_NVIC_DisableIRQ(RTC_Alarm_IRQn);
            
            is_alarm_ = 1;
        }
    }

    /* Clear the EXTI's line Flag for RTC Alarm */
    __HAL_RTC_ALARM_EXTI_CLEAR_FLAG();

    /* Change RTC state */
    prtc_handler->State = HAL_RTC_STATE_READY; 
}

//internal function
static void rtc_set_alarm(uint8_t week, uint8_t hour, uint8_t min, uint8_t sec)
{   
    rtc_arm_handler_.AlarmTime.Hours = hour;  
    rtc_arm_handler_.AlarmTime.Minutes = min; 
    rtc_arm_handler_.AlarmTime.Seconds = sec; 
    rtc_arm_handler_.AlarmTime.SubSeconds = 0;
    rtc_arm_handler_.AlarmTime.TimeFormat = RTC_HOURFORMAT_24;
    
    rtc_arm_handler_.AlarmMask = RTC_ALARMMASK_NONE;
    rtc_arm_handler_.AlarmSubSecondMask = RTC_ALARMSUBSECONDMASK_NONE;
    rtc_arm_handler_.AlarmDateWeekDaySel = RTC_ALARMDATEWEEKDAYSEL_WEEKDAY;
    rtc_arm_handler_.AlarmDateWeekDay = week; 
    rtc_arm_handler_.Alarm = RTC_ALARM_A;     
    
    __HAL_RTC_ALARM_CLEAR_FLAG(&rtc_handler_, RTC_FLAG_ALRAF);
    HAL_RTC_SetAlarm_IT(&rtc_handler_, &rtc_arm_handler_, RTC_FORMAT_MODE);
    
    HAL_NVIC_SetPriority(RTC_Alarm_IRQn, 0x01, 0x02);
    HAL_NVIC_EnableIRQ(RTC_Alarm_IRQn);
}

static GlobalType_t rtc_hardware_init(void)
{
    RCC_PeriphCLKInitTypeDef PeriphClkInitStruct = {0};
    
    PeriphClkInitStruct.PeriphClockSelection = RCC_PERIPHCLK_RTC;
    PeriphClkInitStruct.RTCClockSelection = RCC_RTCCLKSOURCE_LSE;
    if (HAL_RCCEx_PeriphCLKConfig(&PeriphClkInitStruct) != HAL_OK)
    {
        return RT_FAIL;
    }

    /* Peripheral clock enable */
    __HAL_RCC_RTC_ENABLE();
      
    rtc_handler_.Instance = RTC;
    rtc_handler_.Init.HourFormat = RTC_HOURFORMAT_24;
    rtc_handler_.Init.AsynchPrediv = 127;
    rtc_handler_.Init.SynchPrediv = 255;
    rtc_handler_.Init.OutPut = RTC_OUTPUT_DISABLE;
    rtc_handler_.Init.OutPutPolarity = RTC_OUTPUT_POLARITY_HIGH;
    rtc_handler_.Init.OutPutType = RTC_OUTPUT_TYPE_OPENDRAIN;
    if (HAL_RTC_Init(&rtc_handler_) != HAL_OK)
        return RT_FAIL;

    if (HAL_RTCEx_BKUPRead(&rtc_handler_, RTC_BKP_DR0) != RTC_SET_FLAGS)
    {
        RTC_TimeTypeDef time;
        RTC_DateTypeDef date;
   
        /* Update system time */
        time.Hours = rtc_init_def.hour;
        time.Minutes =  rtc_init_def.minute;
        time.Seconds = rtc_init_def.second;
        time.DayLightSaving = RTC_DAYLIGHTSAVING_NONE;
        time.StoreOperation = RTC_STOREOPERATION_RESET;
        if (HAL_RTC_SetTime(&rtc_handler_, &time, RTC_FORMAT_MODE) != HAL_OK)
            return RT_FAIL;
        
        date.Year = rtc_init_def.year;
        date.Month = rtc_init_def.month;
        date.Date = rtc_init_def.day;
        date.WeekDay = rtc_init_def.weekday;
        
        if (HAL_RTC_SetDate(&rtc_handler_, &date, RTC_FORMAT_MODE) != HAL_OK)
            return RT_FAIL;
        
        HAL_RTCEx_BKUPWrite(&rtc_handler_, RTC_BKP_DR0, RTC_SET_FLAGS);
    }
    
    return RT_OK;      
}

