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
#include <stdio.h>

unsigned char holdTriggerSign;
unsigned char mode;
/**
 * @description: 
 * @param {type} 
 * @return {type} 
 */
//设置屏幕显示的模式,仅仅会改变屏幕的显示，而不会真正改变电表模式
void setDisplayMode(unsigned char mode){

    mode=mode;
    LCD_ClearLine(LINE(0));
    switch (mode)
    {
    case MODE_VOLTAGE:
        ILI9341_DispStringLine_EN(LINE(0),"Voltage mode:");
        break;
    case MODE_CURRENT:
        ILI9341_DispStringLine_EN(LINE(0),"Current mode:");
        break;
    case MODE_RESISTANCE:
        ILI9341_DispStringLine_EN(LINE(0),"Resistence mode:");
        break;
    default:
        break;
    }

}
//获取当前显示模式
/**
 * @description: 
 * @param {type} 
 * @return {type} 
 */
unsigned char getDisplayMode(){
    return mode;
}
//绘制系统界面
/**
 * @description: 
 * @param {type} 
 * @return {type} 
 */
void drawOutline(){

    LCD_SetFont(&Font16x24);
	LCD_SetColors(RED,BLACK);
    ILI9341_Clear(0,0,LCD_X_LENGTH,LCD_Y_LENGTH);	
    ILI9341_DispStringLine_EN(LINE(0),"TFT init done......");
    ILI9341_DispStringLine_EN(LINE(1),"loading system ......");
    LCD_SetColors(RED,WHITE);
    ILI9341_Clear(0,0,LCD_X_LENGTH,LCD_Y_LENGTH);

}
//hold功能
/**
 * @description: 
 * @param {type} 
 * @return {type} 
 */
void holdTrigger(){
    LCD_ClearLine(LINE(3));
    if(holdTriggerSign){
        ILI9341_DispStringLine_EN(LINE(3),"HOLDING!");
    }
    else{
        ILI9341_DispStringLine_EN(LINE(3),"RUNNING");
    }
    holdTriggerSign=~holdTriggerSign;
}
//清空界面
/**
 * @description: 
 * @param {type} 
 * @return {type} 
 */
void clearWindow(){
    LCD_SetColors(RED,WHITE);
    ILI9341_Clear(0,0,LCD_X_LENGTH,LCD_Y_LENGTH);
}
//设置主显示数字
/**
 * @description: 
 * @param {type} 
 * @return {type} 
 */
void setNumTitle(float num){

    char dispBuff[100];
    sprintf(dispBuff,"value : %f  ",num);
    ILI9341_DispStringLine_EN(LINE(1),dispBuff);

}
//设置显示的单位
/**
 * @description: 
 * @param {type} 
 * @return {type} 
 */
void setUnit(unsigned char unit){
    
    switch(unit){
        case UNIT_V:ILI9341_DispStringLine_EN(LINE(2),"V"); break;
        case UNIT_MV:ILI9341_DispStringLine_EN(LINE(2),"mV"); break;
        case UNIT_UV:ILI9341_DispStringLine_EN(LINE(2),"uV"); break;
        case UNIT_A:ILI9341_DispStringLine_EN(LINE(2),"A"); break;
        case UNIT_MA:ILI9341_DispStringLine_EN(LINE(2),"mA"); break;
        case UNIT_UA:ILI9341_DispStringLine_EN(LINE(2),"uA"); break;
        case UNIT_OHM:ILI9341_DispStringLine_EN(LINE(2),"ohm"); break;
        case UNIT_KOHM:ILI9341_DispStringLine_EN(LINE(2),"Kohm"); break;
        case UNIT_MOHM:ILI9341_DispStringLine_EN(LINE(2),"Mohm"); break;

        
    }
}
/**
 * @description: 
 * @param {type} 
 * @return {type} 
 */
void GUIInit(){
	
    holdTriggerSign=0;
	ILI9341_Init();
    drawOutline();
}