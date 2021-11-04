#include "led.h" 

//初始化 PF9，PF10，PE13，PE14 为输出口.并使能这两个口的时钟		    
//LED IO初始化
void LED_Init(void)
{    	 
  GPIO_InitTypeDef  GPIO_InitStructure;

  RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOE | RCC_AHB1Periph_GPIOF, ENABLE);			//使能GPIOE，GPIOF时钟

  //GPIOF9,F10初始化设置 
  GPIO_InitStructure.GPIO_Pin   = GPIO_Pin_9 | GPIO_Pin_10;		//LED0和LED1对应IO口
  GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_OUT;			    //普通输出模式，
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;				//推挽输出，驱动LED需要电流驱动
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;		    //100MHz
  GPIO_InitStructure.GPIO_PuPd  = GPIO_PuPd_UP;				    //上拉
  GPIO_Init(GPIOF, &GPIO_InitStructure);						//初始化GPIOF，把配置的数据写入寄存器
  
  //GPIOE13,PE14初始化设置 
  GPIO_InitStructure.GPIO_Pin   = GPIO_Pin_13 | GPIO_Pin_14;	//LED2和LED3对应IO口
  GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_OUT;				//普通输出模式
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;				//推挽输出
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;			//100MHz
  GPIO_InitStructure.GPIO_PuPd  = GPIO_PuPd_UP;					//上拉
  GPIO_Init(GPIOE, &GPIO_InitStructure);						//初始化GPIOE，把配置的数据写入寄存器
	
  GPIO_SetBits(GPIOF,GPIO_Pin_9  | GPIO_Pin_10);			    //GPIOF9,PF10设置高，灯灭
  GPIO_SetBits(GPIOE,GPIO_Pin_13 | GPIO_Pin_14);				//GPIOE13,PE14设置高，灯灭
}






