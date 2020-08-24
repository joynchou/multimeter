/*
 * @文件描述: 电流表模块的头文件
 * @版本: 
 * @作者: 周晨阳
 * @Date: 2020-06-21 15:48:13
 */

#ifndef _CURRENT_METER_H_
#define _CURRENT_METER_H_

#define UNIT_A 0
#define UNIT_MA 1
#define UNIT_UA 2


void CurrentMeterInit();
void openCurrentMeter();
void closeCurrentMeter();
float getCurrent();
unsigned char getI_Unit();
void CurrentMeterLooper();
#endif