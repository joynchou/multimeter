/*
 * @文件描述: 
 * @版本: 
 * @作者: 周晨阳
 * @Date: 2020-06-21 15:43:26
 */

/**************文件说明**************/
/**
 * 该文件是用于按键的初始化以及设置按键回调函数的作用
 * 在按键被按下时可以自动调用设置的回调函数
 * 将buttonLooper（）函数加入main中
 * 
 * 
 * */

/**************头文件**************/
#include "button.h"
#include <stddef.h>
#include "public.h"

//private function:

//private var

/**************变量声明**************/
static void (*modeButtonFunc)();
static void (*holdButtonFunc)();
static void (*changeFactorButtonFunc)();
/**************函数定义**************/
/**
 * @description: 
 * @param {type} 
 * @return {type} 
 */
void setModeButtonListener(void (*callback)())
{
	modeButtonFunc = callback;
}
/**
 * @description: 
 * @param {type} 
 * @return {type} 
 */
void setHoldButtonListener(void (*callback)())
{
	holdButtonFunc = callback;
}
/**
 * @description: 
 * @param {*}
 * @return {*}
 */
void setChangeFactorButtonLis(void(*callback))
{
	changeFactorButtonFunc = callback;
}

/**
 * @description: 
 * @param {type} 
 * @return {type} 
 */
static uint8_t Key_Scan(GPIO_TypeDef *GPIOx, uint16_t GPIO_Pin)
{
	if (GPIO_ReadInputDataBit(GPIOx, GPIO_Pin) == KEY_ON)
	{

		while (GPIO_ReadInputDataBit(GPIOx, GPIO_Pin) == KEY_ON)
			;
		return KEY_ON;
	}
	else
		return KEY_OFF;
}

/**
 * @description: //板载led初始化
 * @param {type} 
 * @return {type} 
 */
void LED_GPIO_Config(void)
{
	GPIO_InitTypeDef GPIO_InitStruct;

	RCC_APB2PeriphClockCmd(LED_G_GPIO_CLK, ENABLE);

	GPIO_InitStruct.GPIO_Pin = LED_G_GPIO_PIN;
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;

	GPIO_Init(LED_G_GPIO_PORT, &GPIO_InitStruct);
}

/**
 * @description: 按键检测，加入main主循环，注意，按键输入引脚不可以悬空
 * @param {type} 
 * @return {type} 
 */
void buttonLooper()
{

	if (modeButtonFunc != NULL)
	{
		//加入判断按键按下的语句
		if (Key_Scan(KEY1_GPIO_PORT, KEY1_GPIO_PIN))
		{
			(*modeButtonFunc)();
		}
	}
	if (holdButtonFunc != NULL)
	{
		//加入判断按键按下的语句

		if (Key_Scan(KEY2_GPIO_PORT, KEY2_GPIO_PIN))
		{
			(*holdButtonFunc)();
			//按hold键led灯会反转
			LED_G_TOGGLE;
		}
	}

	if (changeFactorButtonFunc != NULL)
	{
		//加入判断按键按下的语句
		//todo:更改按键的引脚
		//if(Key_Scan(KEY1_GPIO_PORT,KEY1_GPIO_PIN)){
		//    (*changeFactorButtonFunc)();
		// }
	}
}
/**
 * @description: 按键初始化
 * @param {type} 
 * @return {type} 
 */
void buttonInit()
{

	GPIO_InitTypeDef GPIO_InitStruct;
	LED_GPIO_Config();

	RCC_APB2PeriphClockCmd(KEY1_GPIO_CLK, ENABLE);

	GPIO_InitStruct.GPIO_Pin = KEY1_GPIO_PIN;
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_IN_FLOATING;

	GPIO_Init(KEY1_GPIO_PORT, &GPIO_InitStruct);

	GPIO_InitStruct.GPIO_Pin = KEY2_GPIO_PIN;
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_IN_FLOATING;

	GPIO_Init(KEY2_GPIO_PORT, &GPIO_InitStruct);

	printf("button init done...\n");
}
