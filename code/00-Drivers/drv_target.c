//////////////////////////////////////////////////////////////////////////////
//  (c) copyright 2023-by Persional Inc.  
//  All Rights Reserved
//
//  Name:
//      drv_adc.c
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
#include "includes.h"

#pragma import(__use_no_semihosting)                            
struct __FILE 
{ 
	int handle; 
}; 

FILE __stdout;     
void _sys_exit(int x) 
{ 
	x = x; 
}

//use MicroLIB.
volatile int32_t ITM_RxBuffer = ITM_RXBUFFER_EMPTY;
int fputc(int ch, FILE *f)
{  
    ITM_SendChar((uint32_t)ch);
    return ch;
}
