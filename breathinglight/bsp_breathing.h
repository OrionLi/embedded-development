//#ifndef __PWM_BREATHING_H
//#define	__PWM_BREATHING_H

//#include "stm32f10x.h"

//void TIM3_Breathing_Init(void);

//#endif /* __PWM_BREATHING_H */


#ifndef __PWM_BREATHING_H
#define	__PWM_BREATHING_H

#include "stm32f10x.h"


/********************TIM参数定义，只限TIM2、3、4或5**************************/
#define             macTIMx                                TIM3
#define             macTIM_APBxClock_FUN                   RCC_APB1PeriphClockCmd
#define             macTIM_CLK                             RCC_APB1Periph_TIM3
#define             macTIM_GPIO_APBxClock_FUN              RCC_APB2PeriphClockCmd
#define             macTIM_GPIO_CLK                        RCC_APB2Periph_GPIOB
#define             macTIM_LED_PORT                        GPIOB
#define             macTIM_LED_PIN                         GPIO_Pin_0
#define             macTIM_OCxInit                         TIM_OC3Init            //通道选择，1~4
#define             macTIM_OCxPreloadConfig                TIM_OC3PreloadConfig 
#define             macCCRx                                CCR3
#define             macTIMx_IRQn                           TIM3_IRQn               //中断
#define             macTIMx_IRQHandler                     TIM3_IRQHandler

void                TIMx_Breathing_Init                    (void);

extern uint8_t breathing_start;  // 声明为外部变量，不进行定义

#endif /* __PWM_BREATHING_H */

