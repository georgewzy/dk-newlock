

#include "gps.h"
#include "string.h"		
#include "stdlib.h"
#include "adc.h"
#include "bsp.h"
#include "timer.h"
#include "gprs.h"
#include "usart.h"
#include "aes.h"
#include "aes128.h"
#include "app_md5.h"
#include "button.h"
#include "gu906.h"
#include "transport.h"


extern usart_buff_t *gprs_buff;



uint8_t receiveText[24];
uint8_t expressText[512];  
uint8_t cipherText[512];
uint8_t aesKey[16];


//响铃
uint8_t bell_flag=0;
//手动
extern uint8_t Hand_Flag;
//震动
uint8_t Shaking=0;
uint8_t Shaking_Alarm_Flag=0;
//电压
uint16_t Bat_V;
uint16_t Bat_Pre;


uint8_t *p1;
uint8_t *p2;



uint8_t lock_open_err_flag = 0;
uint8_t lock_close_err_flag = 0;
uint8_t Lock_Open=0;
uint8_t Lock_Open_On=0;
uint8_t Lock_Open_Off=0;

uint8_t Lock_Close=0;
uint8_t Lock_Close_On=0;
uint8_t Lock_Close_Off=0;


uint8_t gps_flag = 0;

uint8_t send_buff[100] = {0};

uint8_t protocol_buff[512] = {0};
uint8_t gps_buff[512] = {0};

extern uint8_t lock_id[17];

extern uint8_t gprs_status;

extern uint8_t usart2_buff[512];
extern uint16_t usart2_cnt; 

extern uint8_t usart3_buff[512];
extern uint16_t usart3_cnt;  
extern uint8_t usart3_rx_status;
extern uint8_t gprs_status;

uint8_t gps_err_cnt = 0;



uint8_t tmp[20];
uint8_t heartbeat_buff[2] = {0};




int main(void)
{ 
	uint16_t j,i,rxlen;
	uint16_t k=0;
	uint8_t upload=0; 
	uint8_t *ret;
	uint8_t status = 0;
	uint8_t data[512];
	uint32_t data_len;
	button_info_s bbb;
	
	bsp_init();
	
                         
	USART_OUT(USART1, "uart1 is ok\r\n");

//	gu906_init();
	
	while(1)
	{
		LED0_LOW();
		
		
		transport_getdata(data, data_len);
		
		USART_OUT(USART1, data);
		memset(data, 0, 512);

		
		if(button_get_state(SW1 ,1000) == BUTTON_CLICK)
		{
			USART_OUT(USART1, "BUTTON_CLICK\r\n");
//			bsp_system_reset();
		}
		else if(button_get_state(SW1 ,1000) == BUTTON_PRESS)
		{
			USART_OUT(USART1, "BUTTON_PRESS\r\n");
		}
//		else if(button_get_state(SW1 ,1000) == BUTTON_DOWN)
//		{
//			USART_OUT(USART1, "BUTTON_DOWN\r\n");
//		}
		
		if(timer_is_timeout_1ms(timer_test, 1000) == 0)
		{
//			USART_OUT(USART1, "george\r\n");
		}

		
//		usart1_recv_data();
		LED0_HIGH();
	}
	
	while(0)
	{	 

		while(1)
		{
			gprs_config();
			if(gprs_status == 255)
			{
				MakeFile_MD5_Checksum(lock_id, 16);			
				break;
			}
		}
	
		
		usart1_recv_data();
		usart2_recv_data();
	

			
		if(timer_is_timeout_1ms(timer_batt, 1000*60*30) == 0)
		{		
					// 电池信息
			Bat_V = adc_get_average(ADC_Channel_0,10);
			Bat_V=Bat_V*3300/4096;
			Bat_V=Bat_V*88/20;
			Bat_Pre=(Bat_V-5000)*100/2400;
			
			memset(send_buff, 0, 100);	
			sprintf((char *)send_buff,"%s%s%s","AT+PUBLISH=lockdata/",lock_id,",24,2\r\n");

			USART_OUT(USART1, "ccc=%s\r\n", send_buff);

			ret = gprs_send_at(send_buff, ">", 300, 2000);
			if(ret != NULL)
			{
				memset(expressText, 0 ,512);
				memset(cipherText, 0 ,512);
				sprintf((char *)expressText, "{%c%s%c:%s}",'"',"battery",'"',"20");
				USART_OUT(USART1, "expressText=%s\r\n", expressText);
				AES_Encrypt((char *)expressText, cipherText, aesKey);
				
				USART_OUT(USART1, "aesKey=%s\r\n", aesKey);
				USART_OUT(USART1, "cipherText=%s\r\n", cipherText);
				ret = gprs_send_at(cipherText, "OK", 300, 0);
				if(ret != NULL)
				{
					timer_is_timeout_1ms(timer_heartbeat, 0);
				}			
			}
			else
			{
				
			}

		}
//		//接收锁数据
		p1 = strstr((uint8_t*)protocol_buff, "topic: lock/");
		p2 = strstr((uint8_t *)p1,(uint8_t *)lock_id);
		if(strncmp((char *)p1,(char *)"topic: lock/",12)==0)
		{
			
			if(LOCK_ON_READ()==0 || LOCK_OFF_READ()==0)
			{
			timer_is_timeout_1ms(timer_heartbeat, 0);
			USART_OUT(USART1, "lock data\r\n");
			memset(receiveText ,0 , 512);
			memset(expressText ,0 , 512);
			
			strcpy((char*)receiveText ,(char *)(p1+33));
			USART_OUT(USART1, "receiveText=%s\r\n", receiveText);
			AES_Decrypt(expressText, receiveText, aesKey);
			if(*expressText==0x31)
			{
//				if(LOCK_ON_READ() == 0)
//				if(lock_on_status_get() == 0)
				if(LOCK_ON_READ()==0 && LOCK_OFF_READ()==1)
				{
					timer_is_timeout_1ms(timer_open_lock, 0);
					Shaking=1;
					Lock_Open=1;
					USART_OUT(USART1, "Lock_Open11111\r\n");
				}
				else
				{
					Lock_Open=0;
				}
			}
			else if(*expressText==0x32)
			{
//				if(LOCK_OFF_READ() == 0)
//				if(lock_off_status_get() == 0)
				if(LOCK_ON_READ()==1 && LOCK_OFF_READ()==0)	
				{
					timer_is_timeout_1ms(timer_close_lock, 0);
					Shaking=1;
					Lock_Close=1;
					USART_OUT(USART1, "Lock_Close11111\r\n");
				}
				else
				{
					Lock_Close=0;
				}
			
			}
			else if(*expressText == 0x30)
			{
				lock_stop();	//停止运行;
			}
			
			memset(protocol_buff, 0, 512);					
		}
	}

		//开锁逻辑		
		if(Lock_Open == 1)
		{

			if(timer_is_timeout_1ms(timer_open_lock, 4000) == 0)
			{
				lock_open_err_flag = 1;
				
				lock_close();
				USART_OUT(USART1, "Lock_Open timer\r\n");
			}
			USART_OUT(USART1, "Lock_Open\r\n");
			if(LOCK_ON_READ()==0 && LOCK_OFF_READ()==1)
			{
				lock_open();	//开锁	
				USART_OUT(USART1, "AAA lock_open\r\n");
			}
//			if(LOCK_OFF_READ()==0) //正常开锁
			if(lock_off_status_get() == 0)
			{
				USART_OUT(USART1, "AAA lock_stop\r\n");
				
				Lock_Open = 0;
				lock_stop();	//停止运行
				Shaking = 0;
				
				sprintf((char *)send_buff,"%s%s%s","AT+PUBLISH=lockback/",(char *)lock_id,",44,2\r\n");
				USART_OUT(USART1, "send_buff=%s\r\n", send_buff);
				ret = gprs_send_at(send_buff, ">", 300, 2000);
				if(ret != NULL)
				{
					memset(expressText, 0 ,512);
					memset(cipherText, 0 ,512);
					sprintf(expressText,"{%c%s%c:%s,%c%s%c:%s}",'"',"cmd",'"',"1",'"',"ok",'"',"0");
					USART_OUT(USART1, "expressText=%s\r\n", expressText);
					AES_Encrypt((char *)expressText, cipherText, aesKey);
					USART_OUT(USART1, "cipherText=%s\r\n", cipherText);
					
					ret = gprs_send_at(cipherText, "OK", 300, 0);
					if(ret != NULL)
					{
						timer_is_timeout_1ms(timer_heartbeat, 0);
					}
				}			
			}
			
			if(LOCK_ON_READ()==0 && LOCK_OFF_READ()==1 && lock_open_err_flag == 1)//开锁超时异常处理
			{
				lock_open_err_flag = 0;
				Lock_Open = 0;
				lock_stop();
				Shaking = 0;
				USART_OUT(USART1, "open lock unusual\r\n");
				sprintf((char *)send_buff,"%s%s%s","AT+PUBLISH=lockback/",(char *)lock_id,",44,2\r\n");
				USART_OUT(USART1, "send_buff=%s\r\n", send_buff);
				ret = gprs_send_at(send_buff, ">", 300, 2000);
				if(ret != NULL)
				{
					memset(expressText, 0 ,512);
					memset(cipherText, 0 ,512);
					sprintf((char *)expressText,"{%c%s%c:%s,%c%s%c:%s}",'"',"cmd",'"',"1",'"',"ok",'"',"1");
					USART_OUT(USART1, "expressText=%s\r\n", expressText);
					AES_Encrypt((char *)expressText, cipherText, aesKey);
					ret = gprs_send_at(cipherText, "OK", 300, 0);
					if(ret != NULL)
					{
						timer_is_timeout_1ms(timer_heartbeat, 0);
					}
				}
				else
				{

				}	
			}
		}
		//关锁逻辑
		else if(Lock_Close == 1)
		{
			if(timer_is_timeout_1ms(timer_close_lock, 4000) == 0)
			{
				lock_open();
				lock_close_err_flag = 1;
				USART_OUT(USART1, "Lock_Close timer\r\n");
			}
			USART_OUT(USART1, "lock close\r\n");
			if(LOCK_ON_READ()==1 && LOCK_OFF_READ()==0)	//正常关锁
			{
				lock_close();
				USART_OUT(USART1, "BBB lock close\r\n");
			}
			
//			if(LOCK_ON_READ() == 0)	//正常关锁
			if(lock_on_status_get() == 0)
			{
				Lock_Close = 0;
				lock_stop();	//停止运行
				Shaking = 0;
				USART_OUT(USART1, "BBB lock_stop\r\n");
				
				sprintf((char *)send_buff,"%s%s%s","AT+PUBLISH=lockback/",(char *)lock_id,",44,2\r\n");
				USART_OUT(USART1, "PublishLockBackbuf=%s\r\n", send_buff);
				ret = gprs_send_at(send_buff, ">", 300, 2000);
				if(ret != NULL)
				{
					memset(expressText, 0 ,512);
					memset(cipherText, 0 ,512);
					sprintf((char *)expressText,"{%c%s%c:%s,%c%s%c:%s}",'"',"cmd",'"',"2",'"',"ok",'"',"0");
					USART_OUT(USART1, "expressText=%s\r\n", expressText);
					AES_Encrypt((char *)expressText, cipherText, aesKey);
					ret = gprs_send_at(cipherText, "OK", 300, 0);
					if(ret != NULL)
					{
						timer_is_timeout_1ms(timer_heartbeat, 0);
					}
				}
			}
			
			if(LOCK_ON_READ()==1 && LOCK_OFF_READ()==0 && lock_close_err_flag == 1) //关锁超时处理
			{
				lock_close_err_flag = 0;
				Lock_Close = 0;
				lock_stop();	//停止运行
				Shaking = 0;
				USART_OUT(USART1, "close lock unusual\r\n");
				sprintf((char *)send_buff,"%s%s%s","AT+PUBLISH=lockback/",(char *)lock_id,",44,2\r\n");
				USART_OUT(USART1, "send_buff=%s\r\n", send_buff);
				ret = gprs_send_at(send_buff, ">", 300, 2000);
				if(ret != NULL)
				{
					memset(expressText, 0 ,512);
					memset(cipherText, 0 ,512);
					sprintf((char *)expressText,"{%c%s%c:%s,%c%s%c:%s}",'"',"cmd",'"',"2",'"',"ok",'"',"1");
					AES_Encrypt((char *)expressText, (char*)cipherText, (char*)aesKey);
				
					ret = gprs_send_at(cipherText, "OK", 300, 0);
					if(ret != NULL)
					{
						timer_is_timeout_1ms(timer_heartbeat, 0);
					}
				}
			}		
		}
		//按键
		if(button_get_state(SW1 ,1000) == BUTTON_CLICK)
		{
			USART_OUT(USART1, "button_get_value\r\n");
			timer_is_timeout_1ms(timer_close_lock, 0);
			Lock_Close = 1;
			Shaking=1;	
		}
			
		
		//报警器
		p1 = strstr((char*)protocol_buff, "topic: bell/");
		p2 = strstr((char *)p1,(char *)lock_id);
		if(strncmp((char *)p1,(char *)"topic: bell/",12)==0 && bell_flag==0)
		{
			timer_is_timeout_1ms(timer_heartbeat, 0);
			USART_OUT(USART1, "bell\r\n");
			memset(protocol_buff, 0, 512);	
			bell_flag=1;
  	
			BEEP_ON();
			timer_delay_1ms(100);
			BEEP_OFF();
		}
				

		//晃动报警
		if(LOCK_ON_READ()==1 && LOCK_OFF_READ()==1 && Shaking==0)
		{
			USART_OUT(USART1, "Sharking\r\n");
			if(timer_is_timeout_1ms(timer_bell_1, 400) == 0)
			{
				BEEP_ON();
			}
			if(timer_is_timeout_1ms(timer_bell_2, 800) == 0)
			{
				BEEP_OFF();
			}
		}
		else
		{
			BEEP_OFF();
		}
		
		//断线重连
		p1 = strstr((char*)protocol_buff, "MQTT CLOSE");
		if(p1 !=NULL)
		{
			memset(protocol_buff, 0, 512);	
			gprs_status = 0;
			USART_OUT(USART1, "MQTT SERVER CLOSE\r\n");
		}
		

		//心跳
		if(timer_is_timeout_1ms(timer_heartbeat, 1000*60*3) == 0)
		{
			memset(send_buff, 0, 100);
			sprintf((char *)send_buff,"%s%s%s","AT+PUBLISH=lockdata/", lock_id,",1,2\r\n");
	
			USART_OUT(USART1, "heartbeat=%s\r\n", send_buff);
			ret = gprs_send_at(send_buff, ">", 300, 2000);
			if(ret != NULL)
			{
				memset(heartbeat_buff, 0, 2);
				heartbeat_buff[0] = 0x30;
				ret = gprs_send_at(heartbeat_buff, "OK", 300, 0);
				if(ret != NULL)
				{
					
				}	
			}
			else
			{
				
			}
		}	
		
	}	 
}
 





















