//////////////////////////////////////////////////////////////////////////////
//  (c) copyright 2023-by Persional Inc.  
//  All Rights Reserved
//
//  Name:
//      drv_sdio.c
//
//  Purpose:
//      driver for sdio module.
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
#include "drv_sdio.h"
#include "drv_target.h"

static SD_HandleTypeDef hsdcard1;
static DMA_HandleTypeDef hdma_sdio_rx;
static DMA_HandleTypeDef hdma_sdio_tx;

GlobalType_t sdcard_driver_init(void)
{
    GPIO_InitTypeDef GPIO_InitStruct = {0};

    //clock rst and enable
    __HAL_RCC_SDIO_FORCE_RESET();
    HAL_Delay(1);
    __HAL_RCC_SDIO_RELEASE_RESET();
    
    __HAL_RCC_SDIO_CLK_ENABLE();
    __HAL_RCC_GPIOC_CLK_ENABLE();
    __HAL_RCC_GPIOD_CLK_ENABLE();
    
    /**SDIO GPIO Configuration
    PC8     ------> SDIO_D0
    PC9     ------> SDIO_D1
    PC10     ------> SDIO_D2
    PC11     ------> SDIO_D3
    PC12     ------> SDIO_CK
    PD2     ------> SDIO_CMD
    */
    GPIO_InitStruct.Pin = GPIO_PIN_8|GPIO_PIN_9|GPIO_PIN_10|GPIO_PIN_11
                          |GPIO_PIN_12;
    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Pull = GPIO_PULLUP;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
    GPIO_InitStruct.Alternate = GPIO_AF12_SDIO;
    HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);

    GPIO_InitStruct.Pin = GPIO_PIN_2;
    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Pull = GPIO_PULLUP;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
    GPIO_InitStruct.Alternate = GPIO_AF12_SDIO;
    HAL_GPIO_Init(GPIOD, &GPIO_InitStruct);
    
    //module init
    hsdcard1.Instance = SDIO;
    hsdcard1.Init.ClockEdge = SDIO_CLOCK_EDGE_RISING;
    hsdcard1.Init.ClockBypass = SDIO_CLOCK_BYPASS_DISABLE;
    hsdcard1.Init.ClockPowerSave = SDIO_CLOCK_POWER_SAVE_DISABLE;
    hsdcard1.Init.BusWide = SDIO_BUS_WIDE_1B;
    hsdcard1.Init.HardwareFlowControl = SDIO_HARDWARE_FLOW_CONTROL_DISABLE;
    hsdcard1.Init.ClockDiv = SDMMC_CLOCK_DIV;

    if (HAL_SD_Init(&hsdcard1) != HAL_OK)
        return RT_FAIL;

    if (HAL_SD_ConfigWideBusOperation(&hsdcard1, SDIO_BUS_WIDE_4B) != HAL_OK)
        return RT_FAIL;

#ifdef _SDIO_DMA_SUPPORT
        /* SDIO DMA Init */
    /* SDIO_RX Init */
    hdma_sdio_rx.Instance = DMA2_Stream3;
    hdma_sdio_rx.Init.Channel = DMA_CHANNEL_4;
    hdma_sdio_rx.Init.Direction = DMA_PERIPH_TO_MEMORY;
    hdma_sdio_rx.Init.PeriphInc = DMA_PINC_DISABLE;
    hdma_sdio_rx.Init.MemInc = DMA_MINC_ENABLE;
    hdma_sdio_rx.Init.PeriphDataAlignment = DMA_PDATAALIGN_WORD;
    hdma_sdio_rx.Init.MemDataAlignment = DMA_MDATAALIGN_WORD;
    hdma_sdio_rx.Init.Mode = DMA_PFCTRL;
    hdma_sdio_rx.Init.Priority = DMA_PRIORITY_HIGH;
    hdma_sdio_rx.Init.FIFOMode = DMA_FIFOMODE_ENABLE;
    hdma_sdio_rx.Init.FIFOThreshold = DMA_FIFO_THRESHOLD_FULL;
    hdma_sdio_rx.Init.MemBurst = DMA_MBURST_INC4;
    hdma_sdio_rx.Init.PeriphBurst = DMA_PBURST_INC4;
    if (HAL_DMA_Init(&hdma_sdio_rx) != HAL_OK)
    {
        return RT_FAIL;
    }
    __HAL_LINKDMA(&hsdcard1, hdmarx, hdma_sdio_rx);

    /* SDIO_TX Init */
    hdma_sdio_tx.Instance = DMA2_Stream6;
    hdma_sdio_tx.Init.Channel = DMA_CHANNEL_4;
    hdma_sdio_tx.Init.Direction = DMA_MEMORY_TO_PERIPH;
    hdma_sdio_tx.Init.PeriphInc = DMA_PINC_DISABLE;
    hdma_sdio_tx.Init.MemInc = DMA_MINC_ENABLE;
    hdma_sdio_tx.Init.PeriphDataAlignment = DMA_PDATAALIGN_WORD;
    hdma_sdio_tx.Init.MemDataAlignment = DMA_MDATAALIGN_WORD;
    hdma_sdio_tx.Init.Mode = DMA_PFCTRL;
    hdma_sdio_tx.Init.Priority = DMA_PRIORITY_HIGH;
    hdma_sdio_tx.Init.FIFOMode = DMA_FIFOMODE_ENABLE;
    hdma_sdio_tx.Init.FIFOThreshold = DMA_FIFO_THRESHOLD_FULL;
    hdma_sdio_tx.Init.MemBurst = DMA_MBURST_INC4;
    hdma_sdio_tx.Init.PeriphBurst = DMA_PBURST_INC4;
    if (HAL_DMA_Init(&hdma_sdio_tx) != HAL_OK)
    {
        return RT_FAIL;
    }
    __HAL_LINKDMA(&hsdcard1, hdmatx, hdma_sdio_tx);
#endif

    return RT_OK;
}

#if _SDIO_DMA_SUPPORT == 1
static volatile  uint32_t  WriteCallback_Status = 0, ReadCallback_Status = 0;

void DMA2_Stream3_IRQHandler(void)
{
    HAL_DMA_IRQHandler(&hdma_sdio_rx);
}

void DMA2_Stream6_IRQHandler(void)
{
    HAL_DMA_IRQHandler(&hdma_sdio_tx);
}

void BSP_SD_WriteCpltCallback(void)
{
    WriteCallback_Status = 1;
}

void BSP_SD_ReadCpltCallback(void)
{
    ReadCallback_Status = 1;
}

HAL_StatusTypeDef sdcard_read_disk(uint8_t *buf, uint32_t startBlocks, uint32_t NumberOfBlocks)
{
    HAL_StatusTypeDef status = HAL_OK;
    uint16_t tick = 0;
    
    ReadCallback_Status = 0;
    status = HAL_SD_ReadBlocks_DMA(&hsdcard1, (uint8_t*)buf, startBlocks, NumberOfBlocks);
    if (status == HAL_ERROR)
    {
       return HAL_ERROR; 
    }
    
    //wait rx finished
    while(ReadCallback_Status == 0
    && (tick < SDMMC_READ_WRITE_TIMEOUT))
    {
        hal_delay_ms(1);
        tick++;
    }
    if(tick >= SDMMC_READ_WRITE_TIMEOUT)
    {
        return HAL_TIMEOUT;
    }
    
    //wait card ok.
    tick = 0;
    while((HAL_SD_GetCardState(&hsdcard1) != HAL_SD_CARD_TRANSFER)
    && (tick < SDMMC_READ_WRITE_TIMEOUT))
    {
        hal_delay_ms(1);
        tick++;
    }
    if(tick >= SDMMC_READ_WRITE_TIMEOUT)
    {
        return HAL_TIMEOUT;
    }
    return status;
}

HAL_StatusTypeDef sdcard_write_disk(const uint8_t *buf, uint32_t startBlocks, uint32_t NumberOfBlocks)
{
    HAL_StatusTypeDef status = HAL_OK;
    uint16_t tick = 0;

    WriteCallback_Status = 0;
    status = HAL_SD_WriteBlocks_DMA(&hsdcard1, (uint8_t*)buf, startBlocks, NumberOfBlocks);
    if (status == HAL_ERROR)
    {
       return HAL_ERROR; 
    }
    
    //wait tx finished
    while(WriteCallback_Status == 0
    && (tick < SDMMC_READ_WRITE_TIMEOUT))
    {
        hal_delay_ms(1);
        tick++;
    }
    if(tick >= SDMMC_READ_WRITE_TIMEOUT)
    {
        return HAL_TIMEOUT;
    }
    
    //wait card ok.
    tick = 0;
    while((HAL_SD_GetCardState(&hsdcard1) != HAL_SD_CARD_TRANSFER)
    && (tick < SDMMC_READ_WRITE_TIMEOUT))
    {
        hal_delay_ms(1);
        tick++;
    }
    if(tick >= SDMMC_READ_WRITE_TIMEOUT)
    {
        return HAL_TIMEOUT;
    }

    return status;
}
#else
HAL_StatusTypeDef sdcard_read_disk(uint8_t *buf, uint32_t startBlocks, uint32_t NumberOfBlocks)
{
    HAL_StatusTypeDef status = HAL_OK;
    uint16_t tick = 0;
    
    status = HAL_SD_ReadBlocks(&hsdcard1, (uint8_t*)buf, startBlocks, NumberOfBlocks, SDMMC_READ_WRITE_TIMEOUT);
    
    //wait card ok.
    while((HAL_SD_GetCardState(&hsdcard1) != HAL_SD_CARD_TRANSFER)
    && (tick < SDMMC_READ_WRITE_TIMEOUT))
    {
        hal_delay_ms(1);
        tick++;
    }
    if(tick >= SDMMC_READ_WRITE_TIMEOUT)
    {
        return HAL_TIMEOUT;
    }
    return status;
}

HAL_StatusTypeDef sdcard_write_disk(const uint8_t *buf, uint32_t startBlocks, uint32_t NumberOfBlocks)
{
    HAL_StatusTypeDef status = HAL_OK;
    uint16_t tick = 0;

    status = HAL_SD_WriteBlocks(&hsdcard1, (uint8_t*)buf, startBlocks, NumberOfBlocks, SDMMC_READ_WRITE_TIMEOUT);
    
    //wait card ok.
    while((HAL_SD_GetCardState(&hsdcard1) != HAL_SD_CARD_TRANSFER)
    && (tick < SDMMC_READ_WRITE_TIMEOUT))
    {
        hal_delay_ms(1);
        tick++;
    }
    if(tick >= SDMMC_READ_WRITE_TIMEOUT)
    {
        return HAL_TIMEOUT;
    }

    return status;
}
#endif
