#include "timer.h"

extern vu16 USART2_RX_STA;

//高级定时器 1  初始化函数
void tim1_init(void)
{
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_TIM1, ENABLE);
	
	TIM_TimeBaseStructure.TIM_Period = (100*10)-1;				//计数值	100ms
	TIM_TimeBaseStructure.TIM_Prescaler = 16800-1;				//预分频值，也可以理解为第一次分频	高级定时器16800
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;	//向上计数
	TIM_TimeBaseInit(TIM1, &TIM_TimeBaseStructure);
	
	TIM_ITConfig(TIM1,TIM_IT_Update , ENABLE);
	
	TIM_Cmd(TIM1, ENABLE);
	
	NVIC_InitStructure.NVIC_IRQChannel = TIM1_UP_TIM10_IRQn;		
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0x0F;//抢占优先级
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0x0F;		//响应优先级
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;		
	NVIC_Init(&NVIC_InitStructure);	
}

//通用定时器 2	初始化函数
void tim2_init(void)
{
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2, ENABLE);
	
	TIM_TimeBaseStructure.TIM_Period = (200*10)-1;				//计数值	200ms
	TIM_TimeBaseStructure.TIM_Prescaler = 8400-1;				//预分频值，也可以理解为第一次分频
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;	//向上计数
	TIM_TimeBaseInit(TIM2, &TIM_TimeBaseStructure);
	
	TIM_ITConfig(TIM2,TIM_IT_Update , ENABLE);
	
	TIM_Cmd(TIM2, ENABLE);
	
	NVIC_InitStructure.NVIC_IRQChannel = TIM2_IRQn;		
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0x0F;//抢占优先级
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0x0F;		//响应优先级
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;		
	NVIC_Init(&NVIC_InitStructure);	
}

//通用定时器 3	初始化函数
void tim3_init(void)
{
	//使能定时器3的硬件时钟
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3, ENABLE);
	
	
	//配置定时器3参数：定时时间
	//定时器3的硬件时钟=84MHz/8400=10000Hz
	//只要定时器3进行10000次计数，就是1秒时间的到达
	//只要定时器3进行10次计数，就是1毫秒时间的到达
	TIM_TimeBaseStructure.TIM_Period = (500*10)-1;				//计数值	500ms
	TIM_TimeBaseStructure.TIM_Prescaler = 8400-1;				//预分频值，也可以理解为第一次分频
	//TIM_TimeBaseStructure.TIM_ClockDivision = 0;				//F407是没有时钟分频
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;	//向上计数
	TIM_TimeBaseInit(TIM3, &TIM_TimeBaseStructure);
	
	//配置定时器3的中断
	TIM_ITConfig(TIM3,TIM_IT_Update , ENABLE);
	
	//启动定时器3
	TIM_Cmd(TIM3, ENABLE);

	NVIC_InitStructure.NVIC_IRQChannel = TIM3_IRQn;				//定时器3的中断号
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0x0F;//抢占优先级
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0x0F;		//响应优先级
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;				//NVIC使能定时器3中断请求通道
	NVIC_Init(&NVIC_InitStructure);	
}

//高级定时器 8	初始化函数
void tim8_init(void)
{
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_TIM8, ENABLE);
	
	TIM_TimeBaseStructure.TIM_Period = (2000*10)-1;				//计数值	2000ms
	TIM_TimeBaseStructure.TIM_Prescaler = 16800-1;				//预分频值，也可以理解为第一次分频
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;	//向上计数
	TIM_TimeBaseInit(TIM8, &TIM_TimeBaseStructure);
	
	TIM_ITConfig(TIM8,TIM_IT_Update , ENABLE);
	
	TIM_Cmd(TIM8, ENABLE);
	
	NVIC_InitStructure.NVIC_IRQChannel = TIM8_UP_TIM13_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0x0F;//抢占优先级
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0x0F;		//响应优先级
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);	
}


//通用定时器中断初始化
//这里始终选择为APB1的2倍，而APB1为42M
//arr：自动重装值。
//psc：时钟预分频数		 
void TIM7_Int_Init(u16 arr,u16 psc)
{	
	NVIC_InitTypeDef NVIC_InitStructure;
	TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;

	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM7, ENABLE);//TIM7时钟使能    
	
	//定时器TIM7初始化
	TIM_TimeBaseStructure.TIM_Period = arr; //设置在下一个更新事件装入活动的自动重装载寄存器周期的值	
	TIM_TimeBaseStructure.TIM_Prescaler =psc; //设置用来作为TIMx时钟频率除数的预分频值
	TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1; //设置时钟分割:TDTS = Tck_tim
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;  //TIM向上计数模式
	TIM_TimeBaseInit(TIM7, &TIM_TimeBaseStructure); //根据指定的参数初始化TIMx的时间基数单位
 
	TIM_ITConfig(TIM7,TIM_IT_Update,ENABLE ); //使能指定的TIM7中断,允许更新中断

	 	  
	NVIC_InitStructure.NVIC_IRQChannel = TIM7_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=1 ;//抢占优先级1
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;		//子优先级1
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;			//IRQ通道使能
	NVIC_Init(&NVIC_InitStructure);	//根据指定的参数初始化VIC寄存器
	
}

//高级定时器 1 10  中断服务函数
void TIM1_UP_TIM10_IRQHandler(void)
{
	//检测标志位
	if(TIM_GetITStatus(TIM1,TIM_IT_Update) == SET)
	{
		PFout(9)^=1;
		//清空标志位
		TIM_ClearITPendingBit(TIM1,TIM_IT_Update);
	
	}
}
//通用定时器 2  中断服务函数
void TIM2_IRQHandler(void)
{
	//检测标志位
	if(TIM_GetITStatus(TIM2,TIM_IT_Update) == SET)
	{

		PFout(10)^=1;
		//清空标志位
		TIM_ClearITPendingBit(TIM2,TIM_IT_Update);
	
	}	
}

//通用定时器 3  中断服务函数
void TIM3_IRQHandler(void)
{
	//检测标志位
	if(TIM_GetITStatus(TIM3,TIM_IT_Update) == SET)
	{
		PEout(13)^=1;
		//清空标志位
		TIM_ClearITPendingBit(TIM3,TIM_IT_Update);
	
	}
}

//高级定时器 8 13  中断服务函数
void TIM8_UP_TIM13_IRQHandler(void)
{
	//检测标志位
	if(TIM_GetITStatus(TIM8,TIM_IT_Update) == SET)
	{
		PEout(14)^=1;
		//清空标志位
		TIM_ClearITPendingBit(TIM8,TIM_IT_Update);
	
	}	
}

//定时器7中断服务程序		    
void TIM7_IRQHandler(void)
{ 	
	if (TIM_GetITStatus(TIM7, TIM_IT_Update) != RESET)//是更新中断
	{	 			   
		USART2_RX_STA|=1<<15;	//标记接收完成
		TIM_ClearITPendingBit(TIM7, TIM_IT_Update  );  //清除TIM7更新中断标志    
		TIM_Cmd(TIM7, DISABLE);  //关闭TIM7 
	}	    
}
 
