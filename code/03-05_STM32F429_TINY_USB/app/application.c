//////////////////////////////////////////////////////////////////////////////
//  (c) copyright 2023-by Persional Inc.  
//  All Rights Reserved
//
//  Name:
//      application.c
//
//  Purpose:
//      application run   
//
//  Author:
//      
//
//  Assumptions:
//	
//  Revision History:
//
/////////////////////////////////////////////////////////////////////////////
#include "application.h"
#include "app_cdc.h"
#include "tusb.h"

GlobalType_t application_init(void)
{ 
    /* VBUS sensing off: the device assumes bus power is always present, which
    * is what you want when VBUS is not routed to the MCU. Must be set before
    * tud_init(). */
    tud_configure_dwc2_t dwc2_cfg = CFG_TUD_CONFIGURE_DWC2_DEFAULT;
    dwc2_cfg.vbus_sensing = false;
    tud_configure(BOARD_TUD_RHPORT, TUD_CFGID_DWC2, &dwc2_cfg);

    const tusb_rhport_init_t rh_init = {
        .role  = TUSB_ROLE_DEVICE,
        .speed = TUSB_SPEED_FULL,
    };
    tusb_init(BOARD_TUD_RHPORT, &rh_init);

    app_cdc_init();
    return RT_OK;
}

GlobalType_t application_run(void)
{
    tud_task();        /* TinyUSB device stack - must be called often */
    
    app_cdc_task();
    
    return RT_OK;
}

