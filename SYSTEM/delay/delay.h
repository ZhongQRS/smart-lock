#ifndef __DELAY_H
#define __DELAY_H 			   
#include <sys.h>	  

extern void systick_init(void);
extern void delay_ms(uint16_t nms);
extern void delay_us(uint32_t nus);

#endif





























