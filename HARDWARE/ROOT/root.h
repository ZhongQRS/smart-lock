#ifndef __ROOT_H
#define __ROOT_H

#include "stm32f4xx.h" 
#include "includes.h"
#include "as608.h"
#include "usart.h"
#include "password.h"

extern OS_Q	g_queue;//消息队列

void *ROOT_Mode(void);

#endif
