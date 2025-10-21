//////////////////////////////////////////////////////////////////////////////
//  (c) copyright 2023-by Persional Inc.  
//  All Rights Reserved
//
//  Name:
//      drv_wdg.c
//
//  Purpose:
//      driver for watchdog, include iwdg and wwdg.
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

#include "drv_wdg.h"

static IWDG_HandleTypeDef hiwdg;
static WWDG_HandleTypeDef hwwdg;

static GlobalType_t iwdg_init(void);
static GlobalType_t wwdg_init(void);

GlobalType_t drv_wdg_init(void)
{
    GlobalType_t  result;
    
    result = iwdg_init();
    
    result |= wwdg_init();
    
    return result;
}

//iwdg clock 32K, count 1K = 1ms
//times 4.095s
static GlobalType_t iwdg_init(void)
{
    hiwdg.Instance = IWDG;
    hiwdg.Init.Prescaler = IWDG_PRESCALER_32; 
    hiwdg.Init.Reload = 4095;
    if (HAL_IWDG_Init(&hiwdg) != HAL_OK)
    {
        return RT_FAIL;
    }

    return RT_OK;
}

void iwdg_reload(void)
{
    HAL_IWDG_Refresh(&hiwdg);
}

static GlobalType_t wwdg_init(void)
{
    __HAL_RCC_WWDG_CLK_ENABLE();

    HAL_NVIC_SetPriority(WWDG_IRQn, 0, 0);
    HAL_NVIC_EnableIRQ(WWDG_IRQn);
    
    hwwdg.Instance = WWDG;
    hwwdg.Init.Prescaler = WWDG_PRESCALER_8;
    hwwdg.Init.Window = 90;
    hwwdg.Init.Counter = 127;
    hwwdg.Init.EWIMode = WWDG_EWI_ENABLE;
    if (HAL_WWDG_Init(&hwwdg) != HAL_OK)
    {
        return RT_FAIL;
    }
 
    return RT_OK;
}

void WWDG_IRQHandler(void)
{
    if (__HAL_WWDG_GET_FLAG(&hwwdg, WWDG_FLAG_EWIF) != RESET)
    {
        __HAL_WWDG_CLEAR_FLAG(&hwwdg, WWDG_FLAG_EWIF);
        HAL_WWDG_Refresh(&hwwdg);
    }
}
