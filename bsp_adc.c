/*
 * @�ļ�����: ADC�����úͳ�ʼ������
 * @�汾: 
 * @����: �ܳ���
 * @Date: 2020-08-21 13:30:41
 */

/**************ͷ�ļ�**************/
#include "bsp_adc.h"
#include <stddef.h>


/**************�궨��**************/


/**************��������**************/

extern void (*ADC_callbackFun)(int);

/**************��������**************/

//static void ADCx_GPIO_Config(void)
//{
//	GPIO_InitTypeDef GPIO_InitStructure;
//	
//	// �� ADC IO�˿�ʱ��
//	ADC_GPIO_APBxClock_FUN ( ADC_GPIO_CLK, ENABLE );
//	
//	// ���� ADC IO ����ģʽ
//	// ����Ϊģ������
//	GPIO_InitStructure.GPIO_Pin = ADC_PIN;
//	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AIN;
//	
//	// ��ʼ�� ADC IO
//	GPIO_Init(ADC_PORT, &GPIO_InitStructure);	
//}

/**
 * @description: 
 * @param {type} 
 * @return {type} 
 */
static void ADCx_Mode_Config_auto(char ADCx,char channel){
	ADC_InitTypeDef ADC_InitStruct;
	
	ADC_APBxClock_FUN ( ADC_CLK, ENABLE );
	ADC_InitStruct.ADC_Mode = ADC_Mode_Independent;
	ADC_InitStruct.ADC_ScanConvMode = DISABLE;
	ADC_InitStruct.ADC_ContinuousConvMode = ENABLE;	
	ADC_InitStruct.ADC_ExternalTrigConv = ADC_ExternalTrigConv_None;
	ADC_InitStruct.ADC_DataAlign = ADC_DataAlign_Right;
	ADC_InitStruct.ADC_NbrOfChannel = 1;
	
	ADC_Init(ADCx, &ADC_InitStruct);
	
	RCC_ADCCLKConfig(RCC_PCLK2_Div8);
	ADC_RegularChannelConfig(ADCx, channel, 1, ADC_SampleTime_55Cycles5);
	
	ADC_ITConfig(ADCx, ADC_IT_EOC, ENABLE);
	ADC_Cmd(ADCx, ENABLE);
}
//}
//static void ADCx_Mode_Config(void)
//{
//	ADC_InitTypeDef ADC_InitStruct;
//	
//	ADC_APBxClock_FUN ( ADC_CLK, ENABLE );
//	ADC_InitStruct.ADC_Mode = ADC_Mode_Independent;
//	ADC_InitStruct.ADC_ScanConvMode = DISABLE;
//	//�Զ�����ת��
//	ADC_InitStruct.ADC_ContinuousConvMode = ENABLE;	
//	ADC_InitStruct.ADC_ExternalTrigConv = ADC_ExternalTrigConv_None;
//	ADC_InitStruct.ADC_DataAlign = ADC_DataAlign_Right;
//	ADC_InitStruct.ADC_NbrOfChannel = 1;
//	
//	ADC_Init(ADC_x, &ADC_InitStruct);
//	
//	RCC_ADCCLKConfig(RCC_PCLK2_Div8);
//	ADC_RegularChannelConfig(ADC_x, ADC_CHANNEL, 1, ADC_SampleTime_55Cycles5);
//	
//	ADC_ITConfig(ADC_x, ADC_IT_EOC, ENABLE);

////	ADC_Cmd(ADC_x, ENABLE);
//	
////   // ADC��ʼУ׼
//// 	ADC_StartCalibration(ADC_x);
//// 	// �ȴ�У׼���
//// 	while(ADC_GetCalibrationStatus(ADC_x));
//	
//// 	ADC_SoftwareStartConvCmd(ADC_x, ENABLE);
//}
/**
 * @description: 
 * @param {type} 
 * @return {type} 
 */
//adc�ж�����
static void ADC_NVIC_Config(void)
{
  NVIC_InitTypeDef NVIC_InitStructure;
	// ���ȼ�����
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_1);

  // �����ж����ȼ�
  NVIC_InitStructure.NVIC_IRQChannel = ADC_IRQ;
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
  NVIC_Init(&NVIC_InitStructure);
}
/**
 * @description: 
 * @param {type} 
 * @return {type} 
 */
//����adc�жϺ����Ļص�����
void setCallbackFunc(void (*callback)(int )){
	
	ADC_callbackFun=callback;
	

}
/**
 * @description: 
 * @param {type} 
 * @return {type} 
 */
//adcת����ʼ��ת���Զ���������
void ADC_Start(){


	ADC_Cmd(ADC2, ENABLE);
	ADC_StartCalibration(ADC2);
	
	while(ADC_GetCalibrationStatus(ADC2));
	//��ʼת��
	ADC_SoftwareStartConvCmd(ADC2, ENABLE);
}
/**
 * @description: 
 * @param {type} 
 * @return {type} 
 */
//����ת��
void ADC_Stop(){

	ADC_Cmd(ADC2, DISABLE);

}
/**
 * @description: 
 * @param {type} 
 * @return {type} 
 */
void ADCx_Init(void)
{
	ADC_NVIC_Config();
	//ADCx_GPIO_Config();
	//ADCx_Mode_Config();
}
/**
 * @description: 
 * @param {type} 
 * @return {type} 
 */
void ADCx_Init_auto(char ADCx,char channel){
	ADC_NVIC_Config();
	
	ADCx_Mode_Config_auto(ADCx,channel);
}


















