#include "lcd.h"


//管理LCD重要参数
//默认为竖屏
_lcd_dev lcddev;

//画笔颜色,背景颜色
uint16_t POINT_COLOR = 0x0000,BACK_COLOR = 0xFFFF;  
uint16_t DeviceCode;	

/****************************************************************************
* 名    称：void  SPIv_WriteData(u8 Data)
* 功    能：STM32_模拟SPI写一个字节数据底层函数
* 入口参数：Data
* 出口参数：无
* 说    明：STM32_模拟SPI读写一个字节数据底层函数
****************************************************************************/
void HAL_LCD_SPIv_WriteData(uint8_t Data) {
  for (uint8_t i = 8; i > 0; i--) {
    if (Data & 0x80) {
      HAL_GPIO_WritePin(GPIOB, LCD_SDA, GPIO_PIN_SET);
    } else {
      HAL_GPIO_WritePin(GPIOB, LCD_SDA, GPIO_PIN_RESET);
    }
	   
    HAL_GPIO_WritePin(GPIOB, LCD_SCL, GPIO_PIN_RESET);
    HAL_GPIO_WritePin(GPIOB, LCD_SCL, GPIO_PIN_SET);
    Data <<= 1; 
  }
}



/****************************************************************************
* 名    称：u8 SPI_WriteByte(SPI_TypeDef* SPIx,u8 Byte)
* 功    能：STM32_硬件SPI读写一个字节数据底层函数
* 入口参数：SPIx,Byte
* 出口参数：返回总线收到的数据
* 说    明：STM32_硬件SPI读写一个字节数据底层函数
****************************************************************************/
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


//******************************************************************
//函数名：  LCD_WR_REG
//功能：    向液晶屏总线写入写16位指令
//输入参数：Reg:待写入的指令值
//返回值：  无
//修改记录：无
//******************************************************************
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


//******************************************************************
//函数名：  LCD_WR_DATA
//功能：    向液晶屏总线写入写8位数据
//输入参数：Data:待写入的数据
//返回值：  无
//修改记录：无
//******************************************************************
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

//******************************************************************
//函数名：  LCD_DrawPoint_16Bit
//功能：    8位总线下如何写入一个16位数据
//输入参数：(x,y):光标坐标
//返回值：  无
//修改记录：无
//******************************************************************
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

//******************************************************************
//函数名：  LCD_WriteReg
//功能：    写寄存器数据
//输入参数：LCD_Reg:寄存器地址
//			LCD_RegValue:要写入的数据
//返回值：  无
//修改记录：无
//******************************************************************
void LCD_WriteReg(u16 LCD_Reg, u16 LCD_RegValue)
{	
	LCD_WR_REG(LCD_Reg);  
	LCD_WR_DATA(LCD_RegValue);
}	   

//******************************************************************
//函数名：  LCD_WriteRAM_Prepare
//功能：    开始写GRAM
//			在给液晶屏传送RGB数据前，应该发送写GRAM指令
//输入参数：无
//返回值：  无
//修改记录：无
//******************************************************************
void LCD_WriteRAM_Prepare(void)
{
	LCD_WR_REG(lcddev.wramcmd);
}
	   
//******************************************************************
//函数名：  LCD_DrawPoint
//功能：    在指定位置写入一个像素点数据
//输入参数：(x,y):光标坐标
//返回值：  无
//修改记录：无
//******************************************************************
void LCD_DrawPoint(u16 x, u16 y)
{
	LCD_SetCursor(x, y);         // 设置光标位置
	LCD_WR_DATA_16Bit(POINT_COLOR);
}

//******************************************************************
//函数名：  LCD_GPIOInit
//功能：    液晶屏IO初始化，液晶初始化前要调用此函数
//输入参数：无
//返回值：  无
//修改记录：无
//******************************************************************
void LCD_GPIOInit(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	      
	__HAL_RCC_GPIOC_CLK_ENABLE();
	__HAL_RCC_GPIOB_CLK_ENABLE();
	
	GPIO_InitStructure.Pin = GPIO_PIN_12 | GPIO_PIN_13 | GPIO_PIN_14 | GPIO_PIN_15;
	GPIO_InitStructure.Mode = GPIO_MODE_OUTPUT_PP;
	GPIO_InitStructure.Speed = GPIO_SPEED_FREQ_HIGH;
	HAL_GPIO_Init(GPIOB, &GPIO_InitStructure);  

	GPIO_InitStructure.Pin = GPIO_PIN_6;
	GPIO_InitStructure.Mode = GPIO_MODE_OUTPUT_PP;
	GPIO_InitStructure.Speed = GPIO_SPEED_FREQ_HIGH;
	HAL_GPIO_Init(GPIOC, &GPIO_InitStructure);
}

//******************************************************************
//函数名：  LCD_RESET
//功能：    LCD复位函数，液晶初始化前要调用此函数
//输入参数：无
//返回值：  无
//修改记录：无
//******************************************************************
void LCD_RESET(void)
{

	delay_us(100);

	delay_us(50);
}

//******************************************************************
//函数名：  LCD_Init
//功能：    LCD初始化
//输入参数：无
//返回值：  无
//修改记录：无
//******************************************************************
void LCD_Init(void)
{  
										 
	LCD_RESET(); //液晶屏复位

	//************* Start Initial Sequence **********//		
	//开始初始化液晶屏
	LCD_WR_REG(0x11);//Sleep exit 
	HAL_Delay(120);		
	//ST7735R Frame Rate
	LCD_WR_REG(0xB1); 
	LCD_WR_DATA(0x01); 
	LCD_WR_DATA(0x2C); 
	LCD_WR_DATA(0x2D); 

	LCD_WR_REG(0xB2); 
	LCD_WR_DATA(0x01); 
	LCD_WR_DATA(0x2C); 
	LCD_WR_DATA(0x2D); 

	LCD_WR_REG(0xB3); 
	LCD_WR_DATA(0x01); 
	LCD_WR_DATA(0x2C); 
	LCD_WR_DATA(0x2D); 
	LCD_WR_DATA(0x01); 
	LCD_WR_DATA(0x2C); 
	LCD_WR_DATA(0x2D); 
	
	LCD_WR_REG(0xB4); //Column inversion 
	LCD_WR_DATA(0x07); 
	
	//ST7735R Power Sequence
	LCD_WR_REG(0xC0); 
	LCD_WR_DATA(0xA2); 
	LCD_WR_DATA(0x02); 
	LCD_WR_DATA(0x84); 
	LCD_WR_REG(0xC1); 
	LCD_WR_DATA(0xC5); 

	LCD_WR_REG(0xC2); 
	LCD_WR_DATA(0x0A); 
	LCD_WR_DATA(0x00); 

	LCD_WR_REG(0xC3); 
	LCD_WR_DATA(0x8A); 
	LCD_WR_DATA(0x2A); 
	LCD_WR_REG(0xC4); 
	LCD_WR_DATA(0x8A); 
	LCD_WR_DATA(0xEE); 
	
	LCD_WR_REG(0xC5); //VCOM 
	LCD_WR_DATA(0x0E); 
	
	LCD_WR_REG(0x36); //MX, MY, RGB mode 				 
	LCD_WR_DATA(0xC8); 
	
	//ST7735R Gamma Sequence
	LCD_WR_REG(0xe0); 
	LCD_WR_DATA(0x0f); 
	LCD_WR_DATA(0x1a); 
	LCD_WR_DATA(0x0f); 
	LCD_WR_DATA(0x18); 
	LCD_WR_DATA(0x2f); 
	LCD_WR_DATA(0x28); 
	LCD_WR_DATA(0x20); 
	LCD_WR_DATA(0x22); 
	LCD_WR_DATA(0x1f); 
	LCD_WR_DATA(0x1b); 
	LCD_WR_DATA(0x23); 
	LCD_WR_DATA(0x37); 
	LCD_WR_DATA(0x00); 	
	LCD_WR_DATA(0x07); 
	LCD_WR_DATA(0x02); 
	LCD_WR_DATA(0x10); 

	LCD_WR_REG(0xe1); 
	LCD_WR_DATA(0x0f); 
	LCD_WR_DATA(0x1b); 
	LCD_WR_DATA(0x0f); 
	LCD_WR_DATA(0x17); 
	LCD_WR_DATA(0x33); 
	LCD_WR_DATA(0x2c); 
	LCD_WR_DATA(0x29); 
	LCD_WR_DATA(0x2e); 
	LCD_WR_DATA(0x30); 
	LCD_WR_DATA(0x30); 
	LCD_WR_DATA(0x39); 
	LCD_WR_DATA(0x3f); 
	LCD_WR_DATA(0x00); 
	LCD_WR_DATA(0x07); 
	LCD_WR_DATA(0x03); 
	LCD_WR_DATA(0x10);  
	
	LCD_WR_REG(0x2a);
	LCD_WR_DATA(0x00);
	LCD_WR_DATA(0x00);
	LCD_WR_DATA(0x00);
	LCD_WR_DATA(0x7f);

	LCD_WR_REG(0x2b);
	LCD_WR_DATA(0x00);
	LCD_WR_DATA(0x00);
	LCD_WR_DATA(0x00);
	LCD_WR_DATA(0x9f);
	
	LCD_WR_REG(0xF0); //Enable test command  
	LCD_WR_DATA(0x01); 
	LCD_WR_REG(0xF6); //Disable ram power save mode 
	LCD_WR_DATA(0x00); 
	
	LCD_WR_REG(0x3A); //65k mode 
	LCD_WR_DATA(0x05); 	
	LCD_WR_REG(0x29);//Display on	

	LCD_SetParam();//设置LCD参数	 
//	HAL_GPIO_WritePin(GPIOB, GPIO_PIN_9, GPIO_PIN_SET);//点亮背光	 
	//LCD_Clear(WHITE);
}

//******************************************************************
//函数名：  LCD_Clear
//功能：    LCD全屏填充清屏函数
//输入参数：Color:要清屏的填充色
//返回值：  无
//修改记录：无
//******************************************************************
void LCD_Clear(uint16_t Color)
{
	uint32_t i, j;      
	LCD_SetWindows(0, 0, lcddev.width - 1, lcddev.height - 1);	  
	for(i = 0; i < lcddev.width; i++)
	{
		for(j = 0; j < lcddev.height; j++)
			LCD_WR_DATA_16Bit(Color);	//写入数据 	 
	}
}

/*************************************************
函数名：LCD_SetWindows
功能：设置lcd显示窗口，在此区域写点数据自动换行
入口参数：xy起点和终点
返回值：无
*************************************************/
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

/*************************************************
函数名：LCD_SetCursor
功能：设置光标位置
入口参数：xy坐标
返回值：无
*************************************************/
void LCD_SetCursor(u16 Xpos, u16 Ypos)
{	  	    			
	LCD_SetWindows(Xpos,Ypos,Xpos,Ypos);
} 

//设置LCD参数
//方便进行横竖屏模式切换
void LCD_SetParam(void)
{ 	
	lcddev.wramcmd=0x2C;
#if USE_HORIZONTAL==1	//使用横屏	  
	lcddev.dir=1;//横屏
	lcddev.width=128;
	lcddev.height=128;
	lcddev.setxcmd=0x2A;
	lcddev.setycmd=0x2B;			
	LCD_WriteReg(0x36,0xA8);

#else//竖屏
	lcddev.dir=0;//竖屏				 	 		
	lcddev.width=128;
	lcddev.height=128;
	lcddev.setxcmd=0x2A;
	lcddev.setycmd=0x2B;	
	LCD_WriteReg(0x36,0xC8);
#endif
}	



