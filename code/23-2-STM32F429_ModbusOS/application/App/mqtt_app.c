//////////////////////////////////////////////////////////////////////////////
//  (c) copyright 2023-by Persional Inc.  
//  All Rights Reserved
//
//  Name:
//      mqtt_app.c
//
//  Purpose:
//
//  Author:
//      @zc
//
//  Assumptions:
//	
//  Revision History:
//
/////////////////////////////////////////////////////////////////////////////
#include <string.h>
#include "mqtt_app.h"
#include "logger_client.h"

struct opts_struct opts =
{
    .clientid = (char*)"stdout-subscriber", 
    .nodelimiter = 0, 
    .delimiter = (char*)"\n", 
    .qos = QOS2, 
    .username = NULL, 
    .password = NULL, 
    .host = {192, 168, 2, 29}, 
    .port = 1883, 
    .showtopics = 0,
    .sub_topic = "stm32_sub"
};

static osThreadId_t mqttHandle;
static const osThreadAttr_t mqttTask_attributes = {
  .name = "mqtt_task",
  .stack_size = 1024 * 8,
  .priority = (osPriority_t) osPriorityHigh,
};

volatile int toStop = 0;
static unsigned char buf[1000];
static unsigned char readbuf[1000];
static Network n;
static MQTTClient c;

extern u8_t netif_link_up(void);

void messageArrived(MessageData* md)
{
	MQTTMessage* message = md->message;
    char *pbuf = (char *)pvPortMalloc(message->payloadlen+1);
    
    memcpy(pbuf, message->payload, message->payloadlen);
	pbuf[message->payloadlen] = '\0';
    
    PRINT_LOG(LOG_INFO, xTaskGetTickCount(), "len:%d data:%s", (int)message->payloadlen, (char*)pbuf);
    
    MQTTPublish(&c, "/info/pub", message);
    vPortFree(pbuf);
}


void mqtt_client_task(void *argument)
{
	int rc = 0;

    while(1)
    {
        //only check link up
		if(!netif_link_up())
		{
			vTaskDelay(1000);
			continue;
		}
        
        NetworkInit(&n);
        
        rc = NetworkConnect(&n, opts.host, opts.port);
        if (rc)
        {  
            PRINT_LOG(LOG_ERROR, xTaskGetTickCount(), "mqtt connecting to %s:%d failed!", opts.host, opts.port);
            NetworkDisconnect(&n);
            vTaskDelay(1000);
            continue; 
        }
        
        MQTTClientInit(&c, &n, 1000, buf, 1000, readbuf, 1000);
        MQTTPacket_connectData data = MQTTPacket_connectData_initializer;       
        data.willFlag = 0;
        data.MQTTVersion = 3;
        data.clientID.cstring = opts.clientid;
        data.username.cstring = opts.username;
        data.password.cstring = opts.password;

        data.keepAliveInterval = 10000;
        data.cleansession = 1;
        
        rc = MQTTConnect(&c, &data);
        if (rc) {
            PRINT_LOG(LOG_INFO, xTaskGetTickCount(), "mqtt connected to %d, topic:%s failed", rc, opts.sub_topic);
            goto exit;
        }    
        rc = MQTTSubscribe(&c, opts.sub_topic, opts.qos, messageArrived);
        if (rc) {
            goto exit;
        }

        PRINT_LOG(LOG_INFO, xTaskGetTickCount(), "mqtt connected to %d success, topic:%s", rc, opts.sub_topic);
        
        while (!toStop)
        {
            if(MQTTYield(&c, 1000) != MQTT_SUCCESS)
            {
                break;
            }
        }

exit:
        PRINT_LOG(LOG_INFO, xTaskGetTickCount(), "mqtt stop connect!", rc);
        MQTTDisconnect(&c);
        NetworkDisconnect(&n);
    }
}

GlobalType_t mqtt_app_init(void)
{   
    mqttHandle = osThreadNew(mqtt_client_task, NULL, &mqttTask_attributes);
    if(mqttHandle == NULL)
    {
        return RT_FAIL;
    }
  
    return RT_OK;
}
