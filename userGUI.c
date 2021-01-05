/**
 * 该文件中的函数不会对万用表除了屏幕的其他模块造成影响 
 * 只会对屏幕造成影响
 * 如需要真正控制其他模块，如电压表
 * 需要使用相应模块中的开关函数
 * 
 * 同时，如果需要更换屏幕，只需要适配这个文件中相应的函数即可
 * 
 * 该文件充当presenter层，用于衔接各个模块和UI之间的数据传递，以便于解耦
 * */
#include "userGUI.h"
#include "palette.h"
#include <stdio.h>
#include "public.h"
#include "bsp_ILI9341_lcd.h"
#include "bsp_xpt2046_lcd.h"
#include "bsp_spi_flash.h"
#include "GUI.h"
#include "mainPage.h"
#include "button.h"
#include "voltageMeter.h"
#include "CurrentMeter.h"
#include "ResistanceMeter.h"
#include "main.h"
//=1的时候代表屏幕被冻结
unsigned char holdTriggerSign = 0;
unsigned char mode;

static unsigned char last_Unit;
static float last_value;
/**
 * @description: 设置ui显示的量程
 * @param {char} *range
 * @return {*}
 */
void setDisRange(unsigned char mode)
{
    int voltageRange;
    static char dispBuff[15];
    switch (mode)
    {
    case MODE_VOLTAGE:
        if (getVoltageFactor() == 1)
        {
            voltageRange = 160;
        }
        else if (getVoltageFactor() == 10)
        {
            voltageRange = 16;
        }
        sprintf(dispBuff, "-%d ~ +%d V", voltageRange, voltageRange);
        UIsetRange(dispBuff);
        break;
    }
}
/**
 * @description: //设置屏幕显示的模式,仅仅会改变屏幕的显示，而不会真正改变电表模式，由内部调用
 * @param {type} 
 * @return {type} 
 */
void setDisplayMode(unsigned char mode)
{
    int voltageRange;
    float currentRange;
    static char dispBuff[15];
    mode = mode;
    setDisRange(mode);
    //LCD_ClearLine(LINE(0));
    
        switch (mode)
        {
        case MODE_VOLTAGE:

            // if (getVoltageFactor() == 1)
            // {
            //     voltageRange = 160;
            // }
            // else if (getVoltageFactor() == 10)
            // {
            //     voltageRange = 16;
            // }
            // ////ILI9341_DispStringLine_EN(LINE(0), "Voltage mode:");
            // sprintf(dispBuff, "Range:-%d to +%d V", voltageRange, voltageRange);
            // ////ILI9341_DispStringLine_EN(LINE(4), dispBuff);
            UIsetMode(MODE_VOLTAGE);
            setUnit(UNIT_V);
            break;
        case MODE_CURRENT:

            if (getCurrentFactor() == 1)
            {
                currentRange = 3;
            }
            else if (getCurrentFactor() == 5)
            {
                currentRange = 0.6f;
            }
            ////ILI9341_DispStringLine_EN(LINE(0), "Current mode:");
            sprintf(dispBuff, "Range:-%f to +%f A", currentRange, currentRange);
            ////ILI9341_DispStringLine_EN(LINE(4), dispBuff);
            UIsetMode(MODE_CURRENT);
            break;
        case MODE_RESISTANCE:
            ////ILI9341_DispStringLine_EN(LINE(0), "Resistence mode:");
            UIsetMode(MODE_RESISTANCE);
            break;
        default:
            break;
        }

        
}
/**
 * @description: 切换电表的模式，由ui调用
 * @param {int} mode
 * @return {*}
 */
void setMeterMode(int mode)
{
    switchMode(mode);
}
/**
 * @description: //获取当前显示模式
 * @param {type} 
 * @return {type} 
 */
unsigned char getDisplayMode()
{
    return mode;
}
/**
 * @description: 获取内部的电表模式，非显示模式，由ui调用
 * @param {*}
 * @return {*}
 */
int getMeterMode()
{
    return getCurrentMode();
}
/**
 * @description: //绘制系统界面(弃用)
 * @param {type} 
 * @return {type} 
 */
void drawOutline()
{

    LCD_SetFont(&Font16x24);
    LCD_SetColors(RED, BLACK);
    //ILI9341_Clear(0, 0, LCD_X_LENGTH, LCD_Y_LENGTH);
    //ILI9341_DispStringLine_EN(LINE(0), "TFT init done......");
    //ILI9341_DispStringLine_EN(LINE(1), "loading system ......");
    LCD_SetColors(RED, BLACK);
    ////ILI9341_Clear(0,0,LCD_X_LENGTH,LCD_Y_LENGTH);
}

/**
 * @description: hold功能，由按键模块和UI调用
 * @param {type} 
 * @return {type} 
 */
void holdTrigger()
{
    //LCD_ClearLine(LINE(3));
    LED_G_TOGGLE;
    UIsetRunningState(holdTriggerSign);
    // if (!holdTriggerSign)
    // {
    //     //ILI9341_DispStringLine_EN(LINE(6), "HOLDING!");

    // }
    // else
    // {
    //     //ILI9341_DispStringLine_EN(LINE(6), "RUNNING");
    // }
    holdTriggerSign = ~holdTriggerSign;
}

/**
 * @description: 清空界面(弃用)
 * @param {type} 
 * @return {type} 
 */
void clearWindow()
{
    LCD_SetColors(RED, WHITE);
    //ILI9341_Clear(0, 0, LCD_X_LENGTH, LCD_Y_LENGTH);
}

/**
 * @description: 设置主界面显示的数字,由数值更新模块调用
 * @param {type} 
 * @return {type} 
 */
void setNumTitle(float num)
{

    //static char dispBuff[15];
    if (!holdTriggerSign)
    {
        // sprintf(dispBuff, "value:%f", num);
        //ILI9341_DispStringLine_EN(LINE(1), dispBuff);
        UIsetNum(num);
        last_value = num;
    }
    else
    {
        // sprintf(dispBuff, "value:%f", last_value);
        UIsetNum(last_value);
        //ILI9341_DispStringLine_EN(LINE(1), dispBuff);
    }
}

/**
 * @description: 设置显示的单位（私有函数）
 * @param {type} 
 * @return {type} 
 */
static void drawUnitByCode(unsigned char code)
{
    switch (code)
    {

    case UNIT_V:
        UIsetDisUnit("V");
        //ILI9341_DispStringLine_EN(LINE(2), "V");
        break;
    case UNIT_MV:
        UIsetDisUnit("mV");
        //ILI9341_DispStringLine_EN(LINE(2), "mV");
        break;
    case UNIT_UV:
        UIsetDisUnit("uV");
        //ILI9341_DispStringLine_EN(LINE(2), "uV");
        break;
    case UNIT_A:
        UIsetDisUnit("A");
        //ILI9341_DispStringLine_EN(LINE(2), "A");
        break;
    case UNIT_MA:
        UIsetDisUnit("mA");
        //ILI9341_DispStringLine_EN(LINE(2), "mA");
        break;
    case UNIT_UA:
        UIsetDisUnit("uA");
        //ILI9341_DispStringLine_EN(LINE(2), "uA");
        break;
    case UNIT_OHM:
        UIsetDisUnit("R");
        //ILI9341_DispStringLine_EN(LINE(2), "ohm");
        break;
    case UNIT_KOHM:
        UIsetDisUnit("KR");
        //ILI9341_DispStringLine_EN(LINE(2), "Kohm");
        break;
    case UNIT_MOHM:
        UIsetDisUnit("MR");
        //ILI9341_DispStringLine_EN(LINE(2), "Mohm");
        break;
    }
}
/**
 * @description: 设置当前主界面所显示的单位
 * @param {unsignedchar} unit
 * @return {*}
 */
void setUnit(unsigned char unit)
{
    if (!holdTriggerSign)
    {

        drawUnitByCode(unit);
        last_Unit = unit;
    }
    else
    {

        drawUnitByCode(last_Unit);
    }
}
/**
 * @description: 设置不同表的量程，每一个表都有两个量程，该函数由ui调用，由用户决定当前的量程
 * @param {int} mode
 * @param {int} range
 * @return {*}
 */
void setRange(int mode)
{
    switch (mode)
    {
    case MODE_VOLTAGE:
        if (getVoltageFactor() == 1)
        {
            changeVoltageFactor(10);
        }
        else if (getVoltageFactor() == 10)
        {
            changeVoltageFactor(1);
        }

        break;

    case MODE_CURRENT:
        //changeCurrentFactor(range);
        break;

    case MODE_RESISTANCE:
        //changeResistanceFactor(range);
        break;
    }
    setDisRange(mode);
}
/**
 * @description: GUI初始化
 * @param {type} 
 * @return {type} 
 */
void GUIInit()
{

    holdTriggerSign = 0;
    ILI9341_Init();
    XPT2046_Init();
    Calibrate_or_Get_TouchParaWithFlash(3, 0);
    ILI9341_DispStringLine_EN(LINE(0), "TFT init done......");
    ILI9341_DispStringLine_EN(LINE(1), "loading system ......");
    GUI_Init(); //ucgui初始化
    printf("GUI init done...\n");
}
