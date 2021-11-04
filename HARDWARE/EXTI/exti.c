#include "exti.h"

//中断线9 初始化函数
void EXTI9_Init(void)
{
	//使能SYSCFG时钟： 使用中断/事件控制器必须使能这个时钟
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_SYSCFG, ENABLE);
	//打开GPIO G组
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOG, ENABLE);
	
	GPIO_InitStruct.GPIO_Pin	= GPIO_Pin_9;  	//引脚6
	GPIO_InitStruct.GPIO_Mode	= GPIO_Mode_IN;	//输入模式
	GPIO_InitStruct.GPIO_PuPd	= GPIO_PuPd_DOWN;	//上拉
	GPIO_Init(GPIOG,&GPIO_InitStruct);//初始化IO口PG9为输入
	
	SYSCFG_EXTILineConfig(EXTI_PortSourceGPIOG, EXTI_PinSource9);	//PG9
	
	EXTI_InitStructure.EXTI_Line = EXTI_Line9;
	EXTI_InitStructure.EXTI_LineCmd = ENABLE;
	EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
	EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Rising;
	EXTI_Init(&EXTI_InitStructure);
	
	NVIC_InitStructure.NVIC_IRQChannel = EXTI9_5_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 2;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 2;
	NVIC_Init(&NVIC_InitStructure);
}
