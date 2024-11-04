//////////////////////////////////////////////////////////////////////////////
//  (c) copyright 2023-by Persional Inc.  
//  All Rights Reserved
//
//  Name:
//      drv_dac.c
//
//  Purpose:
//      dac driver support normal and dma mode.
//      Hardware:PA4
//      1. normal mode output the set volatage.
//      2. dma mode output sine shaped.
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
#include "drv_dac.h"

#if DRIVER_DAC_ENABLE == 1
//interanl define
#define DAC_PORT                    GPIOA
#define DAC_PIN                     GPIO_PIN_4
#define DAC_CHANNEL                 DAC_CHANNEL_1

#define DAC_CYCLE_SIZE              40

//internal variable
static DAC_HandleTypeDef hdac;
static DMA_HandleTypeDef hdma_dac1;
static TIM_HandleTypeDef htim;
#if DAC_RUN_MODE == RUN_MODE_DMA
static const uint16_t vol_cycle[DAC_CYCLE_SIZE] = {
    2048, 2368, 2680, 2977, 3251,
    3496, 3704, 3872, 3995, 4070,
    4095, 4070, 3995, 3872, 3704,
    3496, 3251, 2977, 2680, 2368,
    2048, 1727, 1415, 1118, 844,
    599,  391,  223,  100,  25,
    0,    25,   100,  223,  391,
    599,  844, 1118, 1415, 1727,
};
static uint16_t vol_convert_cycle[DAC_CYCLE_SIZE];
#endif

#if DAC_RUN_MODE == RUN_MODE_NORMAL
GlobalType_t dac_driver_init(void)
{
    DAC_ChannelConfTypeDef sConfig = {0};
    GPIO_InitTypeDef GPIO_InitStruct = {0};
    

    /*step1: enable dac clock*/
    __HAL_RCC_DAC_CLK_ENABLE(); 
    __HAL_RCC_GPIOA_CLK_ENABLE(); 
    
    /*step2: config dac gpio*/
    GPIO_InitStruct.Pin = DAC_PIN;
    GPIO_InitStruct.Mode = GPIO_MODE_ANALOG;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    HAL_GPIO_Init(DAC_PORT, &GPIO_InitStruct);
    
    /*step3: config dac*/
    hdac.Instance = DAC;
    if (HAL_DAC_Init(&hdac) != HAL_OK)
        return RT_FAIL;

    sConfig.DAC_Trigger = DAC_TRIGGER_NONE;
    sConfig.DAC_OutputBuffer = DAC_OUTPUTBUFFER_ENABLE;
    if (HAL_DAC_ConfigChannel(&hdac, &sConfig, DAC_CHANNEL) != HAL_OK)
        return RT_FAIL;
 
    dac_set(1000);
    
    return RT_OK;
}   

void dac_set(uint16_t mv)
{
    float adc_value;
    
    if (mv > DAC_REFERENCE_VOL)
        mv = DAC_REFERENCE_VOL;

    adc_value = (float)mv/DAC_REFERENCE_VOL * DAC_MAX_VALUE;

    HAL_DAC_Stop(&hdac, DAC_CHANNEL_1);
    HAL_DAC_SetValue(&hdac, DAC_CHANNEL_1, DAC_ALIGN_12B_R, (uint32_t)adc_value);
    HAL_DAC_Start(&hdac, DAC_CHANNEL_1);    
}
#elif DAC_RUN_MODE == RUN_MODE_VOICE
extern uint16_t tonePCM_8KHz16bit[2321];
GlobalType_t dac_driver_init(void)
{
    DAC_ChannelConfTypeDef sConfig = {0};
    GPIO_InitTypeDef GPIO_InitStruct = {0};
    TIM_ClockConfigTypeDef sClockSourceConfig = {0};
    TIM_MasterConfigTypeDef sMasterConfig = {0}; 
    
    /*step1: enable dac clock*/
    __HAL_RCC_DMA1_CLK_ENABLE();
    __HAL_RCC_TIM4_CLK_ENABLE();
    __HAL_RCC_DAC_CLK_ENABLE();
    __HAL_RCC_GPIOA_CLK_ENABLE();

    /*step2: config dac gpio*/
    GPIO_InitStruct.Pin = DAC_PIN;
    GPIO_InitStruct.Mode = GPIO_MODE_ANALOG;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    HAL_GPIO_Init(DAC_PORT, &GPIO_InitStruct);

    /*step3: config dac, TIM4 update trigger DAC Update*/
    hdac.Instance = DAC;
    if (HAL_DAC_Init(&hdac) != HAL_OK)
        return RT_FAIL;

    sConfig.DAC_Trigger = DAC_TRIGGER_T4_TRGO;
    sConfig.DAC_OutputBuffer = DAC_OUTPUTBUFFER_DISABLE;
    if (HAL_DAC_ConfigChannel(&hdac, &sConfig, DAC_CHANNEL_1) != HAL_OK)
        return RT_FAIL;

    /*step4: config dma, when trigger, DMA translate*/
    hdma_dac1.Instance = DMA1_Stream5;
    hdma_dac1.Init.Channel = DMA_CHANNEL_7;
    hdma_dac1.Init.Direction = DMA_MEMORY_TO_PERIPH;
    hdma_dac1.Init.PeriphInc = DMA_PINC_DISABLE;
    hdma_dac1.Init.MemInc = DMA_MINC_ENABLE;
    hdma_dac1.Init.PeriphDataAlignment = DMA_PDATAALIGN_HALFWORD;
    hdma_dac1.Init.MemDataAlignment = DMA_MDATAALIGN_HALFWORD;
    hdma_dac1.Init.Mode = DMA_CIRCULAR;
    hdma_dac1.Init.Priority = DMA_PRIORITY_LOW;
    hdma_dac1.Init.FIFOMode = DMA_FIFOMODE_ENABLE;
    hdma_dac1.Init.FIFOThreshold = DMA_FIFO_THRESHOLD_HALFFULL;
    hdma_dac1.Init.MemBurst = DMA_MBURST_SINGLE;
    hdma_dac1.Init.PeriphBurst = DMA_MBURST_SINGLE;
    if (HAL_DMA_Init(&hdma_dac1) != HAL_OK)
      return RT_FAIL;

    __HAL_LINKDMA(&hdac, DMA_Handle1, hdma_dac1);

    //initialize timer
    /*step5: config TIM4, Update to trigger DAC update*/        //clock APB1 90M
    htim.Instance = TIM4;
    htim.Init.Prescaler = 44; //2M
    htim.Init.CounterMode = TIM_COUNTERMODE_UP;
    htim.Init.Period = 124;                                      //clock: 2M/125 = 16Khz
    htim.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
    htim.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_ENABLE;
    if (HAL_TIM_Base_Init(&htim) != HAL_OK)
        return RT_FAIL;
    
    sClockSourceConfig.ClockSource = TIM_CLOCKSOURCE_INTERNAL;
    if (HAL_TIM_ConfigClockSource(&htim, &sClockSourceConfig) != HAL_OK)
        return RT_FAIL;
    
    sMasterConfig.MasterOutputTrigger = TIM_TRGO_UPDATE;
    sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
    if (HAL_TIMEx_MasterConfigSynchronization(&htim, &sMasterConfig) != HAL_OK)
        return RT_FAIL;

    //step6:update TIM4, DAC CHANNEL, DMA
    HAL_TIM_Base_Start(&htim);
    
    //16bit数据输, 保留高位，音频
    HAL_DAC_Start_DMA(&hdac, DAC_CHANNEL_1, (uint32_t *)tonePCM_8KHz16bit, sizeof(tonePCM_8KHz16bit)/sizeof(uint16_t), DAC_ALIGN_12B_L);

    return RT_OK;
}

#else
GlobalType_t dac_driver_init(void)
{
    DAC_ChannelConfTypeDef sConfig = {0};
    GPIO_InitTypeDef GPIO_InitStruct = {0};
    TIM_ClockConfigTypeDef sClockSourceConfig = {0};
    TIM_MasterConfigTypeDef sMasterConfig = {0}; 
    
    /*step1: enable dac clock*/
    __HAL_RCC_DMA1_CLK_ENABLE();
    __HAL_RCC_TIM4_CLK_ENABLE();
    __HAL_RCC_DAC_CLK_ENABLE();
    __HAL_RCC_GPIOA_CLK_ENABLE();

    /*step2: config dac gpio*/
    GPIO_InitStruct.Pin = DAC_PIN;
    GPIO_InitStruct.Mode = GPIO_MODE_ANALOG;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    HAL_GPIO_Init(DAC_PORT, &GPIO_InitStruct);

    /*step3: config dac, TIM4 update trigger DAC Update*/
    hdac.Instance = DAC;
    if (HAL_DAC_Init(&hdac) != HAL_OK)
        return RT_FAIL;

    sConfig.DAC_Trigger = DAC_TRIGGER_T4_TRGO;
    sConfig.DAC_OutputBuffer = DAC_OUTPUTBUFFER_DISABLE;
    if (HAL_DAC_ConfigChannel(&hdac, &sConfig, DAC_CHANNEL_1) != HAL_OK)
        return RT_FAIL;

    /*step4: config dma, when trigger, DMA translate*/
    hdma_dac1.Instance = DMA1_Stream5;
    hdma_dac1.Init.Channel = DMA_CHANNEL_7;
    hdma_dac1.Init.Direction = DMA_MEMORY_TO_PERIPH;
    hdma_dac1.Init.PeriphInc = DMA_PINC_DISABLE;
    hdma_dac1.Init.MemInc = DMA_MINC_ENABLE;
    hdma_dac1.Init.PeriphDataAlignment = DMA_PDATAALIGN_HALFWORD;
    hdma_dac1.Init.MemDataAlignment = DMA_MDATAALIGN_HALFWORD;
    hdma_dac1.Init.Mode = DMA_CIRCULAR;
    hdma_dac1.Init.Priority = DMA_PRIORITY_LOW;
    hdma_dac1.Init.FIFOMode = DMA_FIFOMODE_ENABLE;
    hdma_dac1.Init.FIFOThreshold = DMA_FIFO_THRESHOLD_HALFFULL;
    hdma_dac1.Init.MemBurst = DMA_MBURST_SINGLE;
    hdma_dac1.Init.PeriphBurst = DMA_MBURST_SINGLE;
    if (HAL_DMA_Init(&hdma_dac1) != HAL_OK)
      return RT_FAIL;

    __HAL_LINKDMA(&hdac, DMA_Handle1, hdma_dac1);

    //initialize timer
    /*step5: config TIM4, Update to trigger DAC update*/        //clock APB1 90M
    htim.Instance = TIM4;
    htim.Init.Prescaler = 89; //1M
    htim.Init.CounterMode = TIM_COUNTERMODE_UP;
    htim.Init.Period = 15;
    htim.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
    htim.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_ENABLE;
    if (HAL_TIM_Base_Init(&htim) != HAL_OK)
        return RT_FAIL;
    
    sClockSourceConfig.ClockSource = TIM_CLOCKSOURCE_INTERNAL;
    if (HAL_TIM_ConfigClockSource(&htim, &sClockSourceConfig) != HAL_OK)
        return RT_FAIL;
    
    sMasterConfig.MasterOutputTrigger = TIM_TRGO_UPDATE;
    sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
    if (HAL_TIMEx_MasterConfigSynchronization(&htim, &sMasterConfig) != HAL_OK)
        return RT_FAIL;

    dac_set(DAC_REFERENCE_VOL);

    //step6:update TIM4, DAC CHANNEL, DMA
    HAL_TIM_Base_Start(&htim);
    SET_BIT(hdac.Instance->CR, DAC_CR_DMAEN1);
    HAL_DMA_Start(&hdma_dac1, (uint32_t)vol_convert_cycle, (uint32_t)&hdac.Instance->DHR12R1, DAC_CYCLE_SIZE);
    __HAL_DAC_ENABLE(&hdac, DAC_CHANNEL_1);
    
    return RT_OK;
}

void dac_set(uint16_t mv)
{
    uint8_t i;
    float percent;

    if (mv > DAC_REFERENCE_VOL)
        mv = DAC_REFERENCE_VOL;    

    percent = (float)mv/DAC_REFERENCE_VOL;
    for (i=0; i<DAC_CYCLE_SIZE; i++)
    {
        vol_convert_cycle[i] = percent*vol_cycle[i];
    }
}

#endif //end of DAC_RUN_MODE
#endif //end of DRIVER_DAC_ENABLE
