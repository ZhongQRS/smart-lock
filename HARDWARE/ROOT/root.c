#include "root.h"


void *ROOT_Mode(void)	//root模式
{
	OS_ERR  err;
	OS_MSG_SIZE msg_size;
	uint8_t *p=NULL;
	
	OLED_Clear(); 	//OLED清屏
	OLED_ShowString(8,0,"1 ->Finger");
	OLED_ShowString(8,2,"2 ->RFID");
	OLED_ShowString(8,4,"3 ->Password");
	OLED_ShowString(8,6,"B ->Exit");
	
	p=OSQPend(&g_queue,0,OS_OPT_PEND_BLOCKING,&msg_size,NULL,&err);	//等待消息队列
	if(p != NULL)
	{
		if(strcmp((char*)p,"1")==0)//1-->录指纹
		{				
			OLED_Clear(); 	//OLED清屏
			OLED_ShowString(8,0,"Enroll FingerPrint");	
			//printf("录指纹\r\n");
			Add_FR();
			return NULL;
		}
		else if(strcmp((char*)p,"3")==0)//3-->覆盖密码
		{
			OLED_Clear(); 	//OLED清屏
			OLED_ShowString(8,0,"Overwrite Password");
			//printf("重写密码\r\n");
			OverWrite_Password();
			return NULL;
		}
		else if(strcmp((char*)p,"B")==0)//B退出	B-->Back
		{
			OLED_Clear(); 	//OLED清屏
			OLED_ShowString(8,0,"Exit Root!");	
			//printf("退出root模式\r\n");
			return NULL;
		}
	}
	
}
