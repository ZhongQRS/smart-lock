#include "beep.h"

//蜂鸣器初始化
void BEEP_Init(void)
{
	GPIO_InitTypeDef  GPIO_InitStructure;
	
	//打开端口时钟
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOF, ENABLE);
	
	//配置端口
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_8;				//引脚8
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;			//输出功能
	GPIO_InitStructure.GPIO_Speed = GPIO_High_Speed;		//速率
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;			//推挽输出
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;		//非上拉电阻
	GPIO_Init(GPIOF, &GPIO_InitStructure);					//端口初始化
}
