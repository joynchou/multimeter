/*
 * @文件描述: ADC的配置和初始化程序
 * @版本: 
 * @作者: 周晨阳
 * @Date: 2020-08-21 13:30:41
 */

/**************头文件**************/
#include "bsp_adc.h"
#include <stddef.h>

/**************宏定义**************/

/**************变量声明**************/

extern void (*ADC_callbackFun)(int);

/**************函数定义**************/

//static void ADCx_GPIO_Config(void)
//{
//	GPIO_InitTypeDef GPIO_InitStructure;
//
//	// 打开 ADC IO端口时钟
//	ADC_GPIO_APBxClock_FUN ( ADC_GPIO_CLK, ENABLE );
//
//	// 配置 ADC IO 引脚模式
//	// 必须为模拟输入
//	GPIO_InitStructure.GPIO_Pin = ADC_PIN;
//	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AIN;
//
//	// 初始化 ADC IO
//	GPIO_Init(ADC_PORT, &GPIO_InitStructure);
//}

/**
 * @description: ADC模式设置
 * @param {ADCx:设定哪一个adc，可以为ADC1，2，3
 * 			channel:设定哪一个频道} 
 * @return {type} 
 */
static void ADCx_Mode_Config_auto(ADC_TypeDef *ADCx, char channel)
{
	ADC_InitTypeDef ADC_InitStruct;

	ADC_APBxClock_FUN(ADC_CLK, ENABLE);
	ADC_InitStruct.ADC_Mode = ADC_Mode_Independent;
	ADC_InitStruct.ADC_ScanConvMode = DISABLE;
	ADC_InitStruct.ADC_ContinuousConvMode = DISABLE;
	ADC_InitStruct.ADC_ExternalTrigConv = ADC_ExternalTrigConv_None;
	ADC_InitStruct.ADC_DataAlign = ADC_DataAlign_Right;
	ADC_InitStruct.ADC_NbrOfChannel = 1;

	ADC_Init(ADCx, &ADC_InitStruct);

	RCC_ADCCLKConfig(RCC_PCLK2_Div8);
	ADC_RegularChannelConfig(ADCx, channel, 1, ADC_SampleTime_239Cycles5);

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
//	//自动连续转换
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
////   // ADC开始校准
//// 	ADC_StartCalibration(ADC_x);
//// 	// 等待校准完成
//// 	while(ADC_GetCalibrationStatus(ADC_x));
//
//// 	ADC_SoftwareStartConvCmd(ADC_x, ENABLE);
//}
/**
 * @description: //adc中断设置
 * @param {type} 
 * @return {type} 
 */
static void ADC_NVIC_Config(void)
{
	NVIC_InitTypeDef NVIC_InitStructure;
	// 优先级分组
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_1);

	// 配置中断优先级
	NVIC_InitStructure.NVIC_IRQChannel = ADC_IRQ;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);
}
/**
 * @description: //设置adc中断函数的回调函数
 * @param {type} 
 * @return {type} 
 */
void setCallbackFunc(void (*callback)(int))
{

	ADC_callbackFun = callback;
}
/**
 * @description: //adc转换开始，转换自动连续进行
 * @param {type} 
 * @return {type} 
 */
void ADC_Start()
{

	ADC_Cmd(ADC2, ENABLE);
	 /* Enable ADC1 reset calibration register */   
 // ADC_ResetCalibration(ADC2);
  /* Check the end of ADC1 reset calibration register */
 // while(ADC_GetResetCalibrationStatus(ADC2));

	ADC_StartCalibration(ADC2);
	//这里校准被卡住
	while (ADC_GetCalibrationStatus(ADC2))
		;
	//开始转换
	ADC_SoftwareStartConvCmd(ADC2, ENABLE);
}
/**
 * @description: //结束转换
 * @param {type} 
 * @return {type} 
 */
void ADC_Stop()
{

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
void ADCx_Init_auto(ADC_TypeDef *ADCx, char channel)
{
	ADC_NVIC_Config();

	ADCx_Mode_Config_auto(ADCx, channel);
}
