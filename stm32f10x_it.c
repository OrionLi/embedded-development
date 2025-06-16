#include "stm32f10x_it.h"
#include "bsp_breathing.h"

extern uint8_t indexWave[];

/* �������жϷ����� */
void macTIMx_IRQHandler(void)
{	
	static uint8_t pwm_index = 0;			//����PWM���
	static uint8_t period_cnt = 0;		//���ڼ���������
	
	if (TIM_GetITStatus(macTIMx, TIM_IT_Update) != RESET)	//TIM_IT_Update
 	{			
			period_cnt++;
			if(period_cnt >= 10)										//�����������������10�������һ��������PWM��
			{
				
				macTIMx->macCCRx = indexWave[pwm_index];	//����PWM���޸Ķ�ʱ���ıȽϼĴ���ֵ
				pwm_index++;												//��־PWM�����һ��Ԫ��
			
				if( pwm_index >=  40)								//��PWM������Ѿ�������һ�飬����PWM����־
				{
					pwm_index=0;								
				}
				
				period_cnt=0;												//�������ڼ�����־
		}
		TIM_ClearITPendingBit (macTIMx, TIM_IT_Update);	//����Ҫ����жϱ�־λ
	}
}

