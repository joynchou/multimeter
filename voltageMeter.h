/*
 * @文件描述: 
 * @版本: 
 * @作者: 周晨阳
 * @Date: 2020-06-21 15:48:33
 */
#ifndef _VOLTAGE_METER_H_
#define _VOLTAGE_METER_H_

//电压单位的宏
#define UNIT_V 3
#define UNIT_MV 4
#define UNIT_UV 5

#define  KEY_ON      1
#define  KEY_OFF     0

void VoltageMeterInit();
void openVoltageMeter();
void closeVoltageMeter();
unsigned char getVoltageMeterState();

float getVoltage();
unsigned char getU_Unit();
//电压表的looper，需要放在main的while中不停的循环以便一直能获取最新的数值
void VoltageMeterLooper();

#endif