//////////////////////////////////////////////////////////////////////////////
//  (c) copyright 2023-by Persional Inc.  
//  All Rights Reserved
//
//  Name:
//      drv_i2c_pcf8563.c
//      hardware: 
//          use not hardware i2c interface.
//          SOFT_I2C4
//              I2C_SCL - PB4 - DCMI_SCL
//              I2C_SDA - PB3 - DCMI_SDA
//              I2C_INT - PC6 - DCMI_D0
//  Purpose:
//     i2c driver.
//
// Author:
//      @zc
//
//  Assumptions:
//
//  Revision History:
//
/////////////////////////////////////////////////////////////////////////////
#include "drv_i2c_pcf8563.h"
#include "drv_soft_i2c.h"
#include "drv_target.h"

static RTC_TimerInfo default_rtc_info = {
    .year = 24,
    .month = 3,
    .date = 23,
    .weekday = 6,
    .hour = 15,
    .minutes = 02,
    .seconds = 0,
};
static volatile uint8_t is_alarm_on = 0;
static GlobalType_t pcf8563_set_regs(uint8_t reg, uint8_t *pvalue, uint8_t size);
static GlobalType_t pcf8563_read_regs(uint8_t reg, uint8_t *pvalue, uint8_t size);

//I2C_SCL: PH4
//I2C_SDA: PH5
//I2C_INT: PB12
GlobalType_t pcf8563_driver_init(void)
{
    GPIO_InitTypeDef GPIO_InitStruct = {0};
    RTC_TimerInfo rtc_info;
    
    //clock need enable before software i2c Init
    __HAL_RCC_GPIOC_CLK_ENABLE(); 
    
    /*Configure GPIO pin : PC6 */
    GPIO_InitStruct.Pin = GPIO_PIN_6;
    GPIO_InitStruct.Mode = GPIO_MODE_IT_FALLING;
    GPIO_InitStruct.Pull = GPIO_PULLUP;
    HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);
    
    HAL_NVIC_SetPriority(EXTI9_5_IRQn, 0, 0);
    HAL_NVIC_EnableIRQ(EXTI9_5_IRQn); 

    //pcf8563_set_timer(&pcf8563_rtc_info_);
    if(pcf8563_read_timer(&rtc_info) == RT_OK)
    {     
        if((rtc_info.seconds&(1<<7)) != 0)
        {
            pcf8563_set_timer(&default_rtc_info);
            PRINT_LOG(LOG_INFO, HAL_GetTick(), "rtc read with issue, update use default.");
        }
        else
        {
            RTC_AlarmInfo alarm_info;
            
            alarm_info = pcf8563_formward_minute(&rtc_info, 1);
            pcf8563_set_alarm(&alarm_info);
            pcf8563_ctrl_alarm(ENABLE);
        }
    }
    else
    {
        PRINT_LOG(LOG_ERROR, HAL_GetTick(), "rtc pcf8563 init failed!");
    }
    
    return RT_OK;  
}

GlobalType_t pcf8563_set_regs(uint8_t reg, uint8_t *pvalue, uint8_t size)
{
    uint8_t res;
    
    __disable_irq();
    res = i2c_write_memory(SOFT_I2C4, PCF8563_ADDR<<1, reg, 1, pvalue, size);
    __enable_irq();

    if (res != 0)
    {
        return RT_FAIL;  
    }
    
    return RT_OK;
}

GlobalType_t pcf8563_read_regs(uint8_t reg, uint8_t *pvalue, uint8_t size)
{
    uint8_t res;
                                                                                           
    __disable_irq();
    res = i2c_read_memory(SOFT_I2C4, PCF8563_ADDR<<1, reg, 1, pvalue, size);
    __enable_irq();

    if (res != 0)
    {
        return RT_FAIL;  
    }
    
    return RT_OK;    
}

GlobalType_t pcf8563_read_timer(RTC_TimerInfo *info)
{
    uint8_t res;
    uint8_t data[7];
    
    res = pcf8563_read_regs(PCF8563_REG_SECONDS, data, 7);

    if (res != 0)
    {
        return RT_FAIL;  
    }
    
    __disable_irq();
    info->seconds = bcdToDec(data[0]&0x7f);
    info->minutes = bcdToDec(data[1]&0x7f);
    info->hour = bcdToDec(data[2]&0x3f);
    info->date = bcdToDec(data[3]&0x3f);
    info->weekday = data[4]&0x07;
    info->month = bcdToDec(data[5]&0x1f);
    info->year = bcdToDec(data[6]);
    __enable_irq();
    
    return RT_OK;
}

//when set timer, switch to 24h mode
GlobalType_t pcf8563_set_timer(RTC_TimerInfo *info)
{
    uint8_t res;
    uint8_t data[7];
    
    data[0] = decToBcd(info->seconds);
    data[1] = decToBcd(info->minutes);
    data[2] = decToBcd(info->hour);     //set hours, use 24h forever
    data[3] = decToBcd(info->date); 
    data[4] = info->weekday;
    data[5] = decToBcd(info->month);
    data[6] = decToBcd(info->year);
    
    res = pcf8563_set_regs(PCF8563_REG_SECONDS, data, 7);
    if (res != 0)
    {
        return RT_FAIL;
    }

    return RT_OK;
}

GlobalType_t pcf8563_set_alarm(RTC_AlarmInfo *Info)
{
    uint8_t data[4];
    uint8_t res;
    
    data[0] = decToBcd(Info->minutes);
    data[1] = decToBcd(Info->hour);
    data[2] = (1<<7);                   //day, ingore
    data[3] = decToBcd(Info->weekday);
    
    res = pcf8563_set_regs(PCF8563_REG_ALARM_MINUTE, data, 4);
    if (res != 0)
    {
        return RT_FAIL;
    }
    
    return RT_OK;
}

GlobalType_t pcf8563_ctrl_alarm(FunctionalState state)
{
    uint8_t res;
    uint8_t alarm_val;
    
    if(state == ENABLE)
    {
       alarm_val = (1<<1);
    }
    else
    {
       alarm_val = 0;
    }
    
    res = pcf8563_set_regs(PCF8563_REG_CONTROL2, &alarm_val, 1);
    if (res != 0)
    {
        return RT_FAIL;  
    }
    
    return RT_OK;   
}

RTC_AlarmInfo pcf8563_formward_minute(RTC_TimerInfo *info, uint8_t minute)
{
    RTC_AlarmInfo alarm_info_;
    
    alarm_info_.minutes = info->minutes + minute;
    
    if(alarm_info_.minutes < 60)
    {
        alarm_info_.hour = info->hour;
        alarm_info_.weekday = info->weekday;
    }
    else
    {
        alarm_info_.hour = info->hour + 1;
        if(alarm_info_.hour < 24)
        {
            alarm_info_.weekday = info->weekday; 
        }
        else
        {
            alarm_info_.weekday = info->weekday + 1;
            if(alarm_info_.weekday == 7)
            {
                alarm_info_.weekday = 0;
            }
        }
    }
    
    return alarm_info_;
}

void EXTI9_5_IRQHandler(void)
{
    if(__HAL_GPIO_EXTI_GET_IT(GPIO_PIN_6) != RESET)
    {
       is_alarm_on = 1;
        __HAL_GPIO_EXTI_CLEAR_IT(GPIO_PIN_6); 
    }
    
    HAL_NVIC_DisableIRQ(EXTI9_5_IRQn); 
}

uint8_t get_clear_alarm(void)
{
    uint8_t alarm = 0;
    
    if(is_alarm_on == 1)
    {
        alarm = is_alarm_on;
        is_alarm_on = 0;
        HAL_NVIC_EnableIRQ(EXTI9_5_IRQn);
        pcf8563_ctrl_alarm(ENABLE);
    }
    
    return alarm;
}
