#include "rtc.h"

//设置RTC时间
//参数：hour-小时、min-分钟、sec-秒、ampm-上午下午
ErrorStatus RTC_Set_Time(u8 hour,u8 min,u8 sec,u8 ampm)
{	
	RTC_TimeStructure.RTC_Hours=hour;
	RTC_TimeStructure.RTC_Minutes=min;
	RTC_TimeStructure.RTC_Seconds=sec;
	RTC_TimeStructure.RTC_H12=ampm;
	
	return RTC_SetTime(RTC_Format_BIN,&RTC_TimeStructure);
}

//设置RTC日期
//参数：year-年、month-月、date-日、week-周几
ErrorStatus RTC_Set_Date(u8 year,u8 month,u8 date,u8 week)
{
	RTC_DateTypeDef RTC_DateTypeInitStructure;
	RTC_DateTypeInitStructure.RTC_Date=date;
	RTC_DateTypeInitStructure.RTC_Month=month;
	RTC_DateTypeInitStructure.RTC_WeekDay=week;
	RTC_DateTypeInitStructure.RTC_Year=year;
	return RTC_SetDate(RTC_Format_BIN,&RTC_DateTypeInitStructure);
}

//RTC周期性唤醒函数
//功能：每秒唤醒一次
void RTC_Set_WakeUp(void)
{
	RTC_WakeUpCmd(DISABLE);
	
	//唤醒时钟源的硬件时钟频率为1Hz
	RTC_WakeUpClockConfig(RTC_WakeUpClock_CK_SPRE_16bits);
	
	//只进行一次计数 每秒进行一次 1-1=0
	RTC_SetWakeUpCounter(0);
	
	RTC_WakeUpCmd(ENABLE);

	//配置中断的触发方式：唤醒中断
	RTC_ITConfig(RTC_IT_WUT, ENABLE);
	
	RTC_ClearFlag(RTC_FLAG_WUTF);
	
	EXTI_ClearITPendingBit(EXTI_Line22);
	EXTI_InitStructure.EXTI_Line = EXTI_Line22;			
	EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
	EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Rising;//RTC手册规定
	EXTI_InitStructure.EXTI_LineCmd = ENABLE;
	EXTI_Init(&EXTI_InitStructure);

	//优先级
	NVIC_InitStructure.NVIC_IRQChannel = RTC_WKUP_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 2;	//抢占优先级
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;			//响应优先级
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);
}

//RTC初始化函数
//返回值:	0,初始化成功;
u8 My_RTC_Init(void)
{
	//使能rtc的硬件时钟
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_PWR, ENABLE);
	
	/*使能电源管理时钟 */
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_PWR, ENABLE);
	
	/*允许访问RTC */
	PWR_BackupAccessCmd(ENABLE);
	
	//判断是否第一次配置?
	if (RTC_ReadBackupRegister(RTC_BKP_DR0) != 0x9999)	
	{
		//打开LSE振荡时钟
		RCC_LSEConfig(RCC_LSE_ON);

		//检查LSE振荡时钟是否就绪
		while(RCC_GetFlagStatus(RCC_FLAG_LSERDY) == RESET);
		
		//为RTC选择LSE作为时钟源
		RCC_RTCCLKConfig(RCC_RTCCLKSource_LSE);
		
		/* 使能RTC的硬件时钟 */
		RCC_RTCCLKCmd(ENABLE);
		
		/* 等待所有RTC相关寄存器就绪 */
		RTC_WaitForSynchro();
		
		//配置频率1Hz
		/* ck_spre(1Hz) = RTCCLK(LSE) /(uwAsynchPrediv + 1)*(uwSynchPrediv + 1)*/
		//32768Hz/(127+1)/(255+1)=1Hz
		RTC_InitStructure.RTC_AsynchPrediv = 127;
		RTC_InitStructure.RTC_SynchPrediv = 255;
		RTC_InitStructure.RTC_HourFormat = RTC_HourFormat_24;
		RTC_Init(&RTC_InitStructure);
		
		//设置时间20：33：05 am
		RTC_Set_Time(20,33,05,RTC_H12_AM);	
		
		//设置日期2020-03-23 周一
		RTC_Set_Date(20,3,23,1);		
		
		//周期性唤醒函数
		RTC_Set_WakeUp();
	}
	return 0;
}

//设置闹钟A函数
//按周几设置而不是按具体日期
void RTC_Set_AlarmA(u8 week,u8 hour,u8 min,u8 sec)
{
	RTC_AlarmCmd(RTC_Alarm_A,DISABLE);//关闭闹钟A 
	
	RTC_TimeStructure.RTC_Hours=hour;//小时
	RTC_TimeStructure.RTC_Minutes=min;//分钟
	RTC_TimeStructure.RTC_Seconds=sec;//秒
	RTC_TimeStructure.RTC_H12=RTC_H12_AM;
  
	RTC_AlarmStructure.RTC_AlarmDateWeekDay=week;//星期
	RTC_AlarmStructure.RTC_AlarmDateWeekDaySel=RTC_AlarmDateWeekDaySel_WeekDay;//按星期闹
	RTC_AlarmStructure.RTC_AlarmMask=RTC_AlarmMask_None;//精确匹配星期，时分秒
	RTC_AlarmStructure.RTC_AlarmTime=RTC_TimeStructure;
	RTC_SetAlarm(RTC_Format_BIN,RTC_Alarm_A,&RTC_AlarmStructure);
 
	
	RTC_ClearITPendingBit(RTC_IT_ALRA);//清除RTC闹钟A的标志
	EXTI_ClearITPendingBit(EXTI_Line17);//清除LINE17上的中断标志位 
	
	RTC_ITConfig(RTC_IT_ALRA,ENABLE);//开启闹钟A中断
	RTC_AlarmCmd(RTC_Alarm_A,ENABLE);//开启闹钟A 
	
	EXTI_InitStructure.EXTI_Line = EXTI_Line17;//LINE17
	EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;//中断事件
	EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Rising; //上升沿触发 
	EXTI_InitStructure.EXTI_LineCmd = ENABLE;//使能LINE17
	EXTI_Init(&EXTI_InitStructure);//配置

	NVIC_InitStructure.NVIC_IRQChannel = RTC_Alarm_IRQn; 
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0x02;//抢占优先级1
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0x02;//子优先级2
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;//使能外部中断通道
	NVIC_Init(&NVIC_InitStructure);//配置
}

//串口1打印时间函数
void USART1_Printf_Time(void)
{
	//RTC_GetTime，获取时间
	RTC_GetTime(RTC_Format_BCD, &RTC_TimeStructure); 
	//printf("TIME:%02x:%02x:%02x\r\n",RTC_TimeStructure.RTC_Hours,RTC_TimeStructure.RTC_Minutes,RTC_TimeStructure.RTC_Seconds);
	
	//RTC_GetDate，获取日期
	RTC_GetDate(RTC_Format_BCD, &RTC_DateStructure);
	//printf("DATE:20%02x/%02x/%02x %0x\r\n",RTC_DateStructure.RTC_Year,RTC_DateStructure.RTC_Month,RTC_DateStructure.RTC_Date,RTC_DateStructure.RTC_WeekDay);		

	delay_ms(1000);
}

//RTC唤醒中断服务函数
void  RTC_WKUP_IRQHandler(void)
{
	//检测标志位
	if(RTC_GetITStatus(RTC_IT_WUT) == SET)
	{
		//printf("RTC_WKUP_IRQHandler\r\n");
		
		//清空标志位	
		RTC_ClearITPendingBit(RTC_IT_WUT);
		
		EXTI_ClearITPendingBit(EXTI_Line22);
	}
}

//RTC闹钟中断服务函数
void RTC_Alarm_IRQHandler(void)
{    
	if(RTC_GetFlagStatus(RTC_FLAG_ALRAF)==SET)//ALARM A中断?
	{
		RTC_ClearFlag(RTC_FLAG_ALRAF);//清除中断标志
		//printf("ALARM A!\r\n");
	}   
	EXTI_ClearITPendingBit(EXTI_Line17);	//清除中断线17的中断标志 											 
}
