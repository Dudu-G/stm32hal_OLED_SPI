# STM32HAL库 SPI通讯之TFT1.44屏ST7735驱动
本例程基于一个开源的模板，向HAL库进行移植

本功能包为驱动TFT1.44寸屏幕封装代码，共包含7个文件：
lcd.c、lcd.h、show.c、show.h、font.h、delay_us.c、delay_us.h。

## 前期配置部分：
>需要在CubeMX软件内对SPI进行前期配置（系统配置正常72MHz配置），由于屏幕数据传输中需要用到微秒级延时函数，所以调用了其它功能包中的微秒级延时函数，一同放到这个包里面。微秒级延时函数需要开启定时器6，不用改动配置，只需开启和设置预分频即可。  
***前期配置部分有图片作参考。***
## 函数代码的讲解部分
### 在lcd.h中
```c
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
```
在头文件中定义包含屏幕各项数据的结构体，方便调用，后续在.c文件中，定义该结构体变量使用。 
```c
/////////////////////////////////////用户配置区///////////////////////////////////	 
//支持横竖屏快速定义切换，支持8/16位模式切换
#define USE_HORIZONTAL  	0	//定义是否使用横屏 		0,不使用.1,使用.
//使用硬件SPI 还是模拟SPI作为测试
#define USE_HARDWARE_SPI    1  //1:Enable Hardware SPI;0:USE Soft SPI
//////////////////////////////////////////////////////////////////////////////////	 
```
在这个宏定义中，可以对两个宏定义参数进行修改，实现对屏幕模式、SPI模式的传输方式进行改变，提高屏幕的实用性。  （在头文件中，后面的代码都是对一些基本信息进行初始化，其中都有注释。  
### 在lcd.c中
```c
uint8_t HAL_SPI_WriteByte(SPI_HandleTypeDef *hspi, uint8_t TxData) {
    uint8_t rxData;
    HAL_StatusTypeDef status;
    
    status = HAL_SPI_TransmitReceive(hspi, &TxData, &rxData, 1, HAL_MAX_DELAY);
    
    if (status != HAL_OK) {
        // Handle error here, if needed
        return 0;
    }
    
    return rxData;
}
```
该函数是读写函数，将要发的数据通过SPI通讯发送到元器件上，顺便返回接收到的数据。  
需要传入的参数是：**SPI**的结构体和需要发送的数据。  
函数具体实现流程：定义一个**rxData**变量接收传回来的数据，定义一个**status**结构体来接收**HAL_SPI_TransmitReceive** 返回的值，如果函数正常运行，会返回HAL_OK这个值。我们可以对这个值进行判断，如果返回了其它值，可以在判断语句里面提醒一下报错了，告诉用户数据没传到。函数最后返回读到的rxData这个值。  
___
```c
void LCD_WR_REG(uint16_t data)
{ 
  LCD_CS_CLR;
  LCD_RS_CLR;
  
#if USE_HARDWARE_SPI
  HAL_SPI_Transmit(&hspi2, (uint8_t*)&data, 1, HAL_MAX_DELAY); // 使用 HAL 库的 SPI 传输函数
#else
  HAL_LCD_SPIv_WriteData(data);  // 使用你的 HAL 库的函数
#endif
  
  LCD_CS_SET;
}
```
该函数是写入函数，向屏幕写入16位指令  
需要传入的参数是：16位的**data**数据  
函数具体实现流程：看宏定义可知，**LCD_CS_CLR和LCD_RS_CLR**是将**GPIO_PIN_6(RS)和GPIO_PIN_12(CS)**两个引脚拉低。后面的判断语句是判断使用硬件SPI还是软件SPI传输，然后发送data数据，我已经将软件SPI代码删掉了。最后再将CS引脚拉高。  
___
```c
void LCD_WR_DATA(uint8_t data)
{
    LCD_CS_CLR;
    LCD_RS_SET;

#if USE_HARDWARE_SPI
    HAL_SPI_Transmit(&hspi2, &data, 1, HAL_MAX_DELAY);
#else
    HAL_LCD_SPIv_WriteData(data); // 自定义的 HAL_SPIv_WriteData 函数
#endif

    LCD_CS_SET;
}
```
该函数也是写入函数，向屏幕写入8位指令  
需要传入的参数是：8位的data数据  
函数具体实现流程：看宏定义可知，**LCD_CS_CLR和LCD_RS_SET**一个是将**GPIO_PIN_12(CS)拉低**一个是将**GPIO_PIN_6(RS)拉高**。后面的判断语句是判断使用硬件SPI还是软件SPI传输，然后发送data数据，我已经将软件SPI代码删掉了。最后再将CS引脚拉高。   
___
```c
void LCD_WR_DATA_16Bit(u16 data)
{	
   LCD_CS_CLR;
   LCD_RS_SET;

#if USE_HARDWARE_SPI   
   // 使用硬件SPI写入数据
	uint8_t buf[2];
	buf[0] = data >> 8;
	buf[1] = data;
   HAL_SPI_Transmit(&hspi2, buf, sizeof(buf), HAL_MAX_DELAY);
#else
   // 使用软件SPI写入数据
   SPIv_WriteData(data >> 8);
   SPIv_WriteData(data);
#endif 

   LCD_CS_SET;
}
```
该函数是将16位数据传入8位总线上
需要传入的参数是：16位的data数据  
函数具体实现流程：看宏定义可知，**LCD_CS_CLR和LCD_RS_SET**一个是将**GPIO_PIN_12(CS)拉低**一个是将**GPIO_PIN_6(RS)拉高**。定义一个8位数组把16位数据储存起来，然后发送传入的16位数据   
___
```c
void LCD_WriteReg(u16 LCD_Reg, u16 LCD_RegValue)
{	
	LCD_WR_REG(LCD_Reg);  
	LCD_WR_DATA(LCD_RegValue);
}	   
```
该函数是将要写入的数据传入相应的寄存器内  
需要传入的参数是：16位的寄存器地址，16位的data数据  
函数具体实现流程：将这两个数据用两个发送函数进行发送，先发送16位的寄存器地址定位到寄存器，然后将16位数据传入到八位的发送函数中，取低八位数据发送。
___
```c
void LCD_WriteRAM_Prepare(void)
{
	LCD_WR_REG(lcddev.wramcmd);
}
```
该函数是开始写GRAM指令  
将16位的lcddev.wramcmd数据传进去  
___
```c
void LCD_DrawPoint(u16 x, u16 y)
{
	LCD_SetCursor(x, y);         // 设置光标位置
	LCD_WR_DATA_16Bit(POINT_COLOR);
}
```
该函数是在指定位置写入一个像素点数据  
需要传入的参数是：光标的x,y坐标两个u16类型的数  
___
```c
void LCD_SetCursor(u16 Xpos, u16 Ypos)
{	  	    			
	LCD_SetWindows(Xpos,Ypos,Xpos,Ypos);
} 	
```
该函数是设置光标的位置  
需要传入的参数是：光标两个u16类型的x,y坐标数  
___
```c
void LCD_SetWindows(uint16_t xStart, uint16_t yStart, uint16_t xEnd, uint16_t yEnd)
{
#if USE_HORIZONTAL == 1 //使用横屏
	LCD_WR_REG(lcddev.setxcmd);
	LCD_WR_DATA(xStart >> 8);
	LCD_WR_DATA(0x00FF & xStart + 3);
	LCD_WR_DATA(xEnd >> 8);
	LCD_WR_DATA(0x00FF & xEnd + 3);

	LCD_WR_REG(lcddev.setycmd);
	LCD_WR_DATA(yStart >> 8);
	LCD_WR_DATA(0x00FF & yStart + 2);
	LCD_WR_DATA(yEnd >> 8);
	LCD_WR_DATA(0x00FF & yEnd + 2);

#else

	LCD_WR_REG(lcddev.setxcmd);
	LCD_WR_DATA(xStart >> 8);
	LCD_WR_DATA(0x00FF & xStart + 2);
	LCD_WR_DATA(xEnd >> 8);
	LCD_WR_DATA(0x00FF & xEnd + 2);

	LCD_WR_REG(lcddev.setycmd);
	LCD_WR_DATA(yStart >> 8);
	LCD_WR_DATA(0x00FF & yStart + 3);
	LCD_WR_DATA(yEnd >> 8);
	LCD_WR_DATA(0x00FF & yEnd + 3);
#endif

	LCD_WriteRAM_Prepare(); //开始写入GRAM				
}
```
该函数是设置lcd显示窗口，在此区域写点数据自动换行  
需要传入的参数是：需要传入xy起点和终点数据  
