#include "flash.h"
#include "stdio.h"

//擦出FLASH
void FLASH_erasure(void)
{
	//解锁FLASH，允许对FLASH进行修改
	FLASH_Unlock();
	
	//扇区4擦除
	//以32bit的大小逐次擦除这个扇区
	if (FLASH_EraseSector(FLASH_Sector_4, VoltageRange_3) != FLASH_COMPLETE)
	{
		//printf("FLASH_EraseSector error\r\n");
		while(1);
	}
	//锁定FLASH，不允许对FLASH进行修改
	FLASH_Lock(); 
}

void FLASH_erasure_5(void)
{
	//解锁FLASH，允许对FLASH进行修改
	FLASH_Unlock();
	
	//扇区4擦除
	//以32bit的大小逐次擦除这个扇区
	if (FLASH_EraseSector(FLASH_Sector_5, VoltageRange_3) != FLASH_COMPLETE)
	{
		//printf("FLASH_EraseSector error\r\n");
		while(1);
	}
	//锁定FLASH，不允许对FLASH进行修改
	FLASH_Lock(); 
}


void FLASH_erasure_6(void)
{
	//解锁FLASH，允许对FLASH进行修改
	FLASH_Unlock();
	
	//扇区4擦除
	//以32bit的大小逐次擦除这个扇区
	if (FLASH_EraseSector(FLASH_Sector_6, VoltageRange_3) != FLASH_COMPLETE)
	{
		//printf("FLASH_EraseSector error\r\n");
		while(1);
	}
	//锁定FLASH，不允许对FLASH进行修改
	FLASH_Lock(); 
}



//写入字（32位）
void WriteFlashOneWord(uint32_t WriteAddress, uint32_t WriteData)
{
	//解锁FLASH，允许对FLASH进行修改
	FLASH_Unlock();
	
	//数据编程
	//向扇区4首地址0x8010000写入数据0x12345678
	if (FLASH_ProgramWord(WriteAddress, WriteData) != FLASH_COMPLETE)
	{
		//printf("FLASH_ProgramWord error\r\n");
		while(1);
	}
	
	//锁定FLASH，不允许对FLASH进行修改
	FLASH_Lock();
	

}


//写入字符串（8位）
void WriteFlashData(uint32_t WriteAddress, uint8_t data[], uint32_t num)
{
	uint32_t i = 0;
    uint16_t temp = 0;
	
	
	//解锁FLASH，允许对FLASH进行修改
	FLASH_Unlock();
	
	
	for(i=0; i<num; i++)
	{
		temp = (uint16_t)data[i];
		if (FLASH_COMPLETE != FLASH_ProgramByte((WriteAddress+i), temp))//写入数据
		{
			//printf("FLASH_ProgramWord error\r\n");
			while(1);
		}
	}


	//锁定FLASH，不允许对FLASH进行修改
	FLASH_Lock();
}


//读取一个字（32位）的数据
void ReadFlashOneWord(uint32_t WriteAddress,uint32_t num)
{
	uint32_t i = 0;
	uint32_t d=0;
	
	
	for(i=0;i<num*4;i=i+4)
	{
		//读取数据
		d = *(volatile uint32_t *)(WriteAddress+i);
		
		//printf("d=%X\r\n",d);	
	}
}

//读取字符串（8位）
void ReadFlashData(uint32_t WriteAddress,char *buf,uint32_t num)
{
	uint32_t i = 0;
	uint32_t d=0;

	for(i=0;i<num;i++)
	{
		//读取数据
		d = *(volatile uint32_t *)(WriteAddress+i);
		
		buf[i]=d;
	}
}
