//////////////////////////////////////////////////////////////////////////////
//  (c) copyright 2023-by Persional Inc.  
//  All Rights Reserved
//
//  Name:
//      drv_usart.c
//
//  Purpose:
//      driver for usart.
//      usart hw: PA2, PA3 USART2
//      support:
//          normal
//          dma rx/tx
//          interrupt, see usr_library/logger.c
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
#include <string.h>
#include "drv_usart.h"

#define TEST_USART  "test for usart run!\n"

#if USART_RUN_MODE == RUN_MODE_NORMAL
//local variable
static UART_HandleTypeDef huart2;

//local function
static uint16_t UART_ReceiveData(UART_HandleTypeDef* huart);
static void UART_SendData(UART_HandleTypeDef* huart, uint16_t Data);

//usart interface
void usart_translate(char *ptr, uint16_t size)
{
    uint16_t index;
    for (index=0; index<size; index++)
    {
      while (__HAL_UART_GET_FLAG(&huart2, UART_FLAG_TXE) == RESET)
      {}
      
      UART_SendData(&huart2, ptr[index]);
    }
}

void usart_loop_run(void)
{
    uint16_t rx_data;
    
    if (__HAL_UART_GET_FLAG(&huart2, UART_FLAG_RXNE) != RESET)
    {
        rx_data = UART_ReceiveData(&huart2);

        while (__HAL_UART_GET_FLAG(&huart2, UART_FLAG_TXE) == RESET)
        {}

        UART_SendData(&huart2, rx_data);
    }
}

GlobalType_t drv_usart_init(void)
{
    GPIO_InitTypeDef GPIO_InitStruct = {0};

    __HAL_RCC_USART2_CLK_ENABLE();
    __HAL_RCC_GPIOA_CLK_ENABLE();

    GPIO_InitStruct.Pin = GPIO_PIN_2 | GPIO_PIN_3;
    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
    GPIO_InitStruct.Alternate = GPIO_AF7_USART2;
    HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

    huart2.Instance = USART2;
    huart2.Init.BaudRate = 115200;
    huart2.Init.WordLength = UART_WORDLENGTH_8B;
    huart2.Init.StopBits = UART_STOPBITS_1;
    huart2.Init.Parity = UART_PARITY_NONE;
    huart2.Init.Mode = UART_MODE_TX_RX;
    huart2.Init.HwFlowCtl = UART_HWCONTROL_NONE;
    huart2.Init.OverSampling = UART_OVERSAMPLING_16;
    if (HAL_UART_Init(&huart2) != HAL_OK)
    {
        return RT_FAIL;
    }
    
    PRINT_LOG(LOG_INFO, HAL_GetTick(), "usart init success!");
    
    usart_translate(TEST_USART, strlen(TEST_USART));
    return RT_OK;
}

static uint16_t UART_ReceiveData(UART_HandleTypeDef* huart)
{
    uint16_t rx_data;

    rx_data = huart->Instance->DR&0x1FF;

    return rx_data;
}

static void UART_SendData(UART_HandleTypeDef* huart, uint16_t Data)
{
    huart->Instance->DR = Data&0x1FF;
}
#elif USART_RUN_MODE == RUN_MODE_DMA

//local defined
#define DMA_BUFFER_SIZE     256
typedef struct
{
    uint8_t rx_size;
    
    uint8_t is_rx_ready;
    
    char dma_rx_buffer[DMA_BUFFER_SIZE];
    
    char dma_tx_buffer[DMA_BUFFER_SIZE];   
}USART_DMA_INFO;

//local variable
static UART_HandleTypeDef huart2;
static DMA_HandleTypeDef hdma_usart2_rx;
static DMA_HandleTypeDef hdma_usart2_tx;
static USART_DMA_INFO g_usart_dma_info = {0};

//local function
static void usart_receive(char *ptr, uint16_t size);
static void usart_translate(char *ptr, uint16_t size);

GlobalType_t drv_usart_init(void)
{
    GPIO_InitTypeDef GPIO_InitStruct = {0};

    __HAL_RCC_USART2_CLK_ENABLE();
    __HAL_RCC_GPIOA_CLK_ENABLE();
    __HAL_RCC_DMA1_CLK_ENABLE();
    
    GPIO_InitStruct.Pin = GPIO_PIN_2 | GPIO_PIN_3;
    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
    GPIO_InitStruct.Alternate = GPIO_AF7_USART2;
    HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);
    
    huart2.Instance = USART2;
    huart2.Init.BaudRate = 115200;
    huart2.Init.WordLength = UART_WORDLENGTH_8B;
    huart2.Init.StopBits = UART_STOPBITS_1;
    huart2.Init.Parity = UART_PARITY_NONE;
    huart2.Init.Mode = UART_MODE_TX_RX;
    huart2.Init.HwFlowCtl = UART_HWCONTROL_NONE;
    huart2.Init.OverSampling = UART_OVERSAMPLING_16;
    if (HAL_UART_Init(&huart2) != HAL_OK)
        return RT_FAIL;
    
    //enable uart idle interrupt
    __HAL_UART_ENABLE_IT(&huart2, UART_IT_IDLE);
    HAL_NVIC_EnableIRQ(USART2_IRQn);			
    HAL_NVIC_SetPriority(USART2_IRQn, 0, 1);	
    
    ATOMIC_SET_BIT(huart2.Instance->CR3, USART_CR3_DMAT);
    ATOMIC_SET_BIT(huart2.Instance->CR3, USART_CR3_DMAR);
           
    //update dma communication
    hdma_usart2_rx.Instance = DMA1_Stream5;
    hdma_usart2_rx.Init.Channel = DMA_CHANNEL_4;
    hdma_usart2_rx.Init.Direction = DMA_PERIPH_TO_MEMORY;
    hdma_usart2_rx.Init.PeriphInc = DMA_PINC_DISABLE;
    hdma_usart2_rx.Init.MemInc = DMA_MINC_ENABLE;
    hdma_usart2_rx.Init.PeriphDataAlignment = DMA_PDATAALIGN_BYTE;
    hdma_usart2_rx.Init.MemDataAlignment = DMA_MDATAALIGN_BYTE;
    hdma_usart2_rx.Init.Mode = DMA_NORMAL;
    hdma_usart2_rx.Init.Priority = DMA_PRIORITY_MEDIUM;
    hdma_usart2_rx.Init.FIFOMode = DMA_FIFOMODE_ENABLE;
    hdma_usart2_rx.Init.FIFOThreshold = DMA_FIFO_THRESHOLD_FULL;
    hdma_usart2_rx.Init.MemBurst = DMA_MBURST_SINGLE;
    hdma_usart2_rx.Init.PeriphBurst = DMA_PBURST_SINGLE;
    if (HAL_DMA_Init(&hdma_usart2_rx) != HAL_OK)
    {
      return RT_FAIL;
    }
    __HAL_LINKDMA(&huart2, hdmarx, hdma_usart2_rx);
    
    hdma_usart2_tx.Instance = DMA1_Stream6;
    hdma_usart2_tx.Init.Channel = DMA_CHANNEL_4;
    hdma_usart2_tx.Init.Direction = DMA_MEMORY_TO_PERIPH;
    hdma_usart2_tx.Init.PeriphInc = DMA_PINC_DISABLE;
    hdma_usart2_tx.Init.MemInc = DMA_MINC_ENABLE;
    hdma_usart2_tx.Init.PeriphDataAlignment = DMA_PDATAALIGN_BYTE;
    hdma_usart2_tx.Init.MemDataAlignment = DMA_MDATAALIGN_BYTE;
    hdma_usart2_tx.Init.Mode = DMA_NORMAL;
    hdma_usart2_tx.Init.Priority = DMA_PRIORITY_LOW;
    hdma_usart2_tx.Init.FIFOMode = DMA_FIFOMODE_ENABLE;
    hdma_usart2_tx.Init.FIFOThreshold = DMA_FIFO_THRESHOLD_FULL;
    hdma_usart2_tx.Init.MemBurst = DMA_MBURST_SINGLE;
    hdma_usart2_tx.Init.PeriphBurst = DMA_PBURST_SINGLE;
    if (HAL_DMA_Init(&hdma_usart2_tx) != HAL_OK)
    {
      return RT_FAIL;
    }
    __HAL_LINKDMA(&huart2, hdmatx, hdma_usart2_tx);
    
    //enable dma tc interrupt
    __HAL_DMA_ENABLE_IT(&hdma_usart2_tx, DMA_IT_TC);
    HAL_NVIC_EnableIRQ(DMA2_Stream7_IRQn);			
    HAL_NVIC_SetPriority(DMA2_Stream7_IRQn, 0, 1);	 
    
    //enable dma rx
    usart_translate(TEST_USART, strlen(TEST_USART));
    usart_receive(g_usart_dma_info.dma_rx_buffer, DMA_BUFFER_SIZE);
    return RT_OK;
}

static char data[DMA_BUFFER_SIZE];
void usart_loop_run(void)
{
    if (g_usart_dma_info.is_rx_ready == 1)
    {
        memcpy(data, g_usart_dma_info.dma_rx_buffer, g_usart_dma_info.rx_size);
        memset(g_usart_dma_info.dma_rx_buffer, 0, DMA_BUFFER_SIZE);
        g_usart_dma_info.is_rx_ready = 0;
        
        //start rx
        usart_receive(g_usart_dma_info.dma_tx_buffer, DMA_BUFFER_SIZE);
        
        //start tx
        usart_translate(data, g_usart_dma_info.rx_size);
    }
}

static void usart_translate(char *ptr, uint16_t size)
{
    memcpy((char *)g_usart_dma_info.dma_tx_buffer, ptr, size);

    //clear the flag related to translate
    __HAL_DMA_CLEAR_FLAG(&hdma_usart2_tx, DMA_FLAG_TCIF2_6);
    __HAL_DMA_CLEAR_FLAG(&hdma_usart2_tx, DMA_FLAG_TCIF2_6);
    __HAL_UART_CLEAR_FLAG(&huart2, UART_FLAG_TC);

    //enable dma tc interrupt
    __HAL_DMA_ENABLE_IT(&hdma_usart2_tx, DMA_IT_TC);

    //start dma translate
    HAL_DMA_Start(&hdma_usart2_tx, (uint32_t)g_usart_dma_info.dma_tx_buffer, (uint32_t)&huart2.Instance->DR, size);
}

static void usart_receive(char *ptr, uint16_t size)
{
    //clear dma rx flag
    __HAL_DMA_CLEAR_FLAG(&hdma_usart2_rx, DMA_FLAG_TCIF1_5);
    __HAL_DMA_CLEAR_FLAG(&hdma_usart2_rx, DMA_FLAG_TCIF1_5);

    //start the next data receive
    HAL_DMA_Start(&hdma_usart2_rx, (uint32_t)&huart2.Instance->DR, (uint32_t)ptr, DMA_BUFFER_SIZE);
}

void DMA1_Stream6_IRQHandler(void)
{
    if (__HAL_DMA_GET_FLAG(&hdma_usart2_tx, DMA_FLAG_TCIF3_7) != RESET)
    {      
        //close the dma and all flags, also interrupt
        //need enable next
        HAL_DMA_Abort(&hdma_usart2_tx); 

        //wait for usart truth send, then clear flags
        //rs485 mode need change the i/o until the tc is set
        //
        while (__HAL_UART_GET_FLAG(&huart2, UART_FLAG_TC) == RESET)
        {
        }
        __HAL_UART_CLEAR_PEFLAG(&huart2);
        __HAL_UART_CLEAR_FLAG(&huart2, UART_FLAG_TC);        
    }
}

void USART2_IRQHandler(void)
{
    if (__HAL_UART_GET_FLAG(&huart2, UART_FLAG_IDLE) != RESET)
    {
      __HAL_UART_CLEAR_PEFLAG(&huart2);
      
      HAL_DMA_Abort(&hdma_usart2_rx);
      
      g_usart_dma_info.is_rx_ready = 1;
      g_usart_dma_info.rx_size = DMA_BUFFER_SIZE - __HAL_DMA_GET_COUNTER(&hdma_usart2_rx);
    }
}

#endif

