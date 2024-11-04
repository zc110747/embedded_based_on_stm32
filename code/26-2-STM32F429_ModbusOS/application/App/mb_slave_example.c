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

#include "mb_slave_example.h"
#include "mb.h"
#include "mbport.h"

#define MODBUS_RUN_MODE             2

#define MODBUS_DEF_ADDRESS          0x01        //���豸��ַ
#define MODBUS_DEF_UBAUD            115200      //�豸������
#define MODBUS_DEF_PARITY           MB_PAR_NONE //��żУ��λ
#define MODBUS_DEF_TCP_PORT         8080

static osThreadId_t mbHandle;
static const osThreadAttr_t mbTask_attributes = {
  .name = "mb_task",
  .stack_size = 1024 * 1,
  .priority = (osPriority_t) osPriorityNormal,
};

void mb_process_task(void *argument)
{
    eMBErrorCode eStatus;

#if MODBUS_RUN_MODE == 0    
    eStatus = eMBInit( MB_RTU, MODBUS_DEF_ADDRESS, 0, MODBUS_DEF_UBAUD, MODBUS_DEF_PARITY );
    if( eStatus != MB_ENOERR  ) {
        PRINT_LOG(LOG_ERROR, xTaskGetTickCount(), "eMBInit RTU Mode Failed!");
        return;
    }
#elif MODBUS_RUN_MODE == 1
    eStatus = eMBInit( MB_ASCII, MODBUS_DEF_ADDRESS, 0, MODBUS_DEF_UBAUD, MODBUS_DEF_PARITY );
    if( eStatus != MB_ENOERR  ) {
        PRINT_LOG(LOG_ERROR, xTaskGetTickCount(), "eMBInit ASCII Mode Failed!");
        return;
    }
#elif MODBUS_RUN_MODE == 2
    eStatus = eMBTCPInit(MODBUS_DEF_TCP_PORT);
    if( eStatus != MB_ENOERR  ) {
        PRINT_LOG(LOG_ERROR, xTaskGetTickCount(), "eMBInit TCP Mode Failed!");
        return;
    }
#endif
    
    eStatus = eMBEnable(  );
    if(eStatus != MB_ENOERR)
    {
        PRINT_LOG(LOG_ERROR, xTaskGetTickCount(), "eMBEnable failed:%d!", eStatus);
    }
        
    while(1)
    {
        eStatus = eMBPoll();
        if(eStatus != MB_ENOERR)
        {
            PRINT_LOG(LOG_ERROR, xTaskGetTickCount(), "eMBPoll failed:%d!", eStatus);
        }
    }
}

GlobalType_t mb_app_init(void)
{
    mbHandle = osThreadNew(mb_process_task, NULL, &mbTask_attributes);
    if(mbHandle == NULL)
    {
        return RT_FAIL;
    }

    return RT_OK;   
}

extern UCHAR xMBUtilGetBits( UCHAR * ucByteBuf, USHORT usBitOffset, UCHAR ucNBits );
extern void xMBUtilSetBits( UCHAR * ucByteBuf, USHORT usBitOffset, UCHAR ucNBits, UCHAR ucValue );

/*
CRCУ��: ��λ��ǰ, ��λ�ں�

������: 01(0x01), ����Ȧ״̬����ȡ������ֵ(ÿ���Ĵ�������1bit����)���ɶ���д
#define MB_FUNC_READ_COILS                    ( 1 )
eMBException eMBFuncReadCoils( UCHAR * pucFrame, USHORT * usLen )
RTU����:  | 01 | 01 | 00 00 | 00 08 | 3d cc | => | ���豸��ַ | ������ | �Ĵ����׵�ַ,ʵ��+1 | �Ĵ������� | CRCУ�� |
�ӻ���Ӧ: | 01 | 01 | 01 | 10 | 50 44 | => | ���豸��ַ | ������ | ���ݸ��� | �Ĵ��������� | CRCУ�� |

������: 05(0x05), д������Ȧ(0xFF 0x00��ʾ1, 0x00 0x00Ϊ0, ���������)
#define MB_FUNC_WRITE_SINGLE_COIL             (  5 )
eMBException eMBFuncWriteCoil( UCHAR * pucFrame, USHORT * usLen )
RTU����:  | 01 | 05 | 00 01 | FF 00 | dd fa | => | ���豸��ַ | ������ | �Ĵ����׵�ַ | ������� | CRCУ�� |
�ӻ���Ӧ: | 01 | 05 | 00 01 | FF 00 | dd fa | => | ���豸��ַ | ������ | �Ĵ����׵�ַ | ������� | CRCУ�� |

������: 15(0x0F), д�����Ȧ
#define MB_FUNC_WRITE_MULTIPLE_COILS          ( 15 )
eMBException eMBFuncWriteMultipleCoils( UCHAR * pucFrame, USHORT * usLen )
RTU����:  | 01 | 0F | 00 00 | 00 04 | 01 | 0x0F | 7f 5e | => | ���豸��ַ | ������ | �Ĵ����׵�ַ | ��Ȧ��Ŀ | д���ֽڸ��� | д���ֽ� | CRCУ�� |
�ӻ���Ӧ: | 01 | 0F | 00 00 | 00 04 | 54 08 |             => | ���豸��ַ | ������ | �Ĵ����׵�ַ | ��Ȧ��Ŀ | CRCУ�� |

��ӦӲ��: LED, mos�ܵ�
*/
#define REG_COIL_START           0x0001
#define REG_COIL_NREGS           10
#define REG_COIL_END             (REG_COIL_START + REG_COIL_NREGS*8)
static UCHAR   usRegCoilBuf[REG_COIL_NREGS] = {0x10, 0x01, 0x01, 0x00, 0x01, 0x01, 0x00, 0x01};
eMBErrorCode eMBRegCoilsCB( UCHAR * pucRegBuffer, USHORT usAddress, USHORT usNCoils, eMBRegisterMode eMode )
{
    int i;

    /* check if we away of table size */
    if (usAddress < REG_COIL_START 
    || usAddress + usNCoils > REG_COIL_END) {
        return MB_ENOREG;
    } 
    
    //covert to register list
    usAddress -= REG_COIL_START;
    
    switch (eMode) {
        case MB_REG_WRITE:
            for (i = 0; i < usNCoils; i++) {
                UCHAR wbit = xMBUtilGetBits(pucRegBuffer, i, 1 );
                xMBUtilSetBits( usRegCoilBuf, usAddress+i, 1, wbit );
            }
            break;
        case MB_REG_READ:
            for (i = 0; i < usNCoils; i++) {
                UCHAR rbit = xMBUtilGetBits( usRegCoilBuf, usAddress+i, 1 );
                xMBUtilSetBits( pucRegBuffer, i, 1, rbit );
            }
            break;
    }

    return MB_ENOERR;
}

/*
������: 02(0x02), ��ȡ��ɢ����(ÿ���Ĵ�������1bit����)��ֻ��
#define MB_FUNC_READ_DISCRETE_INPUTS          (  2 )
eMBException eMBFuncReadDiscreteInputs( UCHAR * pucFrame, USHORT * usLen )����eMBRegDiscreteCB
RTU����:  | 01 | 02 | 00 00 | 00 10 | 79 c6 | => | ���豸��ַ | ������ | ��ɢ�Ĵ�����ַ | ��ɢ�Ĵ������� | CRCУ�� |
�ӻ���Ӧ: | 01 | 02 | 02 | 31 15 | 6d e7 | => | ���豸��ַ | ������ | ���ݸ��� | �Ĵ��������� | CRCУ�� |
*/
#define REG_DISCRETE_START           0x0001
#define REG_DISCRETE_NREGS           10
#define REG_DISCRETE_END             (REG_DISCRETE_START + REG_DISCRETE_NREGS*8)
static UCHAR   usRegDiscreateBuf[REG_DISCRETE_NREGS] = {0x31, 0x15, 0x01, 0x00, 0x01, 0x01, 0x01};  
eMBErrorCode eMBRegDiscreteCB( UCHAR * pucRegBuffer, USHORT usAddress, USHORT usNDiscrete )
{
    eMBErrorCode eStatus = MB_ENOERR;

    /* check if we away of table size */
    if (usAddress <  REG_COIL_START
    || usAddress + usNDiscrete > REG_DISCRETE_END) {
        return MB_ENOREG;
    }
    usAddress -= REG_COIL_START;
    
    while (usNDiscrete > 0) {
        *pucRegBuffer++ = xMBUtilGetBits(usRegDiscreateBuf, usAddress, (uint8_t)( usNDiscrete > 8 ? 8 : usNDiscrete ) );
        
        if(usNDiscrete > 8) {
            usNDiscrete -= 8;
            usAddress += 8;
        } else {
            break;
        }
    }
    return eStatus;    
}

/*
������: 03(0x03), ��ȡ���ּĴ���((ÿ���Ĵ�������16bit����)
#define MB_FUNC_READ_DISCRETE_INPUTS          (  2 )
eMBException eMBFuncReadDiscreteInputs( UCHAR * pucFrame, USHORT * usLen )����eMBRegDiscreteCB
RTU����:  | 01 | 03 | 00 00 | 00 02 | 25 3a | => | ���豸��ַ | ������ | ���ּĴ�����ַ | ���ּĴ������� | CRCУ�� |
�ӻ���Ӧ: | 01 | 03 | 04 | 10 00 10 01 | 32 f3 | => | ���豸��ַ | ������ | ���ݸ��� | �Ĵ��������� | CRCУ�� |

������: 06(0x06), д�뱣�ּĴ���((ÿ���Ĵ�������16bit����)
#define MB_FUNC_WRITE_REGISTER                (  6 )
eMBException eMBFuncWriteHoldingRegister( UCHAR * pucFrame, USHORT * usLen )
RTU����:  | 01 | 06 | 00 00 | 01 0A | 08 5d| => | ���豸��ַ | ������ | ���ּĴ�����ַ | ���ּĴ������� | CRCУ�� |
�ӻ���Ӧ: | 01 | 06 | 00 00 | 01 0A | 08 5d | => | ���豸��ַ | ������ | ���ּĴ�����ַ | ���ּĴ������� | CRCУ�� |
*/
#define REG_HOLDING_START           0x0001
#define REG_HOLDING_NREGS           10
#define REG_HOLDING_END             (REG_HOLDING_START+REG_HOLDING_NREGS)
static USHORT   usRegHoldingBuf[REG_HOLDING_NREGS] = {0x1000, 0x1001, 0x1002, 0x1003};
eMBErrorCode eMBRegHoldingCB( UCHAR * pucRegBuffer, USHORT usAddress, USHORT usNRegs, eMBRegisterMode eMode )
{
    eMBErrorCode    eStatus = MB_ENOERR;
    int             iRegIndex;

    if ((usAddress >= REG_HOLDING_START) 
    && ((usAddress+usNRegs) <= REG_HOLDING_END)) {
        iRegIndex = (int)(usAddress - REG_HOLDING_START);
        switch (eMode) {                                       
            case MB_REG_READ://�� MB_REG_READ = 0
                while (usNRegs > 0) {
                    *pucRegBuffer++ = (usRegHoldingBuf[iRegIndex] >> 8);            
                    *pucRegBuffer++ = (usRegHoldingBuf[iRegIndex] & 0xFF); 
                    iRegIndex++;
                    usNRegs--;					
                }                            
                break;
            case MB_REG_WRITE://д MB_REG_WRITE = 0
                while (usNRegs > 0) {         
                    usRegHoldingBuf[iRegIndex] = *pucRegBuffer++ << 8;
                    usRegHoldingBuf[iRegIndex] |= *pucRegBuffer++;
                    iRegIndex++;
                    usNRegs--;
                }
                break;
        }
    } else {
        eStatus = MB_ENOREG;
    }	

    return eStatus;
}

/*
������: 04, ��ȡ����Ĵ���(ÿ������2�ֽ�)

#define MB_FUNC_READ_INPUT_REGISTER          (  4 )
eMBException eMBFuncReadInputRegister( UCHAR * pucFrame, USHORT * usLen )eMBRegInputCB

RTU����:  | 01 | 04 | 00 00 | 00 02 | 25 3a | => | ���豸��ַ | ������ | ����Ĵ�����ַ | ����Ĵ������� | CRCУ�� |
�ӻ���Ӧ: | 01 | 04 | 04 | 10 00 10 01 | 33 44 | => | ���豸��ַ | ������ | ���ݸ��� | �Ĵ��������� | CRCУ�� |
*/
#define REG_INPUT_START           0x0001
#define REG_INPUT_NREGS           10
#define REG_INPUT_END             (REG_INPUT_START+REG_INPUT_NREGS)
static USHORT   usRegInputBuf[REG_INPUT_NREGS] = {0x1000, 0x1001, 0x1002, 0x1003};
eMBErrorCode eMBRegInputCB(UCHAR * pucRegBuffer, USHORT usAddress, USHORT usNRegs)
{
    eMBErrorCode    eStatus = MB_ENOERR;
    int             iRegIndex;

    if ((usAddress >= REG_INPUT_START ) 
    && ((usAddress + usNRegs) <= REG_INPUT_END ) ){
        iRegIndex = ( int )( usAddress - REG_INPUT_START );
        while (usNRegs>0)  {
            *pucRegBuffer++ = ( unsigned char )( usRegInputBuf[iRegIndex] >> 8 );
            *pucRegBuffer++ = ( unsigned char )( usRegInputBuf[iRegIndex] & 0xFF );
            iRegIndex++;
            usNRegs--;
        }
    } else {
        eStatus = MB_ENOREG;
    }

    return eStatus;
}
