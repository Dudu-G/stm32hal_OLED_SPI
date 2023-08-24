#include "delay_us.h"

//设置微妙级延时函数delay_us微秒级延时函数
extern TIM_HandleTypeDef htim6;
	#define DLY_TIM_Handle (&htim6)
	void delay_us(uint16_t nus)
	{
		__HAL_TIM_SET_COUNTER(DLY_TIM_Handle, 0);
		__HAL_TIM_ENABLE(DLY_TIM_Handle);
		while (__HAL_TIM_GET_COUNTER(DLY_TIM_Handle) < nus)
		{
		}
		__HAL_TIM_DISABLE(DLY_TIM_Handle);
	}
