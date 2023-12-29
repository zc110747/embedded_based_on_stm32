//////////////////////////////////////////////////////////////////////////////
//  (c) copyright 2023-by Persional Inc.  
//  All Rights Reserved
//
//  Name:
//      drv_soft_spi.c
//
//  Purpose:
//      spi driver for flash.
//
// Author:
//      @zc
//
//  Assumptions:
//
//  Revision History:
//
/////////////////////////////////////////////////////////////////////////////
#include "drv_soft_spi.h"
#include <string.h>

static uint8_t is_spi_init[SOFT_SPI_NUM] = {0};
static SOFT_SPI_INFO spi_list[SOFT_SPI_NUM] = {0};
static SOFT_SPI_INFO *spi_info_ptr;

#define SPI_SCK_H()         HAL_GPIO_WritePin(spi_info_ptr->sck_port, spi_info_ptr->sck_pin, GPIO_PIN_SET)
#define SPI_SCK_L()         HAL_GPIO_WritePin(spi_info_ptr->sck_port, spi_info_ptr->sck_pin, GPIO_PIN_RESET)
#define SPI_MOSI_H()        HAL_GPIO_WritePin(spi_info_ptr->mosi_port, spi_info_ptr->mosi_pin, GPIO_PIN_SET)
#define SPI_MOSI_L()        HAL_GPIO_WritePin(spi_info_ptr->mosi_port, spi_info_ptr->mosi_pin, GPIO_PIN_RESET)
#define SPI_MISO_INPUT()    (HAL_GPIO_ReadPin(spi_info_ptr->miso_port, spi_info_ptr->miso_pin) == GPIO_PIN_SET)

static void spi_delay(uint32_t count)
{
    unsigned int i, j;

    for(i=0; i<count; i++)
    {
        for(j=0; j<3; j++)
        {
        }
    }    
}

uint8_t spi_soft_init(uint8_t soft_spi_num, SOFT_SPI_INFO *info_ptr)
{
    GPIO_InitTypeDef GPIO_InitStruct = {0};

    if(soft_spi_num >= SOFT_SPI_NUM)
        return SPI_ERROR;
    
    if(is_spi_init[soft_spi_num])
    {
        //spi already init, use same pin
        //support spi with mutliple external device.
        if(memcmp((char *)info_ptr, (char *)&spi_list[soft_spi_num], sizeof(SOFT_SPI_INFO)) != 0)
        {
            return SPI_ERROR;
        }
        return SPI_OK;
    }  
    
    is_spi_init[soft_spi_num] = 1;
    
    spi_info_ptr = &spi_list[soft_spi_num];
    memcpy((char *)spi_info_ptr, (char *)info_ptr, sizeof(SOFT_SPI_INFO));
    
    SPI_MOSI_H();
    if(spi_info_ptr->mode == SPI_MODE_CPOL_H_CPHA_1
    || spi_info_ptr->mode == SPI_MODE_CPOL_H_CPHA_2)
    {
        SPI_SCK_H();
    }
    else
    {
        SPI_SCK_L();
    }
    
    //sck choose
    GPIO_InitStruct.Pin = spi_info_ptr->sck_pin;
    GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
#ifdef GPIO_SPEED_FREQ_VERY_HIGH
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
#else
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
#endif
    HAL_GPIO_Init(spi_info_ptr->sck_port, &GPIO_InitStruct);
    
    //mosi
    GPIO_InitStruct.Pin = spi_info_ptr->mosi_pin;
    HAL_GPIO_Init(spi_info_ptr->mosi_port, &GPIO_InitStruct);   

    //miso
    GPIO_InitStruct.Pin = spi_info_ptr->miso_pin;
    GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    HAL_GPIO_Init(spi_info_ptr->miso_port, &GPIO_InitStruct);      
    return SPI_OK;
}

//SPI_MODE_CPOL_L_CPHA_1
static uint8_t spi_soft_rw_byte_phase_0(SOFT_SPI_INFO* spi_info_ptr, uint8_t data)
{
    uint8_t index;
    uint8_t rdata = 0;
    
    for(index=0; index<8; index++)
    {
        SPI_SCK_L();
        
        if(data & 0x80)
        {
            SPI_MOSI_H();
        }
        else
        {
            SPI_MOSI_L();
        }
        data <<= 1;                 //msb first
        spi_delay(SPI_DELAY_COUNT);
        
        SPI_SCK_H();                //default is low, the first edge
        rdata <<= 1;
        rdata |= SPI_MISO_INPUT();  //lock the rx data
        
        spi_delay(SPI_DELAY_COUNT);
    }
    
    SPI_SCK_L();
    return rdata;
}

//SPI_MODE_CPOL_L_CPHA_2
static uint8_t spi_soft_rw_byte_phase_1(SOFT_SPI_INFO* spi_info_ptr, uint8_t data)
{
    uint8_t index;
    uint8_t rdata = 0;
    
    for(index=0; index<8; index++)
    {
        SPI_SCK_H();              
        
        if(data & 0x80)
        {
            SPI_MOSI_H();
        }
        else
        {
            SPI_MOSI_L();
        }
        data <<= 1;                
        spi_delay(SPI_DELAY_COUNT);
        
        SPI_SCK_L();       //default is low, the second edge         
        rdata <<= 1;
        rdata |= SPI_MISO_INPUT();  
        
        spi_delay(SPI_DELAY_COUNT);
    }
    
    SPI_SCK_L();
    return rdata;    
}

//SPI_MODE_CPOL_H_CPHA_1
static uint8_t spi_soft_rw_byte_phase_2(SOFT_SPI_INFO* spi_info_ptr, uint8_t data)
{
    uint8_t index;
    uint8_t rdata = 0;

    for(index=0; index<8; index++)
    {
        SPI_SCK_H();
        if(data & 0x80)
        {
            SPI_MOSI_H();
        }
        else
        {
            SPI_MOSI_L();
        }
        data <<= 1;               
        spi_delay(SPI_DELAY_COUNT);
        
        SPI_SCK_L();                //default is high, the first edge       
        rdata <<= 1;
        rdata |= SPI_MISO_INPUT();  //lock the rx data
        
        spi_delay(SPI_DELAY_COUNT);
    }
    
    SPI_SCK_H();
    return rdata;
}

//SPI_MODE_CPOL_H_CPHA_2
static uint8_t spi_soft_rw_byte_phase_3(SOFT_SPI_INFO* spi_info_ptr, uint8_t data)
{
    uint8_t index;
    uint8_t rdata = 0;
    
    for(index=0; index<8; index++)
    {
        SPI_SCK_L();                   
        if(data & 0x80)
        {
            SPI_MOSI_H();
        }
        else
        {
            SPI_MOSI_L();
        }
        data <<= 1;                
        spi_delay(SPI_DELAY_COUNT);
        
        SPI_SCK_H();                //default is high, the second edge   
        rdata <<= 1;
        rdata |= SPI_MISO_INPUT();   //lock the rx data
        
        spi_delay(SPI_DELAY_COUNT);
    }
    
    SPI_SCK_H();
    return rdata;    
}

typedef uint8_t (*spi_rw_func)(SOFT_SPI_INFO*, uint8_t);
//static spi_rw_func function_list[] = {
//    spi_soft_rw_byte_phase_0,
//    spi_soft_rw_byte_phase_1,
//    spi_soft_rw_byte_phase_2,
//    spi_soft_rw_byte_phase_3
//};

uint8_t spi_soft_rw_byte(uint8_t soft_spi_num, uint8_t *wdata, uint8_t *rdata, uint8_t size)
{
    uint8_t index;
    SOFT_SPI_INFO* spi_info_ptr;
    
    if(soft_spi_num >= SOFT_SPI_NUM)
        return SPI_ERROR;
       
    if(!is_spi_init[soft_spi_num])
        return SPI_ERROR;
    
    spi_info_ptr = &spi_list[soft_spi_num];
    spi_rw_func pfunc = spi_soft_rw_byte_phase_0;
    
    switch(spi_list[soft_spi_num].mode)
    {
        case SPI_MODE_CPOL_L_CPHA_1:
            pfunc = spi_soft_rw_byte_phase_0;
            break;
        case SPI_MODE_CPOL_L_CPHA_2:
            pfunc = spi_soft_rw_byte_phase_1;
            break;
        case SPI_MODE_CPOL_H_CPHA_1:
            pfunc = spi_soft_rw_byte_phase_2;
            break;
        case SPI_MODE_CPOL_H_CPHA_2:
            pfunc = spi_soft_rw_byte_phase_3;
            break;
    }
    
    if(rdata == NULL && wdata == NULL)
    {
        return SPI_ERROR;
    }
    else if(rdata == NULL)
    {
        //only wrtie
        for(index=0; index<size; index++)
        {
            pfunc(spi_info_ptr, wdata[index]);
        }
    }
    else if(wdata == NULL)
    {
        //only read
        for(index=0; index<size; index++)
        {
            rdata[index] = pfunc(spi_info_ptr, 0xff);
        }
    }
    else
    {      
        for(index=0; index<size; index++)
        {
            rdata[index] = pfunc(spi_info_ptr, wdata[index]);
        } 
    }
    return SPI_OK;
}
