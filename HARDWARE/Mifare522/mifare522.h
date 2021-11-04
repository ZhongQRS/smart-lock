#ifndef __MIFARE522_H
#define __MIFARE522_H
#include "sys.h"
#include "stdio.h"	
#include "stm32f4xx_conf.h"
#include "string.h"


//波特率要求是9600
//数据格式   1 个起始位，8 个数据位、无奇偶校验位、1 个停止位
//


#define STX 0x20	// 开始符
#define ETX 0x03	// 终止符
#define ACK 0x06	// 应答
#define NAK 0x15	// 无应答	 


//读取设备的命令
extern u8 readCmd[];

//请求读卡
extern u8 requestCmd[];

//防碰撞
extern u8 piccAnticollCmd[];

#define USART3_REC_LEN  			200  	//定义最大接收字节数 200
	  	
extern u8  USART3_RX_BUF[USART3_REC_LEN]; //接收缓冲,最大USART_REC_LEN个字节.末字节为换行符 
extern u16 USART3_RX_STA;         		    //接收状态标记	
//如果想串口中断接收，请不要注释以下宏定义
void uart3_init(u32 bound);


u8 mifare522ReadDevice(void);
void mifare522Request(void);
void mifare522ReadID(void);


u8 CalBCC(u8 *buf, int n);
void USART3_SendByte(u8 Data);
void print3RX(u8 * data,u16 nub);




#endif
