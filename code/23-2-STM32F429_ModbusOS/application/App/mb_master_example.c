//////////////////////////////////////////////////////////////////////////////
//  (c) copyright 2023-by Persional Inc.  
//  All Rights Reserved
//
//  Name:
//      mb_slave_example.c
//
//  Purpose:
//      process modbus protocol task.
//
//  Author:
//      @zc
//
//  Assumptions:
//	
//  Revision History:
//
/////////////////////////////////////////////////////////////////////////////

#include "mb_master_example.h"
#include "mb_m.h"
#include "mbport.h"

#define MODBUS_MASTER_RUN_MODE      0

#define MODBUS_DEF_ADDRESS          0x01        //从设备地址
#define MODBUS_DEF_UBAUD            115200      //设备波特率
#define MODBUS_DEF_PARITY           MB_PAR_NONE //奇偶校验位
#define MODBUS_DEF_TCP_PORT         8080

static osThreadId_t mbMasterHandle;
static const osThreadAttr_t mbMasterTask_attributes = {
  .name = "mb_task",
  .stack_size = 1024 * 1,
  .priority = (osPriority_t) osPriorityNormal,
};

void mb_master_task(void *argument)
{
    eMBErrorCode eStatus;

#if MODBUS_MASTER_RUN_MODE == 0    
    eStatus = eMBMasterInit( MB_RTU, MODBUS_DEF_ADDRESS, MODBUS_DEF_UBAUD, MODBUS_DEF_PARITY );
    if( eStatus != MB_ENOERR  ) {
        PRINT_LOG(LOG_ERROR, xTaskGetTickCount(), "eMBInit RTU Mode Failed!");
        return;
    }
#elif MODBUS_MASTER_RUN_MODE == 1
    eStatus = eMBMasterInit( MB_ASCII, MODBUS_DEF_ADDRESS, MODBUS_DEF_UBAUD, MODBUS_DEF_PARITY );
    if( eStatus != MB_ENOERR  ) {
        PRINT_LOG(LOG_ERROR, xTaskGetTickCount(), "eMBInit ASCII Mode Failed!");
        return;
    }
#elif MODBUS_MASTER_RUN_MODE == 2
    eStatus = eMBTCPInit(MODBUS_DEF_TCP_PORT);
    if( eStatus != MB_ENOERR  ) {
        PRINT_LOG(LOG_ERROR, xTaskGetTickCount(), "eMBInit TCP Mode Failed!");
        return;
    }
#endif
    
    eStatus = eMBMasterEnable(  );
    if(eStatus != MB_ENOERR)
    {
        PRINT_LOG(LOG_ERROR, xTaskGetTickCount(), "eMBEnable failed:%d!", eStatus);
    }
        
    while(1)
    {
        eStatus = eMBMasterPoll();
        if(eStatus != MB_ENOERR)
        {
            PRINT_LOG(LOG_ERROR, xTaskGetTickCount(), "eMBPoll failed:%d!", eStatus);
        }
    }
}

GlobalType_t mb_master_example_init(void)
{
    mbMasterHandle = osThreadNew(mb_master_task, NULL, &mbMasterTask_attributes);
    if(mbMasterHandle == NULL)
    {
        return RT_FAIL;
    }

    return RT_OK;   
}

