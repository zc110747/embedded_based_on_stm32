//////////////////////////////////////////////////////////////////////////////
//  (c) copyright 2023-by Persional Inc.  
//  All Rights Reserved
//
//  Name:
//      drv_lcd_text.c
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
#include "drv_lcd_text.h"
#include "font.h"
#include "ff.h"

#define FONT_NUMS   5

// global parameter
extern LCD_INFO g_lcd_info;

// local parameter
typedef struct
{  
    FATFS fs;

    FIL fil_list[FONT_NUMS];
    
    uint8_t fil_valid[FONT_NUMS];
}LCD_FS_INFO;
static LCD_FS_INFO g_lcd_fs_info;

static const char* font_name[FONT_NUMS] = {
    "1:/FONT/UNIGBK.BIN",
    "1:/FONT/GBK12.FONT",
    "1:/FONT/GBK16.FONT",
    "1:/FONT/GBK24.FONT",
    "1:/FONT/GBK32.FONT",
};

// local function
static void lcd_showchar(uint16_t x, uint16_t y, uint8_t num, uint8_t size, uint8_t mode);

GlobalType_t lcd_driver_font_init(void)
{
    FRESULT res;
    uint8_t index;
    
    memset(&g_lcd_fs_info, 0, sizeof(g_lcd_fs_info));
    
    res = f_mount(&g_lcd_fs_info.fs, "1:", 1);
    if (res != FR_OK) {
        return RT_FAIL;
    }
    
    // 打开文件, 用于后续读取
    for (index=0; index<FONT_NUMS; index++)
    {
        res = f_open(&g_lcd_fs_info.fil_list[index], font_name[index], FA_READ);
        if (res != FR_OK) {
            continue;
        } else {
            g_lcd_fs_info.fil_valid[index] = 1;
        }
    }        
    
    return RT_OK;
}
 
//code 字符指针开始
//从字库中查找出字模
//code 字符串的开始地址,GBK码
//mat  数据存放地址 (size/8+((size%8)?1:0))*(size) bytes大小	
//size:字体大小
static GlobalType_t Get_HzMat(uint8_t *code, uint8_t *pbuffer, uint8_t size)
{		    
	uint8_t qh, ql;				  
	uint32_t foffset; 
	uint16_t csize=(size/8+((size%8)?1:0))*(size);//得到字体一个字符对应点阵集所占的字节数
    uint32_t bytes_read;
    FRESULT res;
    uint8_t file_index = 0;
    
	qh=*code;
	ql=*(++code);
    
	if (qh < 0x81 || ql < 0x40 
    || ql == 0xff || qh == 0xff) //非常用汉字
	{   		    
        goto __fail;
	}
    
	if(ql < 0x7f) ql -= 0x40;
	else ql -= 0x41;
	qh -= 0x81;
	foffset = ((uint32_t)190*qh + ql) * csize;	//得到字库中的字节偏移量 
    
	switch(size)
	{
		case 12:
            file_index = 1;
			break;
		case 16:
            file_index = 2;
			break;
		case 24:
            file_index = 3;
			break;
		case 32:
            file_index = 4;
			break;		
	} 

    if (!g_lcd_fs_info.fil_valid[file_index])
        goto __fail;
    res = f_lseek(&g_lcd_fs_info.fil_list[file_index], foffset);
    if (res != FR_OK)
        goto __fail; 
    res = f_read(&g_lcd_fs_info.fil_list[file_index], pbuffer, csize, &bytes_read);
    if (res != FR_OK || bytes_read != csize)
        goto __fail; 
            
    return RT_OK;
    
__fail:
    memset(pbuffer, 0, csize);
    
    return RT_FAIL;
} 

// global function
// 显示一个指定大小的汉字
// @x: 起始坐标x
// @y: 起始坐标y
// @size: 字体大小
// @mode: 0-正常显示, 1-叠加显示	   
void lcd_text_show_font(uint16_t x, uint16_t y, uint8_t *font, uint8_t size, uint8_t mode)
{
	uint8_t temp,t,t1;
	uint16_t y0=y;
	uint8_t dzk[128];   
	uint16_t csize;
    
	if(size != 12 && size != 16 
    && size != 24 && size != 32)
        return;	
    
	Get_HzMat(font, dzk, size);	//得到相应大小的点阵数据 
    
    csize = (size/8+((size%8)?1:0))*(size);
	for(t=0;t<csize;t++)
	{   												   
		temp=dzk[t];			//得到点阵数据                          
		for(t1=0;t1<8;t1++)
		{
			if(temp&0x80) {
                lcd_fast_drawpoint(x, y, POINT_COLOR);
            } else if(mode == 0) {
                lcd_fast_drawpoint(x, y, BACK_COLOR);
            }
            
			temp<<=1;
			y++;
			if((y-y0) == size)
			{
				y=y0;
				x++;
				break;
			}
		}  	 
	}  
}

//在指定位置开始显示一个字符串	    
//支持自动换行
//(x,y):起始坐标
//width,height:区域
//str  :字符串
//size :字体大小
//mode:0,非叠加方式;1,叠加方式    	   		   
void lcd_text_show_str(uint16_t x, uint16_t y, uint16_t width, uint16_t height, uint8_t*str, uint8_t size, uint8_t mode)
{					
	uint16_t x0=x;
	uint16_t y0=y;							  	  
    uint8_t bHz=0;     //字符或者中文  	    				    				  	  
    
    while(*str!=0)//数据未结束
    { 
        if(!bHz)
        {
	        if(*str > 0x80)
                bHz=1;
	        else
	        {      
                if(x>(x0+width-size/2))
				{				   
					y+=size;
					x=x0;	   
				}							    
		        if(y>(y0+height-size))break;   
		        if(*str==13)
		        {         
		            y+=size;
					x=x0;
		            str++; 
		        }  
		        else 
                    lcd_showchar(x,y,*str,size,mode);
				str++; 
		        x+=size/2;
	        }
        }
        else
        {     
            bHz=0;   
            if(x>(x0+width-size))
			{	    
				y+=size;
				x=x0;		  
			}
	        if(y>(y0+height-size))
                break; 						     
	        lcd_text_show_font(x,y,str,size,mode);
	        str += 2; 
	        x+=size;    
        }						 
    }   
} 

void lcd_driver_showstring(uint16_t x, uint16_t y, uint16_t width, uint16_t height, uint8_t size, char *p)
{
    uint8_t x0 = x;
    width += x;
    height += y;
    while((*p<='~')&&(*p>=' '))
    {       
        if(x>=width)
        {
            x=x0;
            y+=size;
        }
        if(y >= height)
            break;

        lcd_showchar(x, y, *p, size, 0);
        x+=size/2;
        p++;
    }
}

void lcd_driver_show_num(uint16_t x, uint16_t y, uint32_t num, uint8_t len, uint8_t size, uint8_t mode)
{  
    uint8_t t,temp;
    uint8_t enshow=0;
    for(t=0;t<len;t++)
    {
        temp=(num/lcd_pow(10,len-t-1))%10;
        if(enshow==0&&t<(len-1))
        {
            if(temp==0)
            {
                if(mode&0x80)
                {
                    lcd_showchar(x+(size/2)*t,y,'0',size,mode&0x01);
                }
                else 
                {
                    lcd_showchar(x+(size/2)*t,y,' ',size,mode&0x01);  
                }
                continue;
            }
            else 
                enshow=1;
        }
        lcd_showchar(x+(size/2)*t,y,temp+'0',size,mode&0x01); 
    }
} 

void lcd_driver_show_extra_num(uint16_t x,uint16_t y, uint32_t num, uint8_t len, uint8_t size, uint8_t mode)
{  
    uint8_t t,temp;
    uint8_t enshow = 0;
    for(t=0;t<len;t++)
    {
        temp=(num/lcd_pow(10,len-t-1))%10;
        if(enshow==0&&t<(len-1))
        {
            if(temp==0)
            {
                if(mode&0x80)
                {
                    lcd_showchar(x+(size/2)*t,y,'0',size,mode&0x01);
                }                    
                else 
                {
                    lcd_showchar(x+(size/2)*t,y,' ',size,mode&0x01);
                }
                continue;
            }
            else 
                enshow=1;
        }
        lcd_showchar(x+(size/2)*t,y,temp+'0',size,mode&0x01); 
    }
}

///////////////////////////////////////local function////////////////////////////
static void lcd_showchar(uint16_t x, uint16_t y, uint8_t num, uint8_t size, uint8_t mode)
{
    uint8_t temp,t1,t;
    uint16_t y0=y;
    uint8_t csize=(size/8+((size%8)?1:0))*(size/2);		
    num=num-' ';
    for(t=0; t<csize; t++)
    { 
        switch(size)
        {
            #if SUPPORT_ASCII_1206 == 1
            case 12:
                temp = asc2_1206[num][t];
                break;
            #endif
            
            #if SUPPORT_ASCII_1608 == 1
            case 16:
                temp = asc2_1608[num][t];	
                break;
            #endif
            
            #if SUPPORT_ASCII_2412 == 1
            case 24:
                temp = asc2_2412[num][t];	
                break; 
            #endif
            
            #if SUPPORT_ASCII_3216 == 1
            case 32:
                temp = asc2_3216[num][t];	
                break;
            #endif
            
            default:
                return;
        }
                            
        for(t1=0; t1<8; t1++)
        {			    
            if(temp&0x80)
            {
                lcd_fast_drawpoint(x, y, POINT_COLOR);
            }
            else if(mode==0)
            {
                lcd_fast_drawpoint(x, y, BACK_COLOR);
            }
            
            temp<<=1;
            y++;
            if(y >= g_lcd_info.lcd_height)
            {
                return;
            }
            if((y-y0)==size)
            {
                y=y0;
                x++;
                if(x>=g_lcd_info.lcd_width)
                    return;	
                break;
            }
        }
    }
}

