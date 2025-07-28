//////////////////////////////////////////////////////////////////////////////
//  (c) copyright 2023-by Persional Inc.  
//  All Rights Reserved
//
//  Name:
//      drv_alg.c
//
//  Purpose:
//      driver for alg
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
#include "drv_alg.h"
#include <stdlib.h>

//------ static internal variable ------
static RNG_HandleTypeDef hrng;
static CRC_HandleTypeDef hcrc;

//------ static internal function ------
static GlobalType_t rng_init(void);
static GlobalType_t rng_sf_init(int seed);
static GlobalType_t crc_init(void);

//------ global function ------
GlobalType_t alg_driver_init(void)
{
    GlobalType_t result;
    
    //rng init
    result = rng_init();
    
    //crc init
    result |= crc_init();
    
    // rng software init
    rng_sf_init(HAL_GetTick());
    return result;
}

int rng_sf_get_value(void)
{
    return rand();
}

HAL_StatusTypeDef rng_get_value(uint32_t *val)
{
    return HAL_RNG_GenerateRandomNumber(&hrng, val);
}

uint32_t calc_hw_crc32(uint32_t *pbuffer, uint32_t size)
{
    uint32_t value;
    
    value = HAL_CRC_Calculate(&hcrc, pbuffer, size);
    
    return value;
}

// ����crc8���㷨
#define CRC8_POLYNOMIAL        0x05
#define CRC8_INIT              0xFF

// ����CRC8���
uint8_t calc_crc8(uint8_t *ptr, uint32_t len) {
    uint8_t i;
    uint8_t crc = CRC8_INIT;

    // ������������
    while (len--) {
        crc ^= *ptr++;
        // ����ÿ���ֽڵ� 8 λ
        for (i = 0; i < 8; ++i) {
            if (crc & 0x80) {
                crc = (crc << 1) ^ CRC8_POLYNOMIAL;
            } else {
                crc = (crc << 1);
            }
        }
    }
    return crc;
}

// ����crc16���㷨
// ���� CRC16ģʽ�Ķ���ʽ�ͳ�ʼֵ
#define CRC16_POLYNOMIAL        0x8005
#define CRC16_INIT              0xFFFF

// ����CRC16���
uint16_t calc_crc16(uint8_t *ptr, uint32_t len) {
    uint8_t i;
    uint16_t crc = CRC16_INIT;

    // ������������
    while (len--) {
        crc ^= (uint16_t)*ptr++ << 8;
        // ����ÿ���ֽڵ� 8 λ
        for (i = 0; i < 8; ++i) {
            if (crc & 0x8000) {
                crc = (crc << 1) ^ CRC16_POLYNOMIAL;
            } else {
                crc = (crc << 1);
            }
        }
    }
    return crc;
}

// ����crc32���㷨
#define CRC32_POLYNOMIAL    0x04C11DB7
#define CRC32_INIT          0xFFFFFFFF
uint32_t calc_crc32(uint8_t *ptr, size_t len) 
{
    uint8_t i;
    uint32_t crc = CRC32_INIT;
    while(len--)
    {
        crc ^= (uint32_t)*ptr++<<24;
        for (i=0; i<8; i++) 
        {
            if (crc & 0x80000000)
            {
                crc = (crc<<1)^CRC32_POLYNOMIAL;
            } 
            else
            {
                crc = (crc<<1);
            }
        }
    }
    return crc;
}

//------ internal function ------
static GlobalType_t rng_init(void)
{
    __HAL_RCC_RNG_CLK_ENABLE();
    
    hrng.Instance = RNG;
    
    if (HAL_RNG_Init(&hrng) != HAL_OK)
    {
        return RT_FAIL;
    }
   
    return RT_OK;    
}

static GlobalType_t crc_init(void)
{
    __HAL_RCC_CRC_CLK_ENABLE();
    
    hcrc.Instance = CRC;
    if (HAL_CRC_Init(&hcrc) != HAL_OK)
    {
        return RT_FAIL;
    }
    
    return RT_OK;
}

static GlobalType_t rng_sf_init(int seed)
{
    srand(seed);
    
    return RT_OK;
}
