/*
 * @文件描述: 电流表模块的主要代码
 * @版本: 1.0
 * @作者: 周晨阳
 * @Date: 2020-06-21 15:47:00
 */

/**************头文件**************/
#include "CurrentMeter.h"
#include "stm32f10x.h"
#include "bsp_adc.h"

/**************宏定义**************/

/**************以下数值自行定义**************/
//R8单位为欧姆，其他电阻单位为k
#define R8 0.01f


#define R9 10.0f
#define R10 100.0f
#define R11 10.0f
#define R12 100.0f

#define GAIN (R10/R9) 

//使用哪一个adc频道，统一使用adc2
#define    CURRENT_ADC_CHANNEL        ADC_Channel_2
#define    CURRENT_ADC_GPIO_PIN       GPIO_Pin_2
#define    CURRENT_ADC_GPIO_PORT      GPIOA
#define    CURRENT_ADC_GPIO_CLK       RCC_APB2Periph_GPIOA 

//控制引脚 使用PB2
#define    CURRENT_GPIO_PIN              GPIO_Pin_8
#define    CURRENT_GPIO_PORT             GPIOC
#define    CURRENT_GPIO_CLK              RCC_APB2Periph_GPIOC
/**************自定义数值到此完成**************/

/**************变量声明**************/
static unsigned char state;
static float raw_value;
static unsigned char unit;
static float current;

/**************函数定义**************/
/**
 * @description: 电流表GPIO初始化
 * @param {type} 
 * @return {type} 
 */
static void Current_GPIO_Init(){

   //配置一个引脚用于控制输入
    GPIO_InitTypeDef  GPIO_InitStruct;
	
	RCC_APB2PeriphClockCmd(CURRENT_GPIO_CLK, ENABLE);
	GPIO_InitStruct.GPIO_Pin = CURRENT_GPIO_PIN;
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(CURRENT_GPIO_PORT, &GPIO_InitStruct);	

    //配置adc引脚
	// 打开 ADC IO端口时钟
	ADC_GPIO_APBxClock_FUN ( CURRENT_ADC_GPIO_CLK, ENABLE );
	// 配置 ADC IO 引脚模式
	// 必须为模拟输入
	GPIO_InitStruct.GPIO_Pin = CURRENT_ADC_GPIO_PIN;
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_AIN;
	// 初始化 ADC IO
	GPIO_Init(CURRENT_ADC_GPIO_PORT, &GPIO_InitStruct);	

}
/**
 * @description: 电流表adc初始化
 * @param {type} 
 * @return {type} 
 */
static void Current_ADC_Init(){

    ADCx_Init_auto(ADC2,CURRENT_ADC_CHANNEL);

}
/**
 * @description: 电流表初始化
 * @param {type} 
 * @return {type} 
 */
void CurrentMeterInit(){

    Current_GPIO_Init();
    Current_ADC_Init();

}
/**
 * @description: adc数值更新的回调函数
 * @param {type} 
 * @return {type} 
 */
static void AdcInterrupt(int value){
    //更新数值
    raw_value=value;
}
/**
 * @description: 打开电流表
 * @param {type} 
 * @return {type} 
 */
void openCurrentMeter(){

    setCallbackFunc(AdcInterrupt);
    GPIO_SetBits(CURRENT_GPIO_PORT,CURRENT_GPIO_PIN);
}
/**
 * @description: 关闭电流表
 * @param {type} 
 * @return {type} 
 */
void closeCurrentMeter(){

    GPIO_ResetBits(CURRENT_GPIO_PORT,CURRENT_GPIO_PIN);
}
/**
 * @description: 获得实时测量的电流
 * @param {type} 
 * @return {type} 
 */
float getCurrent(){
    
    if(state){

        float adcVoltage=(raw_value/4096.0f)*3.3f;
        float realCurrent= (adcVoltage/GAIN)/(R8*0.001f);
        current=realCurrent;
        return current;
    }
}
/**
 * @description: 获得实时测量的电流单位
 * @param {type} 
 * @return {type} 
 */
unsigned char getI_Unit(){

    if(state){

        if(current>1)
        {
            return UNIT_A;
        }
        else if(current<1 &&current>0.001){
            return UNIT_MA;
        }
        else if (current<0.001)
        {
            return UNIT_UA;
        }
        else
        {
            return UNIT_A;
        }
        
        
    }
    else{
        return 0;
    }
}
/**
 * @description: 电流表模块的looper
 * @param {type} 
 * @return {type} 
 */
void CurrentMeterLooper(){

}
