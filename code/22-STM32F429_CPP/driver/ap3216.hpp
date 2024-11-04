//////////////////////////////////////////////////////////////////////////////
//  (c) copyright 2023-by Persional Inc.  
//  All Rights Reserved
//
//  Name:
//      ap3216.hpp
//
//  Purpose:
//      usart driver interface process.
//
// Author:
//      @zc
//
//  Assumptions:
//
//  Revision History:
//
/////////////////////////////////////////////////////////////////////////////
#ifndef _I2C_AP3216_HPP
#define _I2C_AP3216_HPP

#include "i2c.hpp"

/*address*/
#define AP3216C_ADDR    	    0x1e	
#define AP3216C_TIMEOUT         10

/*register*/
#define AP3216C_SYSTEMCONG	    0x00	
#define AP3216C_INTSTATUS	    0X01	
#define AP3216C_INTCLEAR	    0X02	
#define AP3216C_IRDATALOW	    0x0A	
#define AP3216C_IRDATAHIGH	    0x0B
#define AP3216C_ALSDATALOW	    0x0C	
#define AP3216C_ALSDATAHIGH	    0X0D	
#define AP3216C_PSDATALOW	    0X0E	
#define AP3216C_PSDATAHIGH	    0X0F

typedef struct
{
    uint16_t ir;
    uint16_t als;
    uint16_t ps;
}AP3216C_INFO;

class device_ap3216: public device_i2c
{
public:
    device_ap3216(){
    }
    ~device_ap3216(){
    }
    
    GlobalType_t init();

    AP3216C_INFO *get_info() {return &info_;}
    
    void upadte_info();
    
private:
    GlobalType_t read(uint8_t reg, uint8_t *data, uint8_t size);

    GlobalType_t write(uint8_t reg, uint8_t *data, uint8_t size);

    GlobalType_t read_reg(uint8_t reg, uint8_t *data);

    GlobalType_t write_reg(uint8_t reg, uint8_t data);

private:
    AP3216C_INFO info_; 
};

GlobalType_t device_ap3216::init()
{
    GlobalType_t res;
    
    res = device_i2c::init();
    if (res != RT_OK)  
    {
        return RT_FAIL;
    }
    
    write_reg(AP3216C_SYSTEMCONG, 0x40); //reset the ap3216
    HAL_Delay(10);
    write_reg(AP3216C_SYSTEMCONG, 0x03); //config the ALS+PS+LR
    
    return RT_OK;
}

GlobalType_t device_ap3216::read_reg(uint8_t reg, uint8_t *data)
{
    return read(reg, data, 1);
}

GlobalType_t device_ap3216::write_reg(uint8_t reg, uint8_t data)
{
    return write(reg, &data, 1);
}

GlobalType_t device_ap3216::read(uint8_t reg, uint8_t *data, uint8_t size)
{
    uint8_t res;
    
    res = multi_read(AP3216C_ADDR<<1, reg, 1, data, size, AP3216C_TIMEOUT);  
    
    if(res != HAL_OK)
    {
        return RT_FAIL;
    }
    
    return RT_OK;
}

GlobalType_t device_ap3216::write(uint8_t reg, uint8_t *data, uint8_t size)
{
    uint8_t res;
    
    res = multi_write(AP3216C_ADDR<<1, reg, 1, data, size, AP3216C_TIMEOUT);  
    
    if(res != HAL_OK)
    {
        return RT_FAIL;
    }
    return RT_OK;
}

void device_ap3216::upadte_info()
{
    uint8_t buf[6];
    uint8_t index;
    uint8_t is_read_ok = 0;

    is_read_ok = 1;

    for(index=0; index<6; index++)
    {
        if(read_reg(AP3216C_IRDATALOW+index, &buf[index]) != RT_OK)
        {
            is_read_ok = 0;
            break;
        }
    }
    if(is_read_ok == 1)
    {
        if(buf[0]&(1<<7)) 	
            info_.ir = 0;					
        else 			
            info_.ir = (((uint16_t)buf[1])<< 2) | (buf[0]&0X03); 			

        info_.als = ((uint16_t)buf[3]<<8) | buf[2];

        if(buf[4]&(1<<6))	
            info_.ps = 0;    													
        else 				
            info_.ps = ((uint16_t)(buf[5]&0X3F)<<4)|(buf[4]&0X0F);
    } 
}
#endif
