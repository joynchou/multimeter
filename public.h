/*
 * @文件描述:公共头文件及一些全局宏存放文件 
 * @版本: 
 * @作者: 周晨阳
 * @Date: 2020-08-24 10:39:33
 */

#ifndef _PUBLIC_H_
#define _PUBLIC_H_

#include <stdio.h>
#include <stdlib.h>
#include <stddef.h>
#include "bsp_usart.h"
#include <math.h>

//是否打开debug模式，打开之后串口会在某些函数内会有调试用的数据输出
//注释此宏来关闭调试模式
#define DEBUG_MODE

//此宏控制是否要使用模拟的测量值来测试系统,
//开启该宏后不需要电路板也能输出数值（固定不变数字），用于测试屏幕显示
//#define SIMULATE_VALUE

// 是否输出adc原始值，打开此宏会导致只能输出原始值无法输出其他
//#define ADC_RAW_VALUE

//是否需要工作在无外接电路板模式下，如果需要工作在此模式则只能测量电压，0-3.3v，其他模式则无法运行
//可以用来做测试
//#define NO_BOARD

//是否使用数字滤波，使用后刷新速度会变慢
//#define USE_FIR
#endif