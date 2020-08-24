/**
 *欧姆表模块 
 * 
 * 
 * */
#ifndef _RESISTANCE_METER_H_
#define _RESISTANCE_METER_H_

#define UNIT_OHM 6
#define UNIT_KOHM 7
#define UNIT_MOHM 8



void openResistanceMeter();
void closeResistanceMeter();
unsigned char  getResistanceMeterState();
void ResistanceMeterInit();
float getCurrentRes();
unsigned char getR_Unit();
void ResistanceMeterLooper();

#endif