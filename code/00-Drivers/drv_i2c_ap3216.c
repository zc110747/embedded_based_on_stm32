//////////////////////////////////////////////////////////////////////////////
//  (c) copyright 2023-by Persional Inc.  
//  All Rights Reserved
//
//  Name:
//      drv_i2c.c
//          I2C2_SCL ------------ PH4
//          I2C2_SDA ------------ PH5
//  Purpose:
//      driver for i2c module.
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
#include "drv_i2c_ap3216.h"
#include "drv_soft_i2c.h"

#if I2C_RUN_MODE == I2C_USE_HARDWARE
static I2C_HandleTypeDef hi2c2;

GlobalType_t ap3216_driver_init(void)
{
    GlobalType_t res;
    GPIO_InitTypeDef GPIO_InitStruct = {0};

    __HAL_RCC_I2C2_CLK_ENABLE();
    __HAL_RCC_GPIOH_CLK_ENABLE();

    GPIO_InitStruct.Pin = GPIO_PIN_4|GPIO_PIN_5;
    GPIO_InitStruct.Mode = GPIO_MODE_AF_OD;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
    GPIO_InitStruct.Alternate = GPIO_AF4_I2C2;
    HAL_GPIO_Init(GPIOH, &GPIO_InitStruct);

    hi2c2.Instance = I2C2;
    hi2c2.Init.ClockSpeed = 100000;
    hi2c2.Init.DutyCycle = I2C_DUTYCYCLE_2;
    hi2c2.Init.OwnAddress1 = 0;
    hi2c2.Init.AddressingMode = I2C_ADDRESSINGMODE_7BIT;
    hi2c2.Init.DualAddressMode = I2C_DUALADDRESS_DISABLE;
    hi2c2.Init.OwnAddress2 = 0;
    hi2c2.Init.GeneralCallMode = I2C_GENERALCALL_DISABLE;
    hi2c2.Init.NoStretchMode = I2C_NOSTRETCH_DISABLE;
    if (HAL_I2C_Init(&hi2c2) != HAL_OK)
    {
        return RT_FAIL;
    }

    if (HAL_I2CEx_ConfigAnalogFilter(&hi2c2, I2C_ANALOGFILTER_ENABLE) != HAL_OK)
    {
        return RT_FAIL;
    }

    if (HAL_I2CEx_ConfigDigitalFilter(&hi2c2, 0) != HAL_OK)
    {
        return RT_FAIL;
    }

    res = ap3216_reg_write(AP3216C_SYSTEMCONG, 0x40);   //reset the ap3216
    if (res != RT_OK)  
    {
        return RT_FAIL;
    }
    
    HAL_Delay(10);
    
    res = ap3216_reg_write(AP3216C_SYSTEMCONG, 0x03);   //config the ALS+PS+LR
    if (res != RT_OK)  
    {
        return RT_FAIL;
    }
    
    PRINT_LOG(LOG_INFO, HAL_GetTick(), "ap3216 init success!");
    return RT_OK;  
}

GlobalType_t ap3216_i2c_multi_write(uint8_t reg, uint8_t *data, uint8_t size)
{
    uint8_t res;
    
    res = HAL_I2C_Mem_Write(&hi2c2, AP3216C_ADDR<<1, reg, 1, data, size, AP3216C_TIMEOUT);  
    
    if(res != HAL_OK)
    {
        return RT_FAIL;
    }
    
    return RT_OK;
}

GlobalType_t ap3216_i2c_multi_read(uint8_t reg, uint8_t *rdata, uint8_t size)
{
    uint8_t res;
    
    res = HAL_I2C_Mem_Read(&hi2c2, AP3216C_ADDR<<1, reg, 1, rdata, size, AP3216C_TIMEOUT);  
    
    if(res != HAL_OK)
    {
        return RT_FAIL;
    }
    
    return RT_OK;
}
#else
//I2C_SCL: PH4
//I2C_SDA: PH5
GlobalType_t ap3216_driver_init(void)
{
    SOFT_I2C_INFO I2C_Info = {0};
    GlobalType_t res;
    
    //clock need enable before software i2c Init
    __HAL_RCC_GPIOH_CLK_ENABLE(); 

    I2C_Info.scl_pin = GPIO_PIN_4;
    I2C_Info.scl_port = GPIOH;
    I2C_Info.sda_pin = GPIO_PIN_5;
    I2C_Info.sda_port = GPIOH;
    
    if(i2c_soft_init(SOFT_I2C2, &I2C_Info) != I2C_OK)
    {
        return RT_OK;
    }
    
    res = ap3216_reg_write(AP3216C_SYSTEMCONG, 0x40);   //reset the ap3216
    if (res != RT_OK)  
    {
        return RT_FAIL;
    }
    
    HAL_Delay(20);
    
    res = ap3216_reg_write(AP3216C_SYSTEMCONG, 0x03);   //config the ALS+PS+LR
    if (res != RT_OK)  
    {
        return RT_FAIL;
    }
    
    return RT_OK;  
}

GlobalType_t ap3216_i2c_multi_write(uint8_t reg, uint8_t *data, uint8_t size)
{
    uint8_t res;
    
    __disable_irq();
    res = i2c_write_memory(SOFT_I2C2, AP3216C_ADDR<<1, reg, 1, data, size);
    __enable_irq();
    
    if(res != I2C_OK)
        return RT_FAIL;
    
    return RT_OK;
}

GlobalType_t ap3216_i2c_multi_read(uint8_t reg, uint8_t *rdata, uint8_t size)
{
    uint8_t res;
    
    __disable_irq();
    res = i2c_read_memory(SOFT_I2C2, AP3216C_ADDR<<1, reg, 1, rdata, size);
    __enable_irq();   
    
    if(res != I2C_OK)
        return RT_FAIL;
    
    return RT_OK;
}
#endif

GlobalType_t ap3216_reg_read(uint8_t reg, uint8_t data)
{
    return ap3216_i2c_multi_write(reg, &data, 1);
}

GlobalType_t ap3216_reg_write(uint8_t reg, uint8_t data)
{
    return ap3216_i2c_multi_write(reg, &data, 1);
}

void ap3216_loop_run_test(void)
{
    uint8_t buf[6];
    uint8_t index;
    uint8_t is_read_ok = 0;
    AP3216C_INFO ap3216_info; 

    is_read_ok = 1;

    for(index=0; index<6; index++)
    {
        if(ap3216_i2c_multi_read(AP3216C_IRDATALOW+index, &buf[index], 1) != RT_OK)
        {
            is_read_ok = 0;
            PRINT_LOG(LOG_INFO, HAL_GetTick(), "ap3216c i2c device read failed.");
            break;
        }
    }

    if(is_read_ok == 1)
    {
        if(buf[0]&(1<<7)) 	
            ap3216_info.ir = 0;					
        else 			
            ap3216_info.ir = (((uint16_t)buf[1])<< 2) | (buf[0]&0X03); 			

        ap3216_info.als = ((uint16_t)buf[3]<<8) | buf[2];

        if(buf[4]&(1<<6))	
            ap3216_info.ps = 0;    													
        else 				
            ap3216_info.ps = ((uint16_t)(buf[5]&0X3F)<<4)|(buf[4]&0X0F);

        PRINT_LOG(LOG_INFO, HAL_GetTick(), "ap3216c info:%d, %d, %d.", ap3216_info.ir, ap3216_info.als, ap3216_info.ps);
    }    
}
