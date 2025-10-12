//////////////////////////////////////////////////////////////////////////////
//  (c) copyright 2023-by Persional Inc.  
//  All Rights Reserved
//
//  Name:
//      ov5640.c
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
#include "ov5640.h"
#include "ov5640_conf.h"
#include "driver_dcmi.h"
#include "drv_lcd.h"

static GlobalType_t ov5640_write_reg(uint16_t reg, uint8_t data);
static GlobalType_t ov5640_read_reg(uint16_t reg, uint8_t *ptr);

GlobalType_t ov5640_driver_init(void)
{
    uint16_t id_value = 0, i;
    uint8_t reg_value, res;
    
    SOFT_I2C_INFO I2C_Info = {0};
	GPIO_InitTypeDef GPIO_Initure;
    
    //clock need enable before software i2c Init
    __HAL_RCC_GPIOB_CLK_ENABLE(); 
    __HAL_RCC_GPIOA_CLK_ENABLE(); 
    
    //1. 初始化接口用于i2c通讯
    I2C_Info.scl_pin = OV5640_I2C_SCL_PIN;
    I2C_Info.scl_port = OV5640_I2C_SCL_PORT;
    I2C_Info.sda_pin = OV5640_I2C_SDA_PIN;
    I2C_Info.sda_port = OV5640_I2C_SDA_PORT;
    i2c_soft_init(OV5640_I2C_DEVICE, &I2C_Info);
    
    /* 初始化引脚用于复位管理 */
    pcf8574_driver_init();
    
	GPIO_Initure.Pin = OV5640_RST_PIN;          
	GPIO_Initure.Mode = GPIO_MODE_OUTPUT_PP;
	GPIO_Initure.Pull = GPIO_PULLUP;         
    GPIO_Initure.Speed = GPIO_SPEED_HIGH;
    HAL_GPIO_Init(OV5640_RST_PORT, &GPIO_Initure);
    
    /* 复位CMOS芯片 */
    HAL_GPIO_WritePin(OV5640_RST_PORT, OV5640_RST_PIN, GPIO_PIN_RESET);
    HAL_Delay(20); 
    ov5640_pwdn_set(0);
    HAL_Delay(5);
    HAL_GPIO_WritePin(OV5640_RST_PORT, OV5640_RST_PIN, GPIO_PIN_SET);

    HAL_Delay(10);
    
    /* 读取寄存器的id值 */
    res = ov5640_read_reg(OV5640_CHIPIDH, &reg_value);
    if (res == RT_OK) {
        id_value = reg_value<<8;
        res = ov5640_read_reg(OV5640_CHIPIDL, &reg_value);
        if (res == RT_OK) {
            id_value |= reg_value;
            if (id_value != OV5640_ID) {
                return RT_FAIL;
            }
        } else {
            return RT_FAIL;
        }
    } else {
        return RT_FAIL;
    }

    ov5640_write_reg(0x3103, 0x11); //system clock from pad, bit[1]
    ov5640_write_reg(0x3108, 0x82); //软复位
    HAL_Delay(10);
   
	//初始化 OV5640,采用SXGA分辨率(1600*1200)     
 	for(i=0; i<sizeof(ov5640_uxga_init_reg_tbl)/4; i++) {
		ov5640_write_reg(ov5640_uxga_init_reg_tbl[i][0], ov5640_uxga_init_reg_tbl[i][1]);
	}    
    
    /* 开启曝光测试 */
    ov5640_flash_ctrl(1);
    HAL_Delay(50);
    ov5640_flash_ctrl(0);
    
    HAL_Delay(10);
    
    /* 进入RGB565模式 */
    ov5640_enter_rgb565_mode();
    
    /* 初始化dcmi接口 */
    dcmi_driver_init();
    
    ov5640_write_reg(0x3035, 0x51);
    ov5640_outsize_set(4, 0, 480, 800);
    
    lcd_driver_clear(BLACK);
    
    /* 使能dcmi采样 */
    dcmi_start();
    
	ov5640_focus_single();
    
    return RT_OK;
}

uint8_t ov5640_enter_rgb565_mode(void)
{
    /* 进入rgb565模式 */
    ov5640_rgb565_mode();
    
    /* 自动对焦 */
    ov5640_force_init();   
   
    /* 白平衡自动模式 */
    ov5640_light_mode(0);

    /* 色彩饱和度0 */
    ov5640_color_saturation(3);
      
    /* 亮度0 */   
    ov5640_brightness(4);
    
    /* 对比度0 */
	ov5640_contrast(3);

    /* 自动锐度 */
	ov5640_sharpness(33);
	
    /* 启动持续对焦 */
    ov5640_focus_constant(); 

    return 0;
}

//0: 上电 1：掉电
GlobalType_t ov5640_pwdn_set(uint8_t status)
{
    uint8_t res, data;
    
    res = pcf8574_i2c_read(&data);
    if (res != I2C_OK) {
        return RT_FAIL;
    }
    
    if (status == 0) {
        data &= ~(1<< DCMI_PWDN_IO);
    } else {
        data |= (1<<DCMI_PWDN_IO);
    }
    
    res = pcf8574_i2c_write(data);
    if (res != I2C_OK) {
        return RT_FAIL;
    }

    return RT_OK;
}

void ov5640_sharpness(uint8_t sharp)
{
	if (sharp < 33) {   //设置锐度值
		ov5640_write_reg(0x5308, 0x65);
		ov5640_write_reg(0x5302, sharp);
	} else {            //自动锐度
		ov5640_write_reg(0x5308, 0x25);
		ov5640_write_reg(0x5300, 0x08);
		ov5640_write_reg(0x5301, 0x30);
		ov5640_write_reg(0x5302, 0x10);
		ov5640_write_reg(0x5303, 0x00);
		ov5640_write_reg(0x5309, 0x08);
		ov5640_write_reg(0x530a, 0x30);
		ov5640_write_reg(0x530b, 0x04);
		ov5640_write_reg(0x530c, 0x06);
	}	
}

//特效设置参数表,支持7个特效
const static uint8_t OV5640_EFFECTS_TBL[7][3]=
{ 
    0x06, 0x40, 0x10,
    0x1E, 0xA0, 0x40,
    0x1E, 0x80, 0xC0,
    0x1E, 0x80, 0x80,
    0x1E, 0x40, 0xA0,
    0x40, 0x40, 0x10,
    0x1E, 0x60, 0x60,
};

//特效设置
//0:正常    
//1,冷色
//2,暖色   
//3,黑白
//4,偏黄
//5,反色
//6,偏绿	    
void ov5640_special_effects(uint8_t eft)
{ 
	ov5640_write_reg(0x3212, 0x03); //start group 3
	ov5640_write_reg(0x5580, OV5640_EFFECTS_TBL[eft][0]);
	ov5640_write_reg(0x5583, OV5640_EFFECTS_TBL[eft][1]);// sat U
	ov5640_write_reg(0x5584, OV5640_EFFECTS_TBL[eft][2]);// sat V
	ov5640_write_reg(0x5003, 0x08);
	ov5640_write_reg(0x3212, 0x13); //end group 3
	ov5640_write_reg(0x3212, 0xa3); //launch group 3
}

//执行一次自动对焦
//返回值:0,成功;1,失败.
uint8_t ov5640_focus_single(void)
{
	uint8_t temp, res; 
	uint16_t retry=0;
    
	ov5640_write_reg(0x3022, 0x03);		        //触发一次自动对焦 
	while(1) {
		retry++;
		res = ov5640_read_reg(0x3029, &temp);	//检查对焦完成状态
        if (res != RT_OK) {
            return 1;
        } 
        
		if (temp == 0x10) {
            break;
        }
        
		HAL_Delay(5);
		if(retry > 1000) {
            return 1;
        }
	}
	return 0;	 		
}

//持续自动对焦,当失焦后,会自动继续对焦
//返回值:0,成功;其他,失败.
uint8_t ov5640_focus_constant(void)
{
	uint8_t temp = 0, res;   
	uint16_t retry = 0;
    
	ov5640_write_reg(0x3023, 0x01);
	ov5640_write_reg(0x3022, 0x08);//发送IDLE指令 
	do {
		res = ov5640_read_reg(0x3023, &temp);
        if (res != RT_OK) {
            return 2;
        }
        
		retry++;
		if (retry > 1000) {
            return 2;
        }
		HAL_Delay(5);
	} while(temp!=0x00);
    
	ov5640_write_reg(0x3023, 0x01);
	ov5640_write_reg(0x3022, 0x04);//发送持续对焦指令 
	retry=0;
	do {
		res = ov5640_read_reg(0x3023, &temp);
        if (res != RT_OK) {
            return 2;
        }
        
		retry++;
		if(retry > 1000) {
            return 2;
        }
		HAL_Delay(5);
	} while(temp != 0x00);//0,对焦完成;1:正在对焦
    
	return 0;
} 

//闪光灯控制
//mode:0,关闭
//     1,打开 
void ov5640_flash_ctrl(uint8_t switchon)
{
	ov5640_write_reg(0x3016, 0x02);
	ov5640_write_reg(0x301C, 0x02); 
	if (switchon) {
        ov5640_write_reg(0x3019, 0x02);     
    } else {
        ov5640_write_reg(0x3019, 0x00);
    }
} 

//白平衡设置
//0:自动
//1:日光sunny
//2,办公室office
//3,阴天cloudy
//4,家里home
void ov5640_light_mode(uint8_t mode)
{
	uint8_t i;
    
	ov5640_write_reg(0x3212, 0x03);	//start group 3
	for (i=0; i<7; i++) {
        ov5640_write_reg(0x3400+i, OV5640_LIGHTMODE_TBL[mode][i]);//设置饱和度
    }
    
	ov5640_write_reg(0x3212, 0x13); //end group 3
	ov5640_write_reg(0x3212, 0xa3); //launch group 3	
}

//色彩饱和度设置参数表,支持7个等级
const static uint8_t OV5640_SATURATION_TBL[7][6]=
{ 
	0x0C, 0x30, 0x3D, 0x3E, 0x3D, 0x01,//-3 
	0x10, 0x3D, 0x4D, 0x4E, 0x4D, 0x01,//-2	
	0x15, 0x52, 0x66, 0x68, 0x66, 0x02,//-1	
	0x1A, 0x66, 0x80, 0x82, 0x80, 0x02,//+0	
	0x1F, 0x7A, 0x9A, 0x9C, 0x9A, 0x02,//+1	
	0x24, 0x8F, 0xB3, 0xB6, 0xB3, 0x03,//+2
	0x2B, 0xAB, 0xD6, 0xDA, 0xD6, 0x04,//+3
}; 

//色度设置
//sat:0~6,代表饱和度-3~3. 
void ov5640_color_saturation(uint8_t sat)
{ 
	uint8_t i;
    
	ov5640_write_reg(0x3212, 0x03);	//start group 3
	ov5640_write_reg(0x5381, 0x1c);
	ov5640_write_reg(0x5382, 0x5a);
	ov5640_write_reg(0x5383, 0x06);
    
	for(i=0; i<6; i++) {
        ov5640_write_reg(0x5384+i, OV5640_SATURATION_TBL[sat][i]);//设置饱和度
    }
    
	ov5640_write_reg(0x538b, 0x98);
	ov5640_write_reg(0x538a, 0x01);
	ov5640_write_reg(0x3212, 0x13); //end group 3
	ov5640_write_reg(0x3212, 0xa3); //launch group 3	
}

//对比度设置
//contrast:0~6,代表亮度-3~3.
void ov5640_contrast(uint8_t contrast)
{
	uint8_t reg0val = 0x00;//contrast=3,默认对比度
	uint8_t reg1val = 0x20;
    
	switch (contrast) {
		case 0:
			reg1val = 0x14;
            reg0val = 0x14;	 	 
			break;	
		case 1:
			reg1val = 0x18;
            reg0val = 0x18; 	 
			break;	
		case 2:
			reg1val = 0x1C;
            reg0val = 0x1C;	 
			break;	
		case 4:
			reg0val = 0x10;	 	 
			reg1val = 0x24;	 	 
			break;	
		case 5:
			reg0val = 0x18;	 	 
			reg1val = 0x28;	 	 
			break;	
		case 6:
			reg0val = 0x1C;	 	 
			reg1val = 0x2C;	 	 
			break;	
	}
    
	ov5640_write_reg(0x3212, 0x03); //start group 3
	ov5640_write_reg(0x5585, reg0val);
	ov5640_write_reg(0x5586, reg1val); 
	ov5640_write_reg(0x3212, 0x13); //end group 3
	ov5640_write_reg(0x3212, 0xa3); //launch group 3
}


//bright:0~8,代表亮度-4~4.
void ov5640_brightness(uint8_t bright)
{
	uint8_t brtval;
	if (bright < 4) {
        brtval = 4-bright;
    } else {
        brtval = bright-4;
    }
	ov5640_write_reg(0x3212, 0x03);	//start group 3
	ov5640_write_reg(0x5587, brtval<<4);
    
	if (bright < 4) {
        ov5640_write_reg(0x5588, 0x09);
    } else {
        ov5640_write_reg(0x5588, 0x01);
    }
	ov5640_write_reg(0x3212, 0x13); //end group 3
	ov5640_write_reg(0x3212, 0xa3); //launch group 3
}

void ov5640_rgb565_mode(void)
{
	uint16_t i=0;
    
	for (i=0; i<(sizeof(ov5640_rgb565_reg_tbl)/4); i++) {
		ov5640_write_reg(ov5640_rgb565_reg_tbl[i][0], ov5640_rgb565_reg_tbl[i][1]); 
	} 
}

//初始化自动对焦
//返回值:0,成功;1,失败.
uint8_t ov5640_force_init(void)
{ 
	uint16_t i; 
	uint16_t addr = 0x8000;
	uint8_t state = 0x8F, res;
    
	ov5640_write_reg(0x3000, 0x20);			//reset MCU	 
	for (i=0; i<sizeof(ov5640_af_config); i++) {
		ov5640_write_reg(addr, ov5640_af_config[i]);
		addr++;
	}
    
	ov5640_write_reg(0x3022, 0x00);
	ov5640_write_reg(0x3023, 0x00);
	ov5640_write_reg(0x3024, 0x00);
	ov5640_write_reg(0x3025, 0x00);
	ov5640_write_reg(0x3026, 0x00);
	ov5640_write_reg(0x3027, 0x00);
	ov5640_write_reg(0x3028, 0x00);
	ov5640_write_reg(0x3029, 0x7f);
	ov5640_write_reg(0x3000, 0x00);
    
	i=0;
	do {
		res = ov5640_read_reg(0x3029, &state);	
		if (res != RT_OK)
            return RT_FAIL;
		
        HAL_Delay(1);
        i++;
		if(i > 1000) {
            return RT_FAIL;
        }
	} while(state != 0x70);
	
    return 0;    
} 

uint8_t ov5640_outsize_set(uint16_t offx, uint16_t offy, uint16_t width, uint16_t height)
{ 
    ov5640_write_reg(0x3212, 0x03);  	    //开始组3
    
    //以下设置决定实际输出尺寸(带缩放)
    ov5640_write_reg(0x3808,width>>8);	    //设置实际输出宽度高字节
    ov5640_write_reg(0x3809,width&0xff);    //设置实际输出宽度低字节  
    ov5640_write_reg(0x380a,height>>8);     //设置实际输出高度高字节
    ov5640_write_reg(0x380b,height&0xff);   //设置实际输出高度低字节
    
	//以下设置决定输出尺寸在ISP上面的取图范围
	//范围:xsize-2*offx,ysize-2*offy
    ov5640_write_reg(0x3810,offx>>8);	    //设置X offset高字节
    ov5640_write_reg(0x3811,offx&0xff);     //设置X offset低字节
	
    ov5640_write_reg(0x3812,offy>>8);	    //设置Y offset高字节
    ov5640_write_reg(0x3813,offy&0xff);     //设置Y offset低字节
	
    ov5640_write_reg(0x3212, 0x13);		    //结束组3
    ov5640_write_reg(0x3212, 0xa3);		    //启用组3设置
    return 0; 
}

static GlobalType_t ov5640_write_reg(uint16_t reg, uint8_t data)
{
    uint8_t res;

    __disable_irq();
    res = i2c_write_memory(SOFT_I2C1, OV5640_ADDR, reg, 2, &data, 1);
    __enable_irq();
    
    if (res != I2C_OK) {
        return RT_FAIL;
    }
    
    return RT_OK;       
}

static GlobalType_t ov5640_read_reg(uint16_t reg, uint8_t *ptr)
{
    uint8_t res;
    
    __disable_irq();
    res = i2c_read_memory(SOFT_I2C1, OV5640_ADDR, reg, 2, ptr, 1);
    __enable_irq();
    
    if (res != I2C_OK) {
        return RT_FAIL;
    }
    
    return RT_OK;       
}
