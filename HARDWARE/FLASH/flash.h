#ifndef __FLASH_H
#define	__FLASH_H

#include "stm32f4xx.h"


//擦除扇区
void FLASH_erasure(void);
void FLASH_erasure_5(void);
void FLASH_erasure_6(void);

//写入 1字（32位）
void WriteFlashOneWord(uint32_t WriteAddress, uint32_t WriteData);


//写入字符串一个字节（8位）
void WriteFlashData(uint32_t WriteAddress, uint8_t data[], uint32_t num);


//读取num字（32位）
void ReadFlashOneWord(uint32_t WriteAddress,uint32_t num);


//读取num字节（8位）（读字符串）
void ReadFlashData(uint32_t WriteAddress,char *buf,uint32_t num);

#endif
