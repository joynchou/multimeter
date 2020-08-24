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
 * 另外，三个表暂时不支持同时工作
 * 
 * */


/**************头文件**************/
#include "voltageMeter.h"
#include "bsp_adc.h"
#include <stddef.h>
#include "stm32f10x.h"
#include "public.h"
/**************宏定义**************/
#define OPEN 1
#define CLOSED 0

/**************以下数值自行定义**************/
//外围电路电阻值，单位k，参见电路图中的电阻标注
#define R1 900.0f
#define R2 100.0f

#define R3 100.0f
#define R4 100.0f
#define R5 100.0f
#define R6 1.0f
#define R7 2.0f

#define R16 5.9f
#define R17 10.7f

//参考电压,在1.65v左右，这样可以直接测量正负16.5v
#define REF_VOLTAGE ((R16/(R16+R17))*5.0f)
//衰减倍数，在10倍左右
#define DAMPING_FACTOR  (R2/(R1+R2))

//使用哪一个adc频道，统一使用adc2
#define    VOLTAGE_ADC_CHANNEL       ADC_Channel_1
//adc频道所在的引脚pin和port，查询手册可得知是PA1
#define   VOLTAGE_ADC_GPIO_PIN       GPIO_Pin_1
#define   VOLTAGE_ADC_GPIO_PORT      GPIOA
#define   VOLTAGE_ADC_GPIO_CLK       RCC_APB2Periph_GPIOA 
//控制引脚 PB1
#define VOLTAGE_GPIO_PIN              GPIO_Pin_6
#define VOLTAGE_GPIO_PORT             GPIOC
#define VOLTAGE_GPIO_CLK              RCC_APB2Periph_GPIOC

/**************自定义数值到此完成**************/


/**************私有变量**************/
//电压表开关状态
static unsigned char state;
//adc输出值
static float raw_value;
//真实电压值，单位v
static float voltage;
//当前单位
static unsigned char unit;

/**************函数定义**************/

/**
 * 开关引脚和adc引脚初始化
 * */
/**
 * @description: 
 * @param {type} 
 * @return {type} 
 */
void Voltage_GPIO_Init(){

    //配置一个引脚用于控制输入
    GPIO_InitTypeDef  GPIO_InitStruct;
	
	RCC_APB2PeriphClockCmd(VOLTAGE_GPIO_CLK, ENABLE);
	GPIO_InitStruct.GPIO_Pin = VOLTAGE_GPIO_PIN;
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(VOLTAGE_GPIO_PORT, &GPIO_InitStruct);	

    
	

    //配置adc引脚
	// 打开 ADC IO端口时钟
	ADC_GPIO_APBxClock_FUN ( VOLTAGE_ADC_GPIO_CLK, ENABLE );
	// 配置 ADC IO 引脚模式
	// 必须为模拟输入
	GPIO_InitStruct.GPIO_Pin = VOLTAGE_ADC_GPIO_PIN;
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_AIN;
	// 初始化 ADC IO
	GPIO_Init(VOLTAGE_ADC_GPIO_PORT, &GPIO_InitStruct);	


}
/**
 * @description: 
 * @param {type} 
 * @return {type} 
 */
void VoltageMeterInit(){

    Voltage_GPIO_Init();

    ADCx_Init_auto(ADC2,VOLTAGE_ADC_CHANNEL);

    
}
/**
 * @description: 
 * @param {type} 
 * @return {type} 
 */
//adc数值更新的回调函数
static void AdcInterrupt(int value){
    //更新数值
    ADC_Stop();
    //中间可以插入滤波语句，在这之间，adc不会更新数值，也不会产生中断
    raw_value=value;

    ADC_Start();
}
/**
 * @description: 
 * @param {type} 
 * @return {type} 
 */
void openVoltageMeter(){
   
    setCallbackFunc(AdcInterrupt);
    ADC_Start();
    //添加相应的硬件代码，如开启对应的adc
    GPIO_SetBits(VOLTAGE_GPIO_PORT,VOLTAGE_GPIO_PIN);
    state=1;
    
#ifdef DEBUG_MODE
    printf("voltageMeter is open\n");
#endif
}
/**
 * @description: 
 * @param {type} 
 * @return {type} 
 */
void closeVoltageMeter(){

    ADC_Stop();
    //取消回调函数
    setCallbackFunc(NULL);
    //
    GPIO_ResetBits(VOLTAGE_GPIO_PORT,VOLTAGE_GPIO_PIN);
    //添加相应的硬件代码，如关闭adc，继电器等
    state=0;
#ifdef DEBUG_MODE
    printf("voltageMeter is closed\n");
#endif
}
/**
 * @description: 
 * @param {type} 
 * @return {type} 
 */
//取得电压值，配合getU_Unit使用，用于直接驱动屏幕显示
float getVoltage(){
    
    
    if(state){

        float adcVoltage=(raw_value/4096.0f)*3.3f;
        float realVoltage= (adcVoltage-REF_VOLTAGE)*DAMPING_FACTOR;
        voltage=realVoltage;
        
#ifdef DEBUG_MODE
    printf("voltage is : %f v \n",voltage);
#endif
    return realVoltage ;
    }
    else{
        return 0;
    }
}
/**
 * @description: 
 * @param {type} 
 * @return {type} 
 */
unsigned char getU_Unit(){
    
    if(state){
        if(voltage>1)
        {
            return UNIT_V;
        }
        else if(voltage<1 &&voltage>0.001){
            return UNIT_MV;
        }
        else if (voltage<0.001)
        {
            return UNIT_UV;
        }
        else
        {
            return UNIT_V;
        }
        
        
    }
    else{
        return 0;
    }
}

/**
 * @description: 
 * @param {type} 
 * @return {type} 
 */
//数值更新函数，请尽量不要在其中进行耗时操作
//会造成系统无响应（阻塞线程）
void VoltageMeterLooper(){
    //放置能更新value的语句
    if(state){

    }
}
