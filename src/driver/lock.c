

#include "lock.h"
#include "timer.h"
#include "motor.h"
#include "usart.h"
#include "button.h"


extern uint16_t mqtt_publist_msgid;

extern uint8_t receiveText[24];
extern uint8_t expressText[512];  
extern uint8_t cipherText[512];
extern uint8_t aesKey[16];

extern uint8_t lock_id[17];
extern uint8_t topic_buff[100];
extern uint8_t send_buff[100];




uint8_t lock_open_err_flag = 0;
uint8_t lock_close_err_flag = 0;
uint8_t Lock_Open=0;
uint8_t Lock_Close=0;

uint8_t lock_status = 0;
uint8_t lock_open_time_flag = 0;
uint8_t lock_close_time_flag = 0;


uint8_t bell_flag = 0;
uint8_t shake_flag = 0;



void lock_gpio_init(void)
{
	GPIO_InitTypeDef gpio_init_structure;
	//BELL
	gpio_init_structure.GPIO_Pin = GPIO_Pin_11;
  	gpio_init_structure.GPIO_Speed = GPIO_Speed_2MHz;
	gpio_init_structure.GPIO_Mode = GPIO_Mode_OUT;
	gpio_init_structure.GPIO_PuPd = GPIO_PuPd_NOPULL;	
  	GPIO_Init(GPIOB, &gpio_init_structure);
}



 
void lock_shake_alarm(void)
{
	//�ζ�����
	
	if(1 == button_scan(LOCK_ON) && 1 == button_scan(LOCK_OFF) && shake_flag == 0)
	{
		USART_OUT(USART1, "sharking\r\n");
		BEEP_ON();
	}
	else
	{
		BEEP_OFF();
	}	
}



void lock_hand_close(void)
{
	if(BUTTON_CLICK == button_get_state(HAND_CLOSE_LOCK, 2000))
	{
		USART_OUT(USART1, "button_get_value\r\n");
		timer_is_timeout_1ms(timer_close_lock, 0);
		Lock_Close = 1;
		shake_flag = 1;	
	}
}


void lock_self_checking(void)
{
	if(timer_is_timeout_1ms(timer_lock_self_checking, 1000*60*60*24) == 0)
	{
		
	}
	
}


void lock_close_deal(void)
{
	int mqtt_pub;
	
	//�����߼�
	if(lock_status == 0)
	{
		if(timer_is_timeout_1ms(timer_close_lock, 5000) == 0 && lock_close_time_flag == 0)
		{
			motor_forward();
			lock_close_time_flag = 1;
			lock_close_err_flag = 1;
			USART_OUT(USART1, "Lock_Close timer\r\n");
			
		}
		
		if(LOCK_OFF_READ() == 1 && lock_close_time_flag == 0)	//��������
		{
			motor_reversal();
			USART_OUT(USART1, "BBB lock close\r\n");
		}
		
		if(LOCK_OFF_READ() == 0)	//��������
		{

			motor_stop();	//ֹͣ����
			shake_flag = 0;	
			
			if(lock_close_time_flag == 0)
			{
				lock_close_time_flag = 1;
				USART_OUT(USART1, "BBB lock_stop\r\n");
				memset(topic_buff, 0 ,100);
				memset(expressText, 0 ,512);
				memset(cipherText, 0 ,512);
				
				sprintf((char*)topic_buff, "%s%s", "lockback/", (char*)lock_id);
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
		}
		
		if(LOCK_OFF_READ() == 1 && lock_close_err_flag == 1) //������ʱ����
		{
			lock_close_err_flag = 0;
			motor_stop();	//ֹͣ����
			shake_flag = 0;
			USART_OUT(USART1, "BBB close lock unusual\r\n");
			
			memset(topic_buff, 0 ,100);
			memset(expressText, 0 ,512);
			memset(cipherText, 0 ,512);
			
			sprintf((char*)topic_buff, "%s%s", "lockback/", (char*)lock_id);
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

void lock_open_deal(void)
{
	int mqtt_pub;
	//�����߼�		
	if(lock_status == 1)
	{
		if(timer_is_timeout_1ms(timer_open_lock, 5000) == 0 && lock_open_time_flag == 0)
		{
			lock_open_time_flag = 1;
			lock_open_err_flag = 1;
			motor_reversal();
			USART_OUT(USART1, "Lock_Open timer\r\n");
		}

		if(LOCK_ON_READ()==1 && lock_open_time_flag == 0)
		{
			motor_forward();	//����	
			USART_OUT(USART1, "AAA lock_open\r\n");
		}
		if(LOCK_ON_READ() == 0) //��������
		{

			shake_flag = 0;	
			motor_stop();	//ֹͣ����
			
			if(lock_open_time_flag == 0)
			{
				lock_open_time_flag = 1;
				USART_OUT(USART1, "AAA lock_stop\r\n");
				memset(topic_buff, 0 ,100);
				memset(expressText, 0 ,512);
				memset(cipherText, 0 ,512);
				
				sprintf((char*)topic_buff, "%s%s", "lockback/", (char*)lock_id);
				sprintf(expressText,"{%c%s%c:%s,%c%s%c:%s}",'"',"cmd",'"',"1",'"',"ok",'"',"0");
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
			
		}
		
		if(LOCK_ON_READ() == 1 && lock_open_err_flag == 1)//������ʱ�쳣����
		{
			lock_open_err_flag = 0;

			motor_stop();
			shake_flag = 0;

			memset(topic_buff, 0 ,100);
			memset(expressText, 0 ,512);
			memset(cipherText, 0 ,512);
			sprintf((char*)topic_buff, "%s%s", "lockback/", (char*)lock_id);
			sprintf((char*)expressText, "{%c%s%c:%s,%c%s%c:%s}",'"',"cmd",'"',"1",'"',"ok",'"',"1");
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
	}

}




