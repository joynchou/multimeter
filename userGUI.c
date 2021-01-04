/**
 * 该文件中的函数不会对万用表除了屏幕的其他模块造成影响 
 * 只会对屏幕造成影响
 * 如需要真正控制其他模块，如电压表
 * 需要使用相应模块中的开关函数
 * 
 * 同时，如果需要更换屏幕，只需要适配这个文件中相应的函数即可
 * 
 * */
#include "userGUI.h"
#include "palette.h"
#include <stdio.h>
#include "public.h"
#include "bsp_ILI9341_lcd.h"
#include "bsp_xpt2046_lcd.h"
#include "bsp_spi_flash.h"
//#include "mainPage.h"
//=1的时候代表屏幕被冻结
unsigned char holdTriggerSign = 0;
unsigned char mode;

static unsigned char last_Unit;
static float last_value;

/**
 * @description: //设置屏幕显示的模式,仅仅会改变屏幕的显示，而不会真正改变电表模式
 * @param {type} 
 * @return {type} 
 */
void setDisplayMode(unsigned char mode)
{
    int voltageRange;
    float currentRange;
    static char dispBuff[15];
    mode = mode;

    //LCD_ClearLine(LINE(0));

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
        ////ILI9341_DispStringLine_EN(LINE(0), "Voltage mode:");
        sprintf(dispBuff, "Range:-%d to +%d V", voltageRange, voltageRange);
        ////ILI9341_DispStringLine_EN(LINE(4), dispBuff);

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

        break;
    case MODE_RESISTANCE:
        ////ILI9341_DispStringLine_EN(LINE(0), "Resistence mode:");

        break;
    default:
        break;
    }
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
 * @description: //绘制系统界面
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
 * @description: hold功能
 * @param {type} 
 * @return {type} 
 */
void holdTrigger()
{
    //LCD_ClearLine(LINE(3));
    if (!holdTriggerSign)
    {
        //ILI9341_DispStringLine_EN(LINE(6), "HOLDING!");
    }
    else
    {
        //ILI9341_DispStringLine_EN(LINE(6), "RUNNING");
    }
    holdTriggerSign = ~holdTriggerSign;
}

/**
 * @description: 清空界面
 * @param {type} 
 * @return {type} 
 */
void clearWindow()
{
    LCD_SetColors(RED, WHITE);
    //ILI9341_Clear(0, 0, LCD_X_LENGTH, LCD_Y_LENGTH);
}

/**
 * @description: 设置主显示数字
 * @param {type} 
 * @return {type} 
 */
void setNumTitle(float num)
{

    static char dispBuff[15];
    if (!holdTriggerSign)
    {
        // sprintf(dispBuff, "value:%f", num);
        //ILI9341_DispStringLine_EN(LINE(1), dispBuff);
        //setNum(num);
        last_value = num;
    }
    else
    {
        // sprintf(dispBuff, "value:%f", last_value);
        //setNum(last_value);
        //ILI9341_DispStringLine_EN(LINE(1), dispBuff);
    }
}

/**
 * @description: 设置显示的单位
 * @param {type} 
 * @return {type} 
 */

static void drawUnitByCode(unsigned char code)
{
    switch (code)
    {

    case UNIT_V:
        //ILI9341_DispStringLine_EN(LINE(2), "V");
        break;
    case UNIT_MV:
        //ILI9341_DispStringLine_EN(LINE(2), "mV");
        break;
    case UNIT_UV:
        //ILI9341_DispStringLine_EN(LINE(2), "uV");
        break;
    case UNIT_A:
        //ILI9341_DispStringLine_EN(LINE(2), "A");
        break;
    case UNIT_MA:
        //ILI9341_DispStringLine_EN(LINE(2), "mA");
        break;
    case UNIT_UA:
        //ILI9341_DispStringLine_EN(LINE(2), "uA");
        break;
    case UNIT_OHM:
        //ILI9341_DispStringLine_EN(LINE(2), "ohm");
        break;
    case UNIT_KOHM:
        //ILI9341_DispStringLine_EN(LINE(2), "Kohm");
        break;
    case UNIT_MOHM:
        //ILI9341_DispStringLine_EN(LINE(2), "Mohm");
        break;
    }
}
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
 * @description: 
 * @param {type} 
 * @return {type} 
 */
void GUIInit()
{

    holdTriggerSign = 0;
    ILI9341_Init();
    XPT2046_Init();

    Calibrate_or_Get_TouchParaWithFlash(3, 0);

    ////ILI9341_GramScan(4);

    // Palette_Init(LCD_SCAN_MODE);
    // drawOutline();
    printf("GUI init done...\n");
}
