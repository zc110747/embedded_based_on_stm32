//////////////////////////////////////////////////////////////////////////////
//  (c) copyright 2023-by Persional Inc.  
//  All Rights Reserved
//
//  Name:
//      drv_soft_i2c.c
//
//  Purpose:
//      1.driver default run with stm32 HAL library, other need update region below.
//      2.system systick change, can debug 'I2C_DELAY_COUNT' to optimal performance.
//      3.soft i2c interface not protect the time sequence, if used, need disable irq
//        to ensure proper operation, use soft-i2c with multi-thread, must enter
//        critical zone.
//      
//
// Author:
//      @zc
//
//  Assumptions:
//
//  Revision History:
//
/////////////////////////////////////////////////////////////////////////////
#include <string.h>
#include "drv_soft_i2c.h"

static SOFT_I2C_INFO i2c_list[SOFT_I2C_NUM] = {0};
static uint8_t is_i2c_init[SOFT_I2C_NUM] = {0};

///////////////////////////////////////////////////////////////////////////////////////////////////////////
//region need update if not use stm32 hal library
//     I2C_SCL_H(), I2C_SCL_L(), I2C_SDA_H(), I2C_SDA_L(), I2C_SDA_INPUT()
//     i2c_soft_init(), i2c_sda_config_in(), i2c_sda_config_out().
///////////////////////////////////////////////////////////////////////////////////////////////////////////
#define I2C_SCL_H()     HAL_GPIO_WritePin(i2c_info_ptr->scl_port, i2c_info_ptr->scl_pin, GPIO_PIN_SET)
#define I2C_SCL_L()     HAL_GPIO_WritePin(i2c_info_ptr->scl_port, i2c_info_ptr->scl_pin, GPIO_PIN_RESET)
#define I2C_SDA_H()     HAL_GPIO_WritePin(i2c_info_ptr->sda_port, i2c_info_ptr->sda_pin, GPIO_PIN_SET)
#define I2C_SDA_L()     HAL_GPIO_WritePin(i2c_info_ptr->sda_port, i2c_info_ptr->sda_pin, GPIO_PIN_RESET)
#define I2C_SDA_INPUT() (HAL_GPIO_ReadPin(i2c_info_ptr->sda_port, i2c_info_ptr->sda_pin) == GPIO_PIN_SET)

static void i2c_sda_config_in(SOFT_I2C_INFO *i2c_info_ptr)
{
    GPIO_InitTypeDef GPIO_InitStruct;  

    GPIO_InitStruct.Pin = i2c_info_ptr->sda_pin;
    GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
    HAL_GPIO_Init(i2c_info_ptr->sda_port, &GPIO_InitStruct);
}

static void i2c_sda_config_out(SOFT_I2C_INFO *i2c_info_ptr)
{
    GPIO_InitTypeDef GPIO_InitStruct;  

    GPIO_InitStruct.Pin = i2c_info_ptr->sda_pin;
    GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_OD;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
    HAL_GPIO_Init(i2c_info_ptr->sda_port, &GPIO_InitStruct);  
}

uint8_t i2c_soft_init(uint8_t soft_i2c_num, SOFT_I2C_INFO *info_ptr)
{
    GPIO_InitTypeDef GPIO_InitStruct = {0};
    SOFT_I2C_INFO *i2c_info_ptr;
    
    if(soft_i2c_num >= SOFT_I2C_NUM)
        return I2C_ERROR;
    
    if(is_i2c_init[soft_i2c_num])
    {
        //i2c already init, use same pin
        //support i2c with mutliple external device.
        if(memcmp((char *)info_ptr, (char *)&i2c_list[soft_i2c_num], sizeof(SOFT_I2C_INFO)) == 0)
        {
            return I2C_OK;
        }
        
        return I2C_ERROR;
    }
    i2c_info_ptr = &i2c_list[soft_i2c_num];
    
    is_i2c_init[soft_i2c_num] = 1;
    memcpy((char *)i2c_info_ptr, (char *)info_ptr, sizeof(SOFT_I2C_INFO));
    
    I2C_SCL_H();
    I2C_SDA_H();
    
    GPIO_InitStruct.Pin = i2c_info_ptr->scl_pin;
    GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_OD;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
#ifdef GPIO_SPEED_FREQ_VERY_HIGH
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
#else
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
#endif
    HAL_GPIO_Init(i2c_info_ptr->scl_port, &GPIO_InitStruct);

    GPIO_InitStruct.Pin = i2c_info_ptr->sda_pin;
    HAL_GPIO_Init(i2c_info_ptr->sda_port, &GPIO_InitStruct);
    
    return I2C_OK;
}

#if I2C_EXAMPLE_STM32F4_PCF8574 == 1
#define PCF8574_ADDR 	        0x40
void pcf8574_dev_example(void)
{
    uint8_t data, res;
    SOFT_I2C_INFO I2C_Info = {0};

    //clock need enable before software i2c Init
    __HAL_RCC_GPIOH_CLK_ENABLE(); 

    I2C_Info.scl_pin = GPIO_PIN_4;
    I2C_Info.scl_port = GPIOH;
    I2C_Info.sda_pin = GPIO_PIN_4;
    I2C_Info.sda_port = GPIOH;

    i2c_soft_init(SOFT_I2C1, &I2C_Info);
    
    //if used in application, need disable irq
    res = i2c_read_device(SOFT_I2C1, PCF8574_ADDR, &data, 1);
    if(res != I2C_OK)
    {
        i2c_error_handle();
    }
    
    res = i2c_write_device(SOFT_I2C1, PCF8574_ADDR, &data, 1);
    if(res != I2C_OK)
    {
        i2c_error_handle();
    }   
}
#endif

#if I2C_EXAMPLE_STM32F1_BQ24780S == 1
#include <string.h>
#define BQ24780S_SLAVE_ADDRESS      0x12
#define BQ24780S_CHARGE_OPTION_0    0x12
void bq24780s_memory_example(void)
{
    uint8_t wdata[2] = {0};
    uint8_t rdata[2] = {0};
    uint8_t res;
    SOFT_I2C_INFO I2C_Info = {0};
    
    //clock need enable before sd disable irq
    __HAL_RCC_GPIOB_CLK_ENABLE(); 

    I2C_Info.scl_pin = GPIO_PIN_8;
    I2C_Info.scl_port = GPIOB;
    I2C_Info.sda_pin = GPIO_PIN_9;
    I2C_Info.sda_port = GPIOB;
    
    i2c_soft_init(SOFT_I2C3, &I2C_Info); 
    
    //if used in application, need disable irq
    wdata[0] = 0x08;
    wdata[1] = 0x80;
    res = i2c_write_memory(SOFT_I2C3, BQ24780S_SLAVE_ADDRESS, BQ24780S_CHARGE_OPTION_0, 1, wdata, 2);
    if(res != I2C_OK)
    {
        i2c_error_handle();
    }
    
    res = i2c_read_memory(SOFT_I2C3, BQ24780S_SLAVE_ADDRESS, BQ24780S_CHARGE_OPTION_0, 1, rdata, 2);
    if(res != I2C_OK)
    {
        i2c_error_handle();
    }
    
    if(memcmp(wdata, rdata, 2) != 0)
    {
        i2c_error_handle();
    }
}
#endif
///////////////////////////////////////////////////////////////////////////////////////////////////////////
//end of the region
///////////////////////////////////////////////////////////////////////////////////////////////////////////

static void i2c_delay(uint32_t count)
{
    unsigned int i, j;

    for(i=0; i<count; i++)
    {
        for(j=0; j<3; j++)
        {
        }
    }    
}

static uint8_t i2c_idle_check(SOFT_I2C_INFO *i2c_info_ptr)
{
    uint16_t wait_time = 0;
    i2c_sda_config_in(i2c_info_ptr);
    
    while (I2C_SDA_INPUT() != 1)
    {
        wait_time++;
        if(wait_time > 10000)
        {
            return I2C_ERROR;
        }
    }
    return I2C_OK;
}

static void i2c_start(SOFT_I2C_INFO *i2c_info_ptr)
{
    i2c_sda_config_out(i2c_info_ptr);

    I2C_SDA_H();
    i2c_delay(I2C_DELAY_COUNT);
    I2C_SCL_H();
    i2c_delay(I2C_DELAY_COUNT);
    I2C_SDA_L();
    i2c_delay(I2C_DELAY_COUNT);
    I2C_SCL_L();               
}

static void i2c_send_byte(SOFT_I2C_INFO *i2c_info_ptr, uint8_t byte)
{
    uint8_t index;

    i2c_sda_config_out(i2c_info_ptr);
    
    for(index=0; index<8; index++)
    {
        i2c_delay(I2C_DELAY_COUNT);
        if(byte & 0x80)
        {
            I2C_SDA_H();
        }
        else
        {
            I2C_SDA_L();
        }
        byte <<= 1;
        i2c_delay(I2C_DELAY_COUNT);
        I2C_SCL_H();
        i2c_delay(I2C_DELAY_COUNT);
        I2C_SCL_L();   
    }
}

static void i2c_send_ack(SOFT_I2C_INFO *i2c_info_ptr)
{
    i2c_sda_config_out(i2c_info_ptr);
    i2c_delay(I2C_DELAY_COUNT);
    I2C_SDA_L();
    i2c_delay(I2C_DELAY_COUNT);
    I2C_SCL_H();    
    i2c_delay(I2C_DELAY_COUNT);
    I2C_SCL_L();
}

static void i2c_send_nack(SOFT_I2C_INFO *i2c_info_ptr)
{
    i2c_sda_config_out(i2c_info_ptr);
    i2c_delay(I2C_DELAY_COUNT);
    I2C_SDA_H();
    i2c_delay(I2C_DELAY_COUNT);
    I2C_SCL_H();    
    i2c_delay(I2C_DELAY_COUNT);
    I2C_SCL_L();
}

static uint8_t i2c_read_byte(SOFT_I2C_INFO *i2c_info_ptr, uint8_t ack_status)
{
    uint8_t index, value = 0;

    i2c_sda_config_in(i2c_info_ptr);
    for(index=0; index<8; index++)
    {
        i2c_delay(I2C_DELAY_COUNT);
        I2C_SCL_H();
        value |= I2C_SDA_INPUT();
        if(index<7)
        {
            value <<= 1;
        }
        i2c_delay(I2C_DELAY_COUNT);
        I2C_SCL_L();
    }
    
    if(!ack_status)
        i2c_send_nack(i2c_info_ptr);
    else
        i2c_send_ack(i2c_info_ptr);
    
    return value;
}

static uint8_t i2c_wait_ack(SOFT_I2C_INFO *i2c_info_ptr)
{
    uint16_t wait_time = 0;

    i2c_sda_config_in(i2c_info_ptr);
    i2c_delay(I2C_DELAY_COUNT);

    while (I2C_SDA_INPUT())
    {
        wait_time++;
        if(wait_time > 10000)
        {
            return I2C_ERROR;
        }
    }
    
    //master read ack.
    I2C_SCL_H();
    i2c_delay(I2C_DELAY_COUNT);
    I2C_SCL_L();   
    return I2C_OK;
}

static void i2c_stop(SOFT_I2C_INFO *i2c_info_ptr)
{
    i2c_sda_config_out(i2c_info_ptr);
    I2C_SDA_L();
    i2c_delay(I2C_DELAY_COUNT);	
	I2C_SCL_H();
    i2c_delay(I2C_DELAY_COUNT);
	I2C_SDA_H();  
    i2c_delay(I2C_DELAY_COUNT);
	//I2C_SCL_L(); 
}

uint8_t i2c_soft_deinit(uint8_t soft_i2c_num)
{
    if(soft_i2c_num >= SOFT_I2C_NUM)
        return I2C_ERROR; 
    
    is_i2c_init[soft_i2c_num] = 0;
    return I2C_OK;
}

uint8_t i2c_write_device(uint8_t soft_i2c_num, uint8_t addr, uint8_t *data, uint16_t size)
{
    uint16_t index;
    SOFT_I2C_INFO *i2c_info_ptr;
    
    if(soft_i2c_num >= SOFT_I2C_NUM)
    {
        return I2C_ERROR;
    }
    
    if(!is_i2c_init[soft_i2c_num])
    {
        return I2C_ERROR;
    }
    
    i2c_info_ptr = &i2c_list[soft_i2c_num];
   
     //0.i2c idle check must high.
    if(i2c_idle_check(i2c_info_ptr))
    {
       return I2C_ERROR; 
    }
    
    //1. send start
    i2c_start(i2c_info_ptr);

    //2. send the address
    i2c_send_byte(i2c_info_ptr, addr&0xfe);  //the bit0 is w, must 0
    if(i2c_wait_ack(i2c_info_ptr))
    {
        i2c_stop(i2c_info_ptr);
        return I2C_ERROR;
    }

    //3. send the data
    for(index=0; index<size; index++)
    {
        i2c_send_byte(i2c_info_ptr, data[index]);
        if(i2c_wait_ack(i2c_info_ptr))
        {
            i2c_stop(i2c_info_ptr);
            return I2C_ERROR;
        }
    }
    
    //4. send the stop
    i2c_stop(i2c_info_ptr);

    i2c_delay(I2C_DELAY_COUNT);
    return I2C_OK;     
}

uint8_t i2c_read_device(uint8_t soft_i2c_num, uint8_t addr, uint8_t *rdata, uint16_t size)
{
    uint16_t index;
    SOFT_I2C_INFO *i2c_info_ptr;
    
    if(soft_i2c_num >= SOFT_I2C_NUM)
    {
        return I2C_ERROR;
    }
    
    if(!is_i2c_init[soft_i2c_num])
    {
        return I2C_ERROR;
    }
    
    i2c_info_ptr = &i2c_list[soft_i2c_num];
    
    //0.i2c idle check must high.
    if(i2c_idle_check(i2c_info_ptr))
    {
       return I2C_ERROR; 
    }
  
    //1. send start
    i2c_start(i2c_info_ptr);

    //2. send the address
    i2c_send_byte(i2c_info_ptr, addr|0x01);  //the bit0 is r, must 1
    if(i2c_wait_ack(i2c_info_ptr))
    {
        i2c_stop(i2c_info_ptr);
        return I2C_ERROR;
    }

    //3. read the data
    for(index=0; index<size; index++)
    {
        if(index != size-1)
        {
            rdata[index] = i2c_read_byte(i2c_info_ptr, 1);
        }
        else
        {
            rdata[index] = i2c_read_byte(i2c_info_ptr, 0);
        }
    }

    //4. send the stop
    i2c_stop(i2c_info_ptr);
    return I2C_OK;     
}

uint8_t i2c_write_memory(uint8_t soft_i2c_num, uint8_t addr, uint32_t mem_addr, uint8_t mem_size, uint8_t *data, uint16_t size)
{
    uint16_t index;
    SOFT_I2C_INFO *i2c_info_ptr;
    
    if(soft_i2c_num >= SOFT_I2C_NUM)
    {
        return I2C_ERROR;
    }
    
    if(!is_i2c_init[soft_i2c_num])
    {
        return I2C_ERROR;
    }
    
    i2c_info_ptr = &i2c_list[soft_i2c_num];
    
    //0.i2c idle check must high.
    if(i2c_idle_check(i2c_info_ptr))
    {
       return I2C_ERROR; 
    }
    
    //1. send start
    i2c_start(i2c_info_ptr); 
    
    //2. send the address
    i2c_send_byte(i2c_info_ptr, addr&0xfe);  //the bit0 is w, must 0   
    if(i2c_wait_ack(i2c_info_ptr))
    {
        i2c_stop(i2c_info_ptr);
        return I2C_ERROR;
    }

    //3. send the memory address(memory addr can be 2/4byte for epprom)
    for(index=0; index<mem_size; index++)
    {
        uint8_t mem = (mem_addr>>((mem_size - index - 1)*8))&0xff; //high byte first
        i2c_send_byte(i2c_info_ptr, mem);  
        if(i2c_wait_ack(i2c_info_ptr))
        {
            i2c_stop(i2c_info_ptr);
            return I2C_ERROR;
        }
    }
    
    //3. send the data
    for(index=0; index<size; index++)
    {
        i2c_send_byte(i2c_info_ptr, data[index]);
        if(i2c_wait_ack(i2c_info_ptr))
        {
            i2c_stop(i2c_info_ptr);
            return I2C_ERROR;
        }
    }
    
    //4. send the stop
    i2c_stop(i2c_info_ptr);

    i2c_delay(I2C_DELAY_COUNT);
    return I2C_OK;
}

uint8_t i2c_read_memory(uint8_t soft_i2c_num, uint8_t addr, uint32_t mem_addr, uint8_t mem_size, uint8_t *rdata, uint16_t size)
{
    SOFT_I2C_INFO *i2c_info_ptr;
    uint16_t index;
    
    if(soft_i2c_num >= SOFT_I2C_NUM)
    {
        return I2C_ERROR;
    }
    
    if(!is_i2c_init[soft_i2c_num])
    {
        return I2C_ERROR;
    }
    
    i2c_info_ptr = &i2c_list[soft_i2c_num];
    
    //0.i2c idle check must high.
    if(i2c_idle_check(i2c_info_ptr))
    {
       return I2C_ERROR; 
    }
    
    //1. send start
    i2c_start(i2c_info_ptr);    
    
    //2. send the address
    i2c_send_byte(i2c_info_ptr, addr&0xfe);  //the bit0 is w, must 0   
    if(i2c_wait_ack(i2c_info_ptr))
    {
        i2c_stop(i2c_info_ptr);
        return I2C_ERROR;
    }
    
    //3. send the memory address(memory addr can be 2/4byte for epprom)
    for(index=0; index<mem_size; index++)
    {
        uint8_t mem = (mem_addr>>((mem_size - index - 1)*8))&0xff;  //high byte first
        i2c_send_byte(i2c_info_ptr, mem);  
        if(i2c_wait_ack(i2c_info_ptr))
        {
            i2c_stop(i2c_info_ptr);
            return I2C_ERROR;
        }
    }
    
    //4. send restart
    i2c_start(i2c_info_ptr);  
    
    //5. send the address with read
    i2c_send_byte(i2c_info_ptr, addr | 0x01);  
    if(i2c_wait_ack(i2c_info_ptr))
    {
        i2c_stop(i2c_info_ptr);
        return I2C_ERROR;
    }
    
    //6. read the data
    for(index=0; index<size; index++)
    {
        if(index != size-1)
        {
            rdata[index] = i2c_read_byte(i2c_info_ptr, 1);
        }
        else
        {
            rdata[index] = i2c_read_byte(i2c_info_ptr, 0);
        }
    }
    
    //7. send the stop
    i2c_stop(i2c_info_ptr);  
    return I2C_OK;   
}

void i2c_error_handle(void)
{
    while(1)
    {
        
    }
}
