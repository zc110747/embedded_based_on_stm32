//////////////////////////////////////////////////////////////////////////////
//  (c) copyright 2023-by Persional Inc.  
//  All Rights Reserved
//
//  Name:
//      drv_target.c
//
//  Purpose:
//      support printf output.
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
#include <stdio.h>
#include "drv_target.h"

#if defined(RUN_OS_MODE) && RUN_OS_MODE == RUN_WITH_FREERTOS
#include "FreeRTOS.h"
#include "task.h"
#endif

//#pragma import(__use_no_semihosting)                            
//struct __FILE 
//{ 
//	int handle; 
//}; 

//FILE __stdout;     
//void _sys_exit(int x) 
//{ 
//	x = x; 
//}

//use MicroLIB.
volatile int32_t ITM_RxBuffer = ITM_RXBUFFER_EMPTY;
int fputc(int ch, FILE *f)
{  
    ITM_SendChar((uint32_t)ch);
    return ch;
}

static uint8_t is_os_on = 0;

void set_os_on(void)
{
    is_os_on = 1;
}

uint8_t get_os_on(void)
{
    return is_os_on;
}

void hal_delay_ms(uint16_t time)
{
    if(is_os_on == 0)
    {
        HAL_Delay(time);
    }
    else
    {
#if defined(RUN_OS_MODE) && RUN_OS_MODE == RUN_WITH_FREERTOS
        vTaskDelay(time);
#else
#endif
    }
}

uint32_t drv_tick_difference(uint32_t value, uint32_t now_tick)
{
    return (now_tick >= value) ? now_tick - value : UINT_MAX - value + now_tick + 1;
}
