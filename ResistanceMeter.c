/*
 * @文件描述: 欧姆表模块的主要代码
 * @版本: 1.0
 * @作者: 周晨阳
 * @Date: 2020-06-21 15:47:22
 */

/**************头文件**************/
#include "ResistanceMeter.h"
#include "stm32f10x.h"
#include "bsp_adc.h"
#include "public.h"

/**************宏定义**************/

/**************以下数值自行定义**************/
//电阻，单位k
#define R13 1.0f

//使用哪一个adc频道，统一使用adc2
#define    RESISTANCE_ADC_CHANNEL       ADC_Channel_3

//adc频道所在的引脚pin和port，查询手册可得知是PA3
#define   RESISTANCE_ADC_GPIO_PIN       GPIO_Pin_3
#define   RESISTANCE_ADC_GPIO_PORT      GPIOA
#define   RESISTANCE_ADC_GPIO_CLK       RCC_APB2Periph_GPIOA 
//控制引脚 使用PB3
#define RESISTANCE_GPIO_PIN              GPIO_Pin_7
#define RESISTANCE_GPIO_PORT             GPIOC
#define RESISTANCE_GPIO_CLK              RCC_APB2Periph_GPIOC
/**************自定义数值到此完成**************/

/**************变量声明**************/
//私有变量
static unsigned char state;
static float raw_value;
static unsigned char unit;

/**************函数定义**************/

/**
 * @description: 欧姆表模块初始化
 * @param {无} 
 * @return {无} 
 */
void ResistanceMeter_GPIO_Init(){

    //配置一个引脚用于控制输入
    GPIO_InitTypeDef  GPIO_InitStruct;
	
	RCC_APB2PeriphClockCmd(RESISTANCE_GPIO_CLK, ENABLE);
	GPIO_InitStruct.GPIO_Pin = RESISTANCE_GPIO_PIN;
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(RESISTANCE_GPIO_PORT, &GPIO_InitStruct);	

    //配置adc引脚
	// 打开 ADC IO端口时钟
	ADC_GPIO_APBxClock_FUN ( RESISTANCE_ADC_GPIO_CLK, ENABLE );
	// 配置 ADC IO 引脚模式
	// 必须为模拟输入
	GPIO_InitStruct.GPIO_Pin = RESISTANCE_ADC_GPIO_PIN;
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_AIN;
	// 初始化 ADC IO
	GPIO_Init(RESISTANCE_ADC_GPIO_PORT, &GPIO_InitStruct);	

}

/**
 * @description: //adc数值更新的回调函数
 * @param {type} 
 * @return {type} 
 */
static void AdcInterrupt(int value){
    //更新数值
    raw_value=value;
}
/**
 * @description: 
 * @param {type} 
 * @return {type} 
 */
void openResistanceMeter(){

    setCallbackFunc(AdcInterrupt);
    GPIO_SetBits(RESISTANCE_GPIO_PORT,RESISTANCE_GPIO_PIN);
    ADC_Start();
    state=1;
}
/**
 * @description: 
 * @param {type} 
 * @return {type} 
 */
void closeResistanceMeter(){

    setCallbackFunc(NULL);
    GPIO_ResetBits(RESISTANCE_GPIO_PORT,RESISTANCE_GPIO_PIN);
    ADC_Stop();
    state=0;
}
/**
 * @description: 
 * @param {type} 
 * @return {type} 
 */
unsigned char  getResistanceMeterState(){
    return state;
}

/**
 * @description: 
 * @param {type} 
 * @return {type} 
 */
void ResistanceMeter_ADC_Init(){
    ADCx_Init_auto(ADC2,RESISTANCE_ADC_CHANNEL);
}

/**
 * @description: 
 * @param {type} 
 * @return {type} 
 */
void ResistanceMeterInit(){

    ResistanceMeter_GPIO_Init();
    ResistanceMeter_ADC_Init();

}
/**
 * @description: 
 * @param {type} 
 * @return {type} 
 */
float getCurrentRes(){

#ifndef SIMULATE_VALUE
    return raw_value;
#else 
    return 1200.0f;
#endif
    

}
/**
 * @description: 
 * @param {type} 
 * @return {type} 
 */
unsigned char getR_Unit(){

    return unit;
    
}
/**
 * @description: 
 * @param {type} 
 * @return {type} 
 */
//数值更新函数，请尽量不要在其中进行耗时操作
//会造成系统无响应（阻塞线程）
void ResistanceMeterLooper(){

}

