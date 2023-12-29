//////////////////////////////////////////////////////////////////////////////
//  (c) copyright 2023-by Persional Inc.  
//  All Rights Reserved
//
//  Name:
//      drv_soft_spi.h
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
#ifndef _DRV_SOFT_SPI_H
#define _DRV_SOFT_SPI_H

#include "includes.h"

//modify when different sysclk, too small will leader spi error.
#define SPI_DELAY_COUNT         10

#define SOFT_SPI1               0
#define SOFT_SPI2               1
#define SOFT_SPI3               2
#define SOFT_SPI4               3
#define SOFT_SPI5               4
#define SOFT_SPI6               5

#define SOFT_SPI_NUM            6

#define SPI_OK                  0
#define SPI_ERROR               1

//CPOL  default clock level
//CPHA  sampling edge
#define SPI_MODE_CPOL_L_CPHA_1  0
#define SPI_MODE_CPOL_H_CPHA_1  1
#define SPI_MODE_CPOL_L_CPHA_2  3
#define SPI_MODE_CPOL_H_CPHA_2  2   

typedef struct 
{
    uint8_t mode;

    uint32_t sck_pin;
    GPIO_TypeDef *sck_port;

    uint32_t mosi_pin;
    GPIO_TypeDef *mosi_port;

    uint32_t miso_pin;
    GPIO_TypeDef *miso_port;    
}SOFT_SPI_INFO;

uint8_t spi_soft_init(uint8_t soft_spi_num, SOFT_SPI_INFO *info_ptr);
uint8_t spi_soft_rw_byte(uint8_t soft_spi_num, uint8_t *wdata, uint8_t *rdata, uint8_t size);

#endif
