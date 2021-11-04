#ifndef __EXTI_H
#define __EXTI_H

//////////////////////////////////////////////////////////////////////////////////	 
//						用于GEC-STM32f407ZE开发板
//						外部中断驱动代码	   
//						作者：unkim
//						联系方式：122078699@qq.com
//						创建日期:2020/3/21									  
//////////////////////////////////////////////////////////////////////////////////

#include "stm32f4xx.h"
#include "led.h"
#include "sys.h"
#include "delay.h"

static GPIO_InitTypeDef  GPIO_InitStruct;
static EXTI_InitTypeDef  EXTI_InitStructure;
static NVIC_InitTypeDef  NVIC_InitStructure;

void EXTI0_Init(void);
void EXTI2_Init(void);
void EXTI3_Init(void);
void EXTI4_Init(void);
void EXTI9_Init(void);

#endif
