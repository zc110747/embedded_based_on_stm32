//////////////////////////////////////////////////////////////////////////////
//  (c) copyright 2023-by Persional Inc.  
//  All Rights Reserved
//
//  Name:
//      drv_dma.c
//
//  Purpose:
//      driver for dma module.
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
#include "drv_dma.h"
#include "drv_target.h"
#include "logger.h"
#include <string.h>

#define DMA_DST_SIZE        256
#define DMA_SRC_ADDRESS     0x8000000

static uint8_t dma_dst[DMA_DST_SIZE];
static DMA_HandleTypeDef hdma_memtomem;

#define DMA_MEM_STREAM  DMA2_Stream1
#define DMA_MEM_FLAG    DMA_FLAG_TCIF1_5

GlobalType_t dma_driver_init(void)
{
    /* DMA controller clock enable */
    __HAL_RCC_DMA2_CLK_ENABLE();

    /* Configure DMA request hdma_memtomem on DMA2_Stream0 */
    hdma_memtomem.Instance = DMA_MEM_STREAM;
    hdma_memtomem.Init.Channel = DMA_CHANNEL_0;
    hdma_memtomem.Init.Direction = DMA_MEMORY_TO_MEMORY;
    hdma_memtomem.Init.PeriphInc = DMA_PINC_ENABLE;
    hdma_memtomem.Init.MemInc = DMA_MINC_ENABLE;
    hdma_memtomem.Init.PeriphDataAlignment = DMA_PDATAALIGN_BYTE;
    hdma_memtomem.Init.MemDataAlignment = DMA_MDATAALIGN_BYTE;
    hdma_memtomem.Init.Mode = DMA_NORMAL;
    hdma_memtomem.Init.Priority = DMA_PRIORITY_MEDIUM;
    hdma_memtomem.Init.FIFOMode = DMA_FIFOMODE_ENABLE;
    hdma_memtomem.Init.FIFOThreshold = DMA_FIFO_THRESHOLD_FULL;
    hdma_memtomem.Init.MemBurst = DMA_MBURST_SINGLE;
    hdma_memtomem.Init.PeriphBurst = DMA_PBURST_SINGLE;
    if (HAL_DMA_Init(&hdma_memtomem) != HAL_OK)
    {
        return RT_FAIL;
    }
    
    return RT_OK;
}

GlobalType_t dma_translate(uint32_t SrcAddress, uint32_t DstAddress, uint32_t DataLength)
{
    if(HAL_DMA_Start(&hdma_memtomem, SrcAddress, DstAddress, DataLength) != HAL_OK)
    {
        return RT_FAIL;
    }
    
    return RT_OK;                                                                                                
}

GlobalType_t dma_check_finish(uint32_t timeout_ms)
{
    uint32_t index = 0;
    
    if(timeout_ms == 0)
    {
        if(__HAL_DMA_GET_FLAG(&hdma_memtomem, DMA_MEM_FLAG) == SET)
        {
            return RT_OK;
        }
    }
    else
    {
       while(__HAL_DMA_GET_FLAG(&hdma_memtomem, DMA_MEM_FLAG) == RESET)
       {
           index++;
           if(index >= timeout_ms)
           {
               break;
           }
           hal_delay_ms(1);
       }
       
       if(index<timeout_ms)
           return RT_OK;
    }
    
    return RT_FAIL;
}

GlobalType_t dma_memory_run(void)
{
    GlobalType_t xReturn;
    
    memset(dma_dst, 0, DMA_DST_SIZE);
    dma_translate(DMA_SRC_ADDRESS, (uint32_t)dma_dst, DMA_DST_SIZE);
  
    if(dma_check_finish(100) == RT_OK)
    {
        __HAL_DMA_CLEAR_FLAG(&hdma_memtomem, DMA_MEM_FLAG);
        
        if(memcmp((char *)DMA_SRC_ADDRESS, dma_dst, DMA_DST_SIZE) == 0)
        {
            PRINT_LOG(LOG_INFO, HAL_GetTick(), "dma memory to memory test success!");
            return RT_OK;
        }
        else
        {
            PRINT_LOG(LOG_INFO, HAL_GetTick(), "dma memory to memory test data not equal!");
        }
    }
    else
    {
        PRINT_LOG(LOG_INFO, HAL_GetTick(), "dma memory to memory test not finish!");
    }
    
    return xReturn;  
}
