#include "stm32f10x.h"                  // Device header

void PWM_Init(void)
{
	    GPIO_InitTypeDef GPIO_InitStructure;
      TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure;
      TIM_OCInitTypeDef TIM_OCInitStructure;

      //ʹ�ܶ�ʱ��TIM4ʱ�ӣ�ע��TIM4ʱ��ΪAPB1
      RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM4, ENABLE);
      //ʹ��PWM���GPIOʱ��
      RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB , ENABLE); 
                                                                          
      GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6;//��ʱ��TIM4��PWM���ͨ��1,TIM4_CH1
      GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;//�����������
      GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
      GPIO_Init(GPIOB, &GPIO_InitStructure);//��ʼ��GPIO

      GPIO_InitStructure.GPIO_Pin = GPIO_Pin_7;//��ʱ��TIM4��PWM���ͨ��2,TIM4_CH2
      GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;//�����������
      GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
      GPIO_Init(GPIOB, &GPIO_InitStructure);//��ʼ��GPIO

      GPIO_InitStructure.GPIO_Pin = GPIO_Pin_8;//��ʱ��TIM4��PWM���ͨ��3,TIM4_CH3
      GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;//�����������
      GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
      GPIO_Init(GPIOB, &GPIO_InitStructure);//��ʼ��GPIO

      GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9;//��ʱ��TIM4��PWM���ͨ��4,TIM4_CH4
      GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;//�����������
      GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
      GPIO_Init(GPIOB, &GPIO_InitStructure);//��ʼ��GPIO

      TIM_TimeBaseStructure.TIM_Period = 100 - 1;//arr;//自动装载值
      TIM_TimeBaseStructure.TIM_Prescaler =36 - 1;//psc; //ʱ��Ԥ��Ƶ��
      TIM_TimeBaseStructure.TIM_ClockDivision = 0;
      TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;//TIM���ϼ���ģʽ
      TIM_TimeBaseInit(TIM4, &TIM_TimeBaseStructure); //��ʼ��TIM4
     
      //��ʼ��TIM4_CH1��PWMģʽ
      TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1;//??PWM??1
      TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;//??????
      TIM_OCInitStructure.TIM_Pulse = 0; //
      TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High;//??????
      TIM_OC1Init(TIM4, &TIM_OCInitStructure);//???TIM4_CH1

      //��ʼ��TIM4_CH2��PWMģʽ
      TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1;
      TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;
      TIM_OCInitStructure.TIM_Pulse = 0;
      TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High;
       //TIM4_CH2��ʼ��,OC2
      TIM_OC2Init(TIM4, &TIM_OCInitStructure);

       //��ʼ��TIM4_CH3��PWMģʽ
      TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1;
      TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;
      TIM_OCInitStructure.TIM_Pulse = 0;
      TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High;
      TIM_OC3Init(TIM4, &TIM_OCInitStructure);

      //��ʼ��TIM4_CH4��PWMģʽ
      TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1;
      TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;
      TIM_OCInitStructure.TIM_Pulse = 0;
      TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High;
      TIM_OC4Init(TIM4, &TIM_OCInitStructure);

      //ʹ��4��ͨ����Ԥװ�ؼĴ���
      TIM_OC1PreloadConfig(TIM4, TIM_OCPreload_Enable);//OC1
      TIM_OC2PreloadConfig(TIM4, TIM_OCPreload_Enable);//OC2
      TIM_OC3PreloadConfig(TIM4, TIM_OCPreload_Enable);//OC3
      TIM_OC4PreloadConfig(TIM4, TIM_OCPreload_Enable);//OC4
      TIM_ARRPreloadConfig(TIM4, ENABLE); //ʹ����װ�Ĵ���

      TIM_Cmd(TIM4, ENABLE);//ʹ�ܶ�ʱ��TIM4,׼������ 
}

