#include "password.h"

void OverWrite_Password(void)
{
	uint8_t *p=NULL;
	OS_ERR  err;
	OS_MSG_SIZE msg_size;
	char buf[10];
	memset(buf,0,10);
	
	OLED_Clear();
	OLED_ShowString(8,0,"Enter Password");
	//printf("输入密码\r\n");
	//printf("buf: %s\r\n",buf);

	while(1)
	{
		p=OSQPend(&g_queue,0,OS_OPT_PEND_BLOCKING,&msg_size,NULL,&err);//阻塞等待操作

		if(strcmp((char *)p,"B")==0)//退出
		{
			return;
		}
		else if(strcmp((char *)p,"A")!=0 && strcmp((char *)p,"B")!=0)
		{
			if(strlen(buf)<=5)
				strcat(buf,(char *)p);
			else
			{
				//printf("buf: %s\r\n",buf);
				break;
			}				
		}
		OLED_ShowString(40,3,(u8 *)buf);
	}

	FLASH_erasure();//擦除FLASH
	WriteFlashData((FLASH4),(u8 *)buf,6);
	OLED_Clear();
	OLED_ShowString(8,0,"OverWrite success");
	//printf("覆盖成功\r\n");
	delay_ms(1200);
}

u8 * Check_Password(char *buf)
{
	char pass[10];
	//printf("check: %s\r\n",buf);
	ReadFlashData((FLASH4),pass,6);//FLASH读密码
	if(strcmp(buf,pass)==0)
	{
		return (u8 *)"success";
	}
	return NULL;
}


