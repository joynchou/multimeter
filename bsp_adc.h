#ifndef __BSP_ADC_H
#define	__BSP_ADC_H


#include "stm32f10x.h"

// ADC GPIO�궨��
// ע�⣺����ADC�ɼ���IO����û�и��ã�����ɼ���ѹ����Ӱ��
#define    ADC_GPIO_APBxClock_FUN        RCC_APB2PeriphClockCmd
//#define    ADC_GPIO_CLK                  RCC_APB2Periph_GPIOC  
//#define    ADC_PORT                      GPIOC
//#define    ADC_PIN                       GPIO_Pin_1


// ADC ���ѡ��
// ������ ADC1/2�����ʹ��ADC3���ж���ص�Ҫ�ĳ�ADC3��
#define    ADC_APBxClock_FUN             RCC_APB2PeriphClockCmd
//#define    ADC_x                         ADC2
#define    ADC_CLK                       RCC_APB2Periph_ADC2

// ADC ͨ���궨��
//#define    ADC_CHANNEL                   ADC_Channel_11

// ADC �ж���غ궨��
#define    ADC_IRQ                       ADC1_2_IRQn
#define    ADC_IRQHandler                ADC1_2_IRQHandler

void ADCx_Init(void);

//����ת����ʼ���adc�жϵ��õĻص�����
void setCallbackFunc(void (*callback)());
//��ʼת���������ж�
void ADC_Start();
//ֹͣת��
void ADC_Stop();
void ADCx_Init_auto(ADC_TypeDef *  ADCx,char channel);
#endif /* __BSP_ADC_H */


