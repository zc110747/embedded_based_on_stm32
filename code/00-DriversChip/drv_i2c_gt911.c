//////////////////////////////////////////////////////////////////////////////
//  (c) copyright 2023-by Persional Inc.  
//  All Rights Reserved
//
//  Name:
//      drv_i2c_gt911.c
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
#include <string.h>
#include "drv_i2c_gt911.h"
#include "drv_lcd.h"
#include "drv_soft_i2c.h"

_m_tp_dev tp_dev = { 0 };

#define TSC_RST_Pin     GPIO_PIN_8
#define TSC_RST_Port    GPIOI

#define GT911_SOFT_I2C   SOFT_I2C5
static GlobalType_t gt911_i2c_multi_read(uint16_t reg, uint8_t *rdata, uint8_t size);
static GlobalType_t gt911_i2c_multi_write(uint16_t reg, uint8_t *data, uint8_t size);
    
uint8_t gt911_driver_init(void)
{
    SOFT_I2C_INFO I2C_Info = {0};
    GPIO_InitTypeDef GPIO_Initure;
    uint8_t temp[5];
    
    __HAL_RCC_GPIOH_CLK_ENABLE(); 
    __HAL_RCC_GPIOI_CLK_ENABLE(); 
    
    I2C_Info.scl_pin = GPIO_PIN_6;
    I2C_Info.scl_port = GPIOH;
    I2C_Info.sda_pin = GPIO_PIN_3;
    I2C_Info.sda_port = GPIOI;
    
    if(i2c_soft_init(GT911_SOFT_I2C, &I2C_Info) != I2C_OK)
    {
        return RT_FAIL;
    }
   
    //PH7, used to update tsc address
    GPIO_Initure.Pin = GPIO_PIN_7;     
    GPIO_Initure.Mode = GPIO_MODE_INPUT;
    GPIO_Initure.Pull = GPIO_PULLUP;
    GPIO_Initure.Speed = GPIO_SPEED_HIGH;
    HAL_GPIO_Init(GPIOH, &GPIO_Initure);
    
    GPIO_Initure.Pin = TSC_RST_Pin;             
    GPIO_Initure.Mode = GPIO_MODE_OUTPUT_PP;
    HAL_GPIO_Init(TSC_RST_Port, &GPIO_Initure);
    
    //RST the tsc
    HAL_GPIO_WritePin(TSC_RST_Port, TSC_RST_Pin, GPIO_PIN_RESET);
    HAL_Delay(10);
    HAL_GPIO_WritePin(TSC_RST_Port, TSC_RST_Pin, GPIO_PIN_SET);
    HAL_Delay(10); 

    GPIO_Initure.Pin = GPIO_PIN_7;     
    GPIO_Initure.Mode = GPIO_MODE_INPUT;
    HAL_GPIO_Init(GPIOH, &GPIO_Initure);
    HAL_Delay(50);
    
    if(gt911_i2c_multi_read(GT_ID_REG, temp, 4) != RT_OK)
        return RT_FAIL;
    
    if(strncmp((char *)temp, "911", strlen("911")) != 0)
        return RT_FAIL;
    
    return RT_OK;
}

static GlobalType_t gt911_i2c_multi_write(uint16_t reg, uint8_t *data, uint8_t size)
{
    uint8_t res;
    
    __disable_irq();
    res = i2c_write_memory(GT911_SOFT_I2C, GT911_ADDR<<1, reg, 2, data, size);
    __enable_irq();
    
    if(res != I2C_OK)
        return RT_FAIL;
    
    return RT_OK;
}

static GlobalType_t gt911_i2c_multi_read(uint16_t reg, uint8_t *rdata, uint8_t size)
{
    uint8_t res;
    
    __disable_irq();
    res = i2c_read_memory(GT911_SOFT_I2C, GT911_ADDR<<1, reg, 2, rdata, size);
    __enable_irq();   
    
    if(res != I2C_OK)
        return RT_FAIL;
    
    return RT_OK;
}

const uint16_t GT9147_TPX_TBL[5]={GT_TP1_REG, GT_TP2_REG, GT_TP3_REG, GT_TP4_REG, GT_TP5_REG};

uint8_t gt911_scan(void)
{
    static uint8_t times = 0;
    uint8_t temp, i, tempsta, mode = 0;
    uint8_t buf[4], res = 0;
    LCD_INFO *lcd_ptr = get_lcd_info();
    
    times++;
    if((times%10)==0 || times<10)
    {
        gt911_i2c_multi_read(GT_GSTID_REG, &mode, 1);
        if (((mode&0x0F)<6) && ((mode&0x80) != 0))
        {
            temp = 0;
            gt911_i2c_multi_write(GT_GSTID_REG, &temp, 1);
        }

        if ((mode&0XF)&&((mode&0XF)<6))
		{
			temp = 0XFF<<(mode&0XF);		//将点的个数转换为1的位数,匹配tp_dev.sta定义 
			tempsta = tp_dev.sta;		//保存当前的tp_dev.sta值
			tp_dev.sta=(~temp) | TP_PRES_DOWN | TP_CATH_PRES; 
			tp_dev.x[4]=tp_dev.x[0];	
			tp_dev.y[4]=tp_dev.y[0];
            
			for (i=0; i<5; i++)
			{
				if (tp_dev.sta&(1<<i))  //触摸有效
				{
					gt911_i2c_multi_read(GT9147_TPX_TBL[i], buf, 4);	//读取XY坐标值
                    tp_dev.x[i]=((uint16_t)buf[1]<<8)+buf[0];
                    tp_dev.y[i]=((uint16_t)buf[3]<<8)+buf[2];
				}			
			}
         
			res=1;
         
            //非法数据
			if (tp_dev.x[0] > lcd_ptr->lcd_width
            || tp_dev.y[0] > lcd_ptr->lcd_height) 
            { 
				if ((mode&0XF)>1)		//有其他点有数据,则复第二个触点的数据到第一个触点.
				{
					tp_dev.x[0] = tp_dev.x[1];
					tp_dev.y[0] = tp_dev.y[1];
					times=0;				//触发一次,则会最少连续监测10次,从而提高命中率
				} 
                else
                {
					tp_dev.x[0] = tp_dev.x[4];
					tp_dev.y[0] = tp_dev.y[4];
					mode = 0X80;		
					tp_dev.sta = tempsta;	//恢复tp_dev.sta
				}
			} 
            else 
            {                
                times=0;
            }
		}
    }

    if ((mode&0x8F) == 0x80)
    { 
        if (tp_dev.sta&TP_PRES_DOWN)	
        {
            tp_dev.sta &= ~(1<<7);	
        } 
        else 
        { 
            tp_dev.x[0] = 0xffff;
            tp_dev.y[0] = 0xffff;
            tp_dev.sta &= 0XE0;		
        }	 
    }
        
    if (times >= 240) {
        times = 10;
    }
    return res;
}

