/*
 * @文件描述: 电压表模块
 * @版本: 
 * @作者: 周晨阳
 * @Date: 2020-06-21 15:46:46
 *
 *
 * 
 * @文件说明
 * 注意，如果没有使用开启函数，
 * 则该模块不会工作，返回的值也是0或无效值
 * adc采用的是中断+回调函数的方式获取数值
 * 另外，三个表暂时不支持同时工作(因为使用了中断方式读取数据)
 * 
 * 请定义好电阻的大小，电压表有两个量程，和我们定制的电路板一起工作
 * 第一个量程为-160 至 +160v 有效精度为 320/4096  v
 * 第二个量程为-16 至 +16v 有效精度为 32/4096 v 
 * 量程的切换在电路板上通过拨动开关完成，并需要按下量程切换按钮
 * */

/**************头文件**************/
#include "voltageMeter.h"
#include "bsp_adc.h"
#include <stddef.h>
#include "stm32f10x.h"
#include "public.h"
#include "filter.h"
#include <math.h>
/**************宏定义**************/
#define OPEN 1
#define CLOSED 0

/**************以下数值自行定义**************/

//外围电路电阻值，单位k，参见电路图pdf中的电阻标注，实际焊接时根据所测得电阻填写
#define R7 1000.0f
#define R11 6.8f
//提升电压的分压电阻
#define R45 500.0f
#define R46 150.0f

/**************自定义到此为止**************/

//衰减系数，大概在1/148左右
#define DAMPING_FACTOR (R11 / (R7 + R11))

//提升电压，大概在1.1v左右
#define REF_VOLTAGE (R46 / (R45 + R46)) * 5.0f

//使用哪一个adc频道，统一使用adc2
#define VOLTAGE_ADC_CHANNEL ADC_Channel_2
//adc频道所在的引脚pin和port，查询手册可得知是PA1
#define VOLTAGE_ADC_GPIO_PIN GPIO_Pin_2
#define VOLTAGE_ADC_GPIO_PORT GPIOA
#define VOLTAGE_ADC_GPIO_CLK RCC_APB2Periph_GPIOA
//控制引脚 PB1
#define VOLTAGE_GPIO_PIN GPIO_Pin_6
#define VOLTAGE_GPIO_PORT GPIOC
#define VOLTAGE_GPIO_CLK RCC_APB2Periph_GPIOC

/**************私有变量**************/
//电压表开关状态
static unsigned char state;
//adc输出值
static float raw_value;
//真实电压值，但是单位会随着值的不同而改变
static float voltage;
//当前单位
static unsigned char unit;
//放大器的放大倍数
static int ampFactor;

static float inputData[20];
static float outputData[20];
/**************函数定义**************/

/**
 * @description: 开关引脚和adc引脚初始化
 * @param {type} 
 * @return {type} 
 */
void Voltage_GPIO_Init()
{

    //配置一个引脚用于控制输入
    GPIO_InitTypeDef GPIO_InitStruct;

    //array_new(&ar);
    // RCC_APB2PeriphClockCmd(VOLTAGE_GPIO_CLK, ENABLE);
    // GPIO_InitStruct.GPIO_Pin = VOLTAGE_GPIO_PIN;
    // GPIO_InitStruct.GPIO_Mode = GPIO_Mode_Out_PP;
    // GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
    // GPIO_Init(VOLTAGE_GPIO_PORT, &GPIO_InitStruct);

    //配置adc引脚
    // 打开 ADC IO端口时钟
    ADC_GPIO_APBxClock_FUN(VOLTAGE_ADC_GPIO_CLK, ENABLE);
    // 配置 ADC IO 引脚模式
    // 必须为模拟输入
    GPIO_InitStruct.GPIO_Pin = VOLTAGE_ADC_GPIO_PIN;
    GPIO_InitStruct.GPIO_Mode = GPIO_Mode_AIN;
    // 初始化 ADC IO
    GPIO_Init(VOLTAGE_ADC_GPIO_PORT, &GPIO_InitStruct);
}
/**
 * @description: 电压表初始化
 * @param {type} 
 * @return {type} 
 */
void VoltageMeterInit()
{

    Voltage_GPIO_Init();
    //放大器增益默认为x1
    ampFactor = 1;
#ifndef NO_BOARD
    printf("VoltageMeter is in normal mode\n");
#else
    printf("VoltageMeter is in NO_BOARD mode\n");
#endif

    printf("VoltageMeter Init  done...\n");
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
    printf("voltageMeter ADC raw :%d \n", value);
#endif
    //ADC_Start();
}
/**
 * @description: 开启电压表
 * @param {type} 
 * @return {type} 
 */
void openVoltageMeter()
{

    ADCx_Init_auto(ADC2, VOLTAGE_ADC_CHANNEL);
    setCallbackFunc(AdcInterrupt);
    ADC_Start();
    //添加相应的硬件代码，如开启对应的adc
    // GPIO_SetBits(VOLTAGE_GPIO_PORT, VOLTAGE_GPIO_PIN);
    state = 1;

#ifdef DEBUG_MODE
    printf("voltageMeter is open\n");
#endif
}
/**
 * @description: 关闭电压表
 * @param {type} 
 * @return {type} 
 */
void closeVoltageMeter()
{

    ADC_Stop();
    //取消回调函数
    setCallbackFunc(NULL);
    //
    // GPIO_ResetBits(VOLTAGE_GPIO_PORT, VOLTAGE_GPIO_PIN);
    //添加相应的硬件代码，如关闭adc，继电器等
    state = 0;

#ifdef DEBUG_MODE
    printf("voltageMeter is closed\n");
#endif
}
//多项式拟合校准系数
#define X3 0.1086f
#define X2 0.0918f
#define X1 1.0462f
#define X0 -0.0016f

int dataPointer = 0;
float lastVoltage = 0;
/**
 * @description: 取得电压值，配合getU_Unit使用，用于直接驱动屏幕显示
 * @param {type} 
 * @return {type} 
 */
float getVoltage()
{

    int l;
    float adjVoltage;
    float fabsVoltage;
    //ADC_Start();

    if (state)
    {

#ifndef SIMULATE_VALUE

#ifndef NO_BOARD

#ifdef USE_FIR
        //adc实测电压
        float adcVoltage = (raw_value / 4096.0f) * 3.3f;

        //计算后外部设备的实际电压
        float realVoltage = ((adcVoltage / 1.5f) - REF_VOLTAGE) / (DAMPING_FACTOR * ampFactor);
        if (dataPointer < 20)
        {
            inputData[dataPointer] = (float)realVoltage;
            dataPointer++;
            realVoltage = lastVoltage;
        }
        else
        {
            //对数据进行滤波
            firFilter(inputData, outputData, 20);
            dataPointer = 0;

            for (l = 0; l < 20; l++)
            {
                realVoltage += outputData[l];
            }
            realVoltage /= 20.0f;
            lastVoltage = realVoltage;
        }

#else
        //adc实测电压
        float adcVoltage = (raw_value / 4096.0f) * 3.3f;

        //计算后外部设备的实际电压
        float realVoltage = ((adcVoltage / 1.5f) - REF_VOLTAGE) / (DAMPING_FACTOR * ampFactor);
#endif

#else
        float realVoltage = (raw_value / 4096.0f) * 3.3f;
#endif
        voltage = realVoltage;
#else
        voltage = 5.12f;

#endif
#define ZERO 0.0000001f
#ifdef DEBUG_MODE
        printf("voltage is :  %f v \n", voltage);
#endif

        //最后自动根据数值大小放大数值
        //v
        fabsVoltage=fabs(voltage);
        if (fabsVoltage - 1.0f > ZERO)
        {
            adjVoltage = voltage;
        }
        //mv
        else if (fabsVoltage - 1.0f < ZERO && fabsVoltage - 0.001f > ZERO)
        {
            adjVoltage = voltage * 1000.0f;
        }
        //uv
        else if (fabsVoltage - 0.001f < ZERO)
        {
            adjVoltage = voltage * 1000000.0f;
        }
        else
        {
        }

        ADC_Start();

        return adjVoltage;
    }
    else
    {
        return 0;
    }
}
/**
 * @description: 获得当前数值的单位
 * @param {type} 
 * @return {type} 
 */
unsigned char getU_Unit()
{

    if (state)
    {
        if (fabs(voltage) - 1.0f > ZERO)
        {

            return UNIT_V;
        }
        else if (fabs(voltage) - 1.0f < ZERO && fabs(voltage) - 0.001f > ZERO)
        {
            // voltage*1000.0f;
            return UNIT_MV;
        }
        else if (fabs(voltage) - 0.001f < ZERO)
        {
            // voltage*1000000.0f;
            return UNIT_UV;
        }
        else
        {
            return UNIT_V;
        }
    }
    else
    {
        return 0;
    }
}
/**
 * @description: 改变放大器的放大倍数
 * @param {int} factor
 * @return {*}
 */
void changeVoltageFactor(int factor)
{
    ampFactor = factor;
}
/** 
 * @description: 获得当前放大器的放大倍数
 * @param {*}
 * @return {*}
 */
int getVoltageFactor()
{
    return ampFactor;
}

/**
 * @description: 数值更新函数，请尽量不要在其中进行耗时操作,会造成系统无响应（阻塞线程）
 * @param {type} 
 * @return {type} 
 */
void VoltageMeterLooper()
{
    int l = 0;
    //放置能更新value的语句
    if (state)
    {
    }
}
