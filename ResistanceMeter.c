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
#include <math.h>
#include "filter.h"
/**************宏定义**************/

/**************以下数值自行定义**************/
//电阻，单位k
#define R19 1000.0f
#define VCC 5.0f

//使用哪一个adc频道
#define RESISTANCE_ADC_CHANNEL ADC_Channel_4

//adc频道所在的引脚pin和port，查询手册可得知是PA3
#define RESISTANCE_ADC_GPIO_PIN GPIO_Pin_4
#define RESISTANCE_ADC_GPIO_PORT GPIOA
#define RESISTANCE_ADC_GPIO_CLK RCC_APB2Periph_GPIOA
//控制引脚 使用PB3
#define RESISTANCE_GPIO_PIN GPIO_Pin_7
#define RESISTANCE_GPIO_PORT GPIOC
#define RESISTANCE_GPIO_CLK RCC_APB2Periph_GPIOC
/**************自定义数值到此完成**************/

/**************变量声明**************/
//私有变量
static unsigned char state;
static float raw_value;
static unsigned char unit;
//放大器的放大倍数
static int ampFactor;

static float inputData[20];
static float outputData[20];
/**************函数定义**************/

/**
 * @description: 欧姆表模块初始化
 * @param {无} 
 * @return {无} 
 */
void ResistanceMeter_GPIO_Init()
{

    //配置一个引脚用于控制输入
    GPIO_InitTypeDef GPIO_InitStruct;

    // RCC_APB2PeriphClockCmd(RESISTANCE_GPIO_CLK, ENABLE);
    // GPIO_InitStruct.GPIO_Pin = RESISTANCE_GPIO_PIN;
    // GPIO_InitStruct.GPIO_Mode = GPIO_Mode_Out_PP;
    // GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
    // GPIO_Init(RESISTANCE_GPIO_PORT, &GPIO_InitStruct);

    //配置adc引脚
    // 打开 ADC IO端口时钟
    ADC_GPIO_APBxClock_FUN(RESISTANCE_ADC_GPIO_CLK, ENABLE);
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
static void AdcInterrupt(int value)
{
    //更新数值
    ADC_Stop();
    raw_value = value;
#ifdef ADC_RAW_VALUE
    printf("resistanceMeter ADC raw :%d \n", value);
#endif
}
/**
 * @description: 
 * @param {type} 
 * @return {type} 
 */
void ResistanceMeter_ADC_Init()
{
    ADCx_Init_auto(ADC2, RESISTANCE_ADC_CHANNEL);
    ampFactor = 1;
}
/**
 * @description: 
 * @param {type} 
 * @return {type} 
 */
void openResistanceMeter()
{
    ResistanceMeter_ADC_Init();
    setCallbackFunc(AdcInterrupt);
    //  GPIO_SetBits(RESISTANCE_GPIO_PORT, RESISTANCE_GPIO_PIN);
    ADC_Start();
    state = 1;

#ifdef DEBUG_MODE
    printf("ResistanceMeter is opened\n");
#endif
}
/**
 * @description: 
 * @param {type} 
 * @return {type} 
 */
void closeResistanceMeter()
{

    ADC_Stop();
    setCallbackFunc(NULL);
    // GPIO_ResetBits(RESISTANCE_GPIO_PORT, RESISTANCE_GPIO_PIN);

    state = 0;

#ifdef DEBUG_MODE
    printf("ResistanceMeter is closed\n");
#endif
}
/**
 * @description: 
 * @param {type} 
 * @return {type} 
 */
unsigned char getResistanceMeterState()
{
    return state;
}

/**
 * @description: 
 * @param {type} 
 * @return {type} 
 */
void ResistanceMeterInit()
{

    ResistanceMeter_GPIO_Init();
#ifdef NO_BOARD
    printf("ResistanceMeter is OFF due to NO_BOARD mode\n");

#endif
    printf("ResistanceMeter Init   done...\n");
}

static int dataPointer = 0;
static float lastRes = 0;
//
#define X3 0.1086f
#define X2 0.0918f
#define X1 1.0462f
#define X0 -0.0016f
/**
 * @description: 
 * @param {type} 
 * @return {type} 
 */
float getCurrentRes()
{
    //y=0.0722x3 + 0.1771x2 + 0.998x + 0.0015
#ifndef SIMULATE_VALUE
    int l;
    float adcVoltage = (raw_value / 4096.0f) * 3.3f;
    float RxVoltage = (adcVoltage / (1.5f * ampFactor));
    float res;
    RxVoltage = X3 * pow(RxVoltage, 3) + X2 * pow(RxVoltage, 2) + X1 * RxVoltage + X0;

    res = (R19 * RxVoltage) / (VCC - RxVoltage);
    if (dataPointer < 20)
    {
        inputData[dataPointer] = (float)res;
        dataPointer++;
        res = lastRes;
    }
    else
    {
        firFilter(inputData, outputData, 20);
        dataPointer = 0;

        for (l = 0; l < 20; l++)
        {
            res += outputData[l];
        }
        res /= 20.0f;
        lastRes = res;
    }
#ifdef DEBUG_MODE
    printf("Res is :%f omh \n", res * 1000);
#endif
    ADC_Start();
    return res * 1000;
#else
    return 1200.0f;
#endif
}
/**
 * @description: 
 * @param {type} 
 * @return {type} 
 */
unsigned char getR_Unit()
{

    return unit;
}
void changeResistanceFactor(int factor)
{
    ampFactor = factor;
}
int getResistanceFactor()
{
    return ampFactor;
}
/**
 * @description: 
 * @param {type} 
 * @return {type} 
 */
//数值更新函数，请尽量不要在其中进行耗时操作
//会造成系统无响应（阻塞线程）
void ResistanceMeterLooper()
{
}
