//////////////////////////////////////////////////////////////////////////////
//  (c) copyright 2023-by Persional Inc.  
//  All Rights Reserved
//
//  Name:
//      drv_lcd_text.h
//
//  Purpose:
//      lcd driver write interface.
//
// Author:
//      @公众号：<嵌入式技术总结>
//
//  Assumptions:
//
//  Revision History:
//
/////////////////////////////////////////////////////////////////////////////
#ifndef _DRV_LCD_TEXT_H
#define _DRV_LCD_TEXT_H

#include "drv_lcd.h"

void lcd_driver_showstring(uint16_t x,uint16_t y,uint16_t width,uint16_t height, uint8_t size, char *p);
void lcd_driver_show_num(uint16_t x,uint16_t y,uint32_t num,uint8_t len,uint8_t size,uint8_t mode);
void lcd_driver_show_extra_num(uint16_t x,uint16_t y, uint32_t num, uint8_t len, uint8_t size, uint8_t mode);
void lcd_text_show_str(uint16_t x, uint16_t y, uint16_t width, uint16_t height, uint8_t*str, uint8_t size, uint8_t mode);
void lcd_text_show_font(uint16_t x, uint16_t y, uint8_t *font, uint8_t size, uint8_t mode);
#endif
