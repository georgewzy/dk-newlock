#ifndef _GU906_H_
#define _GU906_H_
#include "sys.h"

#define GU906GSM_EN   1    //是否开启短信功能 
#define GPRSCSQ       18   //信号强度,在使用GPRS功能时，最低要求信号强度不得低于18

#define _ATOK          0  //执行成功
#define _ATERROR      -1  //执行错误
#define _ATOTIME      -2  //执行超时
#define _LINKNOT      -3  //掉线了

struct Gprs_Config{
	u8 *server_ip;     //服务器IP
	u32 server_port;   //服务器端口
};

#if GU906GSM_EN
//根据实际内存情况而定
struct user_simdata{
	char phone[15];  //用户手机号
	char dev[50];    //用户使用的设备
	char date[50];   //接收时间
	char data[200];  //接收的数据
};
extern struct user_simdata sim;
s8 GU906_Read_UserSMS(void);
s8 GU906_Chinese_text(char *phone,char* pmsg);
#endif

s8  GU906_init(void);
s8  GU906_Module_State(void);
s8  GU906_TCP_Socket(struct Gprs_Config *GprsCon);
s8  GU906_DTU_Socket(struct Gprs_Config *GprsCon);
s8  GU906_GPRS_write(char* pdat, int len);
u32 GU906_GPRS_read(char *pout, int len);

s8  GU906_make_phone(char *phone);
s8  GU906_Answer_Phone(u32 Delay);
s8  GU906_end_phone(void);
s8  GU906_DtuOrAT(u8 type);


#endif











