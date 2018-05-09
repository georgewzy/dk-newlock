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



extern usart_buff_t  usart1_rx_buff;
extern usart_buff_t  usart2_rx_buff;
extern unsigned short mqtt_publist_msgid;
extern uint8_t usart2_rx_status;
extern uint8_t lock_id[17];
extern DEV_CONFIG_INFO  dev_config_info;

uint8_t mcu_reset_cnt = 0;
uint8_t gprs_reset_cnt = 0;
uint8_t gprs_err_cnt = 0; 	//GPRS错误计数器
uint8_t gprs_status = 0;	//GPRS的状态



uint8_t gprs_send_at_flag = 0;	
uint8_t gprs_rx_flag = 0;



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
	
	timer_is_timeout_1ms(timer_at, 0);	//开始定时器timer_at
	while (res)
	{	
		memset(&usart2_rx_buff, 0, sizeof(usart2_rx_buff));
		
		usart2_rx_status = 0;
		USART_OUT(USART2, cmd);		
		timer_delay_1ms(waittime);				//AT指令延时
	
		usart2_rx_status = 1;	//数据未处理 不在接收数据
		USART_OUT(USART1, usart2_rx_buff.pdata);
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




/*
*********************************************************************************************************
*                                          gprs_init_task_fun()
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
void gprs_init_task(GPRS_CONFIG *gprs_info, MQTTPacket_connectData *mqtt_data)
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
				ret = gprs_send_at("ATE1\r\n", "OK", 500,10000);
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
							if (gprs_err_cnt > 120)
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
				ret = gprs_send_at(cipstart, "CONNECT OK", 1500, 10000);
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
					mqtt_publist_msgid = 1;
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
				mqtt_rc = mqtt_subscribe_topic(buff, 2, mqtt_publist_msgid);
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
				mqtt_rc = mqtt_subscribe_topic(buff, 2, mqtt_publist_msgid);
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
//				timer_is_timeout_1ms(timer_close_lock, 0);
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


int gprs_sleep(void)
{
	uint8_t *ret;
	int rc = 0;
	ret = gprs_send_at("AT+CSCLK=1\r\n", "OK", 100, 1000);
	if(ret != NULL)
	{
		rc = 1;
	}
	else
	{
		rc = 0;
	}
	
	return rc;
}

int gprs_wakeup(uint8_t mode)
{
	uint8_t *ret;
	int rc = 0;
	if(mode == 0)
	{
		USART_OUT(USART2, "\r\n");	
		timer_delay_1ms(5);
		ret = gprs_send_at("AT+CSCLK=0\r\n", "OK", 50, 1000);
		if(ret != NULL)
		{
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
		rc = 1;
	}
	
	return rc;
}













