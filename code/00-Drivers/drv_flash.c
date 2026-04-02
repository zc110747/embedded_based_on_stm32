//////////////////////////////////////////////////////////////////////////////
//  (c) copyright 2023-by Persional Inc.  
//  All Rights Reserved
//
//  Name:
//      drv_flash.c
//
//  Purpose:
//      
//
//  Author:
//      @公众号：<嵌入式技术总结>
//
//  Assumptions:
//	
//
//  Revision History:
//
/////////////////////////////////////////////////////////////////////////////
#include "drv_flash.h"

uint16_t flash_get_sector(uint32_t address);

FlashType_t flash_erase_device(uint32_t sector, uint32_t nb_sectors)
{
    FLASH_EraseInitTypeDef FlashEraseInit = {0};
    uint32_t sector_error;
    
    HAL_FLASH_Unlock(); 

    //erase page0
    FlashEraseInit.TypeErase = FLASH_TYPEERASE_SECTORS;     // 按SECTORS擦除
    FlashEraseInit.Banks = FLASH_BANK_1;                    // 擦除BANK1区域，此型号只有BANK_1
    FlashEraseInit.Sector = sector;                         // 擦除的起始扇区
    FlashEraseInit.NbSectors = nb_sectors;                  // 擦除的扇区数
    FlashEraseInit.VoltageRange = FLASH_VOLTAGE_RANGE_3;    // 擦除时FLASH工作电压范围
    
    if(HAL_FLASHEx_Erase(&FlashEraseInit, &sector_error) != HAL_OK)  
    {
        HAL_FLASH_Lock(); 
        return FLASH_ERROR;
    }

    HAL_FLASH_Lock(); 
    return FLASH_OK;
}

uint16_t flash_get_sector(uint32_t address)
{
	if (address < FLASH_SECTOR1_ADDRESS) {
   		return FLASH_SECTOR_0; 
    } else if (address < FLASH_SECTOR2_ADDRESS) {
        return FLASH_SECTOR_1;
    } else if (address < FLASH_SECTOR3_ADDRESS) {
        return FLASH_SECTOR_2;
    } else if (address < FLASH_SECTOR4_ADDRESS) {
        return FLASH_SECTOR_3;
    } else if (address < FLASH_SECTOR5_ADDRESS) {
        return FLASH_SECTOR_4;
    } else if (address < FLASH_SECTOR6_ADDRESS) {
        return FLASH_SECTOR_5;
    } else if (address < FLASH_SECTOR7_ADDRESS) {
        return FLASH_SECTOR_6;
    } else if (address < FLASH_SECTOR8_ADDRESS) {
        return FLASH_SECTOR_7;
    } else if (address < FLASH_SECTOR9_ADDRESS) {
        return FLASH_SECTOR_8;
    } else if (address < FLASH_SECTOR10_ADDRESS) {
        return FLASH_SECTOR_9;
    } else if (address < FLASH_SECTOR11_ADDRESS) {
        return FLASH_SECTOR_10;
    } else {
        return FLASH_SECTOR_11;
    }
}
   
// 写FLASH设备，按字写入
FlashType_t flash_write_word(uint32_t address, uint32_t *pbuffer, uint32_t size)
{
    uint32_t index;
    
    // 写入地址必须是4字节对齐
    if (address % 4 != 0) {
        return FLASH_ERROR;
    }

    // 如果写入地址或大小不是4字节对齐，有两种处理方式
    // 1. 直接返回错误，此时比较简单(提前暴露问题，不然后续会发生就会多写或者少写，有隐患)
    // 2. 申请4字节对齐的内存，将数据复制进去，写入后释放内存
    if (((uint32_t)pbuffer)%4 != 0) {
        return FLASH_ERROR;
    }

    HAL_FLASH_Unlock();       
    for(index=0; index<size; index++)
    {
        if(HAL_FLASH_Program(FLASH_TYPEPROGRAM_WORD, address+index*4, pbuffer[index]) != HAL_OK)
        {
            HAL_FLASH_Lock();
            return FLASH_ERROR;
        }
    } 
    HAL_FLASH_Lock();
    return FLASH_OK;
}

// 写FLASH设备，按字节写入，不需要考虑对齐和长度问题，不过效率较低
FlashType_t flash_write(uint32_t address, uint8_t *pbuffer, uint32_t size)
{
    uint32_t index;

    HAL_FLASH_Unlock();       
    for(index=0; index<size; index++)
    {
        if(HAL_FLASH_Program(FLASH_TYPEPROGRAM_BYTE, address+index, pbuffer[index]) != HAL_OK)
        {
            HAL_FLASH_Lock();
            return FLASH_ERROR;
        }
    } 
    HAL_FLASH_Lock();
    
    return FLASH_OK;
}

// 读取直接按照字节读，比较简单
FlashType_t flash_read(uint32_t address, uint8_t *pbuffer, uint32_t size)
{
    uint16_t index;
    
    for(index=0; index<size; index++) 
    {
        pbuffer[index] = *((uint8_t *)(address+index));
    }
    return FLASH_OK;    
}

FlashType_t flash_read_word(uint32_t address, uint32_t *pbuffer, uint32_t size)
{
    uint16_t index;
    
    for(index=0; index<size; index++) 
    {
        pbuffer[index] = *((uint32_t *)(address+index));
    }
    return FLASH_OK;    
}
