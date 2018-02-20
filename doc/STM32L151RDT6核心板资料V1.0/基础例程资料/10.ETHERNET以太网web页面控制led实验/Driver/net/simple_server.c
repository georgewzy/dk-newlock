#include <string.h>
#include "enc28j60.h"
#include "ip_arp_udp_tcp.h"
#include "net.h"
#include "hal.h"
#include "led.h"
//#include "uart.h"
#include "simple_server.h"
#include "general_type.h"
#include "temp.h"
#define PSTR(s) s

unsigned char i_1=0,i_2=0,i_3=0,i_4=0;
unsigned char led1=0,led2=0,led3=0,led4=0,led=0;

unsigned char  temp[]={"0123456789ABCDEF"};
unsigned char  temp1[]={"0123456789"}; 
u16 temp_1;
unsigned char temp_2[5];
unsigned char temp_3[7];

//extern void delay_ms(unsigned char ms);

// please modify the following two lines. mac and ip have to be unique
// in your local area network. You can not have the same numbers in
// two devices:
static unsigned char mymac[6] = {0x08,0xD4,0x0C,0x00,0x00,0x01};
//static unsigned char myip[4] = {192,168,0,100};
// base url (you can put a DNS name instead of an IP addr. if you have
// a DNS server (baseurl must end in "/"):
static unsigned int mywwwport =80; // listen port for tcp/www (max range 1-254)
// or on a different port:
//static char baseurl[]="http://10.0.0.24:88/";
//static unsigned int mywwwport =88; // listen port for tcp/www (max range 1-254)
//
static unsigned int myudpport =1200; // listen port for udp
// how did I get the mac addr? Translate the first 3 numbers into ascii is: TUX

#define BUFFER_SIZE 1500//400
static unsigned char buf[BUFFER_SIZE+1];
static unsigned char BUF;
// the password string (only the first 5 char checked), (only a-z,0-9,_ characters):
static char password[]="123456"; // must not be longer than 9 char
#define LED1_ON()  LED1_RUN(1);	             
#define LED2_ON()  LED2_RUN(1);
#define LED3_ON()  LED3_RUN(1);
#define LED4_ON()  LED4_RUN(1);

#define LED1_OFF() LED1_RUN(0);  
#define LED2_OFF() LED2_RUN(0);  
#define LED3_OFF() LED3_RUN(0);  
#define LED4_OFF() LED4_RUN(0);  
//
unsigned char verify_password(char *str)
	{
	// the first characters of the received string are
	// a simple password/cookie:
	//用 法: int strncmp(char *str1, char *str2, int maxlen); 　　
	//说明:比较字符串str1和str2的大小，如果str1小于str2，返回值就<0，反之如果str1大于str2，返回值就>0，
	//如果str1等于str2，返回值就=0，maxlen指的是str1与str2的比较的字符数。
	//此函数功能即比较字符串str1和str2的前maxlen个字符。 
	if (strncmp(password,str,5)==0)
		{
	    return(1);
		}
	return(0);
	}

// takes a string of the form password/commandNumber and analyse it
// return values: -1 invalid password, otherwise command number
//                -2 no command given but password valid
unsigned char analyse_get_url(char *str)
	{
	unsigned char i=0;
	if (verify_password(str)==0)	 //判断密码是否正确，函数返回0说明密码正确即verify_password(str)==0
		{
			return(-1);	////当验证是密码且是正确时就将-1返回跳出函数，不是密码或者密码错误就往下执行
		}
	// find first "/"
	// passw not longer than 9 char:
	while(*str && i<10 && *str >',' && *str<'{')
		{
	        if (*str=='/')
				{
		            str++;
		            break;
	        	}
	        i++;
	        str++;
		}
	if (*str < 0x3a && *str > 0x2f)	//当是ASCII码时将其返回
		{
        // is a ASCII number, return it
        return(*str-0x30);
		}
	return(-2);
	}

// prepare the webpage by writing the data to the tcp send buffer
/**********************************************************************
功能：打印页面代码
***********************************************************************/
unsigned int print_webpage(unsigned char *buf,unsigned char on_off1,unsigned char on_off2,unsigned char on_off3,unsigned char on_off4)
	{
    unsigned int plen,j;
	unsigned char macmac[17];
	plen=fill_tcp_data_p(buf,0,PSTR("HTTP/1.0 200 OK\r\nContent-Type: text/html\r\nPragma: no-cache\r\n\r\n"));
	plen=fill_tcp_data_p(buf,plen,PSTR("<META HTTP-EQUIV=Refresh content=5>"));//页面开始处
	plen=fill_tcp_data_p(buf,plen,PSTR("<form action=""/"" method=get>"));//页面开始处
	plen=fill_tcp_data_p(buf,plen,PSTR("<font size=7><center>基于STM32的以太网通信系统设计</center></font><br><p><p>"));
	plen=fill_tcp_data_p(buf,plen,PSTR("<marquee behavior=alternate scrollamount=15>系统资源：STM32F103RBT6,128K Flash, 64K SRAM</marquee><P>"));
	plen=fill_tcp_data_p(buf,plen,PSTR("<marquee behavior=alternate>嵌入式服务器IP:192.168.1.100 MAC:04-02-35-00-00-01</marquee><P>"));
    plen=fill_tcp_data_p(buf,plen,PSTR("<p><center><a href="));
    plen=fill_tcp_data(buf,plen,"http://");
    plen=fill_tcp_data(buf,plen,"192.168.1.108");
    plen=fill_tcp_data(buf,plen,"/?led=");
    if (led1==0x31)
    {
        plen=fill_tcp_data_p(buf,plen,PSTR("5><font size=5>关闭LED1:</font></a>"));
        plen=fill_tcp_data_p(buf,plen,PSTR("<font size=5 color=\"#00FF00\">  LED1亮</font></center>"));
    }
    else if (led1==0x35|led1==0) 
    {
        plen=fill_tcp_data_p(buf,plen,PSTR("1><font size=5>点亮LED1:</font></a>"));
        plen=fill_tcp_data_p(buf,plen,PSTR("<font size=5>  LED1灭</font></center>"));
    }

    plen=fill_tcp_data_p(buf,plen,PSTR("<p><center><a href="));
    plen=fill_tcp_data(buf,plen,"http://");
    plen=fill_tcp_data(buf,plen,"192.168.1.108");
	plen=fill_tcp_data(buf,plen,"/?led=");
    if (led2==0x32)
    {
        plen=fill_tcp_data_p(buf,plen,PSTR("6><font size=5>关闭LED2:</font></a>"));
        plen=fill_tcp_data_p(buf,plen,PSTR("<font size=5 color=\"#00FF00\">  LED2亮</font></center>"));
    }
    else if(led2==0x36|led2==0) 
    {
        plen=fill_tcp_data_p(buf,plen,PSTR("2><font size=5>点亮LED2:</font></a>"));
        plen=fill_tcp_data_p(buf,plen,PSTR("<font size=5>  LED2灭</font></center>"));
    }

    plen=fill_tcp_data_p(buf,plen,PSTR("<p><center><a href="));
    plen=fill_tcp_data(buf,plen,"http://");
    plen=fill_tcp_data(buf,plen,"192.168.1.108");
    plen=fill_tcp_data(buf,plen,"/?led=");

    if (led3==0x33)
    {
        plen=fill_tcp_data_p(buf,plen,PSTR("7><font size=5>关闭LED3:</font></a>"));
        plen=fill_tcp_data_p(buf,plen,PSTR("<font size=5 color=\"#00FF00\">  LED3亮</font></center>"));
    }
    else if(led3==0x37|led3==0) 
    {
        plen=fill_tcp_data_p(buf,plen,PSTR("3><font size=5>点亮LED3:</font></a>"));
        plen=fill_tcp_data_p(buf,plen,PSTR("<font size=5>  LED3灭</font></center>"));
    }

    plen=fill_tcp_data_p(buf,plen,PSTR("<p><center><a href="));
    plen=fill_tcp_data(buf,plen,"http://");
    plen=fill_tcp_data(buf,plen,"192.168.1.108");
    plen=fill_tcp_data(buf,plen,"/?led=");

    if (led4==0x34)
    {
        plen=fill_tcp_data_p(buf,plen,PSTR("8><font size=5>关闭LED4:</font></a>"));
        plen=fill_tcp_data_p(buf,plen,PSTR("<font size=5 color=\"#00FF00\">  LED4亮</font></center>"));
    }
    else if(led4==0x38|led4==0) 
    {
        plen=fill_tcp_data_p(buf,plen,PSTR("4><font size=5>点亮LED4:</font></a>"));
        plen=fill_tcp_data_p(buf,plen,PSTR("<font size=5>  LED4灭</font></center>"));
    }
		
    plen=fill_tcp_data_p(buf,plen,PSTR("<font size=5><center > STM32内部温度传感器温度： "));
    plen=fill_tcp_data_p(buf,plen,PSTR(temp_2));
    plen=fill_tcp_data_p(buf,plen,"°C</center></font>");
    plen=fill_tcp_data_p(buf,plen,PSTR("<font size=5 color=\"#ff6600\"><center><hr> <br>宇运电子</center>"));
	plen=fill_tcp_data_p(buf,plen,PSTR("<font size=5 color=\"#00FF00\"><center><br>世界触手可及 一切皆有可能</center></font>"));
    plen=fill_tcp_data_p(buf,plen,"	<a href=http://mcu-web.taobao.com><center>想了解更多登入官方淘宝店:http://mcu-web.taobao.com</center></a>"); 
	plen=fill_tcp_data_p(buf,plen,PSTR("<font size=5 ><center>您电脑本地IP地址："));
	plen=fill_tcp_data_p(buf,plen,PSTR(tempfwip));
	plen=fill_tcp_data_p(buf,plen,PSTR("    您电脑网卡物理地址："));
	for(j=0;j<17;j++)
			macmac[j]=tempfwmac[j];
	plen=fill_tcp_data_p(buf,plen,PSTR(macmac));
	plen=fill_tcp_data_p(buf,plen,"</center></font>");
	plen=fill_tcp_data_p(buf,plen,PSTR("</form>"));	////页面结束
    return(plen);
	}

unsigned int simple_server(void)
	{  

	    unsigned int plen,i1=0;
	    unsigned int dat_p;
	    unsigned char ii;
	    unsigned char cmd,*buf1;
	    unsigned int payloadlen=0;
         LED1_ON();
         LED2_ON();
         LED3_ON();
         LED4_ON();   
       plen = enc28j60getrev();
        /*initialize enc28j60*/
        enc28j60Init(mymac);
//        ENC28J60_Init(mymac);
        //把IP地址和MAC地址写入各自的缓存区	ipaddr[] macaddr[]
        init_ip_arp_udp_tcp(mymac,myip,mywwwport);
        //指示灯状态:0x476 is PHLCON LEDA(绿)=links status, LEDB(红)=receive/transmit
        //enc28j60PhyWrite(PHLCON,0x7a4);
        //PHLCON：PHY 模块LED 控制寄存器	
        enc28j60PhyWrite(PHLCON,0x0476);
        enc28j60clkout(2); // change clkout from 6.25MHz to 12.5MHz
     
        //init the ethernet/ip layer:
        while(1)
            {
                get_temperature(&temp_1);
                temp_2[0]=temp1[temp_1/100];
                temp_2[1]=temp1[temp_1%100/10];
                temp_2[2]='.';
                temp_2[3]=temp1[temp_1%10];	 //°C
                plen=0;
                plen = enc28j60PacketReceive(BUFFER_SIZE, buf);

                /*plen will ne unequal to zero if there is a valid packet (without crc error) */
                if(plen==0)
                    {
                        continue;
                    }


                // arp is broadcast if unknown but a host may also
                // verify the mac address by sending it to 
                // a unicast address.

                //ARP 帧包发送 42 
                if(eth_type_is_arp_and_my_ip(buf,plen))
                    {
                        make_arp_answer_from_request(buf);
                        continue;
                    }

                // check if ip packets are for us:
                if(eth_type_is_ip_and_my_ip(buf,plen)==0) 
                    {
                        continue;
                    }

                
                if(buf[IP_PROTO_P]==IP_PROTO_ICMP_V && buf[ICMP_TYPE_P]==ICMP_TYPE_ECHOREQUEST_V)
                    {
                    // a ping packet, let's send pong		  PING的回应
                    make_echo_reply_from_request(buf, plen);
                    /*获取mac地址*/
                    for(ii=0;ii<6;ii++)
                        {
                            fwmac[ii]=buf[ii];
                            tempfwmac[ii*3+0]=temp[fwmac[ii]/16];
                            tempfwmac[ii*3+1]=temp[fwmac[ii]%16];
                            if(ii<5)
                            tempfwmac[ii*3+2]='-';
                        }

                    /*获取ip地址*/
                    for(ii=0;ii<4;ii++)
                        {
                            fwip[ii]=buf[ii+30];//0xbf
                            tempfwip[ii*4+0]=temp1[fwip[ii]/100]; //temp1[]={"0123456789"};
                            tempfwip[ii*4+1]=temp1[fwip[ii]%100/10];
                            tempfwip[ii*4+2]=temp1[fwip[ii]%100%10];
                            if(ii<3)
                            tempfwip[ii*4+3]='.';
                        }



                    continue;
                    }
                       // tcp port www start, compare only the lower byte端口网络开始，仅比低字节
                if (buf[IP_PROTO_P]==IP_PROTO_TCP_V&&buf[TCP_DST_PORT_H_P]==0&&buf[TCP_DST_PORT_L_P]==mywwwport)
                    {

                    /*获取mac地址*/
                    for(ii=0;ii<6;ii++)
                        {
                            fwmac[ii]=buf[ii+6];
                            tempfwmac[ii*3+0]=temp[fwmac[ii]/16];
                            tempfwmac[ii*3+1]=temp[fwmac[ii]%16];
                            if(ii<5)
                            tempfwmac[ii*3+2]='-';
                        }
                    
                    /*获取ip地址*/
                    iptemp[0]=buf[30];
                    iptemp[1]=buf[27];
                    iptemp[2]=buf[28];
                    iptemp[3]=buf[29];

                    for(ii=0;ii<4;ii++)
                        {
                            fwip[ii]=iptemp[ii];
                            tempfwip[ii*4+0]=temp1[fwip[ii]/100];
                            tempfwip[ii*4+1]=temp1[fwip[ii]%100/10];
                            tempfwip[ii*4+2]=temp1[fwip[ii]%100%10];
                            if(ii<3)
                            tempfwip[ii*4+3]='.';
                        }
                    
                    if (buf[TCP_FLAGS_P] & TCP_FLAGS_SYN_V)
                        {
                        make_tcp_synack_from_syn(buf);
                        // make_tcp_synack_from_syn does already send the syn,ack
                        continue;
                        }
                    if (buf[TCP_FLAGS_P] & TCP_FLAGS_ACK_V)
                        {
                        init_len_info(buf); // init some data structures
                        // we can possibly have no data, just ack:
                        dat_p=get_tcp_data_pointer();
                        if (dat_p==0)
                            {
                            if (buf[TCP_FLAGS_P] & TCP_FLAGS_FIN_V)
                                {
                                // finack, answer with ack
                                make_tcp_ack_from_any(buf);
                                }
                            // just an ack with no data, wait for next packet
                            continue;
                            }
                        if (strncmp("GET ",(char *)&(buf[dat_p]),4)!=0)
                            {
                            // head, post and other methods:
                            // for possible status codes see:
                            // http://www.w3.org/Protocols/rfc2616/rfc2616-sec10.html
                            plen=fill_tcp_data_p(buf,0,PSTR("HTTP/1.0 200 OK\r\nContent-Type: text/html\r\n\r\n<h1>200 OK</h1>"));
                            goto SENDTCP;
                            }
                        if (strncmp("/ ",(char *)&(buf[dat_p+4]),2)==0)
                            {
                            plen=fill_tcp_data_p(buf,0,PSTR("HTTP/1.0 200 OK\r\nContent-Type: text/html\r\n\r\n"));
                            plen=fill_tcp_data_p(buf,plen,PSTR("<form action=""/"" method=get>"));//页面开始处
                            plen=fill_tcp_data_p(buf,plen,PSTR("<center>密码：<input type=password name=""mm"" ></center><br>"));
                            plen=fill_tcp_data_p(buf,plen,PSTR("<center><input type=submit value=""登入""><input type=reset value=""重设""></center>"));
                            plen=fill_tcp_data_p(buf,plen,PSTR("</form>"));	////页面结束
                            goto SENDTCP;
                            }
                         if(strncmp("/?mm=654321",(char *)&(buf[dat_p+4]),11)!=0)
                            {
                                if(strncmp("/?led",(char *)&(buf[dat_p+4]),5)==0) goto SENDTCP1;
                            plen=fill_tcp_data_p(buf,0,PSTR("HTTP/1.0 200 OK\r\nContent-Type: text/html\r\n\r\n"));
                            plen=fill_tcp_data_p(buf,plen,PSTR("<form action=""/"" method=get>"));//页面开始处
                            plen=fill_tcp_data_p(buf,plen,PSTR("<center>密码：<input type=password name=""mm"" ></center><br>"));
                            plen=fill_tcp_data_p(buf,plen,PSTR("<center><input type=submit value=""登入""><input type=reset value=""重设""></center><br><br><br><br>"));
                            plen=fill_tcp_data_p(buf,plen,PSTR("<font size=6><center>密码错误请重新输入！！</center></font><br>"));
                            plen=fill_tcp_data_p(buf,plen,PSTR("</form>"));	////页面结束
                            goto SENDTCP;
                            
                            }

                        SENDTCP1: 
                        cmd=analyse_get_url((char *)&(buf[dat_p+10]));
                        BUF=buf[64];
                        cmd=BUF;
                        if (cmd==-1)
                            {
                            plen=fill_tcp_data_p(buf,0,PSTR("HTTP/1.0 401 Unauthorized\r\nContent-Type: text/html\r\n\r\n<h1>401 Unauthorized</h1>"));
                            goto SENDTCP;
                            }
                         if (cmd==0x31)
                        {			   			     
                            LED1_OFF();
                            led1=i_1=0x31;
                            
                        }
                        else if(cmd==0x35)
                        {
                            LED1_ON();
                            led1=i_1=0x35;
                        }
                         if (cmd==0x32)
                        {			   			     
                            LED2_OFF();
                            led2=i_2=0x32;
                        }
                        else if(cmd==0x36)
                        {
                            LED2_ON();
                            led2=i_2=0x36;
                        }
                        if (cmd==0x33)
                        {			   			     
                            LED3_OFF();
                            led3=i_3=0x33;

                        }
                        else if(cmd==0x37)
                        {
                            LED3_ON();
                            led3=i_3=0x37;
                        }
                         if (cmd==0x34)
                        {			   			     
                            LED4_OFF();
                            led4=i_4=0x34;

                        }
                        else if(cmd==0x38)
                        {
                            LED4_ON();
                            led4=i_4=0x38;
                        }
                        // if (cmd==-2) or any other value
                        // just display the status:
                        plen=print_webpage(buf,(i_1),(i_2),(i_3),(i_4)); /////////////////////////////////////////////////
                        SENDTCP:
                        make_tcp_ack_from_any(buf); // send ack for http get
                        make_tcp_ack_with_data(buf,plen); // send data
                        continue;
                        }
                    }
            // tcp port www end
            //
            // udp start, we listen on udp port 1200=0x4B0
                if (buf[IP_PROTO_P]==IP_PROTO_UDP_V&&buf[UDP_DST_PORT_H_P]==4&&buf[UDP_DST_PORT_L_P]==0xb0)
                    {
                    payloadlen=	  buf[UDP_LEN_H_P];
                    payloadlen=payloadlen<<8;
                    payloadlen=(payloadlen+buf[UDP_LEN_L_P])-UDP_HEADER_LEN;
                    //payloadlen=buf[UDP_LEN_L_P]-UDP_HEADER_LEN;
                    
                    ANSWER:
                    //while(1){
                    for(i1=0; i1<payloadlen; i1++) buf1[i1]=buf[UDP_DATA_P+i1];
                    
                    //make_udp_reply_from_request(buf,str,strlen(str),myudpport);
                    make_udp_reply_from_request(buf,buf1,payloadlen,myudpport);
                    }
                }
                return 0;
}

