//////////////////////////////////////////////////////////////////////////////
//  (c) copyright 2023-by Persional Inc.  
//  All Rights Reserved
//
//  Name:
//      drv_i2c.c
//      hardware: 
//          I2C2_SCL ------------ PH4
//          I2C2_SDA ------------ PH5
//          EXIT ---------------- PB12
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
#include "drv_i2c.h"
#include "drv_i2c_pcf8574.h"
#include "drv_soft_i2c.h"

#if I2C_RUN_MODE == I2C_USE_HARDWARE
GlobalType_t pcf8574_driver_init(void)
{
    GPIO_InitTypeDef GPIO_InitStruct = {0};
    
    __HAL_RCC_GPIOB_CLK_ENABLE();

    /*Configure GPIO pin : PB12 */
    GPIO_InitStruct.Pin = GPIO_PIN_12;
    GPIO_InitStruct.Mode = GPIO_MODE_IT_FALLING;
    GPIO_InitStruct.Pull = GPIO_PULLUP;
    HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

    HAL_NVIC_SetPriority(EXTI15_10_IRQn, 0, 0);
    HAL_NVIC_EnableIRQ(EXTI15_10_IRQn);

    return RT_OK;  
}

GlobalType_t pcf8574_i2c_write(uint8_t data)
{
    if(HAL_I2C_Master_Transmit(get_i2c2_handle(), (PCF8574_ADDR<<1) | 0x00, &data, 1, PCF8574_I2C_TIMEOUT) != HAL_OK) {
        return RT_FAIL;
    }
    
    return RT_OK;    
}

GlobalType_t pcf8574_i2c_read(uint8_t *pdata)
{
    if(HAL_I2C_Master_Receive(get_i2c2_handle(), (PCF8574_ADDR<<1) | 0x01, pdata, 1, PCF8574_I2C_TIMEOUT) != HAL_OK) {      
        return RT_FAIL;
    }
    
    return RT_OK;
}
#else
//I2C_SCL: PH4
//I2C_SDA: PH5
//I2C_INT: PB12
GlobalType_t pcf8574_driver_init(void)
{
    GPIO_InitTypeDef GPIO_InitStruct = {0};
    
    //clock need enable before software i2c Init
    __HAL_RCC_GPIOB_CLK_ENABLE(); 
    
    /*Configure GPIO pin : PB12 */
    GPIO_InitStruct.Pin = GPIO_PIN_12;
    GPIO_InitStruct.Mode = GPIO_MODE_IT_FALLING;
    GPIO_InitStruct.Pull = GPIO_PULLUP;
    HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

    HAL_NVIC_SetPriority(EXTI15_10_IRQn, 0, 0);
    HAL_NVIC_EnableIRQ(EXTI15_10_IRQn); 

    return RT_OK;  
}

GlobalType_t pcf8574_i2c_write(uint8_t data)
{
    uint8_t res;
    
    __disable_irq();
    res = i2c_write_device(SOFT_I2C2, PCF8574_ADDR<<1, &data, 1);
    __enable_irq();
    
    if(res != 0)
    {
        return RT_FAIL;
    }
    
    return RT_OK;    
}

GlobalType_t pcf8574_i2c_read(uint8_t *pdata)
{
    uint8_t res;
    
    __disable_irq();
    res = i2c_read_device(SOFT_I2C2, PCF8574_ADDR<<1, pdata, 1);
    __enable_irq();
    
    if(res != 0)
    {
        return RT_FAIL;
    }
    
    return RT_OK;  
}
#endif

void EXTI15_10_IRQHandler(void)
{
    if(__HAL_GPIO_EXTI_GET_IT(GPIO_PIN_12) != RESET)
    {
        __HAL_GPIO_EXTI_CLEAR_IT(GPIO_PIN_12);

        //avoid repeater trigger, just disable until delay
        HAL_NVIC_DisableIRQ(EXTI15_10_IRQn); 
    }
}

