
#include "bsp_breathing.h"


/* LED亮度等级 PWM表 */
uint8_t indexWave[] = {1,1,2,2,3,4,6,8,10,14,19,25,33,44,59,80,
    107,143,191,255,255,191,143,107,80,59,44,33,25,19,14,10,8,6,4,3,2,2,1,1};

uint8_t breathing_start = 0;  // 新增全局变量定义

 /**
  * @brief  配置TIM3复用输出PWM时用到的I/O
  * @param  无
  * @retval 无
  */
static void TIMx_GPIO_Config(void) 
{
  GPIO_InitTypeDef GPIO_InitStructure;
  macTIM_GPIO_APBxClock_FUN  ( macTIM_GPIO_CLK, ENABLE );
  GPIO_InitStructure.GPIO_Pin =  macTIM_LED_PIN ;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;		    // 复用推挽输出
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_Init( macTIM_LED_PORT, &GPIO_InitStructure );
}

static void NVIC_Config_PWM(void)
{
  NVIC_InitTypeDef NVIC_InitStructure;
  NVIC_PriorityGroupConfig(NVIC_PriorityGroup_1);
  NVIC_InitStructure.NVIC_IRQChannel = macTIMx_IRQn;
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 2;
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
  NVIC_Init(&NVIC_InitStructure);
}

static void TIMx_Mode_Config(void)
{
  TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
  TIM_OCInitTypeDef  TIM_OCInitStructure;																				
  macTIM_APBxClock_FUN ( macTIM_CLK, ENABLE );

  TIM_TimeBaseStructure.TIM_Period = 255;       
  TIM_TimeBaseStructure.TIM_Prescaler = 1999;	    						
  TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1 ;			
  TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;  	
  TIM_TimeBaseInit(macTIMx, &TIM_TimeBaseStructure);

  TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1;	    				
  TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;	
  TIM_OCInitStructure.TIM_Pulse = 0;								  			
  TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_Low;  	  	
  macTIM_OCxInit ( macTIMx, &TIM_OCInitStructure );							
  macTIM_OCxPreloadConfig ( macTIMx, TIM_OCPreload_Enable );		
  TIM_ARRPreloadConfig(macTIMx, ENABLE);												
  if (breathing_start) {
    TIM_Cmd(macTIMx, ENABLE);	// 启动定时器
  } else {
    TIM_Cmd(macTIMx, DISABLE); // 停止定时器
  }
  TIM_ITConfig(macTIMx, TIM_IT_Update, ENABLE);								
  NVIC_Config_PWM();																						
}

void TIMx_Breathing_Init(void)
{
  TIMx_GPIO_Config();
  TIMx_Mode_Config();	
}
