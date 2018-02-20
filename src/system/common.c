/*************************************************
  Copyright (C),  RoadLighting Tech. Co., Ltd.
  File name: common.c     
  Author: George      Version: 1.0       Date: 2015/07/1
  Description: 存放程序的一些公共的函数   
  Others: 无        
  Function List:  
    1. delay
	2. system_reset
	3. str_picked
	4. mem_picked
	5. mem_str
	6. itoa
  History:                        
    1. Date: 2015/08/31
       Author: George
       Modification:
    2. 
*************************************************/
#include <string.h>
#include "common.h"
#include "bsp.h"


/*
*Function: delay      
*Description: 延时函数  
*Calls: 无  
*Data Accessed: 无  
*Data Updated: 无    
*Input: count：要延时的时间
*Output: 无
*Return: 无    
*Others: 72MHz时钟, 每个count/100us    
*/
void delay(uint32_t count)
{
	while(count--);
}


/*
*Function: system_reset      
*Description: system reset  
*Calls: 无  
*Data Accessed: 无  
*Data Updated: 无    
*Input: 无
*Output: 无
*Return: 无     
*Others: 无    
*/
void system_reset(void) 
{ 
	__set_FAULTMASK(1); 
	NVIC_SystemReset();  
} 

/*
*Function: system_reset      
*Description: system reset  
*Calls: 无  
*Data Accessed: 无  
*Data Updated: 无    
*Input: 无
*Output: 无
*Return: 无     
*Others: 无    
*/
uint8_t *str_picked(uint8_t *src_str, uint8_t *sub_str1, uint8_t *sub_str2, uint8_t *dst_str)
{
	uint8_t *tmp1 = NULL, *tmp2 = NULL, *str = src_str;
	uint8_t i = 0; 
	int index = 0;
				
	tmp1 = (uint8_t*)strstr((char*)str, (char*)sub_str1);

	if ((tmp1 == NULL) || (str == NULL))
	{
		return NULL;
	}

	for(i=0; i<strlen((char*)sub_str1); i++)
	{
		tmp1++;
	}

	tmp2 = (uint8_t*)strstr((char*)tmp1, (char*)sub_str2);
	if (tmp2 == NULL)
	{
		return NULL;
	}

	while (tmp1 != tmp2)
	{
		dst_str[index] = *tmp1++;
		index++;
	}
	dst_str[index] = '\0';
	
	return dst_str;
}


/*
*Function: system_reset      
*Description: system reset  
*Calls: 无  
*Data Accessed: 无  
*Data Updated: 无    
*Input: 无
*Output: 无
*Return: 无     
*Others: 无    
*/
uint8_t *mem_picked(uint8_t *src_str, uint8_t *sub_str, uint16_t len, uint8_t *dst_str)
{
	uint8_t *tmp1 = NULL, *tmp2 = NULL, *str = src_str;
	uint8_t i = 0; 
	
	tmp1 = (uint8_t*)mem_str((char*)str, 512, (char*)sub_str);
	if ((tmp1 == NULL) || (str == NULL))
	{
		return NULL;
	}

	for(i=0; i<strlen((char*)sub_str); i++)
	{
		tmp1++;
	}

	memcpy(dst_str, tmp1, len);
	
	return str;
}


/*
*Function: mem_str      
*Description:  从内存地址查找字符串
*Calls: 无  
*Data Accessed: 无  
*Data Updated: 无    
*Input: 
	mem：内存地址
	mem_len：内存地址的长度
	sub_str：子字符串
*Output: 无
*Return: 返回查到的子串的位置    
*Others: 无    
*/
char *mem_str(char *mem, int mem_len, char *sub_str)
{
	int sub_len = 0;
	int i;
	char *cur_pos = mem;
	int last_pos = 0;
	
	if (mem == NULL || mem_len <= 0 || sub_str == NULL)
	{
		return NULL;
	}
	
	if (*sub_str == '\0')
	{
		return NULL;
	}
	
	sub_len = strlen(sub_str);
	last_pos = mem_len - sub_len + 1;
	
	for (i=0; i<last_pos; i++)
	{
		if (*cur_pos == *sub_str)
		{
			if (memcmp(cur_pos, sub_str, sub_len) == 0)
			{
				return cur_pos;
			}
		}
		cur_pos++;
	}

	return NULL;
}

/*
*Function: itoa      
*Descriptionsystem reset  
*Calls: 无  
*Data Accessed: 无  
*Data Updated: 无    
*Input: 无
*Output: 无
*Return: 无     
*Others: 无    
*/
char *itoa(int value, char *string, int radix)
{
    int     i, d;
    int     flag = 0;
    char    *ptr = string;

    if (radix != 10)
    {
        *ptr = 0;
        return string;
    }

    if (!value)
    {
        *ptr++ = 0x30;
        *ptr = 0;
        return string;
    }


    if (value < 0)
    {
        *ptr++ = '-';
		
        value *= -1;
    }

    for (i = 10000; i > 0; i /= 10)
    {
        d = value / i;

        if (d || flag)
        {
            *ptr++ = (char)(d + 0x30);
            value -= (d * i);
            flag = 1;
        }
    }

    *ptr = 0;

    return string;
} 










