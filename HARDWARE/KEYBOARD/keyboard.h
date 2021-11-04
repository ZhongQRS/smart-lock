#ifndef __KEYBOARD_H
#define __KEYBOARD_H

#include "sys.h"
#include "stm32f4xx.h"
#include "delay.h"
#include "usart.h"
#include "includes.h"

#define KEY_ON	0
#define KEY_OFF	1

void KEYBOARD_Init(void);
void set_one(void);
void KEYBOARD_Num(void);
u8 KeyBOARD_Scan(GPIO_TypeDef* GPIOx,u16 GPIO_Pin);

#endif
