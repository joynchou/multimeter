#ifndef _MAIN_H_
#define _MAIN_H_

#include "CurrentMeter.h"
#include "ResistanceMeter.h"
#include "voltageMeter.h"
#include "button.h"
#include "userGUI.h"
#include "bsp_usart.h"
//切换表当前的模式
void switchMode(unsigned char mode);
//获取当前模式
unsigned char getCurrentMode();

void valueUpdateLooper();

#endif