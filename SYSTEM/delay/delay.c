#include "delay.h"
#include "sys.h"

#include "includes.h"							//ucos 使用	  



static uint16_t fac_us=0;						//us延时倍乘数			   
static uint16_t fac_ms=0;						//ms延时倍乘数,在os下,代表每个节拍的ms数
	
//systick中断服务函数,使用OS时用到
void SysTick_Handler(void)
{	
	if(OSRunning==OS_STATE_OS_RUNNING)			//OS开始跑了,才执行正常的调度处理
	{
		OSIntEnter();							//进入中断
		OSTimeTick();       					//调用ucos的时钟服务程序               
		OSIntExit();       	 					//触发任务切换软中断
	}
}

			   
//初始化延迟函数
//当使用ucos的时候,此函数会初始化ucos的时钟节拍
void systick_init(void)
{

	uint32_t reload;

 	SysTick_CLKSourceConfig(SysTick_CLKSource_HCLK_Div8);//SYSTICK使用外部时钟源	 
	
	fac_us=SystemCoreClock/8/1000000;					//不论是否使用OS,fac_us都需要使用

	reload=SystemCoreClock/8/1000000;					//每秒钟的计数次数 单位为K	   
	reload*=1000000/OSCfg_TickRate_Hz;					//根据OSCfg_TickRate_Hz设定溢出时间
														//reload为24位寄存器,最大值:16777216,在72M下,约合1.86s左右	
	fac_ms=1000/OSCfg_TickRate_Hz;						//代表OS可以延时的最少单位	   
	SysTick->CTRL|=SysTick_CTRL_TICKINT_Msk;			//开启SYSTICK中断
	SysTick->LOAD=reload; 								//每1/OSCfg_TickRate_Hz秒中断一次	
	SysTick->CTRL|=SysTick_CTRL_ENABLE_Msk; 			//开启SYSTICK
}								    

    								   
void delay_us(uint32_t nus)
{		
	OS_ERR err; 
	uint32_t ticks;
	uint32_t told,tnow,tcnt=0;
	uint32_t reload=SysTick->LOAD;				//LOAD的值	    	 
	ticks=nus*fac_us; 						//需要的节拍数 
	
	OSSchedLock(&err);						//阻止OS调度，防止打断us延时
	
	told=SysTick->VAL;        				//刚进入时的计数器值
	
	while(1)
	{
		tnow=SysTick->VAL;					//获取当前计数值
		
		if(tnow!=told)
		{	    
			if(tnow<told)
				tcnt+=told-tnow;			//这里注意一下SYSTICK是一个递减的计数器就可以了.
			else 
				tcnt+=reload-tnow+told;
			
			told=tnow;
			
			if(tcnt>=ticks)
				break;						//时间超过/等于要延迟的时间,则退出.
		}  
	}
	
	OSSchedUnlock(&err);					//恢复OS调度											    
}  

void delay_ms(uint16_t nms)
{	
	OS_ERR err; 
	
	if(OSRunning&&OSIntNestingCtr==0)		//如果OS已经在跑了,并且不是在中断里面(中断里面不能任务调度)	    
	{		 
		if(nms>=fac_ms)						//延时的时间大于OS的最少时间周期 
			OSTimeDly(nms/fac_ms,OS_OPT_TIME_PERIODIC,&err);
		
		nms%=fac_ms;						//OS已经无法提供这么小的延时了,采用普通方式延时    
	}
	
	delay_us((uint32_t)(nms*1000));			//普通方式延时
}





































