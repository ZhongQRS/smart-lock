#include "bluetooth.h"

//加入以下代码,支持printf函数,而不需要选择use MicroLIB	  

#pragma import(__use_no_semihosting)             
//标准库需要的支持函数                 
struct __FILE 
{ 
	int handle; 
}; 

FILE __stdout;       
//定义_sys_exit()以避免使用半主机模式    
void _sys_exit(int x) 
{ 
	x = x; 
} 
//重定义fputc函数 
int fputc(int ch, FILE *f)
{ 	
	while((USART1->SR&0X40)==0);//循环发送,直到发送完毕   
	USART1->DR = (u8) ch;      
	return ch;
}

void usart_init(uint32_t baud)
{
	//使能串口1硬件时钟
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1, ENABLE); 
	
	//使能端口A的硬件时钟
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA, ENABLE);
	
	//配置PA9 PA10为复用功能模式
	
	GPIO_InitStructure.GPIO_Pin=GPIO_Pin_9|GPIO_Pin_10;	//第9 10个引脚
	GPIO_InitStructure.GPIO_Mode=GPIO_Mode_AF;		//复用功能模式
	GPIO_InitStructure.GPIO_Speed=GPIO_High_Speed;	//引脚高速工作，收到指令立即工作；缺点：功耗高
	GPIO_InitStructure.GPIO_OType=GPIO_OType_PP;	//增加输出电流的能力
	GPIO_InitStructure.GPIO_PuPd=GPIO_PuPd_NOPULL;	//不需要上下拉电阻
	GPIO_Init(GPIOA,&GPIO_InitStructure);
	
	//将PA9和PA10连接到串口1硬件
	GPIO_PinAFConfig(GPIOA, GPIO_PinSource9, GPIO_AF_USART1);	
	GPIO_PinAFConfig(GPIOA, GPIO_PinSource10, GPIO_AF_USART1);		

	//配置串口1：波特率，校验位、数据位、停止位
	USART_InitStructure.USART_BaudRate = baud;						//波特率
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;		//数据位
	USART_InitStructure.USART_StopBits = USART_StopBits_1;			//1个停止位
	USART_InitStructure.USART_Parity = USART_Parity_No;				//无校验位
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;//取消流控制
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;//能够收发数据
	USART_Init(USART1,&USART_InitStructure);
	
	
	//配置串口1的中断触发方式：接收完字节触发中断
	USART_ITConfig(USART1,USART_IT_RXNE , ENABLE);
	
	
	//使能串口1工作	
	USART_Cmd(USART1, ENABLE);
	
	
	//配置NVIC管理串口1
	NVIC_InitStructure.NVIC_IRQChannel = USART1_IRQn;		//串口1的中断号
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0x02;//抢占优先级 0x2
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0x02;//响应优先级 0x2
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;		//NVIC使能串口1中断请求通道
	NVIC_Init(&NVIC_InitStructure);	
}


void usart_send_str(char *str)
{
	char *p = str;
	
	while(p && (*p!='\0'))
	{
	
		USART_SendData(USART1,*p);
		while(USART_GetFlagStatus(USART1,USART_FLAG_TXE)==RESET);	
		p++;
	}
}
