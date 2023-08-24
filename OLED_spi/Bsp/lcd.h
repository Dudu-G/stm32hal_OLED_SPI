#ifndef __LCD_H
#define __LCD_H
#include "main.h"

typedef uint32_t  u32;
typedef uint16_t u16;
typedef uint8_t  u8;
extern SPI_HandleTypeDef hspi2;

// LCD重要参数集
typedef struct
{
    u16 width;      // LCD 宽度
    u16 height;     // LCD 高度
    u16 id;         // LCD ID
    u8 dir;         // 横屏还是竖屏控制：0，竖屏；1，横屏。
    u16 wramcmd;    // 开始写gram指令
    u16 setxcmd;    // 设置x坐标指令
    u16 setycmd;    // 设置y坐标指令
} _lcd_dev;

extern _lcd_dev lcddev;

/////////////////////////////////////用户配置区///////////////////////////////////	 
//支持横竖屏快速定义切换，支持8/16位模式切换
#define USE_HORIZONTAL  	0	//定义是否使用横屏 		0,不使用.1,使用.
//使用硬件SPI 还是模拟SPI作为测试
#define USE_HARDWARE_SPI    1  //1:Enable Hardware SPI;0:USE Soft SPI
//////////////////////////////////////////////////////////////////////////////////	  


#if USE_HORIZONTAL == 1
#define LCD_W 320
#define LCD_H 240
#else
#define LCD_W 240
#define LCD_H 320
#endif

//TFTLCD部分外要调用的函数		   
extern u16  POINT_COLOR;//默认红色    
extern u16  BACK_COLOR; //背景颜色.默认为白色


////////////////////////////////////////////////////////////////////
//-----------------LCD端口定义---------------- 
//模块采用了三极管控制背光亮灭，用户也可以接PWM调节背光亮度
//#define LCD_LED        	GPIO_Pin_9  //PB9 连接至TFT -LED
//接口定义在Lcd_Driver.h内定义，请根据接线修改并修改相应IO初始化LCD_GPIO_Init()
// LCD控制口定义
#define LCD_CTRL GPIOB
#define LCD_CTRL2 GPIOC
#define LCD_RS GPIO_PIN_6
#define LCD_CS GPIO_PIN_12
#define LCD_SCL GPIO_PIN_13
#define LCD_SDA GPIO_PIN_15

// 液晶控制口置1操作宏定义
#define LCD_CS_SET LCD_CTRL->BSRR = LCD_CS
#define LCD_RS_SET LCD_CTRL2->BSRR = LCD_RS
#define LCD_SDA_SET LCD_CTRL->BSRR = LCD_SDA
#define LCD_SCL_SET LCD_CTRL->BSRR = LCD_SCL

// 液晶控制口置0操作宏定义
#define LCD_CS_CLR LCD_CTRL->BRR = LCD_CS
#define LCD_RS_CLR LCD_CTRL2->BRR = LCD_RS
#define LCD_SDA_CLR LCD_CTRL->BRR = LCD_SDA
#define LCD_SCL_CLR LCD_CTRL->BRR = LCD_SCL

//画笔颜色
#define WHITE       0xFFFF
#define BLACK      	0x0000	  
#define BLUE       	0x001F  
#define BRED        0XF81F
#define GRED 			 	0XFFE0
#define GBLUE			 	0X07FF
#define RED         0xF800
#define MAGENTA     0xF81F
#define GREEN       0x07E0
#define CYAN        0x7FFF
#define YELLOW      0xFFE0
#define BROWN 			0XBC40 //棕色
#define BRRED 			0XFC07 //棕红色
#define GRAY  			0X8430 //灰色
//GUI颜色

#define DARKBLUE      	 0X01CF	//深蓝色
#define LIGHTBLUE      	 0X7D7C	//浅蓝色  
#define GRAYBLUE       	 0X5458 //灰蓝色
//以上三色为PANEL的颜色 
 
#define LIGHTGREEN     	0X841F //浅绿色
//#define LIGHTGRAY     0XEF5B //浅灰色(PANNEL)
#define LGRAY 			 		0XC618 //浅灰色(PANNEL),窗体背景色

#define LGRAYBLUE      	0XA651 //浅灰蓝色(中间层颜色)
#define LBBLUE          0X2B12 //浅棕蓝色(选择条目的反色)
	    															  
extern u16 BACK_COLOR, POINT_COLOR ;  

void HAL_LCD_SPIv_WriteData(uint8_t Data);
uint8_t HAL_SPI_WriteByte(SPI_HandleTypeDef *hspi, uint8_t TxData);
void LCD_WR_REG(uint16_t data);
void LCD_WR_DATA(uint8_t data);
void LCD_WR_DATA_16Bit(u16 data);
void LCD_WriteReg(u16 LCD_Reg, u16 LCD_RegValue);
void LCD_WriteRAM_Prepare(void);
void LCD_DrawPoint(u16 x, u16 y);
void LCD_GPIOInit(void);
void LCD_RESET(void);
void LCD_Init(void);
void LCD_Clear(uint16_t Color);
void LCD_SetWindows(uint16_t xStart, uint16_t yStart, uint16_t xEnd, uint16_t yEnd);
void LCD_SetCursor(u16 Xpos, u16 Ypos);
void LCD_SetParam(void);


#endif
