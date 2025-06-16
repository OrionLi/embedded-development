#include "stm32f10x.h"
#include "bsp_breathing.h"

int main(void)
{
    // ��ǰ��������
    GPIO_InitTypeDef GPIO_InitStructure;
    EXTI_InitTypeDef EXTI_InitStructure;
    NVIC_InitTypeDef NVIC_InitStructure;
    uint32_t i;
    
    /* ����GPIOʱ�� */
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA | RCC_APB2Periph_GPIOC | RCC_APB2Periph_GPIOB | RCC_APB2Periph_AFIO, ENABLE);
    
    /* ȷ��LED��ʼ״̬Ϊ�͵�ƽ (�ر�) */
    GPIO_InitStructure.GPIO_Pin = macTIM_LED_PIN;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;  // ��ʱ����Ϊ�������
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(macTIM_LED_PORT, &GPIO_InitStructure);
    GPIO_ResetBits(macTIM_LED_PORT, macTIM_LED_PIN);  // ȷ������͵�ƽ
    
    /* ����K1����Ϊ�������� (��ѯģʽ) */
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
    GPIO_Init(GPIOA, &GPIO_InitStructure);
    
    /* ����K2����Ϊ�������� (�ж�ģʽ) */
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_13;
    GPIO_Init(GPIOC, &GPIO_InitStructure);
    
    /* ����K2���ⲿ�ж� */
    GPIO_EXTILineConfig(GPIO_PortSourceGPIOC, GPIO_PinSource13);
    
    EXTI_InitStructure.EXTI_Line = EXTI_Line13;
    EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
    EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Rising;
    EXTI_InitStructure.EXTI_LineCmd = ENABLE;
    EXTI_Init(&EXTI_InitStructure);
    
    /* ����K2�жϵ�NVIC */
    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_1);
    NVIC_InitStructure.NVIC_IRQChannel = EXTI15_10_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure);
    
    /* ��ʼ�������� */
    TIMx_Breathing_Init();
    
    /* ȷ�������Ƴ�ʼ״̬Ϊ�ر� */
    breathing_start = 1;
    TIM_Cmd(macTIMx, DISABLE);
    
    /* ��LED������������ΪPWM���ģʽ */
    GPIO_InitStructure.GPIO_Pin = macTIM_LED_PIN;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;  // �����������
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(macTIM_LED_PORT, &GPIO_InitStructure);
    
    /* ����ȷ��PWM���Ϊ0 */
    macTIMx->CCR3 = 0;  // ֱ������PWMռ�ձ�Ϊ0
    
    while(1) {
        /* ��ѯK1����״̬ (���͵�ƽ) */
        if (GPIO_ReadInputDataBit(GPIOA, GPIO_Pin_0) == 1) {
            // ������
            for(i = 0; i < 100000; i++);
            
            if (GPIO_ReadInputDataBit(GPIOA, GPIO_Pin_0) == 1) {
                breathing_start = 1;  // ����������
                TIM_Cmd(macTIMx, ENABLE);  // ������ʱ��
                
                // �ȴ������ͷ�
                while(GPIO_ReadInputDataBit(GPIOA, GPIO_Pin_0) == 1);
            }
        }
    }
}

/* K2�����жϴ����� */
void EXTI15_10_IRQHandler(void)
{
    if (EXTI_GetITStatus(EXTI_Line13) != RESET) {
        /* ����ȷ��PWM���Ϊ0 */
        macTIMx->CCR3 = 0;
		TIM_GenerateEvent(macTIMx,TIM_EventSource_Update);
		breathing_start = 0;  // ֹͣ������
        TIM_Cmd(macTIMx, DISABLE);  // ֹͣ��ʱ��
        
        
        EXTI_ClearITPendingBit(EXTI_Line13);  // ����жϱ�־λ
    }
}
    

