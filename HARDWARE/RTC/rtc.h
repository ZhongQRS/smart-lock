#ifndef __RTC_H
#define __RTC_H

//////////////////////////////////////////////////////////////////////////////////	 
//						用于GEC-STM32f407ZE开发板
//						RTC时钟驱动代码	   
//						作者：unkim
//						联系方式：122078699@qq.com
//						创建日期:2020/3/23									  
////////////////////////////////////////////////////////////////////////////////// 

#include "stm32f4xx.h"
#include "usart.h"
#include "delay.h"

static NVIC_InitTypeDef		NVIC_InitStructure;
static EXTI_InitTypeDef  	EXTI_InitStructure;
static RTC_TimeTypeDef  	RTC_TimeStructure;
static RTC_DateTypeDef  	RTC_DateStructure;
static RTC_InitTypeDef  	RTC_InitStructure;
static RTC_AlarmTypeDef 	RTC_AlarmStructure;

void RTC_Set_WakeUp(void);
ErrorStatus RTC_Set_Time(u8 hour,u8 min,u8 sec,u8 ampm);
ErrorStatus RTC_Set_Date(u8 year,u8 month,u8 date,u8 week);
u8 My_RTC_Init(void);
void USART1_Printf_Time(void);
void RTC_Set_AlarmA(u8 week,u8 hour,u8 min,u8 sec);

#endif
