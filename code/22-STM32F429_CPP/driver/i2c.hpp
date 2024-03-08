//////////////////////////////////////////////////////////////////////////////
//  (c) copyright 2023-by Persional Inc.  
//  All Rights Reserved
//
//  Name:
//      gpio.hpp
//
//  Purpose:
//      usart driver interface process.
//
// Author:
//      @zc
//
//  Assumptions:
//
//  Revision History:
//
/////////////////////////////////////////////////////////////////////////////
#ifndef _I2C_HPP
#define _I2C_HPP

#include "clock.hpp"

class device_i2c
{
public:
    device_i2c(){
    }
    ~device_i2c(){
    }
    
    void set_gpio_scl(GPIO_TypeDef *Port, uint16_t Pin, uint16_t Alternate);
    void set_gpio_sda(GPIO_TypeDef *Port, uint16_t Pin, uint16_t Alternate);
    void set_mode(I2C_HandleTypeDef* hi2c);
    GlobalType_t init();

protected:
    GlobalType_t multi_write(uint16_t DevAddress, uint16_t MemAddress, uint16_t MemAddSize, uint8_t *pData, uint16_t Size, uint32_t Timeout);
    GlobalType_t multi_read(uint16_t DevAddress, uint16_t MemAddress, uint16_t MemAddSize, uint8_t *pData, uint16_t Size, uint32_t Timeout);
    GlobalType_t signal_write(uint16_t DevAddress, uint8_t *pData, uint16_t Size, uint32_t Timeout);
    GlobalType_t signal_read(uint16_t DevAddress, uint8_t *pData, uint16_t Size, uint32_t Timeout);

private:
    GPIO_TypeDef *scl_port_;
    uint16_t scl_pin_;
    uint16_t scl_alternate_;

    GPIO_TypeDef *sda_port_;
    uint16_t sda_pin_;
    uint16_t sda_alternate_;

    I2C_HandleTypeDef* hi2c_;

    clock_dev clk_dev_;

private:
    void init_gpio(void);
};

void device_i2c::set_gpio_scl(GPIO_TypeDef *Port, uint16_t Pin, uint16_t Alternate)
{
    scl_port_ = Port;
    scl_pin_ = Pin;
    scl_alternate_ = Alternate;
}

void device_i2c::set_gpio_sda(GPIO_TypeDef *Port, uint16_t Pin,  uint16_t Alternate)
{
    sda_port_ = Port;
    sda_pin_ = Pin;
    sda_alternate_ = Alternate;
}

void device_i2c::init_gpio(void)
{
    GPIO_InitTypeDef GPIO_InitStruct = {0};
        
    clk_dev_.enable_gpio_clk(scl_port_);
    clk_dev_.enable_gpio_clk(sda_port_);
    
    GPIO_InitStruct.Pin = scl_pin_;
    GPIO_InitStruct.Mode = GPIO_MODE_AF_OD;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
    GPIO_InitStruct.Alternate = scl_alternate_;
    HAL_GPIO_Init(scl_port_, &GPIO_InitStruct);
    
    GPIO_InitStruct.Pin = sda_pin_;
    GPIO_InitStruct.Mode = GPIO_MODE_AF_OD;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
    GPIO_InitStruct.Alternate = sda_alternate_;
    HAL_GPIO_Init(sda_port_, &GPIO_InitStruct);
}

void device_i2c::set_mode(I2C_HandleTypeDef* hi2c)
{
    hi2c_ = hi2c;
}

GlobalType_t device_i2c::init()
{
    clk_dev_.enable_i2c_clk(hi2c_->Instance);
    
    init_gpio();
    
    if (HAL_I2C_Init(hi2c_) != HAL_OK)
    {
        return RT_FAIL;
    }

    if (HAL_I2CEx_ConfigAnalogFilter(hi2c_, I2C_ANALOGFILTER_ENABLE) != HAL_OK)
    {
        return RT_FAIL;
    }

    if (HAL_I2CEx_ConfigDigitalFilter(hi2c_, 0) != HAL_OK)
    {
        return RT_FAIL;
    }
    
    return RT_OK;
}

GlobalType_t device_i2c::multi_write(uint16_t DevAddress, uint16_t MemAddress, uint16_t MemAddSize, uint8_t *pData, uint16_t Size, uint32_t Timeout)
{
    uint8_t res;
    
    res = HAL_I2C_Mem_Write(hi2c_, DevAddress, MemAddress, MemAddSize, pData, Size, Timeout);  
    
    if(res != HAL_OK)
    {
        return RT_FAIL;
    }
    
    return RT_OK;   
}

GlobalType_t device_i2c::multi_read(uint16_t DevAddress, uint16_t MemAddress, uint16_t MemAddSize, uint8_t *pData, uint16_t Size, uint32_t Timeout)
{
    uint8_t res;
    
    res = HAL_I2C_Mem_Read(hi2c_, DevAddress, MemAddress, MemAddSize, pData, Size, Timeout);  
    
    if(res != HAL_OK)
    {
        return RT_FAIL;
    }
    
    return RT_OK;  
}

GlobalType_t device_i2c::signal_write(uint16_t DevAddress, uint8_t *pData, uint16_t Size, uint32_t Timeout)
{
    if(HAL_I2C_Master_Transmit(hi2c_, DevAddress, pData, Size, Timeout) != HAL_OK)
        return RT_FAIL;

    return RT_OK;      
}

GlobalType_t device_i2c::signal_read(uint16_t DevAddress, uint8_t *pData, uint16_t Size, uint32_t Timeout)
{
    if(HAL_I2C_Master_Receive(hi2c_, DevAddress, pData, Size, Timeout) != HAL_OK)
        return RT_FAIL;
    
    return RT_OK;    
}

#endif
