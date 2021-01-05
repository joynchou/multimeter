/*
 * @文件描述: main函数所在文件
 * @版本: 
 * @注意: 本项目的程序采用了软硬分离的代码结构，只要对底层驱动做适配，程序本身不依赖具体的硬件，方便运行在各种硬件平台之上
 * @作者: 周晨阳
 * @Date: 2020-06-21 15:37:43
 */
/**
 * 
 * 各种looper函数都需要最终在mian的while中执行
 * 以此来模拟线程
 * 
 * */

/**************头文件**************/
#include "main.h"
#include <stdio.h>
#include "filter.h"
#include "bsp_xpt2046_lcd.h"
#include "GUI.h"
#include <rtthread.h>
#include "mainPage.h"
/**************函数声明**************/
unsigned char currentMode;
void init();
void onHoldButtonClicked();
void onModeButtonClicked();

/**************函数定义**************/
/**
 * @description: 实体按键扫描线程
 * @param {void} *parameter
 * @return {*}
 */
void buttonThread(void *parameter)
{
    while (1)
    {
        buttonLooper();

        rt_thread_mdelay(30);
    }
}
/**
 * @description: 触屏扫描线程
 * @param {void} *parameter
 * @return {*}
 */
void touchScreenThread(void *parameter)
{
    while (1)
    {

        GUI_TOUCH_Exec(); //调用UCGUI TOUCH相关函数
        rt_thread_mdelay(10);
    }
}
/**
 * @description: tft刷新线程
 * @param {void} *parameter
 * @return {*}
 */
void tftUpdateThread(void *parameter)
{
    while (1)
    {

        //mainLoop();
        GUI_Exec(); //调用UCGUI TOUCH相关函数
        rt_thread_mdelay(90);
    }
}
/**
 * @description: 各个模块数值更新线程
 * @param {void} *parameter
 * @return {*}
 */
void valueUpdateThread(void *parameter)
{

    while (1)
    {
        valueUpdateLooper();
        rt_thread_mdelay(200);
    }
}

void setSystem(void *parameter)
{
    switchMode(MODE_VOLTAGE);
}
//各个线程的句柄和栈
static struct rt_thread tftUpdateThread_ptr;
static rt_uint8_t tftUpdateThread_stack[2000];

static struct rt_thread touchScreenThread_ptr;
static rt_uint8_t touchScreenThread_stack[800];

static struct rt_thread buttonThread_ptr;
static rt_uint8_t buttonThread_stack[800];

static struct rt_thread ValueThread_ptr;
static rt_uint8_t ValueThread_stack[1800];

int main()
{

    rt_err_t result;
    rt_thread_t settingPtr;
    init();

    /* 启动线程 */

    result = rt_thread_init(&touchScreenThread_ptr,
                            "touchScreenThread",
                            touchScreenThread, RT_NULL,
                            &touchScreenThread_stack[0], sizeof(touchScreenThread_stack),
                            1, 100);

    /* 启动线程 */
    result = rt_thread_init(&buttonThread_ptr,
                            "buttonThread",
                            buttonThread, RT_NULL,
                            &buttonThread_stack[0], sizeof(buttonThread_stack),
                            2, 100);

    result = rt_thread_init(&tftUpdateThread_ptr,
                            "tftUpdateThread",
                            tftUpdateThread, RT_NULL,
                            &tftUpdateThread_stack[0], sizeof(tftUpdateThread_stack),
                            2, 100);
    result = rt_thread_init(&ValueThread_ptr,
                            "ValueThread",
                            valueUpdateThread, RT_NULL,
                            &ValueThread_stack[0], sizeof(ValueThread_stack),
                            0, 1000);

    settingPtr = rt_thread_create("setting", setSystem, RT_NULL, 600, 3, 100);
    rt_thread_startup(&tftUpdateThread_ptr);
    printf("tftUpdateThread created\n");

    rt_thread_startup(&touchScreenThread_ptr);
    printf("touchScreenThread created\n");

    rt_thread_startup(&buttonThread_ptr);
    printf("button scan thread created\n");
    rt_thread_startup(&ValueThread_ptr);
    printf("Value thread created\n");
    rt_thread_startup(settingPtr);

    //设置初始模式为电压表
    switchMode(MODE_VOLTAGE);
    return 0;
}

/**
 * @description: 系统初始化函数
 * @param {type} 
 * @return {type} 
 */
void init()
{

    //串口初始化
    USART_Config();
    DSP_Init();
    //GUI初始化
    GUIInit();
    //主界面ui初始化
    MainPageInit();

    //显示鼠标
#ifdef SHOW_MOUSE
    GUI_CURSOR_Show();
#endif
    //按键初始化
    buttonInit();

    //不同表初始化
    CurrentMeterInit();
    VoltageMeterInit();
    ResistanceMeterInit();

    //设置按键回调函数
    setHoldButtonListener(onHoldButtonClicked);
    setModeButtonListener(onModeButtonClicked);
}
/**
 * @description: 数值更新函数，根据当前模式自动开启相应的模块并更新屏幕显示
 * @param {type} 
 * @return {type} 
 */
void valueUpdateLooper()
{

    switch (getCurrentMode())
    {
    case MODE_VOLTAGE:
        //模块更新函数
        VoltageMeterLooper();
        setNumTitle(getVoltage());
        setUnit(getU_Unit());
        break;
    case MODE_CURRENT:
        CurrentMeterLooper();
        setNumTitle(getCurrent());
        setUnit(getI_Unit());
        break;
    case MODE_RESISTANCE:
        ResistanceMeterLooper();
        setNumTitle(getCurrentRes());
        setUnit(getR_Unit());
        break;
    }
}
/**
 * @description: hold按键按下时暂停屏幕更新功能
 * @param {type} 
 * @return {type} 
 */
void onHoldButtonClicked()
{
    holdTrigger();
}
/**
 * @description: 模式按键按下时切换模式
 * @param {type} 
 * @return {type} 
 */
void onModeButtonClicked()
{
    if (getCurrentMode() == 2)
    {
        switchMode(0);
    }
    else
    {
        switchMode(getCurrentMode() + 1);
    }
}
/**
 * @description: 切换电表模式
 * @param {type} 
 * @return {type} 
 */
void switchMode(unsigned char mode)
{
    switch (mode)
    {
    case MODE_VOLTAGE:
        currentMode = MODE_VOLTAGE;
        setDisplayMode(MODE_VOLTAGE);

        //打开和关闭相应的模块
        closeCurrentMeter();
        closeResistanceMeter();
        openVoltageMeter();

        break;
    case MODE_CURRENT:
        currentMode = MODE_CURRENT;
        setDisplayMode(MODE_CURRENT);

        closeResistanceMeter();
        closeVoltageMeter();
        openCurrentMeter();

        break;
    case MODE_RESISTANCE:
        currentMode = MODE_RESISTANCE;
        setDisplayMode(MODE_RESISTANCE);

        closeVoltageMeter();
        closeCurrentMeter();
        openResistanceMeter();

        break;
    }
}
/**
 * @description: 取得当前模式
 * @param {type} 
 * @return {type} 
 */
unsigned char getCurrentMode()
{
    return currentMode;
}