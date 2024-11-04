//////////////////////////////////////////////////////////////////////////////
//  (c) copyright 2023-by Persional Inc.  
//  All Rights Reserved
//
//  Name:
//      drv_i2c_gt911.h
//
//  Purpose:
//      touchscreen chip gt911 use i2c
//
// Author:
//      @zc
//
//  Assumptions:
//
//  Revision History:
//
/////////////////////////////////////////////////////////////////////////////
#ifndef _DRV_I2C_GT911_H
#define _DRV_I2C_GT911_H

#include "includes.h"

#define GT911_ADDR      0x14
#define GT_CTRL_REG 	0X8040   	//GT9147���ƼĴ���
#define GT_CFGS_REG 	0X8047   	//GT9147������ʼ��ַ�Ĵ���
#define GT_CHECK_REG 	0X80FF   	//GT9147У��ͼĴ���
#define GT_ID_REG 		0X8140   	//GT9147��ƷID�Ĵ���
#define GT_GSTID_REG 	0X814E   	//GT9147��ǰ��⵽�Ĵ������
#define GT_TP1_REG 		0X8150  	//��һ�����������ݵ�ַ
#define GT_TP2_REG 		0X8158		//�ڶ������������ݵ�ַ
#define GT_TP3_REG 		0X8160		//���������������ݵ�ַ
#define GT_TP4_REG 		0X8168		//���ĸ����������ݵ�ַ
#define GT_TP5_REG 		0X8170		//��������������ݵ�ַ  

#define CT_MAX_TOUCH 10    //������֧�ֵĵ���,�̶�Ϊ5��
#define TP_PRES_DOWN 0x80  //����������	  
#define TP_CATH_PRES 0x40  //�а���������

//������������
typedef struct
{
	uint16_t x[CT_MAX_TOUCH]; 		//��ǰ����
	uint16_t y[CT_MAX_TOUCH];		//�����������10������,����������x[0],y[0]����:�˴�ɨ��ʱ,����������,��
								//x[9],y[9]�洢��һ�ΰ���ʱ������. 
	uint16_t sta;					//�ʵ�״̬ 
								//b15:����1/�ɿ�0; 
	                            //b14:0,û�а�������;1,�а�������. 
								//b13~b10:����
								//b9~b0:���ݴ��������µĵ���(0,��ʾδ����,1��ʾ����)
}_m_tp_dev;

uint8_t gt911_driver_init(void);
#endif
