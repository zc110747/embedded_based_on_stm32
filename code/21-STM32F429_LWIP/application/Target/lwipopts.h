/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * File Name          : Target/lwipopts.h
  * Description        : This file overrides LwIP stack default configuration
  *                      done in opt.h file.
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2024 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */
/* USER CODE END Header */

/* Define to prevent recursive inclusion --------------------------------------*/
#ifndef __LWIPOPTS__H__
#define __LWIPOPTS__H__

#include "main.h"

/*-----------------------------------------------------------------------------*/
/* Current version of LwIP supported by CubeMx: 2.1.2 -*/
/*-----------------------------------------------------------------------------*/

/* Within 'USER CODE' section, code will be kept by default at each generation */
/* USER CODE BEGIN 0 */

/* USER CODE END 0 */

#ifdef __cplusplus
 extern "C" {
#endif

/* STM32CubeMX Specific Parameters (not defined in opt.h) ---------------------*/
/* Parameters set in STM32CubeMX LwIP Configuration GUI -*/
/*----- WITH_RTOS enabled (Since FREERTOS is set) -----*/
#define WITH_RTOS 1
/*----- CHECKSUM_BY_HARDWARE enabled -----*/
#define CHECKSUM_BY_HARDWARE 1
/*-----------------------------------------------------------------------------*/

/* LwIP Stack Parameters (modified compared to initialization value in opt.h) -*/
/* Parameters set in STM32CubeMX LwIP Configuration GUI -*/
/*----- Value in opt.h for MEM_ALIGNMENT: 1 -----*/
#define MEM_ALIGNMENT 4
/*----- Value in opt.h for LWIP_ETHERNET: LWIP_ARP || PPPOE_SUPPORT -*/
#define LWIP_ETHERNET 1
/*----- Value in opt.h for LWIP_DNS_SECURE: (LWIP_DNS_SECURE_RAND_XID | LWIP_DNS_SECURE_NO_MULTIPLE_OUTSTANDING | LWIP_DNS_SECURE_RAND_SRC_PORT) -*/
#define LWIP_DNS_SECURE 7
/*----- Value in opt.h for TCP_SND_QUEUELEN: (4*TCP_SND_BUF + (TCP_MSS - 1))/TCP_MSS -----*/
//#define TCP_SND_QUEUELEN 9
/*----- Value in opt.h for TCP_SNDLOWAT: LWIP_MIN(LWIP_MAX(((TCP_SND_BUF)/2), (2 * TCP_MSS) + 1), (TCP_SND_BUF) - 1) -*/
//#define TCP_SNDLOWAT 1071
/*----- Value in opt.h for TCP_SNDQUEUELOWAT: LWIP_MAX(TCP_SND_QUEUELEN)/2, 5) -*/
#define TCP_SNDQUEUELOWAT 5
/*----- Value in opt.h for TCP_WND_UPDATE_THRESHOLD: LWIP_MIN(TCP_WND/4, TCP_MSS*4) -----*/
//#define TCP_WND_UPDATE_THRESHOLD 536
/*----- Value in opt.h for LWIP_NETIF_LINK_CALLBACK: 0 -----*/
#define LWIP_NETIF_LINK_CALLBACK 1
/*----- Value in opt.h for TCPIP_THREAD_STACKSIZE: 0 -----*/
#define TCPIP_THREAD_STACKSIZE 1024
/*----- Value in opt.h for TCPIP_THREAD_PRIO: 1 -----*/
#define TCPIP_THREAD_PRIO 24
/*----- Value in opt.h for TCPIP_MBOX_SIZE: 0 -----*/
#define TCPIP_MBOX_SIZE 6
/*----- Value in opt.h for SLIPIF_THREAD_STACKSIZE: 0 -----*/
#define SLIPIF_THREAD_STACKSIZE 1024
/*----- Value in opt.h for SLIPIF_THREAD_PRIO: 1 -----*/
#define SLIPIF_THREAD_PRIO 3
/*----- Value in opt.h for DEFAULT_THREAD_STACKSIZE: 0 -----*/
#define DEFAULT_THREAD_STACKSIZE 1024
/*----- Value in opt.h for DEFAULT_THREAD_PRIO: 1 -----*/
#define DEFAULT_THREAD_PRIO 3
/*----- Value in opt.h for DEFAULT_UDP_RECVMBOX_SIZE: 0 -----*/
#define DEFAULT_UDP_RECVMBOX_SIZE 6
/*----- Value in opt.h for DEFAULT_TCP_RECVMBOX_SIZE: 0 -----*/
#define DEFAULT_TCP_RECVMBOX_SIZE 6
/*----- Value in opt.h for DEFAULT_ACCEPTMBOX_SIZE: 0 -----*/
#define DEFAULT_ACCEPTMBOX_SIZE 6
/*----- Value in opt.h for RECV_BUFSIZE_DEFAULT: INT_MAX -----*/
#define RECV_BUFSIZE_DEFAULT 2000000000
/*----- Value in opt.h for LWIP_STATS: 1 -----*/
#define LWIP_STATS 0
/*----- Value in opt.h for CHECKSUM_GEN_IP: 1 -----*/
#define CHECKSUM_GEN_IP 0
/*----- Value in opt.h for CHECKSUM_GEN_UDP: 1 -----*/
#define CHECKSUM_GEN_UDP 0
/*----- Value in opt.h for CHECKSUM_GEN_TCP: 1 -----*/
#define CHECKSUM_GEN_TCP 0
/*----- Value in opt.h for CHECKSUM_GEN_ICMP: 1 -----*/
#define CHECKSUM_GEN_ICMP 0
/*----- Value in opt.h for CHECKSUM_GEN_ICMP6: 1 -----*/
#define CHECKSUM_GEN_ICMP6 0
/*----- Value in opt.h for CHECKSUM_CHECK_IP: 1 -----*/
#define CHECKSUM_CHECK_IP 0
/*----- Value in opt.h for CHECKSUM_CHECK_UDP: 1 -----*/
#define CHECKSUM_CHECK_UDP 0
/*----- Value in opt.h for CHECKSUM_CHECK_TCP: 1 -----*/
#define CHECKSUM_CHECK_TCP 0
/*----- Value in opt.h for CHECKSUM_CHECK_ICMP: 1 -----*/
#define CHECKSUM_CHECK_ICMP 0
/*----- Value in opt.h for CHECKSUM_CHECK_ICMP6: 1 -----*/
#define CHECKSUM_CHECK_ICMP6 0
/*----- Default Value for LWIP_DBG_MIN_LEVEL: LWIP_DBG_LEVEL_ALL ---*/
#define LWIP_DBG_MIN_LEVEL LWIP_DBG_LEVEL_SERIOUS
/*-----------------------------------------------------------------------------*/
/* USER CODE BEGIN 1 */

/* USER CODE END 1 */
#define LWIP_SO_RCVTIMEO                1
#define LWIP_SO_SNDTIMEO                1

//*****************************************************************************
//
// ---------- Platform specific locking ----------
//
//*****************************************************************************
#define SYS_LIGHTWEIGHT_PROT              1          //�ٽ��жϱ�������(������ģʽ�¿���)
#define NO_SYS                            0          //ʹ���Զ���LWIP���� Ϊ1��ֻʹ�ó�������LWIP����

//*****************************************************************************
//
// ---------- Memory options ----------
//
//*****************************************************************************
#define MEM_LIBC_MALLOC                 0				//����LWIP�Դ��������䶯̬�ڴ�
#define MEM_LIBC_MALLOC                 0				//�ڴ�ز�ͨ���ڴ��������
#define MEM_ALIGNMENT                   4				//�ֽڶ���(��CPU��λ���й�,32λ����Ϊ4)
#define MEM_SIZE                        8*1024  //���ڴ��
#define MEMP_SANITY_CHECK               0				//mem_free���ú��������Ƿ���ѭ�� by zc
#define MEMP_OVERFLOW_CHECK             0				//lwip���ڴ������� 
#define MEM_USE_POOLS                   0 			//�ڴ�Ѳ�ͨ���ڴ�ط���
#define MEM_USE_POOLS_TRY_BIGGER_POOL	  0				//�����ڴ�ʧ�ܲ�ѡ����ڴ��
#define MEMP_USE_CUSTOM_POOLS           0			  //ͬ��


//*****************************************************************************
//
// ---------- Internal Memory Pool Sizes ----------
//
//*****************************************************************************
#define MEMP_NUM_PBUF                   	60		//����memp��PBUF_ROM(ROM�ڴ��)��PBUF_REF(RAM�ڴ��)��Ŀ����ܺ�
#define MEMP_NUM_RAW_PCB                	4			//RAW���ӵ�PCB��Ŀ(��ҪLWIP RAW��Ч)
#define MEMP_NUM_UDP_PCB                  4			//�ܹ�ͬʱ���ӵ�UDP��PCB��Ŀ
#define MEMP_NUM_TCP_PCB                	(TCP_WND + TCP_SND_BUF)/TCP_MSS  //�ܹ�ͬʱ���ӵ�TCP��PCB��Ŀ 12
#define MEMP_NUM_TCP_PCB_LISTEN          	2			//�ܹ�ͬʱ������TCP��PCB��Ŀ by zc 2015.10.30
#define MEMP_NUM_TCP_SEG                	40  	//80 �ܹ�ͬʱ�ڶ������TCP��PCB��Ŀ
#define MEMP_NUM_REASSDATA               	8   	//���ͬʱ�ȴ���װ��IP����Ŀ��������IP��������IP��Ƭ
#define MEMP_NUM_ARP_QUEUE              	30		//���ȴ��ظ�ARP�������Ŀ(ARP_QUEUEING��Ч)
#define MEMP_NUM_IGMP_GROUP 							8			//�ಥ��ͬʱ�������ݵ�����Ա��Ŀ(LWIP_IGMP��Ч)
#define MEMP_NUM_SYS_TIMEOUT            	20  	//�ܹ�ͬʱ����ĳ�ʱ������Ŀ(NO_SYS==0��Ϸ)
#define MEMP_NUM_NETBUF                	 	10  	//netbufs�ṹ����Ŀ
#define MEMP_NUM_NETCONN                	16  	//netconns�ṹ����Ŀ
#define MEMP_NUM_TCPIP_MSG_API          	40  	//tcpip_msg�ṹ�������Ŀ������callback��API��ͨѶ by zc
#define MEMP_NUM_TCPIP_MSG_INPKT          40  	//tcpip_msg�������ݰ��ṹ�������Ŀ by zc

//*****************************************************************************
//
// ---------- ARP options ----------
//
//*****************************************************************************
#define LWIP_ARP                        	1		//ARPЭ������ 
#define ARP_TABLE_SIZE                  	10  //ARPά���ı����С
#define ARP_QUEUEING                    	1   //Ӳ����ַ����ʱ�����������ݰ��������
#define ETHARP_TRUST_IP_MAC             	0   //����IP���ݰ�����ֱ������ARP table�ĸ��£�
																							//Ϊ0��Ǳ�����IP-MAC��ϵ������ARP����,���Ա���IP��ƭ�������������ʱ
#define ETHARP_SUPPORT_VLAN             	0   //�������������Ϊ1������������ͨѶ��Ч

//*****************************************************************************
//
// ---------- IP options ----------
//
//*****************************************************************************
#define IP_FORWARD                      	0    //������ƥ�����ݰ�ת��,��ӿ�ʱΪ1
#define IP_OPTIONS_ALLOWED             		1    //��IPѡ�����ݰ����� Ϊ0��������IP���ݰ�
#define IP_FRAG_USES_STATIC_BUF         	1		 //IP��Ƭʹ�þ�̬������
#define IP_FRAG_MAX_MTU                 	1500 //IP��Ƭ��󻺳�����
#define IP_DEFAULT_TTL                  	255  //IP���ݰ�������豸��Ŀ
#define IP_SOF_BROADCAST                	0		 //IP���͹㲥������
#define IP_SOF_BROADCAST_RECV          		0	   //IP���չ㲥������

//*****************************************************************************
//
// ---------- ICMP options ----------
//
//*****************************************************************************
#define LWIP_ICMP                       	1    //����ping������/����
#define ICMP_TTL                       		(IP_DEFAULT_TTL) //ping��������豸��Ŀ
#define LWIP_BROADCAST_PING             	0    //���ظ��㲥ping��
#define LWIP_MULTICAST_PING             	0    //���ظ��ಥping��

//*****************************************************************************
//
// ---------- RAW options ----------
//
//*****************************************************************************
#define LWIP_RAW                        	0		 							//�޲���ϵͳ���ڻص���������
#define RAW_TTL                           (IP_DEFAULT_TTL) 	//Ӧ�ò����ݴ������(����IP���TTL)

//*****************************************************************************
//
// ---------- UDP options ----------
//
//*****************************************************************************
#define LWIP_UDP                       		1			//����UDPЭ��(snmpЭ����ڴ�)
#define LWIP_UDPLITE                   		1			//UDPЭ�����þ����
#define UDP_TTL                        		(IP_DEFAULT_TTL) //UDP���ݰ��������
#define LWIP_NETBUF_RECVINFO           		0 		//���յ������ݰ����׸�������������Ŀ�ĵ�ַ�Ͷ˿�

//*****************************************************************************
//
// ---------- TCP options ----------
//
//*****************************************************************************
#define LWIP_TCP                         	1													//����TCPЭ��(httpЭ����ڴ�)
#define TCP_TTL                         	(IP_DEFAULT_TTL)					//TCP���ݰ��������
#define TCP_WND                         	6*TCP_MSS   							//tcp���ڴ�С
#define TCP_MAXRTX                      	12												//��������ش�TCP���ݰ���Ŀ
#define TCP_SYNMAXRTX                  		6													//��������ش�SYN���Ӱ���Ŀ
#define TCP_QUEUE_OOSEQ                 	(LWIP_TCP)        				//TCP���ն��������ݿ�����
#define TCP_MSS                         	1460  										//tcp�������ֶγ��� 
																																		//Ϊ�˱���IP��Ƭ MSS�ĳ���ΪMTU-IP_header-TCP_header = (1500 - 20 - 20)
#define TCP_CALCULATE_EFF_SEND_MSS       	1													//tcpԤ�Ʒ��͵ķֶγ��ȣ�Ϊ1����ݴ��ڴ�С����
#define TCP_SND_BUF                     	(8*TCP_MSS)   						//TCP���ͻ�����  zc 7.1                                     
#define TCP_SND_QUEUELEN                	(4*(TCP_SND_BUF/TCP_MSS))	//TCP���Ͷ��г���
#define TCP_SNDLOWAT                    	(TCP_SND_BUF/4)						//TCP�ɷ������ݳ���
#define TCP_LISTEN_BACKLOG              	1    											//TCP����������  
#define TCP_DEFAULT_LISTEN_BACKLOG      	0xff											//TCP���ӺͰ����ӵ����� 
#define LWIP_TCP_TIMESTAMPS            		0													//TCPʱ���ѡ��
#define TCP_WND_UPDATE_THRESHOLD          (TCP_WND / 6)							//TCP���ڸ�����ֵ

//TCP��ʱ������ѡ��
#define  TCP_MSL                         	10000UL       						//TCP���Ӵ���ʱ�� ��λms
//#define  TCP_FIN_WAIT_TIMEOUT 		       	20000UL      							//FIN�ȵ�ACKӦ��ʱ�� ��λms
#define  TCP_TMR_INTERVAL                	1           						  //TCP��ʱ��������� 20ms

//*****************************************************************************
//		 
// ---------- Internal Memory Pool Sizes ----------
//
//*****************************************************************************
 #define  PBUF_LINK_HLEN                 	14           //Ϊ������̫��ͷ����İ�����(����MAC��ַ+Զ��MAC��ַ+Э������) 6+6+2																										
 #define  PBUF_POOL_BUFSIZE             	256        	 //�����ڴ�س��ȣ�Ҫ���ǵ�4�ֽڶ������Ѵ�С
 #define  ETH_PAD_SIZE                   	0            //��̫����䳤�ȣ�stm32����Ϊ0 ���ݷ��Ͱ��ж�
 #define 	PBUF_POOL_SIZE                 	64					 //�ڴ������

//*****************************************************************************
//
// ---------- Sequential layer options ----------
//
//*****************************************************************************
#define LWIP_TCPIP_CORE_LOCKING         	0						//?
#define LWIP_NETCONN                      1           //Ӧ�ò�ʹ��NETCONN��غ���

//*****************************************************************************
//
// ---------- TCP KEEPLIVE ----------
//
//*****************************************************************************
#define LWIP_TCP_KEEPALIVE              	1				//tcp���ʱ�� zc 7.22
#define TCP_KEEPIDLE_DEFAULT							60000   //����ʱ�� 60s
#define TCP_KEEPINTVL_DEFAULT							10000   //����̽�� 10s
#define TCP_KEEPCNT_DEFAULT     				 	9U  

#ifdef __cplusplus
}
#endif
#endif /*__LWIPOPTS__H__ */
