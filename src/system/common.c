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
char *str_picked(char *src_str, char *sub_str1, char *sub_str2, char *dst_str)
{
	char *tmp1 = NULL, *tmp2 = NULL, *str = src_str;
	int i = 0; 
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


char *memcat(void *dest, size_t dest_len, const char *src, size_t src_len)
{

//	memcpy(dest+dest_len, src, src_len);

//	return dest;

}





/////////////////////////////////////////////

int hex_char_to_value(const char ch){
	int result = 0;
	//获取16进制的高字节位数据
	if(ch >= '0' && ch <= '9')
	{
		result = (int)(ch - '0');
	}
	else if(ch >= 'a' && ch <= 'z')
	{
		result = (int)(ch - 'a') + 10;
	}
	else if(ch >= 'A' && ch <= 'Z')
	{
		result = (int)(ch - 'A') + 10;
	}
	else
	{
		result = -1;
	}
	
	return result;
}

char value_to_hex_ch(const int value)
{
	char result = '\0';
	
	if(value >= 0 && value <= 9)
	{
		result = (char)(value + 48); //48为ascii编码的‘0’字符编码值
	}
	else if(value >= 10 && value <= 15)
	{
		result = (char)(value - 10 + 65); //减去10则找出其在16进制的偏移量，65为ascii的'A'的字符编码值
	}
	else
		{
	;
	}

	return result;
}

int str_to_hex(char *ch, char *hex)
{
	int high, low;
	int tmp = 0;
	
	if(ch == NULL || hex == NULL)
	{
		return -1;
	}

	if(strlen(ch) == 0)
	{
		return -2;
	}

	while(*ch)
	{
		tmp = (int)*ch;
		high = tmp >> 4;
		low = tmp & 15;
		*hex++ = value_to_hex_ch(high); //先写高字节
		*hex++ = value_to_hex_ch(low); //其次写低字节
		ch++;
	}
	*hex = '\0';
	return 0;
}

int hex_to_str(char *hex, char *ch)
{
	int high,low;
	int tmp = 0;
	
	if(hex == NULL || ch == NULL)
	{
		return -1;
	}

	if(strlen(hex) %2 == 1)
	{
		return -2;
	}

	while(*hex)
	{
		high = hex_char_to_value(*hex);
		if(high < 0)
		{
			*ch = '\0';
			return -3;
		}
		hex++; //指针移动到下一个字符上
		low = hex_char_to_value(*hex);
		if(low < 0)
		{
			*ch = '\0';
			return -3;
		}
		tmp = (high << 4) + low;
		*ch++ = (char)tmp;
		hex++;
	}
	*ch = '\0';
	return 0;
}




void test1(void)
{
	char ch[1024];
    char hex[1024];
    char result[1024];
    char *p_ch = ch;
    char *p_hex = hex;
    char *p_result = result;
//    printf("please input the string:");
//    scanf("%s",p_ch);

    hex_to_str(p_ch,p_hex);
//    printf("the hex is:%s\n",p_hex);
    hex_to_str(p_hex, p_result);
//    printf("the string is:%s\n", p_result);

}
