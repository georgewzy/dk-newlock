


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
#include "transport.h"
#include "button.h"




extern usart_buff_t mqtt_buff;



u8 receiveText[24];
u8 expressText[512];  
u8 cipherText[512];
u8 aesKey[16];



u8 longitudedtbuf[30];
u8 latitudedtbuf[30];
u8 dtbuf[50]; 

//响铃
u8 bell_flag=0;
//手动
extern u8 Hand_Flag;
//震动
u8 Shaking=0;
u8 Shaking_Alarm_Flag=0;
//电压
u16 Bat_V;
u16 Bat_Pre;
u8 Bat_Pre_Flag=0;

u8 *p1;
u8 *p2;



u8 lock_open_err_flag = 0;
u8 lock_close_err_flag = 0;
u8 Lock_Open=0;
u8 Lock_Open_On=0;
u8 Lock_Open_Off=0;

u8 Lock_Close=0;
u8 Lock_Close_On=0;
u8 Lock_Close_Off=0;




u8 lock_open_success_flag = 0;


u8 flag=4;
//__self_calendar_obj self_calendar;	//自检日历结构体

////////////////////////////////////
 u8 second_flag=0;
 u8 LOCK_ON_flag=1;
 u8 LOCK_OFF_flag=1;

u8 lockbuf[16];
u8 lockbuff[16];

u8 gps_flag = 0;

u8 send_buff[100] = {0};

u8 protocol_buff[512] = {0};
u8 gps_buff[512] = {0};

extern u8 PARK_LOCK_Buffer[17];

extern uint8_t gprs_status;

extern u8 usart2_buff[512];
extern u16 usart2_cnt; 

extern u8 usart3_buff[512];
extern u16 usart3_cnt;  
extern u8 usart3_rx_status;
extern uint8_t gprs_status;

u8 gps_err_cnt = 0;



u8 tmp[20];
u8 heartbeat_buff[2] = {0};



static void test_encrypt_ecb(void)
{

    uint8_t key[] = { 0x2b, 0x7e, 0x15, 0x16, 0x28, 0xae, 0xd2, 0xa6, 0xab, 0xf7, 0x15, 0x88, 0x09, 0xcf, 0x4f, 0x3c };
    uint8_t out[] = { 0x3a, 0xd7, 0x7b, 0xb4, 0x0d, 0x7a, 0x36, 0x60, 0xa8, 0x9e, 0xca, 0xf3, 0x24, 0x66, 0xef, 0x97 };


    uint8_t in[]  = { 0x6b, 0xc1, 0xbe, 0xe2, 0x2e, 0x40, 0x9f, 0x96, 0xe9, 0x3d, 0x7e, 0x11, 0x73, 0x93, 0x17, 0x2a };
   	struct AES_ctx ctx;
	
	
	sprintf((char *)expressText,"{%c%s%c:%s}",'"',"battery",'"',"20");
//	expressText[0] = 0x31;
	USART_OUT(USART1, "expressText66=%s\r\n", expressText);
	

	
//	USART_OUT(USART1, "in=%s\r\n", in);
//	USART_OUT(USART1, "out=%s\r\n", out);
//	AES_init_ctx(&ctx, key);
//	AES_ECB_encrypt(&ctx, in);
//	USART_OUT(USART1, "in=%s\r\n", in);
//	USART_OUT(USART1, "out=%s\r\n", out);
//    AES_init_ctx(&ctx, aesKey);
//    AES_ECB_encrypt(&ctx, expressText);

	USART_OUT(USART1, expressText);

    if (0 == memcmp((char*) out, (char*) in, 16))
    {
        USART_OUT(USART1, "SUCCESS!\n");
    }
    else
    {
        USART_OUT(USART1, "FAILURE!\n");
    }
}



int main(void)
{ 
	u16 j,i,rxlen;
	u16 aa = 0;
	u8 upload=0; 
	u8 *ret;
	uint8_t status = 0;
	uint8_t topic[50] = {0};
	uint8_t payload[200] = {0};
	int payloadlen = 0;
	int mqtt_pub;
	
	bsp_init();
                         
	USART_OUT(USART1, "uart1 is ok\r\n");

<<<<<<< HEAD
=======
//	gu906_init();
	
>>>>>>> parent of 59d2934... c
	while(1)
	{
	
		gprs_init_task();
	
		usart1_recv_data();
		usart2_recv_data();
		mqtt_subscribe(topic, payload, payloadlen);
		
		
		
		
		if(timer_is_timeout_1ms(timer_batt, 1000*20) == 0)
		{
//			mqtt_pub = mqtt_publist("test", 0, 0, 2, 1);
//			if(mqtt_pub == 1)
//			{
//				USART_OUT(USART1, "mqtt_publist ok\r\n");
//			}
		
		}
		
		if(BUTTON_CLICK == button_get_state(SW1, 2000))
		{
			USART_OUT(USART1, "BUTTON_CLICK\r\n");
			
			BEEP_ON();
			timer_delay_1ms(200);
			BEEP_OFF();
//			aa = adc_get_value(14);
//			USART_OUT(USART1, "aa = %d\r\n", aa);
		}
		
	
		
		if(timer_is_timeout_1ms(timer_mqtt_keep_alive, 1000*60) == 0)
		{
			mqtt_keep_alive();	
		}

		

		p1 = strstr((char*)mqtt_buff.pdata, "CLOSE");
		if(p1 !=NULL)
		{
			memset(&mqtt_buff, 0, sizeof(usart_buff_t));	
			gprs_status = 0;
			USART_OUT(USART1, "MQTT SERVER CLOSE\r\n");
		}
	}
	
	
	
	
	while(0)
	{	 

		while(1)
		{
			gprs_config();
			if(gprs_status == 255)
			{
				MakeFile_MD5_Checksum(PARK_LOCK_Buffer, 16);			
				break;
			}
		}
	
		
		usart1_recv_data();
		usart2_recv_data();
	
		// 电池信息
		Bat_V =Get_Adc_Average(ADC_Channel_0,10);
		Bat_V=Bat_V*3300/4096;
		Bat_V=Bat_V*88/20;
		Bat_Pre=(Bat_V-5000)*100/2400;
		
//		if(Bat_Pre<20&&Bat_Pre>10&&Bat_Pre_Flag==0)
		if(timer_is_timeout_1ms(timer_batt, 1000*60*60) == 0)
		{	
			Bat_Pre_Flag =  1;
			memset(send_buff, 0, 100);	
			sprintf((char *)send_buff,"%s%s%s","AT+PUBLISH=lockdata/",PARK_LOCK_Buffer,",24,2\r\n");

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
		p1 = strstr((u8*)protocol_buff, "topic: lock/");
		p2 = strstr((u8 *)p1,(u8 *)PARK_LOCK_Buffer);
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
				
				sprintf((char *)send_buff,"%s%s%s","AT+PUBLISH=lockback/",(char *)PARK_LOCK_Buffer,",44,2\r\n");
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
				sprintf((char *)send_buff,"%s%s%s","AT+PUBLISH=lockback/",(char *)PARK_LOCK_Buffer,",44,2\r\n");
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
		if(Lock_Close == 1)
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
				
				sprintf((char *)send_buff,"%s%s%s","AT+PUBLISH=lockback/",(char *)PARK_LOCK_Buffer,",44,2\r\n");
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
				sprintf((char *)send_buff,"%s%s%s","AT+PUBLISH=lockback/",(char *)PARK_LOCK_Buffer,",44,2\r\n");
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
		if(button_get_value() == 0)
		{
			USART_OUT(USART1, "button_get_value\r\n");
			timer_is_timeout_1ms(timer_close_lock, 0);
			Lock_Close = 1;
			Shaking=1;	
		}
			
		
		//报警器
		p1 = strstr((char*)protocol_buff, "topic: bell/");
		p2 = strstr((char *)p1,(char *)PARK_LOCK_Buffer);
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
				
		if(bell_flag == 1)
		{
			bell_flag = 0;
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
		if(timer_is_timeout_1ms(timer_heartbeat, 1000*60*10) == 0)
		{
			memset(send_buff, 0, 100);
			sprintf((char *)send_buff,"%s%s%s","AT+PUBLISH=lockdata/",PARK_LOCK_Buffer,",1,2\r\n");
	
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
		
		
//		if(timer_is_timeout_1ms(timer_gps_cycle, 1000*60*60*24) == 0)
//		{
//			gps_flag = 1;
//		}
		
		

	}	 
}
 





















