//////////////////////////////////////////////////////////////////////////////
//  (c) copyright 2023-by Persional Inc.  
//  All Rights Reserved
//
//  Name:
//      ov5640.h
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
#ifndef __OV5640_INCLUDE_H
#define __OV5640_INCLUDE_H

#include "drv_soft_i2c.h"
#include "drv_i2c_pcf8574.h"

#define OV5640_ID               0x5640  	//OV5640µÄÐ¾Æ¬ID
#define OV5640_ADDR        		0x78		//OV5640µÄIICµØÖ·

#define OV5640_CHIPIDH          0x300A  	//OV5640Ð¾Æ¬ID¸ß×Ö½Ú
#define OV5640_CHIPIDL          0x300B  	//OV5640Ð¾Æ¬IDµÍ×Ö½Ú

#define OV5640_I2C_DEVICE       SOFT_I2C1
#define OV5640_I2C_SCL_PORT     GPIOB
#define OV5640_I2C_SCL_PIN      GPIO_PIN_4
#define OV5640_I2C_SDA_PORT     GPIOB
#define OV5640_I2C_SDA_PIN      GPIO_PIN_3

#define OV5640_RST_PORT         GPIOA
#define OV5640_RST_PIN          GPIO_PIN_15

uint8_t ov5640_force_init(void);
void ov5640_rgb565_mode(void);
void ov5640_light_mode(uint8_t mode);
void ov5640_flash_ctrl(uint8_t switchoff);
void ov5640_color_saturation(uint8_t sat);
void ov5640_brightness(uint8_t bright);
void ov5640_contrast(uint8_t contrast);
void ov5640_sharpness(uint8_t sharp);
void ov5640_special_effects(uint8_t eft);
uint8_t ov5640_focus_single(void);
uint8_t ov5640_focus_constant(void);

uint8_t ov5640_enter_rgb565_mode(void);

GlobalType_t ov5640_driver_init(void);
GlobalType_t ov5640_pwdn_set(uint8_t status);
GlobalType_t ov5640_write_reg(uint16_t reg, uint8_t data);
GlobalType_t ov5640_read_reg(uint16_t reg, uint8_t *ptr);
uint8_t ov5640_outsize_set(uint16_t offx, uint16_t offy, uint16_t width, uint16_t height);
#endif
