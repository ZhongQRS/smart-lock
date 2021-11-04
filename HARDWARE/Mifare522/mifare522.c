#include "sys.h"
#include "mifare522.h"	
#include "usart.h"
#include "delay.h"


//读取设备的命令
u8 readCmd[6]={0x06,0x01,0x41,0x00,0xB9,0x03};
//请求读卡
u8 requestCmd[7]={0x07,0x02,0x41,0x01,0x52,0x00,0x03};
//防碰撞
u8 piccAnticollCmd[8]={0x08,0x02,0x42,0x02,0x93,0x00,0x00,0x03};

//串口1中断服务程序
//注意,读取USARTx->SR能避免莫名其妙的错误   	
u8 USART3_RX_BUF[USART3_REC_LEN];     //接收缓冲,最大USART_REC_LEN个字节.
//接收状态
//bit15，	接收完成标志
//bit14，	接收到0x0d
//bit13~0，	接收到的有效字节数目
u16 USART3_RX_STA=0;       //接收状态标记	

//初始化IO 串口1 
//bound:波特率
void uart3_init(u32 bound){
   //GPIO端口设置
  GPIO_InitTypeDef GPIO_InitStructure;
	USART_InitTypeDef USART_InitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;
	
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB,ENABLE); //使能GPIOB时钟 使能USART1时钟
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART3,ENABLE);
	
	//串口3对应引脚复用映射
	GPIO_PinAFConfig(GPIOB,GPIO_PinSource10,GPIO_AF_USART3); //GPIOB10复用为USART3
	GPIO_PinAFConfig(GPIOB,GPIO_PinSource11,GPIO_AF_USART3); //GPIOB11复用为USART3
	
	//USART1端口配置
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10 | GPIO_Pin_11; //GPIOB10与GPIOB11
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;//复用功能
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;	//速度50MHz
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP; //推挽复用输出
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP; //上拉
	GPIO_Init(GPIOB,&GPIO_InitStructure); //初始化PB10，PB11

   //USART1 初始化设置
	USART_InitStructure.USART_BaudRate = bound;//波特率设置
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;//字长为8位数据格式
	USART_InitStructure.USART_StopBits = USART_StopBits_1;//一个停止位
	USART_InitStructure.USART_Parity = USART_Parity_No;//无奇偶校验位
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;//无硬件数据流控制
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;	//收发模式
  USART_Init(USART3, &USART_InitStructure); //初始化串口3

	
  USART_Cmd(USART3, ENABLE);  //使能串口2 
	
	//USART_ClearFlag(USART3, USART_FLAG_TC);
	
	USART_ITConfig(USART3, USART_IT_RXNE, ENABLE);//开启接收中断

	NVIC_InitStructure.NVIC_IRQChannel = USART3_IRQn;//串口1中断通道
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=3;//抢占优先级3 数值越小优先级越高
	NVIC_InitStructure.NVIC_IRQChannelSubPriority =3; //子优先级3,响应优先级
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE; //IRQ通道使能
	NVIC_Init(&NVIC_InitStructure); //根据指定的参数初始化VIC寄存器、


}


void USART3_IRQHandler(void)                	//串口1中断服务程序
{
	u8 Res;
	
	if(USART_GetITStatus(USART3, USART_IT_RXNE) != RESET)  //接收中断
	{
		Res =USART_ReceiveData(USART3);//(USART3->DR);	//读取接收到的数据
		if(USART3_RX_STA==0)
		{
			USART3_RX_BUF[0]=Res;
			USART3_RX_STA++;
		}
		else
		{		
				USART3_RX_BUF[USART3_RX_STA&0X3FFF]=Res ;
				USART3_RX_STA++;
				if(USART3_RX_STA>(USART3_REC_LEN-1))USART3_RX_STA=0;//接收数据错误,重新开始接收	 
				if(USART3_RX_BUF[0]==USART3_RX_STA)USART3_RX_STA|=0x8000;				
			
		}	  
	} 
}

void USART3_SendByte(u8 Data)
{
   while (!(USART3->SR & USART_FLAG_TXE));	 // while(USART_GetFlagStatus(USART1, USART_FLAG_TXE) == RESET)
		USART_SendData(USART3, Data);	 
   
}

void print3RX(u8 * data,u16 nub)			     // 打印定长字符串
{
	u16 i;
	for (i=0; i<nub; i++){
		USART3_SendByte(data[i]);
	}
}

void UART3_Send_Enter(void)
{
    USART3_SendByte(0x0d);
    USART3_SendByte(0x0a);
}

/*计算校验和*/
u8 CalBCC(u8 *buf, int n)
{
	u8 i;
	u8 bcc=0;
	for(i = 0; i < n; i++)
	{
		bcc ^= *(buf+i);
	}
	return (~bcc);
}

/**********************************
//函数：void mifare522Read() 
//发送数据：06 01 41 00 B9 03
//返回值：14 01 00 0E 52 43 35 32 32 20 56 31 2E 32 30 31 38 00 A2 03  
**************************************/
u8 mifare522ReadDevice(void)
{
	u16 len,t;
	print3RX(readCmd,6);	
	while(!(USART3_RX_STA&0x8000));
	if(USART3_RX_STA&0x8000)
	{					   
			len=USART3_RX_STA&0x3fff;//得到此次接收到的数据长度
			if(USART3_RX_BUF[2]==0x00)
			{
				printf("\r\n扫描设备信息为:\r\n");
				for(t=4;t<(len-2);t++)
				{
					USART_SendData(USART1, USART3_RX_BUF[t]);         //向串口1发送数据
					while(USART_GetFlagStatus(USART1,USART_FLAG_TC)!=SET);//等待发送结束
				}
				printf("\r\n\r\n");//插入换行
				USART3_RX_STA=0;
				memset(USART3_RX_BUF,0,200);
				return 1;

			}
			else
			{
				printf("扫描设备失败\r\n");
				USART3_RX_STA=0;
				memset(USART3_RX_BUF,0,200);
				return 0;
			}
	}
}

void mifare522Request(void)
{
	u16 len,t;
	requestCmd[5]=CalBCC(requestCmd,requestCmd[0]-2);
	print3RX(requestCmd,7);	
	while(!(USART3_RX_STA&0x8000));
	if(USART3_RX_STA&0x8000)
	{					   
			len=USART3_RX_STA&0x3fff;//得到此次接收到的数据长度
			if(USART3_RX_BUF[2]==0x00)
			{
					printf("\r\n请求成功，其ATQ:0x");
					for(t=4;t<(len-2);t++)
					{
						printf("%x",USART3_RX_BUF[t]);
					}
					printf("\r\n\r\n");//插入换行

			}else
			{
					printf("\r\n请求失败\r\n");
			}
		USART3_RX_STA=0;
		memset(USART3_RX_BUF,0,200);
	}
}

void mifare522ReadID(void)
{
	u16 len,t;
	piccAnticollCmd[6]=CalBCC(piccAnticollCmd,piccAnticollCmd[0]-2);
	print3RX(piccAnticollCmd,8);
	while(!(USART3_RX_STA&0x8000));
	if(USART3_RX_STA&0x8000)
	{					   
			len=USART3_RX_STA&0x3fff;//得到此次接收到的数据长度
			if(USART3_RX_BUF[2]==0x00)
			{
					printf("\r\n防碰撞成功，其卡号:0x");
					for(t=(len-3);t>4;t--)
					{
						printf("%x",USART3_RX_BUF[t]);
					}
					printf("\r\n\r\n");//插入换行
			}else
			{
					printf("\r\n读卡失败\r\n");
			}
			USART3_RX_STA=0;
		  memset(USART3_RX_BUF,0,200);
	}
}





