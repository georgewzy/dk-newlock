#ifndef _TCPIP_H
#define _TCPIP_H

#include  "ENC28J60.h"
extern unsigned char verify_password(char *str);

extern unsigned char analyse_get_url(char *str);

extern unsigned int print_webpage(unsigned char *buf,unsigned char on_off1,unsigned char on_off2,unsigned char on_off3,unsigned char on_off4);


extern unsigned int simple_server(void);

#endif


