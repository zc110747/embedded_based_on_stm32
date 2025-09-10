//////////////////////////////////////////////////////////////////////////////
//  (c) copyright 2025-by Persional Inc.  
//  All Rights Reserved
//
//  Name:
//      drv_spi_adxl345.c
//
//  Purpose:
//      driver for spi module.
//      HW: SPI5_CS - PF5, 
//          SPI5_CLK - PF7
//          SPI5_MISO - PF8
//          SPI5_MOSI - PF9
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
#include "drv_spi_adxl345.h"

extern SPI_HandleTypeDef hspi5;

#define ADXL345_DATA_TIMEOUT     2

#define ADXL345_CS_PORT       GPIOF
#define ADXL345_CS_PIN        GPIO_PIN_5

#define ADXL345_CS_HIGH       HAL_GPIO_WritePin(ADXL345_CS_PORT, ADXL345_CS_PIN, GPIO_PIN_SET)
#define ADXL345_CS_LOW        HAL_GPIO_WritePin(ADXL345_CS_PORT, ADXL345_CS_PIN, GPIO_PIN_RESET)

#define ADXL_DEVICE_ID      0xE5

static uint8_t is_adxl_error = 0;
static int adxl345_single_read(uint8_t reg_addr, uint8_t *data);
static int adxl345_single_write(uint8_t reg_addr, uint8_t data);

 
GlobalType_t drv_adxl345_init(void)
{    
    uint8_t adxl_id = 0;
    
    GPIO_InitTypeDef GPIO_InitStruct = {0};

    // SPI-CS引脚初始化
    __HAL_RCC_GPIOF_CLK_ENABLE();

    HAL_GPIO_WritePin(ADXL345_CS_PORT, ADXL345_CS_PIN, GPIO_PIN_SET);
    
    /*Configure GPIO pin : PF6 */
    GPIO_InitStruct.Pin = GPIO_PIN_6;
    GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
    GPIO_InitStruct.Pull = GPIO_PULLUP;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
    HAL_GPIO_Init(ADXL345_CS_PORT, &GPIO_InitStruct);
    
    spi_driver_init();
    
    if(adxl345_single_read(0x00, &adxl_id) == HAL_OK)
    {
        if (adxl_id != ADXL_DEVICE_ID)
        {
            is_adxl_error = 1;
            return RT_FAIL;
        }
    }
    else
    {
        is_adxl_error = 1;
    }
    
    adxl345_single_write(0x31, 0x0);//SPI 4-wire, 10bit, right-algin, +-2g
    adxl345_single_write(0x2C, 0xA);//speed 100hz
    adxl345_single_write(0x2D, 0x8);//normal mode
    adxl345_single_write(0x2E, 0x0);//no interrupt
    return RT_OK;
}

int drv_adxl345_multi_read(int16_t *Data_X, int16_t *Data_Y, int16_t *Data_Z)
{
    HAL_StatusTypeDef status;
    uint8_t addr = 0x32;
    uint8_t buffer[6];

    // 读取ID失败, ADL345异常
    if (is_adxl_error != 0) {
        return -1;
    }   

    ADXL345_CS_LOW;
    addr |= 0xC0; // 连续读取数据(RW和MB位)
	status = HAL_SPI_Transmit(&hspi5, &addr, 1, 10);
    if (status != HAL_OK) {
        return -1;
    }
    status = HAL_SPI_Receive(&hspi5, buffer, 6, 10);
    if (status != HAL_OK) {
        return -1;
    }
    ADXL345_CS_HIGH;

    *Data_X = (buffer[1] << 8) | buffer[0];
    *Data_Y = (buffer[3] << 8) | buffer[2];
    *Data_Z = (buffer[5] << 8) | buffer[4];
    return 0;
}

// 单字节读取
static int adxl345_single_read(uint8_t reg_addr, uint8_t *data) 
{
    HAL_StatusTypeDef status;

    reg_addr |= 0x80; // 设置读位

    ADXL345_CS_LOW;
    status = HAL_SPI_Transmit(&hspi5, &reg_addr, 1, 10);    // 发送寄存器地址和读功能位
    if (status != HAL_OK) {
        return -1;
    }

    status = HAL_SPI_Receive(&hspi5, data, 1, 10);          // 接收数据
    if (status != HAL_OK) {
        return -1;
    }
    ADXL345_CS_HIGH;
    return 0;
}

// 单字节写入
static int adxl345_single_write(uint8_t reg_addr, uint8_t data) 
{
    HAL_StatusTypeDef status;

    reg_addr &= 0x7F; // 清除读位

    ADXL345_CS_LOW;
    status = HAL_SPI_Transmit(&hspi5, &reg_addr, 1, 10);    // 发送寄存器地址和写功能位
    if (status != HAL_OK) {
        return -1;
    }
    status = HAL_SPI_Transmit(&hspi5, &data, 1, 10);          // 发送数据
    if (status != HAL_OK) {
        return -1;
    }
    ADXL345_CS_HIGH;
    return 0;
}

// 读取x,y,z轴数据
int drv_adxl345_single_read(int16_t *Data_X, int16_t *Data_Y, int16_t *Data_Z)
{
    uint8_t data[6];
    uint8_t reg_addr = 0x32;
    uint8_t index;
    
    
    for (index = 0; index < 6; index++) {
        if (!adxl345_single_read(reg_addr + index, &data[index])) {
            return -1;
        }
    }

    *Data_X = (data[0] << 8) | data[1];
    *Data_Y = (data[2] << 8) | data[3];
    *Data_Z = (data[4] << 8) | data[5];
    return 0;
}
