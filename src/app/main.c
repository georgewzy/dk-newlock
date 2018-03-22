


#include <string.h>		
#include <stdlib.h>
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
#include "usart.h"
#include "flash.h"
#include "motor.h"


extern usart_buff_t mqtt_buff;
extern uint16_t mqtt_publist_msgid;
extern uint8_t gprs_status;
extern u8 usart2_buff[512];
extern u16 usart2_cnt; 





u8 receiveText[24];
u8 expressText[512];  
u8 cipherText[512];
u8 aesKey[16];



//响铃
u8 bell_flag=0;


//震动
u8 shake_flag = 0;

//电压
u16 Bat_V;
u16 Bat_Pre;
u8 Bat_Pre_Flag=0;


u8 *p1;
u8 *p2;



u8 lock_open_err_flag = 0;
u8 lock_close_err_flag = 0;
u8 Lock_Open=0;
u8 Lock_Close=0;


u8 protocol_buff[512] = {0};





u8 mqtt_keep_alive_flag = 0;
u8 mqtt_keep_alive_err_cnt = 0;


u8 PARK_LOCK_Buffer[17] = {0};
u8 topic_buff[100] = {0};
u8 send_buff[100] = {0};


void shake_alarm(void)
{
	//晃动报警
	
	if(1 == LOCK_ON_READ() && 1 == LOCK_OFF_READ() && shake_flag == 0)
	{
		USART_OUT(USART1, "Sharking\r\n");
		BEEP_ON();
	}
	else
	{
		BEEP_OFF();
	}	
}

void lock_hand_close(void)
{
	if(BUTTON_CLICK == button_get_state(SW1, 2000))
	{
		USART_OUT(USART1, "button_get_value\r\n");
		timer_is_timeout_1ms(timer_close_lock, 0);
		Lock_Close = 1;
		shake_flag = 1;	
	}
}

void heartbeat(uint32_t ms)
{
	int mqtt_pub = 0;
	u8 heartbeat_buff[2] = {0};
	
	memset(topic_buff, 0, 100);	
	memset(heartbeat_buff, 0, 2);
	
	if(timer_is_timeout_1ms(timer_heartbeat, ms) == 0)
	{
		
		sprintf((char *)topic_buff,"%s%s","lockdata/", PARK_LOCK_Buffer);
		heartbeat_buff[0] = 0x30;

		USART_OUT(USART1, "heartbeat=%s\r\n", topic_buff);
		mqtt_pub = mqtt_publist(topic_buff, heartbeat_buff, 1, 2, mqtt_publist_msgid);
		if(mqtt_pub == 1)
		{
			USART_OUT(USART1, "heartbeat mqtt_publist ok\r\n");
		}	

	}
	
}

void dev_to_srv_batt_voltage(uint32_t ms)
{
	int mqtt_pub;
	
	if(timer_is_timeout_1ms(timer_batt, ms) == 0)
	{
		Bat_V = adc_get_average(ADC_Channel_0, 10);
		Bat_V = Bat_V*3300/4096;
		Bat_V = Bat_V*88/20;
		Bat_Pre = (Bat_V-5000)*100/2400;

		memset(topic_buff, 0, 100);	
		memset(expressText, 0, 512);
		memset(cipherText, 0 ,512);	
		
		sprintf((char *)topic_buff, "%s%s", "lockdata/", (char*)PARK_LOCK_Buffer);	
		sprintf((char *)expressText, "{%c%s%c:%s}",'"',"battery",'"',"20");
		AES_Encrypt((char *)expressText, (char*)cipherText, (char*)aesKey);
		
		USART_OUT(USART1, "expressText=%s\r\n", expressText);
		USART_OUT(USART1, "aesKey=%s\r\n", aesKey);
		USART_OUT(USART1, "cipherText=%s\r\n", cipherText);
		
		mqtt_pub = mqtt_publist(topic_buff, expressText, 24, 2, mqtt_publist_msgid);
		if(mqtt_pub == 1)
		{
			USART_OUT(USART1, "dev_to_srv_batt mqtt_publist ok\r\n");
		}	
	}
}



void close_lock_deal(void)
{
	int mqtt_pub;
	
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
		if(LOCK_ON_READ()==0 && LOCK_OFF_READ() == 1)	//正常关锁
		{
			lock_close();
			USART_OUT(USART1, "BBB lock close\r\n");
		}
		
		if(LOCK_OFF_READ() == 0 )	//正常关锁
		{
			Lock_Close = 0;
			lock_stop();	//停止运行
			shake_flag = 0;
			USART_OUT(USART1, "BBB lock_stop\r\n");
			
			memset(topic_buff, 0 ,100);
			memset(expressText, 0 ,512);
			memset(cipherText, 0 ,512);
			
			sprintf((char*)topic_buff, "%s%s", "lockback/", (char*)PARK_LOCK_Buffer);
			sprintf((char*)expressText, "{%c%s%c:%s,%c%s%c:%s}",'"',"cmd",'"',"2",'"',"ok",'"',"0");
			AES_Encrypt((char*)expressText, cipherText, aesKey);
			
			USART_OUT(USART1, "send_buff=%s\r\n", topic_buff);
			USART_OUT(USART1, "expressText=%s\r\n", expressText);
			USART_OUT(USART1, "cipherText=%s\r\n", cipherText);
			
			mqtt_pub = mqtt_publist(topic_buff, cipherText, 44, 2, mqtt_publist_msgid);
			if(mqtt_pub == 1)
			{
				USART_OUT(USART1, "mqtt_publist ok\r\n");
			}
		}
		
		
		if(LOCK_OFF_READ() == 0 && lock_close_err_flag == 1) //关锁超时处理
		{
			lock_close_err_flag = 0;
			Lock_Close = 0;
			lock_stop();	//停止运行
			shake_flag = 0;
			USART_OUT(USART1, "close lock unusual\r\n");
			
			memset(topic_buff, 0 ,100);
			memset(expressText, 0 ,512);
			memset(cipherText, 0 ,512);
			
			sprintf((char*)topic_buff, "%s%s", "lockback/", (char*)PARK_LOCK_Buffer);
			sprintf((char*)expressText, "{%c%s%c:%s,%c%s%c:%s}",'"',"cmd",'"',"2",'"',"ok",'"',"1");
			AES_Encrypt((char*)expressText, (char*)cipherText, (char*)aesKey);
		
			USART_OUT(USART1, "send_buff=%s\r\n", topic_buff);
			USART_OUT(USART1, "expressText=%s\r\n", expressText);
			USART_OUT(USART1, "cipherText=%s\r\n", cipherText);
			
			mqtt_pub = mqtt_publist(topic_buff, expressText, 44, 2, mqtt_publist_msgid);
			if(mqtt_pub == 1)
			{
				USART_OUT(USART1, "mqtt_publist ok\r\n");
			}
		}		
	}
}

void open_lock_deal(void)
{
	int mqtt_pub;
	//开锁逻辑		
	if(Lock_Open == 1)
	{
		if(timer_is_timeout_1ms(timer_open_lock, 5000) == 0)
		{
			lock_open_err_flag = 1;
			lock_close();
			USART_OUT(USART1, "Lock_Open timer\r\n");
		}
		USART_OUT(USART1, "Lock_Open\r\n");
		if(LOCK_OFF_READ()==0 && LOCK_ON_READ()==1)
//		if(BUTTON_CLICK == button_get_state(LOCK_ON, 2000) && BUTTON_UP == button_get_state(LOCK_OFF, 2000))
		{
			lock_open();	//开锁	
			USART_OUT(USART1, "AAA lock_open\r\n");
		}
		if(LOCK_ON_READ() == 0) //正常开锁
		{
			USART_OUT(USART1, "AAA lock_stop\r\n");
			
			Lock_Open = 0;
			lock_stop();	//停止运行
			shake_flag = 0;
			
			memset(topic_buff, 0 ,100);
			memset(expressText, 0 ,512);
			memset(cipherText, 0 ,512);
			
			sprintf((char *)topic_buff, "%s%s", "lockback/", (char *)PARK_LOCK_Buffer);
			sprintf(expressText,"{%c%s%c:%s,%c%s%c:%s}",'"',"cmd",'"',"1",'"',"ok",'"',"0");
			AES_Encrypt((char *)expressText, cipherText, aesKey);
			
			USART_OUT(USART1, "send_buff=%s\r\n", topic_buff);
			USART_OUT(USART1, "expressText=%s\r\n", expressText);
			USART_OUT(USART1, "cipherText=%s\r\n", cipherText);

			mqtt_pub = mqtt_publist(topic_buff, cipherText, 44, 2, mqtt_publist_msgid);
			if(mqtt_pub == 1)
			{
				USART_OUT(USART1, "mqtt_publist ok\r\n");
			}
				
		}
		
		if(LOCK_ON_READ()==0 && LOCK_OFF_READ()==1 && lock_open_err_flag == 1)//开锁超时异常处理
		{
			lock_open_err_flag = 0;
			Lock_Open = 0;
			lock_stop();
			shake_flag = 0;

			memset(topic_buff, 0 ,100);
			memset(expressText, 0 ,512);
			memset(cipherText, 0 ,512);
			sprintf((char *)topic_buff, "%s%s", "lockback/", (char *)PARK_LOCK_Buffer);
			sprintf((char *)expressText, "{%c%s%c:%s,%c%s%c:%s}",'"',"cmd",'"',"1",'"',"ok",'"',"1");
			AES_Encrypt((char *)expressText, cipherText, aesKey);
			
			USART_OUT(USART1, "send_buff=%s\r\n", topic_buff);
			USART_OUT(USART1, "expressText=%s\r\n", expressText);
			USART_OUT(USART1, "cipherText=%s\r\n", cipherText);
				
			mqtt_pub = mqtt_publist(topic_buff, cipherText, 44, 2, mqtt_publist_msgid);
			if(mqtt_pub == 1)
			{
				USART_OUT(USART1, "mqtt_publist ok\r\n");
			}
		}
	}

}




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
	int rc = 0;
	u16 j,i,rxlen;
	u16 aa = 0;
	u8 upload=0; 
	u8 *ret;
	uint8_t status = 0;
	uint8_t topic[50] = {0};
	uint8_t payload[100];
	int payloadlen = 0;
	int mqtt_pub;
	int mqtt_sub;
	u32 id = 0;
	bsp_init();
                         
	USART_OUT(USART1, "uart1 is ok\r\n");

	
	while(1)
	{
		if(button_scan(SW2A) == 0 && button_scan(SW2B))
		{
			
		}
		usart1_recv_data();
		p1 = strstr((u8*)protocol_buff, "lockid=");
		if(p1 != NULL)
		{
			memcpy((char*)PARK_LOCK_Buffer ,(char *)(p1+7), 16);		
			eeprom_write_data(0x08080000, PARK_LOCK_Buffer, 16);
			eeprom_read_data(0x08080000, PARK_LOCK_Buffer, 16);
			MakeFile_MD5_Checksum(PARK_LOCK_Buffer, 16);
			USART_OUT(USART1, "wang11=%s\r\n", PARK_LOCK_Buffer);
			sprintf((char*)expressText, "{%c%s%c:%s}",'"',"battery",'"',"20");
			
			USART_OUT(USART1, "expressText=%s\r\n", expressText);
			
			break;
		}	
		
		eeprom_read_data(0x08080000, PARK_LOCK_Buffer, 16);
		MakeFile_MD5_Checksum(PARK_LOCK_Buffer, 16);
		USART_OUT(USART1, "PARK_LOCK_Buffer=%s\r\n", PARK_LOCK_Buffer);
		USART_OUT(USART1, "aesKey=%s\r\n", aesKey);
		break;
	}
	
	while(1)
	{
		
		gprs_init_task();

		usart1_recv_data();
		usart2_recv_data();
		
		mqtt_sub = mqtt_subscribe(topic, payload, &payloadlen);
		if(mqtt_sub == 1)
		{
			timer_is_timeout_1ms(timer_heartbeat, 0);
			USART_OUT(USART1, "AAAA=%s=%s\r\n", payload, topic);
			
		}
		
		if(strncmp((char*)topic, (char*)"lock/", 5)==0)
		{	
			USART_OUT(USART1, "topic==%s\r\n", topic);
					
			timer_is_timeout_1ms(timer_heartbeat, 0);

			memset(receiveText , 0, 24);
			memset(expressText , 0, 512);
			
			strncpy((char*)receiveText, (char*)payload, payloadlen);
			AES_Decrypt(expressText, receiveText, aesKey);
			
			USART_OUT(USART1, "receiveText=%s\r\n", receiveText);
			USART_OUT(USART1, "expressText=%s\r\n", expressText);	
			if(*expressText == 0x31)
			{
				timer_is_timeout_1ms(timer_open_lock, 0);
				shake_flag = 1;
				Lock_Open = 1;
				USART_OUT(USART1, "Lock_Open11111\r\n");
			
			}
			else if(*expressText == 0x32)
			{
				timer_is_timeout_1ms(timer_close_lock, 0);
				shake_flag = 1;
				Lock_Close = 1;
				USART_OUT(USART1, "Lock_Close11111\r\n");
			}
			else if(*expressText == 0x30)
			{
				lock_stop();	//停止运行;
			}
								
			payloadlen = 0;
			memset(topic, 0, 50);
			memset(payload, 0, 100);	
		}
		
	
		if(strncmp((char*)topic,(char *)"bell/", 5)==0)
		{
			timer_is_timeout_1ms(timer_heartbeat, 0);
			USART_OUT(USART1, "bell\r\n");

			BEEP_ON();
			timer_delay_1ms(100);
			BEEP_OFF();
			
			payloadlen = 0;
			memset(topic, 0, 50);
			memset(payload, 0, 100);
		}
				
		
		if((timer_is_timeout_1ms(timer_mqtt_keep_alive, 1000*120) == 0) || (mqtt_keep_alive_flag == 1))
		{
			USART_OUT(USART1, "mqtt_keep_alive\r\n");
			rc = mqtt_keep_alive(1);
			if(rc == 1)
			{
				mqtt_keep_alive_flag = 0;
				mqtt_keep_alive_err_cnt = 0;
			}  
			else
			{
				mqtt_keep_alive_flag = 1;
				mqtt_keep_alive_err_cnt++;
				if(mqtt_keep_alive_err_cnt > 5)
				{
					gprs_status = 0;
					mqtt_keep_alive_flag = 0;
					mqtt_keep_alive_err_cnt = 0;
					
					USART_OUT(USART1, "mqtt_keep_alive_err\r\n");
				}
			}
		}
		
		
		open_lock_deal();
		close_lock_deal();
		
		lock_hand_close();
	
		dev_to_srv_batt_voltage(1000*60*60);	

		heartbeat(1000*60*5);
//		shake_alarm();
		
	}

}
 





















