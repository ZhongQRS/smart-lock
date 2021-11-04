#ifndef __BLUETOOTH_H
#define __BLUETOOTH_H

#include "stm32f4xx.h"
#include "sys.h"
#include "stdio.h"	
/*
#define LED1 PFout(9)
#define LED2 PFout(10)
#define LED3 PEout(13)
#define LED4 PEout(14)
*/
static GPIO_InitTypeDef		GPIO_InitStructure;
static USART_InitTypeDef	USART_InitStructure;
static NVIC_InitTypeDef		NVIC_InitStructure;

void usart_init(uint32_t baud);
void usart_send_str(char *str);

#endif
