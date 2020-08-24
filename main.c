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

/**************函数声明**************/
unsigned char currentMode;
void init();
void onHoldButtonClicked();
void onModeButtonClicked();

/**************函数定义**************/

int main()
{

    init();

    while(1){
        //放置各个模块的looper，模拟不同的线程运行

        //按键监听循环
        buttonLooper();
        //数值更新循环
        valueUpdateLooper();
       
        
    }
}

/**
 * @description: 数值更新函数，根据当前模式自动开启相应的模块并更新屏幕显示
 * @param {type} 
 * @return {type} 
 */
void valueUpdateLooper(){
    
        switch( getCurrentMode() ){
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
 * @description: 系统初始化函数
 * @param {type} 
 * @return {type} 
 */
void init(){
	
    //GUI初始化
    GUIInit();
    
	USART_Config();
    //按键初始化
    buttonInit();
    //设置按键回调函数
    setHoldButtonListener(onHoldButtonClicked);
    setModeButtonListener(onModeButtonClicked);

    
    //不同表初始化
    VoltageMeterInit();
    ResistanceMeterInit();
    CurrentMeterInit();
    
    //设置初始模式为电压表
	
    switchMode(MODE_VOLTAGE);
    


}
/**
 * @description: hold按键按下时暂停屏幕更新功能
 * @param {type} 
 * @return {type} 
 */
void onHoldButtonClicked(){
    holdTrigger();
}
/**
 * @description: 模式按键按下时切换模式
 * @param {type} 
 * @return {type} 
 */
void onModeButtonClicked(){
    if (getCurrentMode()==2)
    {
        switchMode(0);
    }
    else{
        switchMode(getCurrentMode()+1);
    }
}
/**
 * @description: 切换电表模式
 * @param {type} 
 * @return {type} 
 */
void switchMode(unsigned char mode){
    switch (mode)
    {
    case MODE_VOLTAGE:
        currentMode=MODE_VOLTAGE;
        setDisplayMode(MODE_VOLTAGE);
        //打开和关闭相应的模块
        closeCurrentMeter();
        closeResistanceMeter();
        openVoltageMeter();
       
        
        break;
    case MODE_CURRENT:
        currentMode=MODE_CURRENT;
        setDisplayMode(MODE_CURRENT);

        closeResistanceMeter();
        closeVoltageMeter();
        openCurrentMeter();
        
        break;
    case MODE_RESISTANCE:
        currentMode=MODE_RESISTANCE;
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
unsigned char getCurrentMode(){
    return currentMode;
}