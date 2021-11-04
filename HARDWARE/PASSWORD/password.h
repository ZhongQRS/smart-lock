#ifndef __PASSWORD_H
#define __PASSWORD_H

#include "stm32f4xx.h" 
#include "includes.h"
#include "usart.h"
#include "flash.h"
#include "delay.h"
#include "oled.h"

#define FLASH4 0x8010000 //扇区4

extern OS_Q	g_queue;//消息队列

void OverWrite_Password(void);
u8 * Check_Password(char *buf);

#endif 
