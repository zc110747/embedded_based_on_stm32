//////////////////////////////////////////////////////////////////////////////
//  (c) copyright 2023-by Persional Inc.  
//  All Rights Reserved
//
//  Name:
//      drv_timer.c
//
//  Purpose:
//      driver for timer.
//      TIM4 - update count
//      TIM2 - capture, PA5
//      TIM3 - pwm, PB1
//  Author:
//      @zc
//
//  Assumptions:
//	
//
//  Revision History:
//
/////////////////////////////////////////////////////////////////////////////
#include "drv_timer.h"
#include <stdlib.h>

#define TPAD_TIMES_CAPTURE_LOW              50
#define TPAD_TIMES_CAPTURE_HIGH             2500
#define TPAD_CAPTURE_LOOP_TIMES             4
#define TPAD_INIT_CHECK_TIMES               10

#define TPAD_IS_VALID_PUSH_KEY(value, no_push_value) \
    (((value)>((no_push_value)*4/3)) && ((value)<((no_push_value)*10)))
#define TPAD_IS_VALID_CAPTURE(value) \
   (((value)>=TPAD_TIMES_CAPTURE_LOW) && ((value)<=(TPAD_TIMES_CAPTURE_HIGH)))
   
//internal variable
static uint32_t tick = 0;
static uint16_t no_push_value_ = 0;
static uint16_t current_value_ = 0;
static TIM_HandleTypeDef htim4;
static TIM_HandleTypeDef htim2;

//internal function
static GlobalType_t timer4_driver_init(void);

static void tpad_init(void);
static GlobalType_t timer2_driver_init(void);
static int comp(const void *a,const void* b);
static uint16_t avg(uint16_t *buf, uint8_t size);
static uint16_t tpad_get_value(void);
static uint16_t tpad_get_max_value(void);
static void tpad_reset(void);

static GlobalType_t timer3_driver_init(void);

static GlobalType_t timer5_driver_init(void);

//------ global function ------
GlobalType_t timer_driver_init(void)
{
    GlobalType_t result;
    
    /*update count*/
    result = timer4_driver_init();

    /*capture*/
    result |= timer2_driver_init();

    /*pwm(dma)*/
    result |= timer3_driver_init();
    
    /*extend clock*/
    result |= timer5_driver_init();
    
    return result;
}

uint32_t ger_timer_tick(void)
{
    return tick;
}


uint8_t tpad_scan_key(void)
{
    /*get the max value when read capture.*/
    current_value_ = tpad_get_max_value(); 

    //check wheather is valid key push value.
    if(TPAD_IS_VALID_PUSH_KEY(current_value_, no_push_value_))					 
        return 1;  

    return 0;
}

//------------------ internal function ---------------------------------
//--------------------------- timer4 driver update mode -------------------------
static GlobalType_t timer4_driver_init(void)
{
    TIM_ClockConfigTypeDef sClockSourceConfig = {0};
    TIM_MasterConfigTypeDef sMasterConfig = {0};

    __HAL_RCC_TIM4_CLK_ENABLE();

    //TIME4, APB1TIME=90M, 
    htim4.Instance = TIM4;
    htim4.Init.Prescaler = 89;     
    htim4.Init.CounterMode = TIM_COUNTERMODE_UP;
    htim4.Init.Period = 1000;      
    htim4.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1; 
    htim4.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_ENABLE;
    if (HAL_TIM_Base_Init(&htim4) != HAL_OK)
    {
        return RT_FAIL;
    }
    
    sClockSourceConfig.ClockSource = TIM_CLOCKSOURCE_INTERNAL;
    if (HAL_TIM_ConfigClockSource(&htim4, &sClockSourceConfig) != HAL_OK)
    {
        return RT_FAIL;
    }
    
    sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;
    sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
    if (HAL_TIMEx_MasterConfigSynchronization(&htim4, &sMasterConfig) != HAL_OK)
    {
        return RT_FAIL;
    }

    HAL_NVIC_SetPriority(TIM4_IRQn, 0, 0);
    HAL_NVIC_EnableIRQ(TIM4_IRQn);

    HAL_TIM_Base_Start_IT(&htim4);
    
    return RT_OK;
}

void TIM4_IRQHandler(void)
{
    if (__HAL_TIM_GET_FLAG(&htim4, TIM_FLAG_UPDATE) != RESET)
    {
        if (__HAL_TIM_GET_IT_SOURCE(&htim4, TIM_IT_UPDATE) != RESET)
        {
            __HAL_TIM_CLEAR_IT(&htim4, TIM_IT_UPDATE);
            tick++;
        }
    }
}

//--------------------------- timer2 capture mode -------------------------------
static GlobalType_t timer2_driver_init(void)
{
    TIM_ClockConfigTypeDef sClockSourceConfig = {0};
    TIM_MasterConfigTypeDef sMasterConfig = {0};
    TIM_IC_InitTypeDef sConfigIC = {0};
    GPIO_InitTypeDef GPIO_Initure;

    __HAL_RCC_TIM2_CLK_ENABLE();
    __HAL_RCC_GPIOA_CLK_ENABLE();

    //step1:config the pin
    GPIO_Initure.Pin = GPIO_PIN_5;                     
    GPIO_Initure.Mode = GPIO_MODE_AF_PP;      
    GPIO_Initure.Pull = GPIO_NOPULL;      
    GPIO_Initure.Speed = GPIO_SPEED_HIGH;    
    GPIO_Initure.Alternate = GPIO_AF1_TIM2;  
    HAL_GPIO_Init(GPIOA, &GPIO_Initure); 

    //step2:config the tim2 clock
    htim2.Instance = TIM2;
    htim2.Init.Prescaler = 89;      
    htim2.Init.CounterMode = TIM_COUNTERMODE_UP;
    htim2.Init.Period = 1000;     
    htim2.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
    htim2.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_ENABLE;
    if (HAL_TIM_IC_Init(&htim2) != HAL_OK)
    {
        return RT_FAIL;
    }
    
    //internal clock
    sClockSourceConfig.ClockSource = TIM_CLOCKSOURCE_INTERNAL;
    if (HAL_TIM_ConfigClockSource(&htim2, &sClockSourceConfig) != HAL_OK)
    {
        return RT_FAIL;
    }

    //not trigger mode, just default
    sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;
    sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
    if (HAL_TIMEx_MasterConfigSynchronization(&htim2, &sMasterConfig) != HAL_OK)
    {
        return RT_FAIL;
    }

    //step3: confog capture mode.
    sConfigIC.ICPolarity = TIM_INPUTCHANNELPOLARITY_RISING;
    sConfigIC.ICSelection = TIM_ICSELECTION_DIRECTTI;
    sConfigIC.ICPrescaler = TIM_ICPSC_DIV1;
    sConfigIC.ICFilter = 0;
    if (HAL_TIM_IC_ConfigChannel(&htim2, &sConfigIC, TIM_CHANNEL_1) != HAL_OK)
    {
        return RT_FAIL;
    }
    HAL_TIM_IC_Start(&htim2, TIM_CHANNEL_1);
    
    tpad_init();
    
    return RT_OK;
}	

static void tpad_init(void)
{
    uint16_t buf[10];
    
    /*read the capture value for no push, sort, used middle*/
    for(int i=0; i<TPAD_INIT_CHECK_TIMES;i++)
    {				 
        buf[i] = tpad_get_value();
        HAL_Delay(5);	    
    }
    qsort(buf, TPAD_INIT_CHECK_TIMES, sizeof(uint16_t), comp);
    no_push_value_ = avg(buf, TPAD_INIT_CHECK_TIMES-2);
    PRINT_LOG(LOG_INFO, HAL_GetTick(), "tpad no_push_value_:%d", no_push_value_);            
}

static int comp(const void *a,const void* b)
{
    return *(uint16_t*)b - *(uint16_t*)a;
}

static uint16_t avg(uint16_t *buf, uint8_t size)
{
   uint8_t index;
   uint16_t sum = 0;
   
   for(index=0; index<size; index++)
   {
     sum += buf[index];
   }
   return sum/size;
}

static void tpad_reset(void)
{
    GPIO_InitTypeDef GPIO_Initure;
	
    GPIO_Initure.Pin = GPIO_PIN_5;            
    GPIO_Initure.Mode = GPIO_MODE_OUTPUT_PP;  
    GPIO_Initure.Pull = GPIO_PULLDOWN;        
    GPIO_Initure.Speed = GPIO_SPEED_HIGH;     
    HAL_GPIO_Init(GPIOA, &GPIO_Initure);
    
    HAL_GPIO_WritePin(GPIOA, GPIO_PIN_5, GPIO_PIN_RESET);	
	
    //delay to wait capture run to zero
    HAL_Delay(3);	

    __HAL_TIM_CLEAR_FLAG(&htim2, TIM_FLAG_CC1|TIM_FLAG_UPDATE);   
    __HAL_TIM_SET_COUNTER(&htim2, 0); 
    
    GPIO_Initure.Mode = GPIO_MODE_AF_PP;      
    GPIO_Initure.Pull = GPIO_NOPULL;         
    GPIO_Initure.Alternate = GPIO_AF1_TIM2;  
    HAL_GPIO_Init(GPIOA, &GPIO_Initure); 
}

static uint16_t tpad_get_value(void)
{
    /*reset the pin for next capture.*/
    tpad_reset();
    
    while(__HAL_TIM_GET_FLAG(&htim2, TIM_FLAG_CC1) == RESET) 
    {
        //not more than TPAD_TIMES_CAPTURE_HIGH
        if(__HAL_TIM_GET_COUNTER(&htim2) > TPAD_TIMES_CAPTURE_HIGH) 
        {
            return __HAL_TIM_GET_COUNTER(&htim2);
        }
    };
    return HAL_TIM_ReadCapturedValue(&htim2, TIM_CHANNEL_1);
}

static uint16_t tpad_get_max_value(void)
{
    uint16_t temp; 
    uint16_t res = 0; 
    uint8_t okcnt = 0;
    uint8_t times = TPAD_CAPTURE_LOOP_TIMES;

    while(times--)
    {
        //get the capture value, store the max in res.
        temp = tpad_get_value();
        if(temp > res)
        {
            res = temp;
        }

        //key capture need in the scope, so include means valid.
        if(TPAD_IS_VALID_CAPTURE(temp))
        {
            okcnt++;
        }
    }

    if(okcnt >= TPAD_CAPTURE_LOOP_TIMES*2/3)
        return res;
    else 
        return 0;
}

//--------------------------- timer3 driver pwm(dma) mode -------------------------
uint16_t timer_pwm_buffer[] = {
    200, 400, 600, 800
};
#define TIME_PWM_PEROID     1000
#define HAL_TIMER_PWM_DMA   1
static DMA_HandleTypeDef hdma_tim3;
static TIM_HandleTypeDef htim3_pwm;
static GlobalType_t timer3_driver_init(void)
{
    TIM_ClockConfigTypeDef sClockSourceConfig = {0};
    TIM_MasterConfigTypeDef sMasterConfig = {0};
    TIM_OC_InitTypeDef sConfigOC = {0};
    GPIO_InitTypeDef GPIO_InitStruct;

    __HAL_RCC_TIM3_CLK_ENABLE();
    __HAL_RCC_GPIOB_CLK_ENABLE();
    __HAL_RCC_DMA1_CLK_ENABLE();

    //STM32F42x数据手册有引脚的复用说明
    GPIO_InitStruct.Pin = GPIO_PIN_1;
    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
    GPIO_InitStruct.Alternate = GPIO_AF2_TIM3;
    HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

    //配置TIM3的时钟
    htim3_pwm.Instance = TIM3;
    htim3_pwm.Init.Prescaler = 89;                          //分频，计数时钟为TimerClk/(prescaler+1), 以90M为例则为1M
    htim3_pwm.Init.CounterMode = TIM_COUNTERMODE_UP;
    htim3_pwm.Init.Period = TIME_PWM_PEROID;                //自动重装载计数器值
    htim3_pwm.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;  //分频作为Timer的实际输入时钟
    htim3_pwm.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_ENABLE;
    if (HAL_TIM_PWM_Init(&htim3_pwm) != HAL_OK)
    {
        return RT_FAIL;
    }
    
    //定义timer选择的时钟来源
    sClockSourceConfig.ClockSource = TIM_CLOCKSOURCE_INTERNAL;
    if (HAL_TIM_ConfigClockSource(&htim3_pwm, &sClockSourceConfig) != HAL_OK)
    {
        return RT_FAIL;
    }

    //关闭外部触发
    sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;
    sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
    if (HAL_TIMEx_MasterConfigSynchronization(&htim3_pwm, &sMasterConfig) != HAL_OK)
    {
        return RT_FAIL;
    }

    //配置Output Compare功能，用于PWM输出
    sConfigOC.OCMode = TIM_OCMODE_PWM1;
    sConfigOC.Pulse = TIME_PWM_PEROID/2;  //50%占空比
    sConfigOC.OCPolarity = TIM_OCPOLARITY_HIGH;
    sConfigOC.OCFastMode = TIM_OCFAST_DISABLE;
    if (HAL_TIM_PWM_ConfigChannel(&htim3_pwm, &sConfigOC, TIM_CHANNEL_4) != HAL_OK)
    {
        return RT_FAIL;
    }

#if HAL_TIMER_PWM_DMA == 1    
    //TIM3_CH4, TIM3_UPDATA对应的DMA通道为
    //DMA1_Stream2, Channel5, 详细见参考手册10.3.3 Channel Selection
    hdma_tim3.Instance = DMA1_Stream2;
    hdma_tim3.Init.Channel = DMA_CHANNEL_5;
    hdma_tim3.Init.Direction = DMA_MEMORY_TO_PERIPH;
    hdma_tim3.Init.PeriphInc = DMA_PINC_DISABLE;
    hdma_tim3.Init.MemInc = DMA_MINC_ENABLE;
    hdma_tim3.Init.PeriphDataAlignment = DMA_PDATAALIGN_HALFWORD;
    hdma_tim3.Init.MemDataAlignment = DMA_MDATAALIGN_HALFWORD;
    hdma_tim3.Init.Mode = DMA_CIRCULAR;
    hdma_tim3.Init.Priority = DMA_PRIORITY_HIGH;
    hdma_tim3.Init.FIFOMode = DMA_FIFOMODE_DISABLE;
    if (HAL_DMA_Init(&hdma_tim3) != HAL_OK)
    {
      return RT_FAIL;
    }
    
    //使用UPDATA作为PWM更换周期
    __HAL_TIM_ENABLE_DMA(&htim3_pwm, TIM_DMA_UPDATE);
    HAL_DMA_Start(&hdma_tim3, (uint32_t)timer_pwm_buffer, (uint32_t)&htim3_pwm.Instance->CCR4, 4);
#endif   

    HAL_TIM_PWM_Start(&htim3_pwm, TIM_CHANNEL_4);
    return RT_OK;
}

//modify the percent for square wave
void pwm_set_percent(float percent)
{
    uint8_t index;
    
    for (index=0; index<4; index++)
    {
        timer_pwm_buffer[index] = timer_pwm_buffer[index]*percent;
    }
}

//--------------------------- timer5 driver input mode -------------------------
//step1:link the PH11 to pwm output
//step2:read the output by get_cnt_per_second
static TIM_HandleTypeDef htim5;
GlobalType_t timer5_driver_init(void)
{
    GPIO_InitTypeDef GPIO_InitStruct = {0};
    TIM_SlaveConfigTypeDef sSlaveConfig = {0};
    TIM_MasterConfigTypeDef sMasterConfig = {0};

    __HAL_RCC_TIM5_CLK_ENABLE();

    __HAL_RCC_GPIOH_CLK_ENABLE();

    /**TIM5 GPIO Configuration
    PH11     ------> TIM5_CH2
    */
    GPIO_InitStruct.Pin = GPIO_PIN_11;
    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
    GPIO_InitStruct.Alternate = GPIO_AF2_TIM5;
    HAL_GPIO_Init(GPIOH, &GPIO_InitStruct);

    /* USER CODE BEGIN TIM5_Init 1 */

    /* USER CODE END TIM5_Init 1 */
    htim5.Instance = TIM5;
    htim5.Init.Prescaler = 0;
    htim5.Init.CounterMode = TIM_COUNTERMODE_UP;
    htim5.Init.Period = 4294967295;
    htim5.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
    htim5.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_ENABLE; 
    if (HAL_TIM_Base_Init(&htim5) != HAL_OK)
    {
        return RT_FAIL;
    }

    sSlaveConfig.SlaveMode = TIM_SLAVEMODE_EXTERNAL1;
    sSlaveConfig.InputTrigger = TIM_TS_TI2FP2;
    sSlaveConfig.TriggerPolarity = TIM_TRIGGERPOLARITY_RISING;
    sSlaveConfig.TriggerFilter = 0;
    if (HAL_TIM_SlaveConfigSynchro(&htim5, &sSlaveConfig) != HAL_OK)
    {
        return RT_FAIL;
    }
    sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;
    sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
    if (HAL_TIMEx_MasterConfigSynchronization(&htim5, &sMasterConfig) != HAL_OK)
    {
        return RT_FAIL;
    }
    
    __HAL_TIM_SetCounter(&htim5, 0);
    HAL_TIM_Base_Start(&htim5);
    return RT_OK;
}

//read cnt every second
//then 1/n Second is the avg peroid
uint32_t get_cnt_per_second(void)
{
    uint32_t cnt;
    
    cnt = __HAL_TIM_GetCounter(&htim5);
    __HAL_TIM_SetCounter(&htim5, 0);
    
    return cnt;
}
