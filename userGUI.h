/*
 * @文件描述: 
 * @版本: 
 * @作者: 周晨阳
 * @Date: 2020-06-21 16:05:31
 */
/**
 * 
 * 这个usergui文件是一个类似显卡的处理
 * 通过一些特定的函数，去控制屏幕的显示，屏蔽屏幕底层信息
 * 上层只需要调用有限的一些函数即可控制屏幕显示
 * 
 * 
 * 
 */
#ifndef _USER_GUI_H_
#define _USER_GUI_H_

#include "bsp_ili9341_lcd.h"
#include "voltageMeter.h"
#include "CurrentMeter.h"
#include "ResistanceMeter.h"

#define MODE_VOLTAGE 0
#define MODE_CURRENT 1
#define MODE_RESISTANCE 2

//设置电表的模式
void setDisplayMode(unsigned char mode);
unsigned char getDisplayMode();
//绘制系统界面
void drawOutline();
//hold功能
void holdTrigger();
//清空界面
void clearWindow();
//设置主显示数字
void setNumTitle(float num);
//设置单位
void setUnit(unsigned char unit);
void GUIInit();

#endif