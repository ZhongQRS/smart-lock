#include "lock.h" 
    
//LOCK IO初始化	PD6
void LOCK_Init(void)
{    	 
  GPIO_InitTypeDef  GPIO_InitStructure;

  RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOD, ENABLE);			//使能GPIOD时钟

  //GPIOD6初始化设置 
  GPIO_InitStructure.GPIO_Pin   = GPIO_Pin_6;					//对应IO口	6
  GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_IN;			   		//普通输入模式，
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;				//推挽输出
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;		    //100MHz
  GPIO_InitStructure.GPIO_PuPd  = GPIO_PuPd_UP;				    //上拉
  GPIO_Init(GPIOD, &GPIO_InitStructure);						//初始化GPIOD，把配置的数据写入寄存器
}

void LOCK_ON(void)
{
	GPIO_InitTypeDef  GPIO_InitStruct;
	GPIO_InitStruct.GPIO_Pin	= GPIO_Pin_6; 		//引脚
	GPIO_InitStruct.GPIO_Mode	= GPIO_Mode_OUT;	//输出/输入
	GPIO_InitStruct.GPIO_OType  = GPIO_OType_PP;    //推挽
	GPIO_InitStruct.GPIO_PuPd	= GPIO_PuPd_NOPULL;     //不拉
	GPIO_InitStruct.GPIO_Speed	= GPIO_Speed_50MHz; //速度
	
	GPIO_Init(GPIOD, &GPIO_InitStruct);
}

void LOCK_OFF(void)
{
	GPIO_InitTypeDef  GPIO_InitStruct;
	GPIO_InitStruct.GPIO_Pin	= GPIO_Pin_6; 		//引脚
	GPIO_InitStruct.GPIO_Mode	= GPIO_Mode_IN;	//输出/输入
	GPIO_InitStruct.GPIO_OType  = GPIO_OType_PP;    //推挽
	GPIO_InitStruct.GPIO_PuPd	= GPIO_PuPd_NOPULL;     //不拉
	GPIO_InitStruct.GPIO_Speed	= GPIO_Speed_50MHz; //速度
	
	GPIO_Init(GPIOD, &GPIO_InitStruct);
}


