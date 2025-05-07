//////////////////////////////////////////////////////////////////////////////
//  (c) copyright 2023-by LeXin Inc.  
//  All Rights Reserved
//
//  Name:
//      drv_spi_ad7791.c
//
//  Purpose:
//      AD7791   - SPI1
//      SPI1_CS   - PA15
//      SPI1_CLK  - PB3
//      SPI1_MOSI - PB5
//      SPI1_MISO - PB4
//
// Author:
//      @zc
//
// Assumptions:
//
// Revision History:
//      Version V1.0b1 Create.
/////////////////////////////////////////////////////////////////////////////
#include "drv_spi_ad7791.h"
#include "drv_target.h"

#define SPI1_WART_TIMEOUT    	10000
#define AD7791_TIMEOUT		    10000

#define AD7791_DELAY            2

uint8_t AD7991_Check_Ready(void);
void AD7791_WR_1BYTE(uint8_t cmd); //write 8-bit data

void ad7791_init(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;

    __HAL_RCC_GPIOA_CLK_ENABLE();
    __HAL_RCC_GPIOB_CLK_ENABLE();
    __HAL_RCC_GPIOC_CLK_ENABLE();
    
    HAL_GPIO_WritePin(SPI1_CS_PORT, SPI1_CS_PIN, GPIO_PIN_SET);
    HAL_GPIO_WritePin(SPI1_CLK_PORT, SPI1_CLK_PIN, GPIO_PIN_SET);
    HAL_GPIO_WritePin(SPI1_MOSI_PORT, SPI1_MOSI_PIN, GPIO_PIN_SET);

    //CS, CLK, MOSI, MISO, 
    GPIO_InitStructure.Pin = SPI1_CS_PIN;
    GPIO_InitStructure.Mode = GPIO_MODE_OUTPUT_PP;
    GPIO_InitStructure.Speed = GPIO_SPEED_HIGH;
    HAL_GPIO_Init(SPI1_CS_PORT, &GPIO_InitStructure); 

    GPIO_InitStructure.Pin = SPI1_CLK_PIN;
    HAL_GPIO_Init(SPI1_CLK_PORT, &GPIO_InitStructure);

    GPIO_InitStructure.Pin = SPI1_MOSI_PIN;
    HAL_GPIO_Init(SPI1_MOSI_PORT, &GPIO_InitStructure);
    
    GPIO_InitStructure.Pin = SPI1_MISO_PIN;
    GPIO_InitStructure.Mode = GPIO_MODE_INPUT;
    HAL_GPIO_Init(SPI1_MISO_PORT, &GPIO_InitStructure);
}

void ad7791_rst(void)
{
    AD7791_CS_HIGH;
    AD7791_DIN_HIGH;
    AD7791_CLK_HIGH;

    AD7791_CS_LOW;
    delay_us(AD7791_DELAY);

    __disable_irq();
    for(uint8_t i=0;i<32;i++)
    {
        AD7791_CLK_LOW;
        delay_us(AD7791_DELAY);
        AD7791_CLK_HIGH;
        delay_us(AD7791_DELAY);
    }
    __enable_irq();

    AD7791_DIN_LOW;
    AD7791_CS_HIGH;  
}
                
void AD7791_WR_1BYTE(uint8_t cmd) //write 8-bit data
{
    __disable_irq();
    for(uint8_t i=0; i<8; i++)
    {
        AD7791_CLK_LOW;
        if ((cmd<<i)&0x80)
        {
            AD7791_DIN_HIGH;
        }
        else 
        {
            AD7791_DIN_LOW;
        }
        delay_us(AD7791_DELAY);
        AD7791_CLK_HIGH;
        delay_us(AD7791_DELAY);
    }
    __enable_irq();
}

void ad7791_startconvert(void)
{  
    AD7791_CS_LOW;

    AD7791_WR_1BYTE(0x10); 
    AD7791_WR_1BYTE(0x86);//single conversion mode, no burn-out current, uni-polar and buffered
}

uint8_t ad7791_loop_read(uint32_t *rdata)
{
    uint32_t data = 0, rbit;

    if(AD7791_DOUT_nRDY == 0)
    {
        AD7791_WR_1BYTE(0x38);//single read

        AD7791_DIN_LOW;
        AD7791_CLK_HIGH;

        for(uint8_t i=1; i<=24; i++)
        {
            delay_us(AD7791_DELAY);
            AD7791_CLK_LOW;
            delay_us(AD7791_DELAY);
            AD7791_CLK_HIGH;
            rbit = AD7791_DOUT_nRDY;
            if (rbit != 0) data |= ((rbit)<<(24-i));
        }
        *rdata = data;

        AD7791_CS_HIGH;

        ad7791_rst();
        return RT_AD7791_OK; 
    }
  
  return RT_AD7791_NOT_READY;
}

uint8_t ad7791_reg_read(uint8_t reg, uint8_t *rdata)
{
	uint8_t rbit = 0;
	uint8_t data = 0;

	AD7791_CLK_HIGH;
	AD7791_CS_LOW;

	AD7791_WR_1BYTE(reg);
  
	AD7791_DIN_LOW;
	AD7791_CLK_HIGH;
  
	for(uint8_t i=1; i<=8; i++)
	{
		delay_us(AD7791_DELAY);
		AD7791_CLK_LOW;
		delay_us(AD7791_DELAY);
		AD7791_CLK_HIGH;
		rbit = AD7791_DOUT_nRDY;
		if (rbit != 0) data |= ((rbit)<<(8-i));
	}

	AD7791_CS_HIGH;
	*rdata = data;
	return RT_OK;
}

uint8_t ad7791_read(uint32_t *rdata)
{
    uint32_t data = 0, rbit;

    AD7791_CS_LOW;

    AD7791_WR_1BYTE(0x10); 
    AD7791_WR_1BYTE(0x86);//single conversion mode, no burn-out current, uni-polar and buffered

    while(AD7791_DOUT_nRDY) {
    }

    AD7791_WR_1BYTE(0x38);//single read

    AD7791_DIN_LOW;
    AD7791_CLK_HIGH;

    for(uint8_t i=1; i<=24; i++)
    {
        delay_us(AD7791_DELAY);
        AD7791_CLK_LOW;
        delay_us(AD7791_DELAY);
        AD7791_CLK_HIGH;
        rbit = AD7791_DOUT_nRDY;
        if (rbit != 0) data |= ((rbit)<<(24-i));
    }

    (*rdata) =  data;

    AD7791_CS_HIGH;

    ad7791_rst();

    return RT_OK;  
}

uint8_t AD7991_Check_Ready(void)
{
    while(AD7791_DOUT_nRDY) {
    }
    return RT_OK;	
}

void ad7791_loop_test(void)
{
    static uint32_t state = RT_AD7791_OK;
    uint32_t adc_value;
    
    if (state == RT_AD7791_OK)
    {
        ad7791_startconvert();
        state = RT_AD7791_NOT_READY;
    }
    else
    {
        state = ad7791_loop_read(&adc_value);
        if(state == RT_AD7791_OK)
        {
            PRINT_LOG(LOG_INFO, HAL_GetTick(), "adc val:%d", adc_value);
        }
    }
}
