#ifndef __CONFIG_H
#define __CONFIG_H

#define PL2303 //当定义了PL2303，串口1是使用USB接口，否则是使用GU900

void RCC_Configuration(void);
int ParsingData(char *pstr,char *dev,char *port,char *pout);
int SizRightWrong(char *pin);

#endif



