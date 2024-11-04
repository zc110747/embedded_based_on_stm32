//////////////////////////////////////////////////////////////////////////////
//  (c) copyright 2023-by Persional Inc.  
//  All Rights Reserved
//
//  Name:
//      drv_spi.c
//
//  Purpose:
//      driver for spi module.
//      HW: CS-PF6, SPI5-PF7(clk), PF8(rx), PF9(tx)
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
#include <string.h>
#include "drv_spi_wq.h"

static uint16_t wq_id = ID_W25Q80;
static uint32_t memory_end = 0x0100000;
static const uint32_t memory_end_limit[] = {
    0x0100000,      //ID_W25Q80
    0x0200000,      //ID_W25Q16
    0x0400000,      //ID_W25Q32
    0x0800000,      //ID_W25Q64
    0x1000000,      //ID_W25Q128
    0x2000000,      //ID_W25Q256
};


static uint16_t wq_read_chipid(void);
static void wq_write_enable(void);
static GlobalType_t wq_wait_busy(uint16_t timeout);
static GlobalType_t wq_write_page(uint32_t start_addr, uint8_t* pbuffer, uint16_t num);
static GlobalType_t wq_write_sector(uint32_t start_addr, uint8_t* pbuffer, uint16_t num);

GlobalType_t wq_driver_init(void)
{
    GlobalType_t xReturn;

    xReturn = spi_driver_init();
    if(xReturn == RT_OK)
    {
        wq_id = wq_read_chipid();
        switch(wq_id)
        {
            case ID_W25Q80:
                memory_end = memory_end_limit[0];
                break;
             case ID_W25Q16:
                memory_end = memory_end_limit[1];
                break;
            case ID_W25Q32:
                memory_end = memory_end_limit[2];
                break;
            case ID_W25Q64:
                memory_end = memory_end_limit[3];
                break;
            case ID_W25Q128:
                memory_end = memory_end_limit[4];
                break;
            case ID_W25Q256:
                memory_end = memory_end_limit[5];
                spi_rw_byte_nocheck(W25X_Enable4ByteAddr); //enter 4byte mode
                break;
            default:
                xReturn = RT_FAIL;
                break;
        }
    }

    return xReturn;
}

uint16_t wq_get_id(void)
{
    return wq_id;
}

GlobalType_t wq_erase_chip(void)   
{                                   
    wq_write_enable(); 
    if(wq_wait_busy(100) != RT_OK)
    {
        return RT_FAIL;
    }
  
	WQ25_CS_ON();
    spi_rw_byte_nocheck(W25X_ChipErase);       
	WQ25_CS_OFF();  	      
    if(wq_wait_busy(4000) != RT_OK)
    {
        return RT_FAIL;
    }
    
    return RT_OK;
}   

//按块擦除, 容量4k
GlobalType_t wq_erase_sector(uint32_t addr)   
{  
    if(addr > memory_end)
    {
        return RT_FAIL;
    }
    
    wq_write_enable(); 
    if(wq_wait_busy(100) != RT_OK)
    {
        return RT_FAIL;
    }
   
    addr = addr - addr%WQ_SECTOR_SIZE;
    WQ25_CS_ON();
    spi_rw_byte_nocheck(W25X_SectorErase);       
    if(wq_id == ID_W25Q256)             
    {
        spi_rw_byte_nocheck((uint8_t)(addr>>24));    
    }
    spi_rw_byte_nocheck((uint8_t)(addr>>16));  
    spi_rw_byte_nocheck((uint8_t)(addr>>8));    
    spi_rw_byte_nocheck((uint8_t)addr); 
	WQ25_CS_OFF(); 
    if(wq_wait_busy(300) != RT_OK)
    {
        return RT_FAIL;
    }
    
    return RT_OK;
}

//按block擦除, 长度为64KB
GlobalType_t wq_erase_block(uint32_t addr)   
{  
    if(addr > memory_end)
    {
        return RT_FAIL;
    }
    
    wq_write_enable(); 
    if(wq_wait_busy(100) != RT_OK)
    {
        return RT_FAIL;
    }
   
    addr = addr - addr%WQ_BLOCK_SIZE;
    WQ25_CS_ON();
    spi_rw_byte_nocheck(W25X_BlockErase);       
    if(wq_id == ID_W25Q256)             
    {
        spi_rw_byte_nocheck((uint8_t)(addr>>24));    
    }
    spi_rw_byte_nocheck((uint8_t)(addr>>16));  
    spi_rw_byte_nocheck((uint8_t)(addr>>8));    
    spi_rw_byte_nocheck((uint8_t)addr); 
	WQ25_CS_OFF(); 
    if(wq_wait_busy(300) != RT_OK)
    {
        return RT_FAIL;
    }
    
    return RT_OK;
}

static uint8_t wq_cache_buffer[WQ_SECTOR_SIZE];
GlobalType_t wq_write(uint32_t start_addr, uint8_t* pbuffer, uint16_t write_num)
{
	uint32_t secpos;
	uint32_t secoff;
	uint32_t left_write;	   
 	uint32_t index;    
	uint8_t  *buffer_ptr;	  
    
   	buffer_ptr = wq_cache_buffer;	     
 	secpos=start_addr/WQ_SECTOR_SIZE;
	secoff=start_addr%WQ_SECTOR_SIZE;
	
    left_write=WQ_SECTOR_SIZE-secoff; 
    left_write = write_num<left_write?write_num:left_write;
    
	do
	{	
		if(wq_read(secpos*WQ_SECTOR_SIZE, buffer_ptr, WQ_SECTOR_SIZE) != RT_OK)
        {
            return RT_FAIL;
        }
        
		for(index=0; index<left_write;index++)
		{
			if(buffer_ptr[secoff+index] != 0xFF)
            {
                break;
            }
		}
        
		if(index < left_write)
		{
            //erase and write
			wq_erase_sector(secpos*WQ_SECTOR_SIZE);
			for(index=0; index<left_write; index++)
			{
				buffer_ptr[index+secoff] = pbuffer[index];	  
			}
            if(wq_write_sector(secpos*WQ_SECTOR_SIZE, buffer_ptr, WQ_SECTOR_SIZE) != RT_OK)
            {
                return RT_FAIL;
            }
		}
        else 
        {
            //already erase, just write
            if(wq_write_sector(start_addr, buffer_ptr, left_write) != RT_OK)
            {
                return RT_FAIL;
            }
        }
        
		if(write_num <= left_write)
        {
            break;
        }
		else
		{
            //update for next write.
			secpos++;
			secoff = 0;
		   	pbuffer += left_write;
			start_addr += left_write;	   
		   	write_num -= left_write;
			if(write_num > WQ_SECTOR_SIZE)
            {
                left_write = WQ_SECTOR_SIZE;
            }
			else 
            {
                left_write = write_num;
            }
		}	 
	}while(1);

    return RT_OK;
}

GlobalType_t wq_read(uint32_t start_addr, uint8_t *pbuffer, uint32_t num)   
{ 
 	uint32_t index;  
    
    //1.detect not over memory
    if((start_addr+num) >= memory_end)
    {
        return RT_FAIL;
    }
    
    //2.read memory
	WQ25_CS_ON();                   
    spi_rw_byte_nocheck(W25X_ReadData);
    if(wq_id == ID_W25Q256)             
    {
        spi_rw_byte_nocheck((uint8_t)(start_addr>>24));    
    }
    spi_rw_byte_nocheck((uint8_t)(start_addr>>16));  
    spi_rw_byte_nocheck((uint8_t)(start_addr>>8));    
    spi_rw_byte_nocheck((uint8_t)start_addr);
    for(index=0; index<num; index++)
	{ 
        pbuffer[index]=spi_rw_byte_nocheck(0XFF); 
    } 
	WQ25_CS_OFF();

    return RT_OK;
}


#define BUFFER_SIZE 512
#define WQ_TEST_START_ADDR  0x10000
static uint8_t buffer[2][BUFFER_SIZE];
void wq_rw_test(void)
{
#if WQ_TEST_RW_ENABLE == 1

    uint16_t index;
    for(index=0; index<BUFFER_SIZE; index++)
    {
        buffer[0][index] = index&0xff;
        buffer[1][index] = 0;
    }
    
    if(wq_write(WQ_TEST_START_ADDR, buffer[0], BUFFER_SIZE) == RT_OK)
    {
        wq_read(WQ_TEST_START_ADDR, buffer[1], BUFFER_SIZE);
        if(memcmp(buffer[0], buffer[1], BUFFER_SIZE) == 0)
        {
            PRINT_LOG(LOG_INFO, HAL_GetTick(), "wq read/write test success!");
        }
        else
        {
            PRINT_LOG(LOG_INFO, HAL_GetTick(), "wq read/write test failed!");            
        }
    }
    else
    {
        PRINT_LOG(LOG_INFO, HAL_GetTick(), "wq read/write test failed!");   
    }
#endif
}

///////////////////////////////////////////// internal function //////////////////////
//write to sector, must ensure already erase before run.
static GlobalType_t wq_write_sector(uint32_t start_addr, uint8_t* pbuffer, uint16_t num)
{
    uint16_t header_n;
    uint16_t page_n, index;

    //head offset
    header_n = WQ_PAGE_SIZE - start_addr%WQ_PAGE_SIZE;
    if(header_n != WQ_PAGE_SIZE)
    {
        if(wq_write_page(start_addr, pbuffer, header_n) != RT_OK)
        {
            return RT_FAIL;
        }
        start_addr += header_n;
        pbuffer += header_n;
        num -= header_n;
    }
    
    page_n = num/WQ_PAGE_SIZE;
    for(index=0; index<page_n; index++)
    {
        if(wq_write_page(start_addr, pbuffer, WQ_PAGE_SIZE) != RT_OK)
        {
            return RT_FAIL;
        }
        start_addr += WQ_PAGE_SIZE;
        pbuffer += WQ_PAGE_SIZE;
        num -= WQ_PAGE_SIZE;        
    }
    
    //write the tail file.
    if(num != 0)
    {
        if(wq_write_page(start_addr, pbuffer, num) != RT_OK)
        {
            return RT_FAIL;
        }
    }  
    return RT_OK;    
}

//WQ-FLASH是按页写入的，不应该超过允许的最大写入
static GlobalType_t wq_write_page(uint32_t start_addr, uint8_t* pbuffer, uint16_t num)
{
 	uint16_t index;
    
    if((start_addr%256 + num) > WQ_PAGE_SIZE
    || (start_addr+num) >= memory_end)
    {
        return RT_FAIL;        
    }
    
    //1.enable page write feture
    wq_write_enable();
    if(wq_wait_busy(100) != RT_OK)
    {
        return RT_FAIL;
    }
    
    //2. write the data information
	WQ25_CS_ON();
    spi_rw_byte_nocheck(W25X_PageProgram);
    if(wq_id == ID_W25Q256)             
    {
        spi_rw_byte_nocheck((uint8_t)(start_addr>>24));    
    }
    spi_rw_byte_nocheck((uint8_t)(start_addr>>16));  
    spi_rw_byte_nocheck((uint8_t)(start_addr>>8));    
    spi_rw_byte_nocheck((uint8_t)start_addr);       
    for(index=0; index<num; index++)
	{ 
        spi_rw_byte_nocheck(pbuffer[index]); 
    } 
	WQ25_CS_OFF();
    
    //3.wait for write finished
    if(wq_wait_busy(1000) != RT_OK)
    {
        return RT_FAIL;
    }
	return RT_OK;
}

static uint16_t wq_read_chipid(void)
{
    uint16_t id = 0;
    
    WQ25_CS_ON();
    spi_rw_byte_nocheck(0x90);
    spi_rw_byte_nocheck(0x00);
    spi_rw_byte_nocheck(0x00);
    spi_rw_byte_nocheck(0x00);
    id = spi_rw_byte_nocheck(0xff)<<8;
    id |= spi_rw_byte_nocheck(0xff);
    WQ25_CS_OFF(); 
    
    return id;  
}

static void wq_write_enable(void)   
{
	WQ25_CS_ON();
    spi_rw_byte_nocheck(W25X_WriteEnable);
	WQ25_CS_OFF();    	      
} 

static GlobalType_t wq_wait_busy(uint16_t timeout)
{   
    uint8_t byte;
    uint32_t index;
    
    WQ25_CS_ON(); 
    do
    {
        spi_rw_byte_nocheck(W25X_ReadStatusReg1);
        byte = spi_rw_byte_nocheck(0xff);
        if((byte&0x01) == 0 || index < 100)
        {
            break;
        }   
        index++;
        HAL_Delay(1);
    }while(1);
   	WQ25_CS_OFF();
    
    if((byte&0x01) == 0)
        return RT_OK;
    
	return RT_FAIL;   
}
