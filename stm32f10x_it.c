#include "stm32f10x_it.h"
#include "bsp_breathing.h"

extern uint8_t indexWave[];

/* 呼吸灯中断服务函数 */
void macTIMx_IRQHandler(void)
{	
	static uint8_t pwm_index = 0;			//用于PWM查表
	static uint8_t period_cnt = 0;		//用于计算周期数
	
	if (TIM_GetITStatus(macTIMx, TIM_IT_Update) != RESET)	//TIM_IT_Update
 	{			
			period_cnt++;
			if(period_cnt >= 10)										//若输出的周期数大于10，输出下一种脉冲宽的PWM波
			{
				
				macTIMx->macCCRx = indexWave[pwm_index];	//根据PWM表修改定时器的比较寄存器值
				pwm_index++;												//标志PWM表的下一个元素
			
				if( pwm_index >=  40)								//若PWM脉冲表已经输出完成一遍，重置PWM查表标志
				{
					pwm_index=0;								
				}
				
				period_cnt=0;												//重置周期计数标志
		}
		TIM_ClearITPendingBit (macTIMx, TIM_IT_Update);	//必须要清除中断标志位
	}
}

