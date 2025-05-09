//////////////////////////////////////////////////////////////////////////////
//  (c) copyright 2023-by Persional Inc.  
//  All Rights Reserved
//
//  Name:
//      lcd.hpp
//
//  Purpose:
//      lcd driver interface use fmc.
//
// Author:
//      @zc
//
//  Assumptions:
//
//  Revision History:
//
/////////////////////////////////////////////////////////////////////////////
#ifndef _DRV_LCD_H
#define _DRV_LCD_H

#include "includes.h"

#define LCD_TEST    1

//LCD register and data 
typedef struct
{
    volatile uint16_t LCD_REG;
    volatile uint16_t LCD_RAM;
} LCD_TypeDef;	

typedef struct
{
    uint16_t reg;
    uint16_t val;
}LCD_RegValue;

typedef struct
{
    uint16_t lcd_id;
    
    uint16_t lcd_dir;
    
    uint16_t wramcmd;	
	
    uint16_t setxcmd;
	
    uint16_t setycmd;		
    
    uint32_t lcd_width;
    
    uint32_t lcd_height;
}LCD_INFO;

#define LCD_BASE        ((uint32_t)(0x60000000 | 0x0007FFFE))
#define LCD             ((LCD_TypeDef *) LCD_BASE)

//scan director
#define L2R_U2D  		0 		
#define L2R_D2U  		1 		
#define R2L_U2D  		2 		
#define R2L_D2U  		3 		
#define U2D_L2R  		4 		
#define U2D_R2L  		5 		
#define D2U_L2R  		6 		
#define D2U_R2L  		7	
#define DFT_SCAN_DIR  	L2R_U2D  

//GUI Color
#define WHITE         	 0xFFFF
#define BLACK         	 0x0000	  
#define BLUE         	 0x001F  
#define BRED             0XF81F
#define GRED 			 0XFFE0
#define GBLUE			 0X07FF
#define RED           	 0xF800
#define MAGENTA       	 0xF81F
#define GREEN         	 0x07E0
#define CYAN          	 0x7FFF
#define YELLOW        	 0xFFE0
#define BROWN 			 0XBC40 
#define BRRED 			 0XFC07 
#define GRAY  			 0X8430 
#define DARKBLUE      	 0X01CF	
#define LIGHTBLUE      	 0X7D7C	
#define GRAYBLUE       	 0X5458 

//back light
#define LED_BACKLIGHT_ON	HAL_GPIO_WritePin(GPIOB, GPIO_PIN_5, GPIO_PIN_SET);
#define LED_BACKLIGHT_OFF	HAL_GPIO_WritePin(GPIOB, GPIO_PIN_5, GPIO_PIN_RESET);


#ifdef __cplusplus
extern "C" {
#endif

GlobalType_t lcd_driver_init(void);
void lcd_write_ram_prepare(void);
void lcd_driver_clear(uint32_t color);
void lcd_set_cursor(uint16_t Xpos, uint16_t Ypos);
void lcd_driver_showstring(uint16_t x,uint16_t y,uint16_t width,uint16_t height, uint8_t size, char *p);
void lcd_driver_show_num(uint16_t x,uint16_t y,uint32_t num,uint8_t len,uint8_t size,uint8_t mode);
void lcd_driver_show_extra_num(uint16_t x,uint16_t y, uint32_t num, uint8_t len, uint8_t size, uint8_t mode);
LCD_INFO *get_lcd_info(void);
#ifdef __cplusplus
}
#endif
#endif
