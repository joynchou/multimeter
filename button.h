/*
 * @文件描述: 
 * @版本: 
 * @作者: 周晨阳
 * @Date: 2020-06-21 15:43:40
 */
#ifndef _BUTTON_H_
#define _BUTTON_H_
#include "stm32f10x.h"
#define KEY_ON 1
#define KEY_OFF 0

#define KEY1_GPIO_PIN GPIO_Pin_0
#define KEY1_GPIO_PORT GPIOA
#define KEY1_GPIO_CLK RCC_APB2Periph_GPIOA

#define KEY2_GPIO_PIN GPIO_Pin_13
#define KEY2_GPIO_PORT GPIOC
#define KEY2_GPIO_CLK RCC_APB2Periph_GPIOC

#define LED_G_GPIO_PIN GPIO_Pin_0
#define LED_G_GPIO_PORT GPIOB
#define LED_G_GPIO_CLK RCC_APB2Periph_GPIOB

#define ON 1
#define OFF 0

#define LED_G(a)                                         \
    if (a)                                               \
        GPIO_ResetBits(LED_G_GPIO_PORT, LED_G_GPIO_PIN); \
    else                                                 \
        GPIO_SetBits(LED_G_GPIO_PORT, LED_G_GPIO_PIN);

#define LED_G_TOGGLE                            \
    {                                           \
        LED_G_GPIO_PORT->ODR ^= LED_G_GPIO_PIN; \
    }

void buttonInit();
void buttonLooper();
void setModeButtonListener(void (*callback)());
void setChangeFactorButtonLis(void(*callback));
void setHoldButtonListener(void (*callback)());

#endif