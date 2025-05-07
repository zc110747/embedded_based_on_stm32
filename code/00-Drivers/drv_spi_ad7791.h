//////////////////////////////////////////////////////////////////////////////
//  (c) copyright 2023-by LeXin Inc.  
//  All Rights Reserved
//
//  Name:
//      drv_spi_ad7791.h
//
//  Purpose:
//      AD7791   - SPI1
//      SPI1_CS   - PA4
//      SPI1_CLK  - PA5
//      SPI1_MOSI - PA7
//      SPI1_MISO - PA6
//
// Author:
//      @zc
//
// Assumptions:
//
// Revision History:
//      Version V1.0b1 Create.
/////////////////////////////////////////////////////////////////////////////
#ifndef __DRVAD7791_H__
#define __DRVAD7791_H__

#include "drv_spi.h"

#define RT_AD7791_OK            0
#define RT_AD7791_NOT_READY     1

void ad7791_init(void);
void ad7791_rst(void);
void ad7791_startconvert(void);
uint8_t ad7791_read(uint32_t *rdata);
uint8_t ad7791_loop_read(uint32_t *rdata);
uint8_t ad7791_reg_read(uint8_t reg, uint8_t *rdata);

#define AD7791_STATUS_REG       0x08
#define AD7791_MODE_REG         0x18
#define AD7791_FILTER_REG       0x28

//      SPI1_CS   - PA4
//      SPI1_CLK  - PA5
//      SPI1_MOSI - PA7
//      SPI1_MISO - PA6
#define SPI1_CS_PORT        GPIOA
#define SPI1_CS_PIN         GPIO_PIN_4
#define SPI1_CLK_PORT       GPIOA
#define SPI1_CLK_PIN        GPIO_PIN_5
#define SPI1_MOSI_PORT      GPIOA
#define SPI1_MOSI_PIN       GPIO_PIN_7
#define SPI1_MISO_PORT      GPIOA
#define SPI1_MISO_PIN       GPIO_PIN_6

#define AD7791_CS_LOW      	HAL_GPIO_WritePin(SPI1_CS_PORT,     SPI1_CS_PIN,    GPIO_PIN_RESET)
#define AD7791_CS_HIGH     	HAL_GPIO_WritePin(SPI1_CS_PORT,     SPI1_CS_PIN,    GPIO_PIN_SET)
#define AD7791_CLK_LOW     	HAL_GPIO_WritePin(SPI1_CLK_PORT,    SPI1_CLK_PIN,   GPIO_PIN_RESET)
#define AD7791_CLK_HIGH    	HAL_GPIO_WritePin(SPI1_CLK_PORT,    SPI1_CLK_PIN,   GPIO_PIN_SET)
#define AD7791_DIN_LOW     	HAL_GPIO_WritePin(SPI1_MOSI_PORT,   SPI1_MOSI_PIN,  GPIO_PIN_RESET)
#define AD7791_DIN_HIGH   	HAL_GPIO_WritePin(SPI1_MOSI_PORT,   SPI1_MOSI_PIN,  GPIO_PIN_SET)
#define AD7791_DOUT_nRDY    (HAL_GPIO_ReadPin(SPI1_MISO_PORT,   SPI1_MISO_PIN) == GPIO_PIN_SET?1:0)
#endif
