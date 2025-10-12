//////////////////////////////////////////////////////////////////////////////
//  (c) copyright 2023-by Persional Inc.  
//  All Rights Reserved
//
//  Name:
//      driver_dcmi.c
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
#include "driver_dcmi.h"
#include "drv_lcd.h"

static DCMI_HandleTypeDef hdcmi;
static DMA_HandleTypeDef hdma_dcmi;

GlobalType_t dcmi_driver_init(void)
{
    hdcmi.Instance = DCMI;
    hdcmi.Init.SynchroMode = DCMI_SYNCHRO_HARDWARE;     //Ӳ��ͬ��HSYNC,VSYNC
    hdcmi.Init.PCKPolarity = DCMI_PCKPOLARITY_RISING;   //PCLK ��������Ч
    hdcmi.Init.VSPolarity = DCMI_VSPOLARITY_LOW;        //VSYNC �͵�ƽ��Ч
    hdcmi.Init.HSPolarity = DCMI_HSPOLARITY_LOW;        //HSYNC �͵�ƽ��Ч
    hdcmi.Init.CaptureRate = DCMI_CR_ALL_FRAME;         //ȫ֡����
    hdcmi.Init.ExtendedDataMode = DCMI_EXTEND_DATA_8B;  //8λ���ݸ�ʽ
    hdcmi.Init.JPEGMode = DCMI_JPEG_DISABLE;
    HAL_DCMI_Init(&hdcmi);

    __HAL_RCC_DMA2_CLK_ENABLE();                       //ʹ��DMA2ʱ��
    __HAL_LINKDMA(&hdcmi, DMA_Handle, hdma_dcmi);       
	
    hdma_dcmi.Instance = DMA2_Stream1;                                
    hdma_dcmi.Init.Channel = DMA_CHANNEL_1;                 
    hdma_dcmi.Init.Direction = DMA_PERIPH_TO_MEMORY;           
    hdma_dcmi.Init.PeriphInc = DMA_PINC_DISABLE;              
    hdma_dcmi.Init.MemInc = DMA_MINC_DISABLE;              
    hdma_dcmi.Init.PeriphDataAlignment = DMA_PDATAALIGN_WORD;    
    hdma_dcmi.Init.MemDataAlignment = DMA_MDATAALIGN_HALFWORD;
    hdma_dcmi.Init.Mode = DMA_CIRCULAR;                    
    hdma_dcmi.Init.Priority = DMA_PRIORITY_HIGH;
    hdma_dcmi.Init.FIFOMode = DMA_FIFOMODE_ENABLE;              //ʹ��FIFO
    hdma_dcmi.Init.FIFOThreshold = DMA_FIFO_THRESHOLD_HALFFULL; //ʹ��1/2��FIFO 
    hdma_dcmi.Init.MemBurst = DMA_MBURST_SINGLE;            
    hdma_dcmi.Init.PeriphBurst = DMA_PBURST_SINGLE;
    HAL_DMA_DeInit(&hdma_dcmi);                                 //�������ǰ������
    HAL_DMA_Init(&hdma_dcmi);	                                //��ʼ��DMA

    __HAL_UNLOCK(&hdma_dcmi);
    HAL_DMA_Start(&hdma_dcmi,(uint32_t)&DCMI->DR, (uint32_t)&(LCD->LCD_RAM), 1);
    
    return RT_OK;
}

//DCMI����
void dcmi_start(void)
{  
    lcd_set_cursor(0, 0);  
	lcd_write_ram_prepare();		        //��ʼд��GRAM
    
    __HAL_DMA_ENABLE(&hdma_dcmi);           //ʹ��DMA
    __HAL_DCMI_ENABLE(&hdcmi);

    HAL_Delay(100);
    DCMI->CR |= DCMI_CR_CAPTURE;            //DCMI����ʹ��
}

//�رմ���
void dcmi_stop(void)
{ 
    DCMI->CR &=~(DCMI_CR_CAPTURE);
    
    while (DCMI->CR&0x01) {
         HAL_Delay(1);
    }
    __HAL_DMA_DISABLE(&hdma_dcmi);
} 

//DCMI�жϷ�����
void DCMI_IRQHandler(void)
{    
    HAL_DCMI_IRQHandler(&hdcmi);
}

void HAL_DCMI_ErrorCallback(DCMI_HandleTypeDef *hdcmi)
{
    lcd_set_cursor(0, 0);  
    lcd_write_ram_prepare();		        //��ʼд��GRAM
    __HAL_DMA_ENABLE(&hdma_dcmi);   
}

//����һ֡ͼ������
void HAL_DCMI_FrameEventCallback(DCMI_HandleTypeDef *hdcmi)
{
    //����ʹ��֡�ж�,��ΪHAL_DCMI_IRQHandler()������ر�֡�ж�
    __HAL_DCMI_ENABLE_IT(hdcmi, DCMI_IT_FRAME);  
}

void HAL_DCMI_MspInit(DCMI_HandleTypeDef* hdcmi)
{
    GPIO_InitTypeDef GPIO_Initure;
    
    __HAL_RCC_DCMI_CLK_ENABLE();
    __HAL_RCC_GPIOA_CLK_ENABLE();
    __HAL_RCC_GPIOB_CLK_ENABLE();
    __HAL_RCC_GPIOC_CLK_ENABLE();
    __HAL_RCC_GPIOD_CLK_ENABLE();
    __HAL_RCC_GPIOH_CLK_ENABLE(); 
    
    //PA6
    GPIO_Initure.Pin = GPIO_PIN_6;  
    GPIO_Initure.Mode =GPIO_MODE_AF_PP;
    GPIO_Initure.Pull =GPIO_PULLUP;
    GPIO_Initure.Speed =GPIO_SPEED_HIGH;
    GPIO_Initure.Alternate =GPIO_AF13_DCMI;
    HAL_GPIO_Init(GPIOA, &GPIO_Initure);
    
    //PB7,8,9
    GPIO_Initure.Pin=GPIO_PIN_7|GPIO_PIN_8|GPIO_PIN_9;  
    HAL_GPIO_Init(GPIOB,&GPIO_Initure);
    
    //PC6,7,8,9,11
    GPIO_Initure.Pin=GPIO_PIN_6|GPIO_PIN_7|GPIO_PIN_8|\
                     GPIO_PIN_9|GPIO_PIN_11;  
    HAL_GPIO_Init(GPIOC,&GPIO_Initure);
    
    //PD3
    GPIO_Initure.Pin=GPIO_PIN_3; 
    HAL_GPIO_Init(GPIOD,&GPIO_Initure);
    
    //PH8
    GPIO_Initure.Pin=GPIO_PIN_8; 
    HAL_GPIO_Init(GPIOH,&GPIO_Initure);
    
    HAL_NVIC_SetPriority(DCMI_IRQn,0,0);
    HAL_NVIC_EnableIRQ(DCMI_IRQn);
}
