//////////////////////////////////////////////////////////////////////////////
//  (c) copyright 2023-by Persional Inc.  
//  All Rights Reserved
//
//  Name:
//      main.c
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
#include "drv_global.h"
#include "mb.h"
#include "mbport.h"

/* ----------------------- Defines ------------------------------------------*/
#define DEBUG_JTAG                  0
#define DEBUG_STLINK                1
#define DEBUG_INTEFACE              DEBUG_STLINK

#define MODBUS_DEF_ADDRESS          0x01        //���豸��ַ
#define MODBUS_DEF_UBAUD            115200      //�豸������
#define MODBUS_DEF_PARITY           MB_PAR_NONE //��żУ��λ

static GlobalType_t driver_initialize(void);
static GlobalType_t system_clock_init(void);
static eMBErrorCode modbus_initialize(void);

extern UCHAR xMBUtilGetBits( UCHAR * ucByteBuf, USHORT usBitOffset, UCHAR ucNBits );
extern void xMBUtilSetBits( UCHAR * ucByteBuf, USHORT usBitOffset, UCHAR ucNBits, UCHAR ucValue );

//main entery function
int main(void)
{    
    HAL_Init();
    
    //system clock tick init.
    system_clock_init();

    //logger init before other device
    logger_module_init();

    //driver initialize.
    driver_initialize();
 
    //modbus initialize.
    modbus_initialize();

    for(;;)
    {
        ( void )eMBPoll(  );
    }    
}

static eMBErrorCode modbus_initialize(void)
{
    eMBErrorCode eStatus;
    
    eStatus = eMBInit( MB_RTU, MODBUS_DEF_ADDRESS, 0, MODBUS_DEF_UBAUD, MODBUS_DEF_PARITY );

    eStatus = eMBEnable(  );
    
    return eStatus;
}


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
#define REG_COIL_NREGS           48         //divisible by 8
static UCHAR   usRegCoilBuf[REG_COIL_NREGS/8] = {0x10, 0xf2, 0x35, 0x00, 0x00, 0x00};
eMBErrorCode eMBRegCoilsCB( UCHAR * pucRegBuffer, USHORT usAddress, USHORT usNCoils, eMBRegisterMode eMode )
{
    UCHAR *pucStartRegBuffer = pucRegBuffer;

    /* check if we away of table size */
    if (usAddress < REG_COIL_START
    || usAddress + usNCoils > REG_COIL_START+REG_COIL_NREGS) {
        return MB_ENOREG;
    } 
    
    //covert to register list
    usAddress -= REG_COIL_START;

    switch (eMode)
    {
        case MB_REG_WRITE:
            while( usNCoils > 0 ) {
                xMBUtilSetBits( usRegCoilBuf, usAddress, (uint8_t)( usNCoils > 8 ? 8 : usNCoils ), *pucStartRegBuffer++ );
                if (usNCoils > 8) {
                    usNCoils -= 8;
                    usAddress += 8;
                } else {
                    break;
                }
            }
            break;
        case MB_REG_READ:
            while( usNCoils > 0 ) {
                *pucStartRegBuffer++ = xMBUtilGetBits(usRegCoilBuf, usAddress, ( uint8_t )( usNCoils > 8 ? 8 : usNCoils ) );
                if (usNCoils > 8) {
                    usNCoils -= 8;
                    usAddress += 8;
                } else {
                    break;
                }
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
#define REG_DISCRETE_NREGS           48         //divisible by 8
static UCHAR   usRegDiscreateBuf[REG_DISCRETE_NREGS/8] = {0x31, 0x15, 0x01, 0x00, 0x01, 0x01};  
eMBErrorCode eMBRegDiscreteCB( UCHAR * pucRegBuffer, USHORT usAddress, USHORT usNDiscrete )
{
    eMBErrorCode eStatus = MB_ENOERR;

    /* check if we away of table size */
    if (usAddress <  REG_COIL_START
    || usAddress + usNDiscrete > REG_DISCRETE_START + REG_DISCRETE_NREGS) {
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
�ӻ���Ӧ: | 01 | 03 | 04 | 0x10 0x00 0x10 0x01 | 32 f3 | => | ���豸��ַ | ������ | ���ݸ��� | �Ĵ��������� | CRCУ�� |

������: 06(0x06), д�뱣�ּĴ���((ÿ���Ĵ�������16bit����)
#define MB_FUNC_WRITE_REGISTER                (  6 )
eMBException eMBFuncWriteHoldingRegister( UCHAR * pucFrame, USHORT * usLen )
RTU����:  | 01 | 06 | 00 00 | 01 0A | 08 5d| => | ���豸��ַ | ������ | ���ּĴ�����ַ | ���ּĴ������� | CRCУ�� |
�ӻ���Ӧ: | 01 | 06 | 00 00 | 01 0A | 08 5d | => | ���豸��ַ | ������ | ���ݸ��� | �Ĵ��������� | CRCУ�� |

*/
#define REG_HOLDING_START           0x0001
#define REG_HOLDING_NREGS           10
static USHORT   usRegHoldingBuf[REG_HOLDING_NREGS] = {0x1000, 0x1001, 0x1002, 0x1003};
eMBErrorCode eMBRegHoldingCB( UCHAR * pucRegBuffer, USHORT usAddress, USHORT usNRegs, eMBRegisterMode eMode )
{
    eMBErrorCode    eStatus = MB_ENOERR;
    int             iRegIndex;

    if (usAddress <  REG_HOLDING_START
    || usAddress + usNRegs > REG_HOLDING_START + REG_HOLDING_NREGS) {
        return MB_ENOREG;
    }
    
    iRegIndex = usAddress - REG_HOLDING_START;
    
    switch (eMode) {                                       
        case MB_REG_READ:
            while (usNRegs > 0) {
                *pucRegBuffer++ = (usRegHoldingBuf[iRegIndex] >> 8);            
                *pucRegBuffer++ = (usRegHoldingBuf[iRegIndex] & 0xFF); 
                iRegIndex++;
                usNRegs--;
            }                            
            break;
        case MB_REG_WRITE:
            while (usNRegs > 0) {         
                usRegHoldingBuf[iRegIndex] = *pucRegBuffer++ << 8;
                usRegHoldingBuf[iRegIndex] |= *pucRegBuffer++;
                iRegIndex++;
                usNRegs--;
            }
            break;
    }

    return eStatus;
}

/*
������: 04, ��ȡ����Ĵ���(ÿ������2�ֽ�)

#define MB_FUNC_READ_INPUT_REGISTER          (  4 )
eMBException eMBFuncReadInputRegister( UCHAR * pucFrame, USHORT * usLen )eMBRegInputCB

RTU����:  | 01 | 04 | 00 00 | 00 02 | 25 3a | => | ���豸��ַ | ������ | ����Ĵ�����ַ | ����Ĵ������� | CRCУ�� |
�ӻ���Ӧ: | 01 | 04 | 04 | 0x10 0x00 0x10 0x01 | 33 44 | => | ���豸��ַ | ������ | ���ݸ��� | �Ĵ��������� | CRCУ�� |
*/
#define REG_INPUT_START           0x0001
#define REG_INPUT_NREGS           10
static USHORT   usRegInputBuf[REG_INPUT_NREGS] = {0x1000, 0x1001, 0x1002, 0x1003};
eMBErrorCode eMBRegInputCB(UCHAR * pucRegBuffer, USHORT usAddress, USHORT usNRegs)
{
    eMBErrorCode    eStatus = MB_ENOERR;
    int             iRegIndex;
    
    if (usAddress <  REG_INPUT_START
    || usAddress + usNRegs > REG_INPUT_START + REG_INPUT_NREGS) {
        return MB_ENOREG;
    }
    
    iRegIndex = usAddress - REG_INPUT_START;
    while (usNRegs>0)  {
        *pucRegBuffer++ = ( unsigned char )( usRegInputBuf[iRegIndex] >> 8 );
        *pucRegBuffer++ = ( unsigned char )( usRegInputBuf[iRegIndex] & 0xFF );
        iRegIndex++;
        usNRegs--;
    }

    return eStatus;
}

static GlobalType_t driver_initialize(void)
{
    GlobalType_t xReturn;
    
    //gpio driver init
    xReturn = gpio_driver_init();
    
    if (xReturn == RT_OK)
    {
        PRINT_LOG(LOG_INFO, HAL_GetTick(), "device driver init success!");
    }
    
    return xReturn;
}

//system clock config
static GlobalType_t system_clock_init(void)
{
    RCC_OscInitTypeDef RCC_OscInitStruct = {0};
    RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

    __HAL_RCC_PWR_CLK_ENABLE();
    __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE1);
    

    RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_LSI|RCC_OSCILLATORTYPE_HSE
                                    |RCC_OSCILLATORTYPE_LSE;
    
    //RCC_HSE_BYPASS: OSC with source, one line out clock
    //RCC_HSE_ON: OSC without source, two line by microchip
    RCC_OscInitStruct.HSEState = RCC_HSE_ON;    
    RCC_OscInitStruct.LSEState = RCC_LSE_ON;
    RCC_OscInitStruct.LSIState = RCC_LSI_ON;
    RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
    RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
    
    //SYSCLK HSECLK/PLLM*PLLN/PLLP=25Mhz/25*360/2=180Mhz
    //PLLQCLK for I2S = 45M
    RCC_OscInitStruct.PLL.PLLM = 25;
    RCC_OscInitStruct.PLL.PLLN = 360;
    RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV2;
    RCC_OscInitStruct.PLL.PLLQ = 8;
    if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
    {
        return RT_FAIL;
    }

    if (HAL_PWREx_EnableOverDrive() != HAL_OK)
    {
        return RT_FAIL;
    }

    //AHBCLK    = SYSCLK    =180Mhz
    //APB1CLK   = AHBCLK/4  =45M 
    //APB1TIME  = APB1CLK*2 =90M 
    //APB2CLK   = AHBCLK/2  =90M
    //APB2TIME  =APB2*2     =180M   
    RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                            |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
    RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
    RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
    RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV4;
    RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV2;

    if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_5) != HAL_OK)
    {
        return RT_FAIL;
    }
    return RT_OK;    
}
