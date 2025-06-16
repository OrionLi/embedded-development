#include "stm32f10x.h"
#include "bsp_breathing.h"

int main(void)
{
    // 提前声明变量
    GPIO_InitTypeDef GPIO_InitStructure;
    EXTI_InitTypeDef EXTI_InitStructure;
    NVIC_InitTypeDef NVIC_InitStructure;
    uint32_t i;
    
    /* 配置GPIO时钟 */
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA | RCC_APB2Periph_GPIOC | RCC_APB2Periph_GPIOB | RCC_APB2Periph_AFIO, ENABLE);
    
    /* 确保LED初始状态为低电平 (关闭) */
    GPIO_InitStructure.GPIO_Pin = macTIM_LED_PIN;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;  // 临时配置为推挽输出
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(macTIM_LED_PORT, &GPIO_InitStructure);
    GPIO_ResetBits(macTIM_LED_PORT, macTIM_LED_PIN);  // 确保输出低电平
    
    /* 配置K1按键为浮空输入 (轮询模式) */
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
    GPIO_Init(GPIOA, &GPIO_InitStructure);
    
    /* 配置K2按键为浮空输入 (中断模式) */
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_13;
    GPIO_Init(GPIOC, &GPIO_InitStructure);
    
    /* 配置K2的外部中断 */
    GPIO_EXTILineConfig(GPIO_PortSourceGPIOC, GPIO_PinSource13);
    
    EXTI_InitStructure.EXTI_Line = EXTI_Line13;
    EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
    EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Rising;
    EXTI_InitStructure.EXTI_LineCmd = ENABLE;
    EXTI_Init(&EXTI_InitStructure);
    
    /* 配置K2中断的NVIC */
    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_1);
    NVIC_InitStructure.NVIC_IRQChannel = EXTI15_10_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure);
    
    /* 初始化呼吸灯 */
    TIMx_Breathing_Init();
    
    /* 确保呼吸灯初始状态为关闭 */
    breathing_start = 1;
    TIM_Cmd(macTIMx, DISABLE);
    
    /* 将LED引脚重新配置为PWM输出模式 */
    GPIO_InitStructure.GPIO_Pin = macTIM_LED_PIN;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;  // 复用推挽输出
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(macTIM_LED_PORT, &GPIO_InitStructure);
    
    /* 额外确保PWM输出为0 */
    macTIMx->CCR3 = 0;  // 直接设置PWM占空比为0
    
    while(1) {
        /* 轮询K1按键状态 (检测低电平) */
        if (GPIO_ReadInputDataBit(GPIOA, GPIO_Pin_0) == 1) {
            // 简单消抖
            for(i = 0; i < 100000; i++);
            
            if (GPIO_ReadInputDataBit(GPIOA, GPIO_Pin_0) == 1) {
                breathing_start = 1;  // 启动呼吸灯
                TIM_Cmd(macTIMx, ENABLE);  // 启动定时器
                
                // 等待按键释放
                while(GPIO_ReadInputDataBit(GPIOA, GPIO_Pin_0) == 1);
            }
        }
    }
}

/* K2按键中断处理函数 */
void EXTI15_10_IRQHandler(void)
{
    if (EXTI_GetITStatus(EXTI_Line13) != RESET) {
        /* 额外确保PWM输出为0 */
        macTIMx->CCR3 = 0;
		TIM_GenerateEvent(macTIMx,TIM_EventSource_Update);
		breathing_start = 0;  // 停止呼吸灯
        TIM_Cmd(macTIMx, DISABLE);  // 停止定时器
        
        
        EXTI_ClearITPendingBit(EXTI_Line13);  // 清除中断标志位
    }
}
    

