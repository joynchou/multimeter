/*
 * @文件描述: 
 * @版本: 
 * @作者: 周晨阳
 * @Date: 2021-01-02 16:36:18
 */
#ifndef _FILTER_H_
#define _FILTER_H_


void DSP_Init();

void firFilter(float *input, float *output, int blockSize);

#endif