/*
 * @文件描述: DSP相关函数
 * @版本: 
 * @作者: 周晨阳
 * @Date: 2021-01-02 15:51:14
 */
#include "arm_math.h"

#define BLOCK_SIZE 20           /* 调用一次arm_fir_f32处理的采样点个数 */
#define NUM_TAPS 31             /* 滤波器系数个数 */

arm_fir_instance_f32 S;
float32_t *inputF32, *outputF32;
uint32_t blockSize = BLOCK_SIZE;


static float32_t firStateF32[BLOCK_SIZE + NUM_TAPS - 1]; /* 状态缓存，大小numTaps + blockSize - 1*/

const float32_t firCoeffs32LP[NUM_TAPS] = {
    0.004914000165, 0.005531451665, 0.007356818765,  0.01031032484,  0.01426288858,
    0.01904176362,  0.02443808876,  0.03021602146,  0.03612303734,  0.04190097004,
    0.04729729891,  0.05207617581,  0.05602874234,  0.05898225307,  0.06080762297,
    0.06142507493,  0.06080762297,  0.05898225307,  0.05602874234,  0.05207617581,
    0.04729729891,  0.04190097004,  0.03612303734,  0.03021602146,  0.02443808876,
    0.01904176362,  0.01426288858,  0.01031032484, 0.007356818765, 0.005531451665,
   0.004914000165};
/**
 * @description: FIR数字低通滤波器，输入的数组长度必须为20的倍数，如 20，40，60
 * @param {float} *input 输入数组
 * @param {float} *output 输出数组
 * @param {int} blockSize 数组长度
 * @return {*}
 */
void firFilter(float *input, float *output, int blockSize)
{
    int i;
    /* 实现FIR滤波 */
     for(i=0; i < blockSize/BLOCK_SIZE; i++)
     {
         arm_fir_f32(&S, input + (i * blockSize), output + (i * blockSize), blockSize);
     }

}
/**
 * @description: DSP模块初始化
 * @param {*}
 * @return {*}
 */
void DSP_Init()
{
    /* fir低通滤波器初始化 */
     arm_fir_init_f32(&S, NUM_TAPS, (float32_t *)&firCoeffs32LP[0], &firStateF32[0], blockSize);
}
