//////////////////////////////////////////////////////////////////////////////
//  (c) copyright 2023-by Persional Inc.  
//  All Rights Reserved
//
//  Name:
//      drv_i2c_ds3213.c
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
#include "drv_i2c_ds3213.h"
#include "drv_soft_i2c.h"
#include <string.h>

static RTC_DS3213_Info  ds3213_rtc_info_ = {
    .year = 24,
    .month = 3,
    .date = 23,
    .weekday = 6,
    .hour = 8,
    .minutes = 48,
    .seconds = 8,
};

static uint8_t bcdToDec(uint8_t val) 
{
	return ((val/16*10) + (val%16));
}

static uint8_t decToBcd(uint8_t val) {
	return ( (val/10*16) + (val%10) );
}

void get_rtc_ds3213_info(RTC_DS3213_Info *rtc_info)
{
    __disable_irq();
    memcpy((char *)rtc_info, (char *)&ds3213_rtc_info_, sizeof(RTC_DS3213_Info));
    __enable_irq();
}

//I2C_SCL: PH4
//I2C_SDA: PH5
//I2C_INT: PB12
GlobalType_t ds3213_driver_init(void)
{
    GPIO_InitTypeDef GPIO_InitStruct = {0};
    
    __HAL_RCC_GPIOC_CLK_ENABLE(); 
    
    /*Configure GPIO pin : PC6 */
    GPIO_InitStruct.Pin = GPIO_PIN_6;
    GPIO_InitStruct.Mode = GPIO_MODE_IT_FALLING;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);
    
    HAL_NVIC_SetPriority(EXTI9_5_IRQn, 0, 0);
    HAL_NVIC_EnableIRQ(EXTI9_5_IRQn); 
 
    ds3213_set_timer(&ds3213_rtc_info_);
    ds3213_read_timer(&ds3213_rtc_info_);
    
    return RT_OK;  
}

GlobalType_t ds3213_update_timer(void)
{
    return ds3213_read_timer(&ds3213_rtc_info_);
}

GlobalType_t ds3213_read_timer(RTC_DS3213_Info *info)
{
    uint8_t res;
    uint8_t data[7];
    
    __disable_irq();
    res = i2c_read_memory(SOFT_I2C4, DS3213_ADDR<<1, DS3213_REG_SECONDS, 1, data, 7);
    __enable_irq();

    if (res != 0)
    {
        return RT_FAIL;
    }
    
    __disable_irq();
    info->seconds = bcdToDec(data[0]);
    info->minutes = bcdToDec(data[1]);
    //12/24 hour mode convert
    if(data[2]&(1<<6)) //12h
    {
        if(data[2]&(1<<5))  //if pm
            info->hour = bcdToDec(data[2]&0x1f) + 12; //convert to 24h
        else
            info->hour = bcdToDec(data[2]&0x1f);
    }
    else
    {
        info->hour = bcdToDec(data[2]&0x3f);
    }
    info->weekday = bcdToDec(data[3]);
    info->date = bcdToDec(data[4]);
    info->month = bcdToDec(data[5]);
    info->year = bcdToDec(data[6]);
    __enable_irq();
    
    return RT_OK;
}

//when set timer, switch to 24h mode
GlobalType_t ds3213_set_timer(RTC_DS3213_Info *info)
{
    uint8_t res;
    uint8_t data[7];
    
    data[0] = decToBcd(info->seconds);
    data[1] = decToBcd(info->minutes);
    data[2] = decToBcd(info->hour);     //set hours, use 24h forever
    data[3] = decToBcd(info->weekday);
    data[4] = decToBcd(info->date);
    data[5] = decToBcd(info->month);
    data[6] = decToBcd(info->year);
    
    __disable_irq();
    res = i2c_write_memory(SOFT_I2C4, DS3213_ADDR<<1, DS3213_REG_SECONDS, 1, data, 7);
    __enable_irq();

    if (res != 0)
    {
        return RT_FAIL;
    }

    return RT_OK;
}

GlobalType_t ds3213_clear_alarm(DS3213_ALARM alarm)
{
    uint8_t data = 0;
    uint8_t res; 

     __disable_irq();
    res = i2c_write_memory(SOFT_I2C2, DS3213_ADDR<<1, DS3213_REG_CONTROL2_STATUS, 1, &data, 1);
    __enable_irq();   
    
    if(res != RT_OK)
        return RT_FAIL;
    
    return RT_OK;
}

GlobalType_t ds3213_control_alarm(DS3213_ALARM alarm, FunctionalState mode)
{
    uint8_t data[2];
    uint8_t res;
    
    __disable_irq();
    res = i2c_read_memory(SOFT_I2C2, DS3213_ADDR<<1, DS3213_REG_CONTROL1, 1, data, 2);
    __enable_irq();

    if(res != RT_OK)
        return RT_FAIL;
    
    if(alarm == DS3213_ALARM1)
    {
        data[1] &= ~(1<<0);     //clear interrupt flag
        if(mode == ENABLE)
        {
            data[0] |= (1<<0) | (1<<2); //enable interrupt check
        }
        else
        {
            data[0] &= ~(1<<0);         //disable interrupt check
        }
    }
    else
    {
        data[1] &= ~(1<<1);     //clear interrupt flag
        if(mode == ENABLE)
        {
            data[0] |= (1<<1) | (1<<2);
        }
        else
        {
            data[0] &= ~(1<<1);
        }        
    }
    
    __disable_irq();
    res = i2c_write_memory(SOFT_I2C2, DS3213_ADDR<<1, DS3213_REG_CONTROL1, 1, data, 2);
    __enable_irq();

    if(res != RT_OK)
        return RT_FAIL;
    
    return RT_OK;
}

GlobalType_t ds3213_set_alarm(DS3213_ALARM alarm, RTC_DS3213_ALARM_Info *Info)
{
    uint8_t data[4];
    uint8_t index = 0;
    uint8_t start_reg = DS3213_REG_ALARM2_MINUTES;
    uint8_t res;
    
    if(alarm == DS3213_ALARM1)
    {
        start_reg = DS3213_REG_ALARM1_SECONDS;
        data[index++] = decToBcd(Info->seconds);
    }
    
    data[index++] = decToBcd(Info->minutes);
    data[index++] = decToBcd(Info->hour);
    data[index++] = decToBcd(Info->weekday) | 1<<6;     //switch to weekday mode
    
    __disable_irq();
    res = i2c_write_memory(SOFT_I2C2, DS3213_ADDR<<1, start_reg, 1, data, index);
    __enable_irq();
    
    if(res != RT_OK)
       return RT_FAIL;  
   
    res = ds3213_control_alarm(alarm, ENABLE);
    if(res != RT_OK)
       return RT_FAIL;

    return RT_OK;
}

