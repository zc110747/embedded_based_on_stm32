//////////////////////////////////////////////////////////////////////////////
//  (c) copyright 2023-by Persional Inc.  
//  All Rights Reserved
//
//  Name:
//      mqtt_app.h
//
//  Purpose:
//
//  Author:
//      @公众号：<嵌入式技术总结>
//
//  Assumptions:
//	
//  Revision History:
//
/////////////////////////////////////////////////////////////////////////////
#ifndef _MQTT_APP_H
#define _MQTT_APP_H

#include "MQTTClient.h"
#include "drv_global.h"
#include "cmsis_os.h"


struct opts_struct
{
	char* clientid;
	int nodelimiter;
	char* delimiter;
	enum QoS qos;
	char* username;
	char* password;
	char  host[4];
	int port;
	int showtopics;
    char *sub_topic;
};

GlobalType_t mqtt_app_init(void);
#endif
