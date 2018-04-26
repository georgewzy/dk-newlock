




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

#include "flash.h"










uint32_t flash_get_sector(uint32_t addr)
{



}



/*
*Function: flash_erase      
*Description: ??????  
*Calls:  ?
*Data Accessed: ?  
*Data Updated: ?    
*Input: 
	start_addr:???????
	sector_sum:????????
*Output: ? 
*Return: void        
*Others:  start_addr????????????,STM32F103C8?????1k   
*/
void flash_page_erase(uint32_t start_addr, uint32_t sector_sum)
{

	FLASH_Status flash_status = FLASH_COMPLETE;
	uint32_t i = 0;	

	FLASH_Unlock();

	FLASH_ClearFlag(FLASH_FLAG_BSY | FLASH_FLAG_EOP  | FLASH_FLAG_WRPERR | FLASH_FLAG_PGAERR );

	for (i=0; (i<sector_sum) && (flash_status==FLASH_COMPLETE); i++)
	{
	//	flash_status = FLASH_ProgramByte(start_addr + (PAGE_SIZE * i));
	}
	
	FLASH_Lock();
	
}

void flash_erase_sector(uint8_t start_sector, uint8_t sector_sum, uint8_t voltage_range)
{
	FLASH_Status flash_status;
	uint8_t end_sector = start_sector + sector_sum;
	uint8_t i = 0;
	
	
	FLASH_Unlock();
	
	FLASH_ClearFlag(FLASH_FLAG_BSY | FLASH_FLAG_EOP | FLASH_FLAG_WRPERR | FLASH_FLAG_PGAERR);
	
	for (i=start_sector; (i<end_sector)&&(flash_status==FLASH_COMPLETE); i++)
	{
//		flash_status = FLASH_EraseSector(i, voltage_range);
	}
	
	FLASH_Lock();	
}

/*
*Function: flash_write_data      
*Description: ?flash??????????size??????? 
*Calls:  ?
*Data Accessed: ?  
*Data Updated: ?    
*Input: 
	addr:???????
	data:?????
	size:???????
*Output: ? 
*Return: ??true  ??FLASH        
*Others: ?    
*/
bool flash_write_data(uint32_t addr, uint8_t *data, uint16_t size)
{
	uint8_t *tmp_data = data;
	uint32_t tmp_addr = addr;	
	
	
	
	DATA_EEPROM_Unlock();
	for (tmp_addr=addr; tmp_addr<(addr + size); tmp_data++, tmp_addr += 2)
	{
//		FLASH_ProgramByte(tmp_addr, *tmp_data);	
		if (*tmp_data != *(uint8_t *)tmp_addr)	 		// ???????? ? ??????? ????
		{
			return false;								// 
		}
	}
	DATA_EEPROM_Lock();
		
	return true;
}


/*
*Function: flash_read_data      
*Description: ?FLASH??????????size???????? 
*Calls: ?
*Data Accessed: ?  
*Data Updated: ?    
*Input: 
	start_addr:???????
	size:????????	
*Output: pdata:?????
*Return: void      0  
*Others: ?    
*/
void flash_read_data(uint32_t addr, uint8_t *pdata, uint32_t size)
{
	uint32_t end_addr = addr + size;
	uint32_t tmp_addr = addr;
	uint32_t i = 0;

	while (tmp_addr < end_addr) 
	{
		
		pdata[i++] = (*(uint32_t*)tmp_addr);
		tmp_addr++;
	}
}

/*
*Function: flash_write_byte      
*Description: ?FLASH????????????????
*Calls: ?
*Data Accessed: ?  
*Data Updated: ?    
*Input: 
	addr:???????
	ch:?????
*Output: ? 
*Return: ??true  ??FALSH       
*Others: ?    
*/
bool flash_write_byte(uint32_t addr, uint8_t ch)
{
	
	FLASH_Unlock();
	
//	FLASH_ProgramByte(addr, ch);

	FLASH_Lock();
		
	return true;
}


/*
*Function: flash_read_byte      
*Description: ?FLASH???????????????? 
*Calls:   
*Data Accessed: ?  
*Data Updated: ?    
*Input: addr:???????    
*Output: ? 
*Return: value:?????        
*Others: ?    
*/
uint8_t flash_read_byte(uint32_t addr)
{
	uint8_t ch;

	ch = *(uint32_t*)addr;

	return ch;
}



/*------------------------------------------------------------
 Func: EEPROM???????
 Note:
-------------------------------------------------------------*/
void eeprom_erase_data(uint32_t addr, uint16_t length)
{
	uint8_t *wAddr;
	uint16_t i = 0;
	volatile FLASH_Status flash_status = FLASH_COMPLETE;
	
	if(addr > EEPROM_BASE_ADDR+EEPROM_BYTE_SIZE)
	{
		return;
	}
	DATA_EEPROM_Unlock();
	for(i=0; i<length; i++)
	{
		flash_status = DATA_EEPROM_FastProgramByte(addr+i, 0);
		while(flash_status != FLASH_COMPLETE);
	}
	
	DATA_EEPROM_Lock();
}

void eeprom_write_data(uint32_t addr, uint8_t *buffer, uint16_t length)
{
	uint8_t *wAddr;
	uint16_t i = 0;
	volatile FLASH_Status flash_status = FLASH_COMPLETE;
	
	if(addr > EEPROM_BASE_ADDR+EEPROM_BYTE_SIZE)
	{
		return ;
	}
	DATA_EEPROM_Unlock();
	for(i=0; i<length; i++)
	{
		flash_status = DATA_EEPROM_FastProgramByte(addr+i, *(buffer+i));
		while(flash_status != FLASH_COMPLETE);
	}
	
	DATA_EEPROM_Lock();
}


void eeprom_read_data(uint32_t addr, uint8_t *pdata, uint32_t size)
{
	uint32_t end_addr = addr + size;
	uint32_t tmp_addr = addr;
	uint32_t i = 0;

	while (tmp_addr < end_addr) 
	{
		pdata[i++] = (*(uint32_t*)tmp_addr);
		tmp_addr++;
	}

}

uint8_t eeprom_read_bytes(uint32_t addr)
{
	uint8_t ch;
	if(addr > EEPROM_BASE_ADDR+EEPROM_BYTE_SIZE)
	{
		return 0;
	}
	ch = *(uint32_t*)addr;

	return ch;
}






