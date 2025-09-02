//////////////////////////////////////////////////////////////////////////////
//  (c) copyright 2023-by Persional Inc.  
//  All Rights Reserved
//
//  Name:
//      drv_adc.c
//
//  Purpose:
//      driver for adc.
//      Hardware: PA6
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
#include "drv_adc.h"

//internal define
#define ADC_AVG_TIMES               5
#define ADC_BUFFER_SIZE             ADC_AVG_TIMES*2
    
#define ADC_PIN                     GPIO_PIN_6
#define ADC_PORT                    GPIOA

//internal variable
static ADC_HandleTypeDef hadc1;
static DMA_HandleTypeDef hdma_adc1;
static uint32_t vref_val = 3300;

//internal function
static GlobalType_t adc_calibration(void);

#if ADC_RUN_MODE == RUN_MODE_NORMAL
GlobalType_t driver_adc_init(void)
{
    GPIO_InitTypeDef GPIO_InitStruct = {0};
    
    // 使能引脚和ADC模块时钟
    __HAL_RCC_ADC1_CLK_ENABLE();
    __HAL_RCC_GPIOA_CLK_ENABLE();

    // 配置引脚为模拟输入模式，无上拉
    GPIO_InitStruct.Pin = ADC_PIN;
    GPIO_InitStruct.Mode = GPIO_MODE_ANALOG;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    HAL_GPIO_Init(ADC_PORT, &GPIO_InitStruct);
    
    // 初始化ADC模块
    hadc1.Instance = ADC1;
    hadc1.Init.ClockPrescaler = ADC_CLOCK_SYNC_PCLK_DIV6;   // ADC工作时钟，需要满足时钟要求
    hadc1.Init.Resolution = ADC_RESOLUTION_12B;             // ADC 12bit数据
    hadc1.Init.DataAlign = ADC_DATAALIGN_RIGHT;             // ADC采样数据右对齐
    hadc1.Init.ScanConvMode = DISABLE;                      // 是否工作在扫描模式(扫描模式下会每读取一次切换通道) 
    hadc1.Init.EOCSelection = ADC_EOC_SINGLE_CONV;          // 在每个常规转换序列结束时将EOC位置1。溢出检测仅在DMA=1时使能
    hadc1.Init.ContinuousConvMode = DISABLE;                // 单次转换，转换完结束
    hadc1.Init.NbrOfConversion = 1;                         // 设置转换的规则通道数目  
    hadc1.Init.DiscontinuousConvMode = DISABLE;             // 关闭不连续转换(根据指定数目每次转换特定通道)
    hadc1.Init.NbrOfDiscConversion = 0;                     // 设置不连续转换单次中转换的数目
    hadc1.Init.ExternalTrigConv = ADC_SOFTWARE_START;       // 触发方式，软件触发
    hadc1.Init.ExternalTrigConvEdge = ADC_EXTERNALTRIGCONVEDGE_NONE;   // 外部中断触发检测方式(仅外部中断触发有效)
    hadc1.Init.DMAContinuousRequests = DISABLE;             // 只要发生数据转换且DMA = 1，便会发出DAM请求

    if (HAL_ADC_Init(&hadc1) != HAL_OK)
        return RT_FAIL;
    
    adc_calibration();
    return RT_OK;  
}

static uint16_t driver_adc_read(uint32_t channel)
{
    ADC_ChannelConfTypeDef sConfig = {0};
    
    // 设置ADC通道
    sConfig.Channel = channel;                          // ADC硬件通道
    sConfig.Rank = 1;                                   // ADC规则通道(1 ~ 16)
    sConfig.SamplingTime = ADC_SAMPLETIME_56CYCLES;     // ADC采样时间
    sConfig.Offset = 0;
    HAL_ADC_ConfigChannel(&hadc1, &sConfig);
    
    // 启动ADC转换
    HAL_ADC_Start(&hadc1);                              
    
    // 等待ADC转换完成
    if (HAL_ADC_PollForConversion(&hadc1, 10) != HAL_OK) {
        return 0;
    }        
    
    return HAL_ADC_GetValue(&hadc1);    
}

uint16_t driver_adc_avg_read(uint32_t channel)
{
    uint32_t temp = 0;
    uint8_t index;
    
    for (index=0; index<ADC_AVG_TIMES; index++)
    {
        temp += driver_adc_read(channel);
    }
    
    return temp/ADC_AVG_TIMES;
}
#else
static uint16_t ADC_Buffer[ADC_BUFFER_SIZE];

GlobalType_t driver_adc_init(void)
{
    ADC_ChannelConfTypeDef sConfig = {0};
    GPIO_InitTypeDef GPIO_InitStruct = {0};

    // 模块时钟初始化
    __HAL_RCC_ADC1_CLK_ENABLE();
    __HAL_RCC_GPIOA_CLK_ENABLE();
    __HAL_RCC_DMA2_CLK_ENABLE();

    // ADC引脚初始化
    GPIO_InitStruct.Pin = ADC_PIN;
    GPIO_InitStruct.Mode = GPIO_MODE_ANALOG;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    HAL_GPIO_Init(ADC_PORT, &GPIO_InitStruct);

    // ADC模块初始化
    hadc1.Instance = ADC1;
    hadc1.Init.ClockPrescaler = ADC_CLOCK_SYNC_PCLK_DIV6;   // ADC工作时钟，需要满足时钟要求
    hadc1.Init.Resolution = ADC_RESOLUTION_12B;             // ADC 12bit数据
    hadc1.Init.DataAlign = ADC_DATAALIGN_RIGHT;             // ADC采样数据右对齐
    hadc1.Init.ScanConvMode = ENABLE;                       // 扫描模式，规则通道扫描
    hadc1.Init.EOCSelection = ADC_EOC_SEQ_CONV;             // ADC EOC触发条件, 序列完成触发中断
    hadc1.Init.ContinuousConvMode = ENABLE;                 // 持续模式，循环扫描通道
    hadc1.Init.NbrOfConversion = 2;                         // 循环扫描的通道数目，和涉及到读取通道一致
    hadc1.Init.DiscontinuousConvMode = DISABLE;             // 关闭不连续转换(根据指定数目每次转换特定通道)
    hadc1.Init.NbrOfDiscConversion = 0;                     // 设置不连续转换单次中转换的数目
    hadc1.Init.ExternalTrigConv = ADC_SOFTWARE_START;       // 触发方式，软件触发
    hadc1.Init.ExternalTrigConvEdge = ADC_EXTERNALTRIGCONVEDGE_NONE; // 外部中断触发检测方式(仅外部中断触发有效) 
    hadc1.Init.DMAContinuousRequests = ENABLE;              // 使能DMA请求

    if (HAL_ADC_Init(&hadc1) != HAL_OK)
    {
        return RT_FAIL;
    }

    sConfig.Channel = ADC_CHANNEL_6;
    sConfig.Rank = 1;
    sConfig.SamplingTime = ADC_SAMPLETIME_144CYCLES;
    if (HAL_ADC_ConfigChannel(&hadc1, &sConfig) != HAL_OK)
    {
        return RT_FAIL;
    }

    sConfig.Channel = ADC_CHANNEL_TEMPSENSOR;
    sConfig.Rank = 2;
    sConfig.SamplingTime = ADC_SAMPLETIME_144CYCLES;
    if (HAL_ADC_ConfigChannel(&hadc1, &sConfig) != HAL_OK)
    {
        return RT_FAIL;
    }
    
    //run adc calibration
    adc_calibration();
    
    //enable the adc dma
    SET_BIT(hadc1.Instance->CR2, ADC_CR2_DMA);
    
    hdma_adc1.Instance = DMA2_Stream0;                              // 配置DMA模块DMA2_stream0
    hdma_adc1.Init.Channel = DMA_CHANNEL_0;                         // 配置DMA通道
    hdma_adc1.Init.Direction = DMA_PERIPH_TO_MEMORY;                // 外设到内存
    hdma_adc1.Init.PeriphInc = DMA_PINC_DISABLE;                    // 外设地址不增加
    hdma_adc1.Init.MemInc = DMA_MINC_ENABLE;                        // 内存地址增加
    hdma_adc1.Init.PeriphDataAlignment = DMA_PDATAALIGN_HALFWORD;   // 外设数据长度半字，和ADC寄存器位宽一致
    hdma_adc1.Init.MemDataAlignment = DMA_MDATAALIGN_HALFWORD;      // 内存数据长度半字
    hdma_adc1.Init.Mode = DMA_CIRCULAR;                             // 循环模式
    hdma_adc1.Init.Priority = DMA_PRIORITY_HIGH;                    // DMA优先级高
    hdma_adc1.Init.FIFOMode = DMA_FIFOMODE_DISABLE;                 // 不需要队列
    if (HAL_DMA_Init(&hdma_adc1) != HAL_OK)
    {
        return RT_FAIL;
    }
    __HAL_LINKDMA(&hadc1, DMA_Handle, hdma_adc1);
    
    __HAL_DMA_ENABLE_IT(&hdma_adc1, DMA_IT_TC);
    HAL_NVIC_SetPriority(DMA2_Stream0_IRQn, 0, 0);
    HAL_NVIC_EnableIRQ(DMA2_Stream0_IRQn);
    
    HAL_DMA_Start(&hdma_adc1, (uint32_t)&hadc1.Instance->DR, (uint32_t)ADC_Buffer, ADC_BUFFER_SIZE);
    HAL_ADC_Start(&hadc1);
    
    return RT_OK;
}

void DMA2_Stream0_IRQHandler(void)
{
    if (__HAL_DMA_GET_FLAG(&hdma_adc1, DMA_FLAG_TCIF0_4) != RESET)
    {      
       __HAL_DMA_CLEAR_FLAG(&hdma_adc1, DMA_FLAG_TCIF0_4);
       __HAL_DMA_CLEAR_FLAG(&hdma_adc1, DMA_FLAG_TEIF0_4);
    }
}

uint16_t driver_adc_avg_read(uint32_t channel)
{
    uint32_t temp = 0;
    uint8_t index;
    uint16_t *pstart;
    
    switch (channel)
    {
        case ADC_CHANNEL_6:
            pstart = ADC_Buffer;
            break;
        case ADC_CHANNEL_TEMPSENSOR:
            pstart = ADC_Buffer+1;
            break;
        default:
           pstart = ADC_Buffer;
            break;   
    }
    
    for (index=0; index<ADC_AVG_TIMES; index++)
    {
        temp += pstart[index*2];
    }
    
    return temp/ADC_AVG_TIMES;
}

#endif

uint32_t get_vref_val(void)
{
    return vref_val;
}

static GlobalType_t adc_calibration(void)
{
    ADC_InjectionConfTypeDef sConfigInjected = {0};    

    sConfigInjected.InjectedChannel = ADC_CHANNEL_VREFINT;
    sConfigInjected.InjectedRank = 1;
    sConfigInjected.InjectedNbrOfConversion = 1;
    sConfigInjected.InjectedSamplingTime = ADC_SAMPLETIME_56CYCLES;
    sConfigInjected.ExternalTrigInjecConvEdge = ADC_EXTERNALTRIGINJECCONVEDGE_NONE;
    sConfigInjected.ExternalTrigInjecConv = ADC_INJECTED_SOFTWARE_START;
    sConfigInjected.AutoInjectedConv = DISABLE;
    sConfigInjected.InjectedDiscontinuousConvMode = ENABLE;
    sConfigInjected.InjectedOffset = 0;
    if (HAL_ADCEx_InjectedConfigChannel(&hadc1, &sConfigInjected) != HAL_OK)
    {
        return RT_FAIL;
    }

    HAL_ADCEx_InjectedStart(&hadc1);
    
    if (HAL_ADCEx_InjectedPollForConversion(&hadc1, 100) != HAL_OK)
    {
        return RT_FAIL;
    }

    vref_val = HAL_ADCEx_InjectedGetValue(&hadc1, ADC_INJECTED_RANK_1);
    vref_val = __LL_ADC_CALC_VREFANALOG_VOLTAGE(vref_val, LL_ADC_RESOLUTION_12B);
    
    return RT_OK;
}
