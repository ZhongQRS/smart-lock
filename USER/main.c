#include "sys.h"
#include "delay.h"
#include "usart2.h"
#include "led.h"
#include "beep.h"
#include "oled.h"
#include "rtc.h"
#include "keyboard.h"
#include "includes.h"
#include "bmp.h"
#include "as608.h"
#include "timer.h"
#include "exti.h"
#include "root.h"
#include "password.h"
#include "flash.h"
#include "bluetooth.h"
//#include "mifare522.h"
#include "lock.h"

#define usart2_baund  57600//串口2波特率，根据指纹模块波特率更改

extern SysPara AS608Para;//指纹模块AS608参数
extern u16 ValidN;//模块内有效指纹个数

static volatile uint8_t  usart_buf[32]={0};
static volatile uint32_t usart_cnt=0;


OS_Q		g_queue;		//消息队列
OS_MUTEX	g_mutex;		//互斥锁
OS_FLAG_GRP	g_flag_grp;		//事件标志组
OS_SEM	g_sem;					//信号量

//初始化任务控制块
OS_TCB TaskInit_TCB;
void TaskInit(void *parg);
CPU_STK TaskInit_stk[128];			//初始化任务的任务堆栈，大小为128字，也就是512字节

//任务1控制块
OS_TCB Task1_TCB;
void task1(void *parg);
CPU_STK task1_stk[128];			//任务1的任务堆栈，大小为128字，也就是512字节

//任务2控制块
OS_TCB Task2_TCB;
void task2(void *parg);
CPU_STK task2_stk[128];			//任务2的任务堆栈，大小为128字，也就是512字节

//任务3控制块
OS_TCB Task3_TCB;
void task3(void *parg);
CPU_STK task3_stk[128];			//任务3的任务堆栈，大小为128字，也就是512字节

//任务4控制块
OS_TCB Task4_TCB;
void task4(void *parg);
CPU_STK task4_stk[128];			//任务4的任务堆栈，大小为128字，也就是512字节


//主函数
int main(void)
{
	OS_ERR err;
	systick_init();  													//时钟初始化
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);						//中断分组配置

	//OS初始化，它是第一个运行的函数,初始化各种的全局变量，例如中断嵌套计数器、优先级、存储器
	OSInit(&err);

	//创建初始化任务(初始化函数)
	OSTaskCreate(	(OS_TCB *)&TaskInit_TCB,								//任务控制块，等同于线程id
					(CPU_CHAR *)"TaskInit_TCB",								//任务的名字，名字可以自定义的
					(OS_TASK_PTR)TaskInit,									//任务函数，等同于线程函数
					(void *)0,												//传递参数，等同于线程的传递参数
					(OS_PRIO)4,											 	//任务的优先级4	最高 最先执行		
					(CPU_STK *)TaskInit_stk,								//任务堆栈基地址
					(CPU_STK_SIZE)128/10,									//任务堆栈深度限位，用到这个位置，任务不能再继续使用
					(CPU_STK_SIZE)128,										//任务堆栈大小			
					(OS_MSG_QTY)0,											//禁止任务消息队列
					(OS_TICK)0,												//默认时间片长度																
					(void  *)0,												//不需要补充用户存储区
					(OS_OPT)OS_OPT_TASK_NONE,								//没有任何选项
					&err													//返回的错误码
				);
	
	//创建任务1
	OSTaskCreate(	&Task1_TCB,									//任务控制块，等同于线程id
					"Task1",									//任务的名字，名字可以自定义的
					task1,										//任务函数，等同于线程函数
					0,											//传递参数，等同于线程的传递参数
					6,											//任务的优先级6		
					task1_stk,									//任务堆栈基地址
					128/10,										//任务堆栈深度限位，用到这个位置，任务不能再继续使用
					128,										//任务堆栈大小			
					0,											//禁止任务消息队列
					0,											//默认是抢占式内核															
					0,											//不需要补充用户存储区
					OS_OPT_TASK_NONE,							//没有任何选项
					&err										//返回的错误码
				);

	//创建任务2
	OSTaskCreate(	&Task2_TCB,									//任务控制块
					"Task2",									//任务的名字
					task2,										//任务函数
					0,											//传递参数
					7,											//任务的优先级7		
					task2_stk,									//任务堆栈基地址
					128/10,										//任务堆栈深度限位，用到这个位置，任务不能再继续使用
					128,										//任务堆栈大小			
					0,											//禁止任务消息队列
					0,											//默认是抢占式内核																
					0,											//不需要补充用户存储区
					OS_OPT_TASK_NONE,							//没有任何选项
					&err										//返回的错误码
				);

	//创建任务3
	OSTaskCreate(	&Task3_TCB,									//任务控制块
					"Task3",									//任务的名字
					task3,										//任务函数
					0,											//传递参数
					7,											//任务的优先级7		
					task3_stk,									//任务堆栈基地址
					128/10,										//任务堆栈深度限位，用到这个位置，任务不能再继续使用
					128,										//任务堆栈大小			
					0,											//禁止任务消息队列
					0,											//默认是抢占式内核																
					0,											//不需要补充用户存储区
					OS_OPT_TASK_NONE,							//没有任何选项
					&err										//返回的错误码
				);
			
	//创建任务4
	OSTaskCreate(	&Task4_TCB,									//任务控制块
					"Task4",									//任务的名字
					task4,										//任务函数
					0,											//传递参数
					7,											//任务的优先级7		
					task4_stk,									//任务堆栈基地址
					128/10,										//任务堆栈深度限位，用到这个位置，任务不能再继续使用
					128,										//任务堆栈大小			
					0,											//禁止任务消息队列
					0,											//默认是抢占式内核																
					0,											//不需要补充用户存储区
					OS_OPT_TASK_NONE,							//没有任何选项
					&err										//返回的错误码
				);

	OSQCreate(&g_queue,"g_queue",64,&err);		//创建消息队列		
	OSMutexCreate(&g_mutex,"g_mutex",&err);		//创建互斥锁
	OSSemCreate (&g_sem,"g_sem",0,&err);			//创建信号量,初值为0	
	OSFlagCreate(&g_flag_grp,"g_flag_grp",0,&err);//消息标志组

	//启动OS，进行任务调度
	OSStart(&err);					
	//printf("never run.......\r\n");
					
	while(1);
}



void TaskInit(void *parg)	//初始化硬件函数
{
	OS_ERR err;
	u8 ensure;
	OSTaskSuspend(&Task1_TCB,&err);	//任务1挂起
	OSTaskSuspend(&Task2_TCB,&err);	//任务2挂起
	OSTaskSuspend(&Task3_TCB,&err);	//任务3挂起
	OSTaskSuspend(&Task4_TCB,&err);	//任务4挂起
	
	LED_Init();         		//LED初始化	
	BEEP_Init();  				//蜂鸣器初始化
	usart_init(9600);  		//串口1蓝牙9600 \ usb串口115200
	usart2_init(usart2_baund);	//指纹串口2
	EXTI9_Init();				//指纹中断---PG9
	KEYBOARD_Init();			//键盘初始化	
	My_RTC_Init();				//时间初始化
	OLED_Init();				//OLED初始化  
	OLED_Clear(); 				//OLED清屏
	//uart3_init(9600);			//串口3RFID初始化
	//LOCK_Init();				//锁初始化
	
	//printf("lnited\r\n");
	OLED_ShowString(8,0,"Init Successed!");  
	delay_ms(500);

	//printf("stm32智能指纹锁\r\n");
	//printf("by：unkim\r\n");
	//printf("与AS608模块握手....\r\n");
	
	OLED_Clear();
	OLED_ShowString(24,2,"Smart lock");  
	OLED_ShowString(24,4,"by: unkim");  
	delay_ms(2000);
	OLED_Clear(); 
	
	while(PS_HandShake(&AS608Addr))//与AS608模块握手
	{
		delay_ms(400);
		OLED_ShowString(16,2,"connecting...");
		//printf("正在连接\n");
		delay_ms(400);
	}
	
	OLED_Clear();
	//printf("连接成功\r\n");
	OLED_ShowString(16,2,"connected");
	delay_ms(1000);
	
	OLED_Clear(); 
	//printf("波特率:%d   地址:%x\r\n",usart2_baund,AS608Addr);
	
	ensure=PS_ValidTempleteNum(&ValidN);//读库指纹个数
	if(ensure!=0x00)
		ShowErrMessage(ensure);//显示确认码错误信息
	ensure=PS_ReadSysPara(&AS608Para);  //读参数
	if(ensure==0x00)
	{
		//printf("指纹库容量:%d     对比等级: %d\r\n",AS608Para.PS_max-ValidN,AS608Para.PS_level);
		OLED_ShowCHinese(0,2,0);//库
		OLED_ShowCHinese(16,2,1);//容
		OLED_ShowCHinese(32,2,2);//量
		OLED_ShowCHinese(48,2,7);//：
		OLED_ShowNum(64,2,AS608Para.PS_max-ValidN,3,16);
		
		OLED_ShowCHinese(0,4,3); //对
		OLED_ShowCHinese(16,4,4);//比
		OLED_ShowCHinese(32,4,5);//等
		OLED_ShowCHinese(48,4,6);//级
		OLED_ShowCHinese(64,4,7);//：
		OLED_ShowNum(80,4,AS608Para.PS_level,1,16);
	}
	else
		ShowErrMessage(ensure);
	delay_ms(1000);
	OSTaskResume(&Task1_TCB,&err);//任务1恢复
	OSTaskResume(&Task2_TCB,&err);//任务2恢复
	OSTaskResume(&Task3_TCB,&err);//任务3恢复
	OSTaskResume(&Task4_TCB,&err);//任务4恢复
	
	OSTaskDel(NULL,&err);//删除当前任务
	while(1);
}


void task1(void *parg)	//模式及功能任务
{
	OS_ERR  err;
	OS_MSG_SIZE msg_size;
	char *bp=NULL;
	uint8_t *p=NULL;
	char buf[10];
	memset(buf, 0,10);
	OSFlagPost(&g_flag_grp,0x01,OS_OPT_POST_FLAG_SET,&err);  //事件标志0x01 ---> 激活时间任务
	
	//printf("task1-Model and function is create ok\r\n");
	
	while(1)
	{
		OSMutexPost(&g_mutex,OS_OPT_POST_NONE,&err);						//解锁，释放OLED资源
		p=OSQPend(&g_queue,0,OS_OPT_PEND_BLOCKING,&msg_size,NULL,&err);		//等待消息队列
		if(p != NULL)	//有消息进入
		{
			OSMutexPend(&g_mutex,0,OS_OPT_PEND_BLOCKING,NULL,&err);			//上锁，锁定OLED资源
			if(strlen(buf)<=2 && (strcmp((char *)p,"A")==0 || strcmp((char *)p,"C")==0))
			{
				strcat(buf,(char *)p);
			}	
			if(strcmp(buf,"ACC")==0)	//输入ACC 进入root模式	ACC-->Account
			{
				//printf("enter root!\r\n");
				OLED_Clear(); 	//OLED清屏
				OLED_ShowString(8,2,"Enter Root!");	
				delay_ms(2000);
				p = (uint8_t *)ROOT_Mode();		//root模式函数
				memset(buf, 0,10);
				//printf("buf: %s\r\n",buf);
			}
			else if(strcmp((char*)p,"FingerPrint")==0)	//验证指纹
			{
				p=press_FR();//刷指纹
				//printf("finger: %s\r\n",p);
				if(strcmp((char *)p,"success")==0)
				{
					//printf("刷指纹成功，开锁\r\n");
					//printf("finger: %s\r\n",p);
					usart_send_str("FingerPrint unlock!\n");
					OSSemPost(&g_sem,OS_OPT_POST_1,&err);//S=S+1=1，这时V操作//释放信号量//解锁
					OSTaskResume(&Task4_TCB,&err);//任务4恢复
				}
			}
			else if(strcmp((char*)p,"Bluetooth")==0)
			{
				//"PW-123456-unkim"
				if(strstr((const char *)usart_buf,"PW"))
				{
					//分解字符串,p="PW"
					bp=strtok((char *)usart_buf,"-");
					//分解字符串,p="123456"
					bp=strtok(NULL,"-");
					p = Check_Password(bp);//校验密码
					if(strcmp((char *)p,"success")==0)
					{
						//printf("\n蓝牙密码正确，开锁\r\n");
						BEEP=1;
						delay_ms(500);
						BEEP=0;
						//memset(bp, 0,sizeof(bp));
						//memset(p, 0,sizeof(p));
						memset(p, 0,sizeof(usart_buf));
						usart_cnt=0;
						//printf("bluetooth p: %s\r\n",p);
						usart_send_str("Bluetooth unlock!\n");
						OSSemPost(&g_sem,OS_OPT_POST_1,&err);//S=S+1=1，这时V操作//释放信号量//解锁
						OSTaskResume(&Task4_TCB,&err);//任务4恢复
						
					}
					else
					{
						//printf("\n蓝牙密码错误\r\n");
						//printf("bluetooth: %s\r\n",p);
						OLED_Clear(); 	//OLED清屏
						OLED_ShowString(24,3,"close");						
						memset(buf, 0,10);
						delay_ms(2000);
						usart_cnt=0;
						
					}
				}
			}
			else if(strcmp((char *)p,"A")!=0 && strcmp((char *)p,"C")!=0)	//验证密码 排斥AC按键
			{
				strcat(buf,(char *)p);
				while(1)
				{
					p=OSQPend(&g_queue,0,OS_OPT_PEND_BLOCKING,&msg_size,NULL,&err);		//等待消息队列
					//printf("请输入密码：\r\n");
					if(strcmp((char *)p,"B")==0)//B退出	B-->Back
					{
						break;
					}
					else if(strlen(buf)<=5)
					{
						strcat(buf,(char *)p);
						//printf("buf: %s\r\n",buf);
						OLED_Clear(); 	//OLED清屏
						OLED_ShowString(30,2,"password:");//显示密码
						OLED_ShowString(30,6,(u8 *)buf);//显示密码
					}
					else 
					{
						//printf("password: %s\r\n",buf);
						p = Check_Password(buf);//校验密码
						if(strcmp((char *)p,"success")==0)
						{
							//printf("键盘密码正确，开锁\r\n");
							BEEP=1;
							delay_ms(500);
							BEEP=0;
							memset(buf, 0,10);
							usart_send_str("Password unlock!\n");
							OSSemPost(&g_sem,OS_OPT_POST_1,&err);//S=S+1=1，这时V操作//释放信号量//解锁
							OSTaskResume(&Task4_TCB,&err);//任务4恢复
							break;
						}
						else
						{
							//printf("键盘密码错误\r\n");
							OLED_Clear(); 	//OLED清屏
							OLED_ShowString(24,3,"close");						
							memset(buf, 0,10);
							break;
						}
					}
				
						
				}
					
			}
		}
	}
}

void task2(void *parg)	//键盘任务
{	
	//printf("task2-keyboard is create ok\r\n");
	while(1)
	{
		KEYBOARD_Num();
	}
}

void task3(void *parg)	//时间任务
{	
	OS_ERR  err;
	u8 time[20];
	u8 date[20];
	OSFlagPend(&g_flag_grp,0x01,0, OS_OPT_PEND_FLAG_SET_ANY+OS_OPT_PEND_BLOCKING,NULL,&err);	//时间工作标志位 --> 0x01
	//printf("task3-time is create ok\r\n");
	
	while(1)
	{
		delay_ms(1000);
		RTC_GetTime(RTC_Format_BIN, &RTC_TimeStructure);//获取时间
		RTC_GetDate(RTC_Format_BIN, &RTC_DateStructure);//获取日期
	
		OSMutexPend(&g_mutex,0,OS_OPT_PEND_BLOCKING,NULL,&err);//等待互斥锁，若等待成功，则锁定共享资源
		
		OLED_Clear(); //OLED清屏
		sprintf((char*)time,"%02d:%02d:%02d",RTC_TimeStructure.RTC_Hours,RTC_TimeStructure.RTC_Minutes,RTC_TimeStructure.RTC_Seconds);
		OLED_ShowString(35,3,time);//显示时间
		sprintf((char*)date,"%02d",RTC_DateStructure.RTC_Month);
		OLED_ShowString(37,0,date);
		OLED_ShowCHinese(53,0,8);//显示月
		sprintf((char*)date,"%02d",RTC_DateStructure.RTC_Date);
		OLED_ShowString(69,0,date);
		OLED_ShowCHinese(85,0,9);//显示日
		OLED_ShowCHinese(44,6,10);//星
		OLED_ShowCHinese(60,6,11);//期
		OLED_ShowNum(80,6,RTC_DateStructure.RTC_WeekDay,1,16);//显示星期
		
		OSMutexPost(&g_mutex,OS_OPT_POST_NONE,&err);//释放互斥锁，解锁
	}
	
}

void task4(void *parg)	//锁任务
{	
	OS_ERR  err;
	OSFlagPend(&g_flag_grp,0x01,0, OS_OPT_PEND_FLAG_SET_ANY+OS_OPT_PEND_BLOCKING,NULL,&err);//开锁工作标志位
	LOCK_Init();	//锁初始化
	while(1)
	{
		OSSemPend(&g_sem,0,OS_OPT_PEND_BLOCKING,NULL,&err);	//得到资源后，S=S-1=0，这时P操作//等待信号量
		//OSTaskSuspend(&Task3_TCB,&err);	//任务3时间挂起
		OLED_Clear(); 	//OLED清屏
		OLED_ShowString(8,2,"open");
		LOCK_ON();
		delay_ms(2000);
		LOCK_OFF();
		//OSTaskResume(&Task3_TCB,&err);//任务3时间恢复
		OSTaskSuspend(&Task4_TCB,&err);	//任务4挂起
	}
}


void EXTI9_5_IRQHandler(void)	//检测PG9指纹触发中断服务函数
{
	OS_ERR  err;
	//检测是否中断线0已经发生中断
	if(EXTI_GetITStatus(EXTI_Line9) == SET)
	{
		//中断处理事件
		if(KeyBOARD_Scan(GPIOG,GPIO_Pin_9))
		{
			OSQPost(&g_queue,"FingerPrint",strlen("FingerPrint"),OS_OPT_POST_FIFO,&err);
		}
	}
	//清除中断标志位，用于下次中断设置
	EXTI_ClearITPendingBit(EXTI_Line9);
}

//蓝牙串口1中断服务函数
void  USART1_IRQHandler(void)
{
	OS_ERR  err;
	uint8_t d;
	
	OSIntEnter(); 

	//检测标志位
	if(USART_GetITStatus(USART1,USART_IT_RXNE) == SET)
	{
		//接收数据
		d = USART_ReceiveData(USART1);
		
		usart_buf[usart_cnt] =d ;
		usart_cnt++;
		
		if(d == '#' || usart_cnt >= sizeof usart_buf)
		{
			OSQPost(&g_queue,"Bluetooth",strlen("Bluetooth"),OS_OPT_POST_FIFO,&err);
		}
		
		//清空标志位	
		USART_ClearITPendingBit(USART1,USART_IT_RXNE);
	}

	OSIntExit();  
}







