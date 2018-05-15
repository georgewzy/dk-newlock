#include <string.h>
#include <stdlib.h>
#include "stdio.h"
#include "delay.h"
#include "GU906.h"
#include "config.h"
#include "usart1.h"
#include "usart4.h"

int main(void)
{    
	u32 ret = 0;
	char buff[200]="";
	struct Gprs_Config GprsCon;
	delay_init();
	usart4_Configuration(115200);    //GU900默认通信波特率是115200
	usart1_Configuration(115200);    //调试输出端口波特率设置
	delay_s(5);                      //刚上电 要等待10秒，等待GU906模块初始化完成
	
	printf("\r\nBegin...\r\n");
	GprsCon.server_ip = (u8 *)"218.66.59.201"; //GPRS通信时的服务器IP
	GprsCon.server_port = atoi("8888");        //GPRS通信时的服务器端口
	
	//GSM初始化
	while(1)
	{
		if(_ATOK == GU906_init()){
			printf("GU906 init ok.\r\n\r\n");
			break;
		}
		printf("init error.\r\n");
		delay_s(1);
	}
	
	/*****************************************************************************/
	//GU906 GPRS TCP 非透传模式通信测试
	while(1)
	{
		if(_ATOK == GU906_TCP_Socket(&GprsCon))
		{
			printf("socket ok\r\n\r\n");
			delay_s(3);	
			while(1)
			{
				ret = GU906_GPRS_read(buff, 200);
				if(ret)
				{
					printf("GPRS:[%d][%s]\r\n", ret,buff);
					if(_ATOK != GU906_GPRS_write((char *)"OK", 2))
					{
						printf("Send Error.\r\n");
					}					
				}
			}
		}
		printf("GU906_TCP_Socket ERROR.\r\n");
		while(1);
	}
	/*******************************************************************************/
	
	/*****************************************************************************/
	//GU906 GPRS TCP 透传模式通信测试
	while(1)
	{
		if(_ATOK == GU906_DTU_Socket(&GprsCon))
		{
			printf("socket ok\r\n\r\n");
			delay_s(3);	
			while(1)
			{
				ret = GU906_GPRS_read(buff, 200);
				if(ret)
				{
					printf("GPRS:[%d][%s]\r\n", ret,buff);
					if(_ATOK != GU906_GPRS_write((char *)buff, ret))
					{
						printf("Send Error.\r\n");
					}					
					
					if(strstr(buff,"CLOSE"))
					{
						GU906_DtuOrAT(0);
					}
					if(strstr(buff,"OPEN"))
					{
						GU906_DtuOrAT(1);
					}
				}
			}
		}
		printf("GU906_TCP_Socket ERROR.\r\n");
		while(1);
	}
	/*******************************************************************************/
	
	/*****************************************************************************/
	//发送短信测试
    while(_ATOK != GU906_Chinese_text("18750******", "123abd 测试"))
    {
        delay_s(5);
    }

	//接收短信测试
    while(1)
    {
        if(0 == GU906_Read_UserSMS())
        {
            printf("------------------------------\r\n");
            printf("号码:%s\r\n",sim.phone);
            printf("设备:%s\r\n",sim.dev);
            printf("时间:%s\r\n",sim.date);
            printf("信息:%s\r\n",sim.data);
        }
        delay_ms(50);
    }
	/******************************************************************************/
	
	/*****************************************************************************/
	//打电话测试
	if (_ATOK == GU906_make_phone("18750******"))
	{
		//等待接听
		while(_ATOTIME == GU906_Answer_Phone(1000))
		{
			printf("make ok\r\n");
			GU906_end_phone();			
		}
		printf("make ok\r\n");
	}
	else 
	{
		printf("make error\r\n");
		//SoftReset();
	}
	/******************************************************************************/
	while(1);
}

