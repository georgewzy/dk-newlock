/*************************************************
  Copyright (C), 1988-1999,  Tech. Co., Ltd.
  File name:      
  Author:       Version:        Date:
  Description:   
  Others:         
  Function List:  
    1. ....
  History:         
                   
    1. Date:
       Author:
       Modification:
    2. ...
*************************************************/

/*
*********************************************************************************************************
*                                             INCLUDE FILES
*********************************************************************************************************
*/
#include <stdio.h>
#include <string.h>
#include "gprs.h"
#include "bsp.h"
#include "usart.h"
#include "timer.h"
#include "transport.h"
#include "main.h"
#include "list.h"
#include "protocol.h"



extern usart_buff_t usart1_rx_buff;
extern usart_buff_t usart2_rx_buff;
extern usart_buff_t at_rx_buff;
extern unsigned short mqtt_publish_msgid;
extern uint8_t usart2_rx_status;	//usart2接收状态

extern DEV_CONFIG_INFO  dev_config_info;

uint8_t mcu_reset_cnt = 0;	//系统重启计数器
uint8_t gprs_reset_cnt = 0;	//GPRS重启计数器
uint8_t gprs_err_cnt = 0; 	//GPRS错误计数器
uint8_t gprs_status = 0;	//GPRS的状态




/*
*********************************************************************************************************
*                                          gprs_gpio_init()
*
* Description : Create application tasks.
*
* Argument(s) : none
*
* Return(s)   : none
*
* Caller(s)   : gprs_init_task_fun()
*
* Note(s)     : none.
*********************************************************************************************************
*/
void gprs_gpio_init(void)
{
	GPIO_InitTypeDef gpio_init_structure;
	//GPRS POWER EN
	gpio_init_structure.GPIO_Pin = GPIO_Pin_6;
  	gpio_init_structure.GPIO_Speed = GPIO_Speed_2MHz;
	gpio_init_structure.GPIO_Mode = GPIO_Mode_OUT;
//	gpio_init_structure.GPIO_PuPd = GPIO_PuPd_NOPULL;
  	GPIO_Init(GPIOA, &gpio_init_structure);
	
	//GPRS PWON
	gpio_init_structure.GPIO_Pin = GPIO_Pin_7;
  	gpio_init_structure.GPIO_Speed = GPIO_Speed_2MHz;
	gpio_init_structure.GPIO_Mode = GPIO_Mode_OUT; 
//	gpio_init_structure.GPIO_PuPd = GPIO_PuPd_NOPULL;	
  	GPIO_Init(GPIOA, &gpio_init_structure);
	
	//GPRS DTR
	gpio_init_structure.GPIO_Pin = GPIO_Pin_11;
  	gpio_init_structure.GPIO_Speed = GPIO_Speed_2MHz;
	gpio_init_structure.GPIO_Mode = GPIO_Mode_OUT; 
//	gpio_init_structure.GPIO_PuPd = GPIO_PuPd_NOPULL;
  	GPIO_Init(GPIOA, &gpio_init_structure);
	GPIO_SetBits(GPIOA, GPIO_Pin_11);
}






/*
*********************************************************************************************************
*                                          gprs_power_on()
*
* Description : Create application tasks.
*
* Argument(s) : none
*
* Return(s)   : none
*
* Caller(s)   : gprs_init_task_fun()
*
* Note(s)     : none.
*********************************************************************************************************
*/
void gprs_power_on(void)
{

	GPIO_ResetBits(GPIOA, GPIO_Pin_6);		//GPRS POWER EN
	timer_delay_1ms(10);
	GPIO_SetBits(GPIOA, GPIO_Pin_6);				
	timer_delay_1ms(10);
	
	GPIO_SetBits(GPIOA, GPIO_Pin_7);				//GPRS_PWON
	timer_delay_1ms(10);
	GPIO_ResetBits(GPIOA, GPIO_Pin_7);				//GPRS_PWON
	timer_delay_1ms(1200);
	
}






/*
*********************************************************************************************************
*                                          gprs_check_cmd()
*
* Description : 检测字符串中是否存在子字符串.
*
* Argument(s) : p_str ： 要检测的子字符串
*
* Return(s)   : 返回子字符串的位置指针
*
* Caller(s)   : gprs_send_at()
*
* Note(s)     : 
*********************************************************************************************************
*/
uint8_t *gprs_check_cmd(uint8_t *src_str, uint8_t *p_str)
{
	char *str = NULL;
	
	str = strstr((const char*)src_str, (const char*)p_str);

	return (uint8_t*)str;
}


/*
*********************************************************************************************************
*                                          gprs_send_at()
*
* Description : Create application tasks.
*
* Argument(s) : none
*
* Return(s)   : none
*
* Caller(s)   : gprs_init_task_fun()
*
* Note(s)     : none.
*********************************************************************************************************
*/
uint8_t* gprs_send_at(uint8_t *cmd, uint8_t *ack, uint16_t waittime, uint16_t timeout)
{
	uint8_t res = 1;
	uint8_t buff[512] = {0};
	uint8_t usart2_rx_finish_flag = 0;
	
	timer_is_timeout_1ms(timer_at, 0);	//开始定时器timer_at
	while (res)
	{		
		if(usart2_rx_buff.index == 0)	//等待数据接收完
		{
			usart2_rx_finish_flag = 1;
			USART_OUT(USART2, cmd);		
			timer_delay_1ms(waittime);				//AT指令延时
			usart_send_data(USART1, usart2_rx_buff.pdata, usart2_rx_buff.index);
		}
		
		if(usart2_rx_finish_flag == 0)	//接收数据
		{
			usart2_recv_data();
			USART_OUT(USART1, "dddd\r\n");
		}
			
		if(strstr((const char*)usart2_rx_buff.pdata, "+CME ERROR: 9"))
		{
			res = 0;
			gprs_status = 0;
			memset(&usart2_rx_buff, 0, sizeof(usart2_rx_buff));	
		}
		
		if (strstr((const char*)usart2_rx_buff.pdata, (const char*)ack))	
		{
			res = 0;				//监测到正确的应答数据
			usart2_rx_status = 0;	//数据处理完 开始接收数据
			
			memcpy(buff, usart2_rx_buff.pdata, 512);		
			memset(&usart2_rx_buff, 0, sizeof(usart2_rx_buff));	
			
			return buff;
		}
		
		if (timer_is_timeout_1ms(timer_at, timeout) == 0)	//定时器timer_at结束
		{
			res = 0;
			memset(&usart2_rx_buff, 0, sizeof(usart2_rx_buff));	
			return NULL;
		}
	}	
}

uint8_t* gprs_send_at1(uint8_t *cmd, uint8_t *ack, uint16_t waittime, uint16_t timeout)
{
	uint8_t res = 1;
	uint8_t buff[512] = {0};
	
	timer_is_timeout_1ms(timer_at, 0);	//开始定时器timer_at
	while (res)
	{		
		memset(&usart2_rx_buff, 0, sizeof(usart2_rx_buff));

		if(usart2_rx_buff.index == 0)
		{
			
		}
		USART_OUT(USART2, cmd);		
		timer_delay_1ms(waittime);				//AT指令延时
		usart_send_data(USART1, usart2_rx_buff.pdata, usart2_rx_buff.index);
		if(strstr((const char*)usart2_rx_buff.pdata, "+CME ERROR: 9"))
		{
			res = 0;
			gprs_status = 0;	
		}
		if (strstr((const char*)usart2_rx_buff.pdata, (const char*)ack))	
		{
			res = 0;				//监测到正确的应答数据				
			memcpy(buff, usart2_rx_buff.pdata, 512);		
			memset(&usart2_rx_buff, 0, sizeof(usart2_rx_buff));	
			
			return buff;
		}
			
		if (timer_is_timeout_1ms(timer_at, timeout) == 0)	//定时器timer_at结束
		{
			res = 0;
			usart2_rx_status = 0;	//数据处理完 开始接收数据
			return NULL;
		}
	}	
}

uint8_t* gprs_wakeup_at(uint8_t *cmd, uint8_t *ack, uint16_t waittime, uint16_t timeout)
{
	uint8_t res = 1;
	uint8_t buff[512] = {0};
	
	timer_is_timeout_1ms(timer_at, 0);	//开始定时器timer_at
	while (res)
	{		
		memset(&usart2_rx_buff, 0, sizeof(usart2_rx_buff));

		
		USART_OUT(USART2, cmd);		
		timer_delay_1ms(waittime);				//AT指令延时

		if(strstr((const char*)usart2_rx_buff.pdata, "+CME ERROR: 9"))
		{
			res = 0;
			gprs_status = 0;	
		}
		if (strstr((const char*)usart2_rx_buff.pdata, (const char*)ack))	
		{
			res = 0;				//监测到正确的应答数据
			usart2_rx_status = 0;	//数据处理完 开始接收数据
			
			memcpy(buff, usart2_rx_buff.pdata, 512);		
			memset(&usart2_rx_buff, 0, sizeof(usart2_rx_buff));	
			
			return buff;
		}
			
		if (timer_is_timeout_1ms(timer_at, timeout) == 0)	//定时器timer_at结束
		{
			res = 0;
			usart2_rx_status = 0;	//数据处理完 开始接收数据
			return NULL;
		}
	}	
}

uint8_t* gprs_send_at2(uint8_t *cmd, uint8_t *ack, uint16_t waittime, uint16_t timeout)
{
	uint8_t res = 1;
	uint8_t buff[512] = {0};
	uint8_t usart2_rx_finish_flag = 0;
	
	timer_is_timeout_1ms(timer_at, 0);	//开始定时器timer_at
	while (res)
	{		
		if(usart2_rx_buff.index == 0)	//等待数据接收完 步骤2
		{
			usart2_rx_finish_flag = 1;
			USART_OUT(USART2, cmd);		
			USART_OUT(USART1, "yyyyyyyyyy\r\n");
		}
		
		if(usart2_rx_finish_flag == 1)
		{
			if(usart2_rx_buff.index > 0)
			{
				usart_send_data(USART1, usart2_rx_buff.pdata, usart2_rx_buff.index);	
				if (timer_is_timeout_1ms(timer_at, timeout) == 0)	//定时器timer_at结束
				{
					res = 0;
					memset(&usart2_rx_buff, 0, sizeof(usart2_rx_buff));	
					return NULL;
				}
			}
		}
		
		if(usart2_rx_finish_flag == 0)	//接收数据步骤1
		{
			usart2_recv_data();
		}
			
		if(strstr((const char*)usart2_rx_buff.pdata, "+CME ERROR: 9"))
		{
			res = 0;
			gprs_status = 0;
			memset(&usart2_rx_buff, 0, sizeof(usart2_rx_buff));	
		}
		
		if (strstr((const char*)usart2_rx_buff.pdata, (const char*)ack))	
		{
			res = 0;				//监测到正确的应答数据
			usart2_rx_status = 0;	//数据处理完 开始接收数据			
			memcpy(buff, usart2_rx_buff.pdata, 512);		
			memset(&usart2_rx_buff, 0, sizeof(usart2_rx_buff));	
	
			return buff;
		}
	}	
}
/*
*********************************************************************************************************
*                                          gprs_send_at()
*
* Description : Create application tasks.
*
* Argument(s) : none
*
* Return(s)   : none
*
* Caller(s)   : gprs_init_task_fun()
*
* Note(s)     : none.
*********************************************************************************************************
*/
uint8_t* gprs_send_at3(uint8_t *cmd, uint8_t *ack, uint16_t waittime, uint16_t timeout)
{
	uint8_t res = 0;
	uint8_t buff[512] = {0};
	uint8_t usart2_rx_finish_flag = 0;
	
	while(1)
	{
		usart2_recv_data();
		if(usart2_rx_buff.index == 0)	//等待缓冲区数据处理完
		{
			USART_OUT(USART1, "GPRS000000000\r\n");
			res = 1;
			timer_is_timeout_1ms(timer_at, 0);	//开始定时器timer_at	
			USART_OUT(USART2, cmd);	
			break;
		}
	}
	
	while (res)
	{	
		memset(&at_rx_buff, 0, sizeof(at_rx_buff));
		while(at_rx_buff.index == 0)
		{
			usart2_recv_data();

			if(timer_is_timeout_1ms(timer_at, timeout) == 0)	//定时器timer_at结束
			{
				res = 0;
				return NULL;
			}
		}
		
		if(strstr((const char*)at_rx_buff.pdata, "+CME ERROR: 9"))
		{
			res = 0;
			gprs_status = 0;	
		}
		
		if (strstr((const char*)at_rx_buff.pdata, (const char*)ack))	
		{
			res = 0;				//监测到正确的应答数据
			memcpy(buff, at_rx_buff.pdata, 512);						
			return buff;
		}
	}	
}



/*
*********************************************************************************************************
*                                          gprs_init_task()
*
* Description : GPRS初始化任务函数.
*
* Argument(s) : p_arg :该任务要传的参数
*
* Return(s)   : none
*
* Caller(s)   : 
*
* Note(s)     : none.
*********************************************************************************************************
*/
void gprs_init_task(list_node ** list, GPRS_CONFIG *gprs_info, MQTTPacket_connectData *mqtt_data)
{
	int mqtt_rc = 0;

	uint8_t *ret;
	uint8_t buff[100] = {0};
	uint8_t cipstart[100] = {0};

	while(1)
	{
		iwatchdog_clear();
	
		switch(gprs_status)
		{
			case 0:
				gprs_power_on();
				
				USART_OUT(USART1, "gprs_power_on\r\n");
				gprs_status = 1;
				gprs_err_cnt = 0;
				gprs_reset_cnt++;
				if(gprs_reset_cnt > 3)
				{
					bsp_system_reset();		
					USART_OUT(USART1, "bsp_system_reset\r\n");
				}
			break;
					
			case 1:
				ret = gprs_send_at("AT\r\n", "OK", 300,10000);
				if (ret != NULL)
				{
					gprs_status++;
					gprs_err_cnt = 0;
				}
				else
				{
					gprs_err_cnt++;
					if (gprs_err_cnt > 5)
					{
						gprs_status = 0;
					}
				}
			break;
			
			case 2:
				ret = gprs_send_at("ATE0\r\n", "OK", 500,10000);
				if (ret != NULL)
				{
					gprs_status++;
					gprs_err_cnt = 0;
				}
				else
				{
					gprs_err_cnt++;
					if (gprs_err_cnt > 5)
					{
						gprs_status = 0;
					}
				}
			break;
				
			case 3:
				ret = gprs_send_at("AT+CGSN\r\n", "OK", 500, 10000);
				if (ret != NULL)
				{
					gprs_status++;
					gprs_err_cnt = 0;
				}
				else
				{
					gprs_err_cnt++;
					if (gprs_err_cnt > 5)
					{
						gprs_status = 0;
					}
				}
			break;
			
			case 4:			
				ret = gprs_send_at("AT+CPIN?\r\n", "OK", 500, 10000);
				if (ret != NULL)
				{
					gprs_status++;
					gprs_err_cnt = 0;
				}
				else
				{
					gprs_err_cnt++;
					if (gprs_err_cnt > 5)
					{
						gprs_status = 0;
					}
				}
			break;
			
			case 5:
				ret = gprs_send_at("AT+CSQ\r\n", "OK", 500, 10000);
				if (ret != NULL)
				{
					gprs_status++;
					gprs_err_cnt = 0;
				}
				else
				{
					if (gprs_err_cnt > 5)
					{
						gprs_status = 0;
					}
				}
			break;
			
			case 6:
				ret = gprs_send_at("AT+CREG?\r\n", "OK", 1000, 30000);
				if (ret != NULL)
				{
					if((strstr((const char*)ret, (const char*)"+CREG: 0,1") != NULL) 
						|| (strstr((const char*)ret, (const char*)"+CREG: 0,5") != NULL))
					{
						gprs_status++;
						gprs_err_cnt = 0;
					}
					else
					{
						gprs_err_cnt++;
						if (gprs_err_cnt > 500)
						{
							gprs_status = 0;
						}
					}
				}
				else
				{
					gprs_err_cnt++;
					if (gprs_err_cnt > 5)
					{
						gprs_status = 0;
					}
				}
			break;
			
			case 7:
				ret = gprs_send_at("AT+CIPCLOSE=0\r\n", "ERROR", 800, 10000);//
				if (ret != NULL)
				{
					gprs_status++;
					gprs_err_cnt = 0;
				}
				else
				{
					gprs_err_cnt++;
					if (gprs_err_cnt > 5)
					{
						gprs_status = 0;
					}
				}
			break;
			
			case 8:
				 sprintf(cipstart, "%s,\"%s\",%d\r\n","AT+CIPSTART=\"TCP\"", gprs_info->server_ip, gprs_info->server_port);
//				ret = gprs_send_at("AT+CIPSTART=\"TCP\",\"103.46.128.47\",14947\r\n", "CONNECT OK", 1500, 20000);//
//				ret = gprs_send_at("AT+CIPSTART=\"TCP\",\"118.31.69.148\",1883\r\n", "CONNECT OK", 1500, 10000);
				ret = gprs_send_at(cipstart, "CONNECT OK", 1500, 20000);
				if (ret != NULL)
				{
					gprs_status++;
					gprs_err_cnt = 0;
					timer_delay_1ms(1000);
				}
				else
				{
					gprs_err_cnt++;
					if (gprs_err_cnt > 5)
					{
						gprs_status = 0;
					}
				}
			break;
				
			case 9:
				gprs_status++;
//				mqtt_rc = mqtt_disconnect();
//				if(1 == mqtt_rc)
//				{
//					gprs_status=255;
//					USART_OUT(USART1, "mqtt_disconnect ok\r\n");
//				}
//				else
//				{
//					gprs_err_cnt++;
//					if (gprs_err_cnt > 5)
//					{
//						gprs_status = 0;
//						USART_OUT(USART1, "mqtt_disconnect error\r\n");
//					}
//				}
			break;
				
			case 10:
				mqtt_rc = mqtt_connect(mqtt_data);
				if(1 == mqtt_rc)
				{
					mqtt_publish_msgid = 1;
					gprs_status++;
					USART_OUT(USART1, "mqtt_connect ok\r\n");
				}
				else
				{
					gprs_err_cnt++;
					if (gprs_err_cnt > 5)
					{
						gprs_status = 0;
						USART_OUT(USART1, "mqtt_connect error\r\n");
					}
				}
			break;				
				
			case 11:
				sprintf((char*)buff, "%s%s", "bell/", dev_config_info.dev_id);
				mqtt_rc = mqtt_subscribe_topic(buff, 0, mqtt_publish_msgid);
				if(1 == mqtt_rc)
				{
					gprs_status++;
					USART_OUT(USART1, "mqtt_subscribe_topic bell ok=%s\r\n", buff);
				}
				else
				{
					gprs_err_cnt++;
					if (gprs_err_cnt > 5)
					{
						gprs_status = 0;
						USART_OUT(USART1, "mqtt_subscribe_topic bell error\r\n");
					}
				}	
			break;
				
			case 12:
				sprintf((char*)buff, "%s%s", "lock/", dev_config_info.dev_id);
				mqtt_rc = mqtt_subscribe_topic(buff, 2, mqtt_publish_msgid);
				if(1 == mqtt_rc)
				{
					gprs_status++;
					USART_OUT(USART1, "mqtt_subscribe_topic lock ok=%s\r\n", buff);
				}
				else
				{
					gprs_err_cnt++;
					if (gprs_err_cnt > 5)
					{
						gprs_status = 0;
						USART_OUT(USART1, "mqtt_subscribe_topic lock error\r\n");
					}
				}
				
			break;
				
			case 13:
//				dev_first_power_on(list);
				dev_to_srv_lock_status(list);
				gprs_status = 255;
			break;
			
			case 254:
			
				bsp_system_reset();
			
			break;
			
			case 255:	//gprs 初始化完成后进行数据传输
				
//				gprs_recv_task_create();
			
			break;
				
				
			default:
			break;		
		}	//switch end	
		
		if(gprs_status == 255)
		{
			break;
		}
	} // while end	
}

/*
*********************************************************************************************************
*                                          gprs_sleep()
*
* Description : Create application tasks.
*
* Argument(s) : none
*
* Return(s)   : none
*
* Caller(s)   : gprs_init_task_fun()
*
* Note(s)     : none.
*********************************************************************************************************
*/
int gprs_sleep(void)
{
	uint8_t *ret;
	int rc = 0;
	ret = gprs_send_at("AT+CSCLK=1\r\n", "OK", 100, 1000);
	if(ret != NULL)
	{
		//DTR
		GPIO_SetBits(GPIOA, GPIO_Pin_11);
		rc = 1;
	}
	else
	{
		rc = 0;
	}
	
	return rc;
}

/*
*********************************************************************************************************
*                                          gprs_wakeup()
*
* Description : Create application tasks.
*
* Argument(s) : none
*
* Return(s)   : none
*
* Caller(s)   : gprs_init_task_fun()
*
* Note(s)     : none.
*********************************************************************************************************
*/
int gprs_wakeup(uint8_t mode)
{
	uint8_t *ret;
	int rc = 0;
	if(mode == 0)
	{
		USART_OUT(USART2, "\r\n");	
		timer_delay_1ms(1);
		ret = gprs_wakeup_at("AT+CSCLK=0\r\n", "OK", 50, 2000);
		if(ret != NULL)
		{
			GPIO_ResetBits(GPIOA, GPIO_Pin_11);
			timer_delay_1ms(10);
			rc = 1;
		}
		else
		{
			rc = 0;
		}
	}
	else
	{
		//DTR
		GPIO_ResetBits(GPIOA, GPIO_Pin_11);
		timer_delay_1ms(500);
		rc = 1;
	}
	
	return rc;
}


int gprs_wakeup_dtr(void)
{
	int rc = 0;

	//DTR
	GPIO_ResetBits(GPIOA, GPIO_Pin_11);
	timer_delay_1ms(10);
	rc = 1;

	return rc;
}










