#ifndef _GU906_H_
#define _GU906_H_


#include "bsp.h"  
#include <stdbool.h>

#define GU906GSM_EN   0    //是否开启短信功能
#define GPRSCSQ       18   //信号强度，在使用GPRS功能时，最低要求信号强度不低于18

#define _ATOK          0  //执行成功
#define _ATERROR      -1  //执行错误
#define _ATOTIME      -2  //执行超时
#define _LINKNOT      -3  //掉线了




#define DEBUG_EN  0



#define MAXRECVBUFF  		512

#define AT                  "AT\r\n"                                        //测试命令
#define ATE(x)              ((x)?("ATE1\r\n"):("ATE0\r\n"))                 //开回显、关回显
#define ATESIM              "AT+ESIMS?\r\n"                                 //检查卡是否存在
#define ATCNMI              "AT+CNMI=2,1\r\n"                               //设置这组参数来了新信息存储起来 

#define ATCMGD              "AT+CMGD=1,4\r\n"                               //删除当前存储器中的信息
#define ATCMGF              "AT+CMGF=1\r\n"                                 //0设置短消息为PDU模式，1设置短消息为

#define ATCSMP              "AT+CSMP=17,167,2,25\r\n"                       //设置文本模式的参数
#define ATUCS2              "AT+CSCS=\"UCS2\"\r\n"                          //设置为UCS2编码字符集
#define ATGB2312            "AT+CSCS=\"GB2312\"\r\n"                        //设置为GB2312编码
#define ATATD               "ATD%s;\r\n"                                    //对指定手机拨号
#define ATATH               "ATH\r\n"                                       //挂机
#define ATGSM               "AT+CSCS=\"GSM\"\r\n"                           //设置GSM字符集  
#define ATCPMS              "AT+CPMS=\"SM\",\"SM\",\"SM\"\r\n"              //设置短信存储单元为SIM卡 
#define ATCSQ               "AT+CSQ\r\n"                                    //获取信号强度
#define ATCREG              "AT+CREG?\r\n"                                  //确保模块注册到GSM网络
#define ATCIICR             "AT+CIICR\r\n"                                  //让模块激活GPRS网络，在需要反复建立 TCP 链接的场合可提高速度    
#define ATCIPSTARTOK        "AT+CIPSTART?\r\n"                              //查询当前模块是否有网络连接 
#define ATCIPCLOSE          "AT+CIPCLOSE=0\r\n"                             //关闭当前网络
#define ATCIPSCONT(x)       ((x)?("AT+CIPSCONT=0,\"%s\",\"%s\",%d,2")\
                                :("AT+CIPSCONT\r\n"))                       //保存设置   
#define ATCIPSTART          "AT+CIPSTART=\"%s\",\"%s\",%d\r\n"              //设置TCP连接的IP和端口号
#define ATCIPMUX            "AT+CIPMUX=0\r\n"                               //关闭网络连接
#define ATCIPMODE(x)        ((x)?("AT+CIPMODE=1,0\r\n")\
                                :("AT+CIPMODE=0,0\r\n"))                    //数据透传模式、非透传
#define ATCIPCFG(x)         ((x)?("AT+CIPCFG=1,50,0\r\n")\
                                :("AT+CIPCFG=0,50,0\r\n"))                  //自动启动连接命令                      
#define ATCIPPACK(x)        ((x)?("AT+CIPPACK=1,\"4C4F47494E3A31303031\"\r\n")\
                                :("AT+CIPPACK=0,\"0102A0\"\r\n"))           //设置注册包、心跳包设置
#define ATCIPSEND(x)        ((x)?("AT+CIPSEND=%d\r\n")\
                                :("AT+CIPSEND\r\n"))                        //设置发送的数据长度
								
#define ATCGMR              "AT+CGMR\r\n"                                   //获取基站信息
#define ATCMGS              "AT+CMGS=\"%s\"\r\n"                            //设置需要发送短信的手机号
#define ATCMGR              "AT+CMGR=%s\r\n"                                //设置要读取短信的位置
#define ATCSTT              "AT+CSTT=\"CMNET\"\r\n"                         //账户配置
#define ATCIPSCONT_C        "AT+CIPSCONT?\r\n"                              //查看透传连接情况            
#define GPRSSEND            0x1A                                 
#define CLOSEDTU            "+++"                                            //关闭透传
#define OPENDTU             "ATO0\r\n"                                      //重新进入透传









struct Gprs_Config{
	uint8_t *server_ip;     //服务器ip
	uint32_t server_port;   //服务器端口号
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

s8  gu906_init(void);
s8  gu906_Module_State(void);
s8  gu906_TCP_Socket(struct Gprs_Config *GprsCon);
s8  gu906_DTU_Socket(struct Gprs_Config *GprsCon);
s8  gu906_GPRS_write(char* pdat, int len);
uint32_t gu906_GPRS_read(char *pout, int len);


#endif