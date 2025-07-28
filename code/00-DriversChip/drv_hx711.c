//////////////////////////////////////////////////////////////////////////////
//  (c) copyright 2023-by Persional Inc.  
//  All Rights Reserved
//
//  Name:
//      drv_hx711.c
//
//  Purpose:
//      drv_i2c_ds3213.c
//      hardware: 
//          use not hardware i2c interface.
//          SOFT_I2C4
//              I2C_SCL - PB4 - DCMI_SCL
//              I2C_SDA - PB3 - DCMI_SDA
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
#include "drv_hx711.h"
#include "drv_target.h"

GlobalType_t hx711_driver_init(void)
{
    GPIO_InitTypeDef GPIO_InitStruct = {0};

    HX711_CLK_ENABLE();
    
    HX711_SCK_LOW;
    
    //with outside upload.
    GPIO_InitStruct.Pin = HX711_SCK_PIN;
    GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
    HAL_GPIO_Init(HX711_SCK_PORT, &GPIO_InitStruct);

    GPIO_InitStruct.Pin = HX711_SDA_PIN;
    GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    HAL_GPIO_Init(HX711_SDA_PORT, &GPIO_InitStruct);
    
    return RT_OK;
}

uint32_t HX711_Read(void)
{
    unsigned long count; 
    unsigned char i;
    uint32_t index = 0;

    delay_us(10);
    HX711_SCK_LOW; 
    count=0;
    do
    {
        index++;
    }while(HX711_SDA_READ && index < 100000);
    
    if (index >= 100000) {
        return 0;
    }

    for(i=0; i<24; i++)
    { 
        HX711_SCK_HIGH; 
        count = count<<1; 
        delay_us(10);
        HX711_SCK_LOW;       
        if(HX711_SDA_READ)
        count |= 0x1;

        delay_us(10);
    }

    HX711_SCK_HIGH; 
    count=count^0x800000;
    delay_us(10);
    HX711_SCK_LOW;  
    return(count);
}
