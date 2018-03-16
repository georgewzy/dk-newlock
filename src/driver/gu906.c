
#include <string.h>
#include <stdlib.h>
#include "stdio.h"
#include "gu906.h"
#include "usart.h"


extern uint8_t gprs_err_cnt; 		//GPRS错误计数器
extern uint8_t gprs_status;			//GPRS的状态
extern usart_buff_t usart2_rx_buff;		//GPRS 接收缓冲区


enum order{
    //与命令对应
    _AT = 1,_ATE,_ATESIM,_ATCNMI,_ATCMGD,_ATCMGF,_ATCSMP,_ATUCS2,
    _ATGB2312,_ATATD,_ATATH,_ATGSM,_ATCPMS,_ATCSQ,_ATCREG,
    _ATCIICR,_ATCIPSTARTOK,_ATCIPCLOSE,_ATCIPSCONT,_ATCIPSTART,
    _ATCIPMUX,_ATCIPMODE,_ATCIPCFG,_ATCIPPACK,_ATCIPSEND,_ATCGMR,
    _ATCMGS,_ATCMGR,_GPRSSEND,_ATCSTT,_ATCIPSCONT_C,_CLOSEDTU,_OPENDTU,
    
    //额外的数据类型
    _GSMSEND,_GSMSENDEND
};

struct GprsData{
    char *order;   
    int olen;         
    enum order type;    
};

//GPRS数据保存位置
static char GPRS_Data[MAXRECVBUFF]={0};
static int  GPRS_Dlen = 0;
static u8   GPRS_Dtu_ConLock = 0;

u8 RestartGprs = 0; //重启GPRS标志





#if GU906GSM_EN
//短信信息在SIM卡中的位置
static char SIMDataID[5]=""; 
struct user_simdata sim;
#endif

/*********************************************************
  * @function  GPRS_ascii_to_hex
  * @role      
  * @input     
  * @output    None
  * @return    
  ********************************************************/
static int gprs_ascii_to_hex(u8 *asc_data, u8 *hex_data, int len)
{
    int i;
    u8 tmp_dat;
    for(i = 0; i < len; i++)
    {
        if ((asc_data[i] >= '0') && (asc_data[i] <= '9')){
            tmp_dat = asc_data[i] - '0';
        }else if ((asc_data[i] >= 'A') && (asc_data[i] <= 'F')){ // A....F
            tmp_dat = asc_data[i] - 0x37;
        }
        else if((asc_data[i] >= 'a') && (asc_data[i] <= 'f')){ // a....f
            tmp_dat = asc_data[i] - 0x57;
        }else return -1;
        hex_data[i] = tmp_dat;  
    }
    return 0;
}

/*********************************************************
  * @function  mypow
  * @role      pow??????,??num?n??,??n??? 
  * @input     num
  * @output    n
  * @return    ????
  *******************************************************
static int mypow(int num,int n)
{
    int powint=1;
    int i;
    for(i=1;i<=n;i++) powint*=num;
    return powint;
}
*/
/*********************************************************
  * @function  FreeStr
  * @role      ?????????,??16????,?????
  * @input     ???????????????????????????
  * @output    None
  * @return    None
  ********************************************************/
static void FreeStr(char *str, int strsiz, int head, int len)
{
    int i = 0;
    while(len--)
    {
        for(i = head; i < strsiz;i++)
        {
            str[i] = str[i+1];
        }
    }
}

#if GU906GSM_EN
/*********************************************************
  * @function  GU906_ParsingSIM
  * @role      ??SIM???????
  * @input     ?????
  * @output    None
  * @return    ????:0,????:-1
    @data      
    +CMGR: "REC READ","18750******",,"2015/03/14 20:02:15+32"
     124abcABC

    OK
  ********************************************************/
static int gu906_parsing_SIM(char *pinput)
{
    char *p = pinput;
    int i;
    #if DEBUG_EN
//    printf("\n?????\n");
    #endif
    if((p = strstr(p,"\",\"")) == 0)
        return -1;
    p += 3;
    memset(sim.phone,0,sizeof(sim.phone));
    for (i = 0; (*p != '\"') && (*p != '\0'); ++i,p++){
        sim.phone[i] = *p;
    }
    sim.phone[i] = '\0';
    #if DEBUG_EN
//    printf("sms.phone[%s]\r\n",sim.phone);
//    printf("\n??????\n");
    #endif
    
    p +=2;
    memset(sim.dev,0,sizeof(sim.dev));
    for (i = 0; (*p != ',') && (*p != '\0'); ++i,p++){
        sim.dev[i] = *p;
    }
    #if DEBUG_EN
//    printf("sms.dev[%s]\r\n",sim.dev);
//    printf("\n????\n");
    #endif
    
    p += 2;
    memset(sim.date,0,sizeof(sim.date));
    for (i = 0; (*p != '\"') && (*p != '\0'); ++i,p++){
        sim.date[i] = *p;
    }
    #if DEBUG_EN
//    printf("sms.date[%s]\r\n",sim.date);
//    printf("\n????\n");
    #endif
    
    p++;
    memset(sim.data,0,sizeof(sim.data));
    while((*p != '\0') && ((*p == '\n') || (*p == '\r')) ) p++;
    for (i = 0; (*p != '\0') && (*p != '\n') && (*p != '\r'); ++i,p++){
        sim.data[i] = *p;
    }
    sim.data[i] = '\0';
    #if DEBUG_EN
//    printf("sms.data:[%s]\r\n",sim.data );
    #endif
    return 0;
}
#endif

/*********************************************************
  * @function  gu906_GetRecvData
  * @role       提取字符串中跟命令无关的数据,有时在进行命令操作时，
               会突然收到短信，什么的，这里要做的就是处理并过滤掉这些数据。
               还有模块突然复位了，这里也做判断，并复位CPU。
  * @input     数据和数据长度
  * @input     ???????
  * @output    None
  * @return    None
  ********************************************************/
static void gu906_GetRecvData(char *pBuff, int *pLen)
{
    int rlen = 0;
	char buff[5]="";
    int i = 0;
    char *p1 = NULL;
    char *p2 = NULL;    

    if((pBuff == NULL) || (*pLen == 0))
        return;
    if (((p1 = strstr(pBuff, "+IPD,")) != 0) && ((p2 = strchr(pBuff, ':')) != 0))
    {
        p1+=5;
		for (i = 0; ((p1-pBuff) < *pLen) && (i < 5) && (*p1 != ':'); ++i,++p1) {
			buff[i] = *p1;
		}
		buff[i] = '\0';
		rlen = atoi(buff);
        p2++;
		GPRS_Dlen = ((rlen >= (*pLen - (p2 - pBuff)))?(*pLen - (p2 - pBuff)):rlen);
		memcpy(GPRS_Data, p2,GPRS_Dlen);
		rlen = GPRS_Dlen;
		
        p1 = strstr(pBuff, "+IPD,");
        p2 = strchr(pBuff, ':');
	    rlen += ((p2+1)-p1);
		FreeStr(pBuff, *pLen,p1-pBuff, rlen);
		if((*pLen -rlen) <=3)
			*pLen = 0;
		else
			*pLen -=rlen;
        #if DEBUG_EN
//        printf("B[%d][%s]\r\n",*pLen, pBuff);
        #endif
    }
    #if GU906GSM_EN
    else if (strstr(pBuff, "+CMTI:") && ((p1 = strchr(pBuff, ',')) != 0)){   //+CMTI: "SM",2 ???????  
        rlen = 0;
        p1++;
        for(i = 0; *p1 != '\r' && *p1 != '\n' && *p1 != '\0' && rlen < sizeof(SIMDataID);i++, p1++){
            if(*p1 >= '0' && *p1 <= '9')
                SIMDataID[rlen++] = *p1;
        }
        SIMDataID[rlen] = '\0'; 
    }
    else if ((p1 = strstr(pBuff, "+CMGR:")) != 0){ //??????
        gu906_parsing_SIM(p1);
    }
    #endif
    else if(strstr(pBuff,"[0000]") || strstr(pBuff,"Build Time")) 
    {
        #if (DEBUG_EN == 1)
//        printf("restart...\r\n\r\n");
        #endif
        RestartGprs = 1;
    }
}


static void GetFreeBuff(int num)
{
    char buff[MAXRECVBUFF] = {0};
    int siz = 0;
    while(num--)
    {
		usart2_recv_data();

        if(usart2_rx_buff.index)
        {
            gu906_GetRecvData(buff, &siz);    
        }
    }
}

    

static s8 SendAT(struct GprsData *gprs, char *out, u32 Delay)
{
    int siz = 0;
    int i = 0;
    char *p = gprs->order;  
    u8 dat[2];
    u8 csq = 0;
    s8 ret = _ATERROR;
    RestartGprs = 0;	
    i = 0;
    p = NULL;
 
	
	usart_send(USART1, gprs->order, gprs->olen);
	
    if((gprs->type == _GSMSEND) || (gprs->type == _ATATD)) 
    {
        ret = _ATOK;
        goto GU906_SENDATRET;
    }

    while(1)
    {
       
		memset(&usart2_rx_buff, 0, sizeof(usart_buff_t));
		
        while(usart2_rx_buff.index == 0)
        {
			usart1_recv_data();
        }
        
        if(RestartGprs){
            ret = _ATERROR;
            goto GU906_SENDATRET;
        }
        
        switch(gprs->type)
        {
            case _AT:
            case _ATE:   
            case _ATCNMI:
            case _ATCMGD:
            case _ATCMGF:
            case _ATCSMP:
            case _ATUCS2:
            case _ATATH :
            case _ATGSM :
			case _ATCSTT:
            case _ATCIICR:
            case _ATCIPCFG:
            case _ATCIPPACK:
            case _ATCIPSCONT:
			case _OPENDTU:
            case _CLOSEDTU:
            case _ATGB2312: //
                if(strstr((char*)usart2_rx_buff.pdata, "OK"))
				{
                    ret = _ATOK;
                    goto GU906_SENDATRET;
                }
				else if(strstr((char*)usart2_rx_buff.pdata, "ERROR") || strstr((char*)usart2_rx_buff.pdata,"NO CARRIER")) 
				{
                    GetFreeBuff(100);
                    ret = _ATERROR;
                    goto GU906_SENDATRET;
                }
            break;
                
            case _ATCPMS:	//优选消息存储器
				if(strstr((char*)usart2_rx_buff.pdata, "OK") && strstr((char*)usart2_rx_buff.pdata, "+CPMS:"))
				{
					 ret = _ATOK;
                     goto GU906_SENDATRET;
				}else if(strstr((char*)usart2_rx_buff.pdata, "ERROR"))
				{
					ret = _ATERROR;
                    goto GU906_SENDATRET;
				}
				break;
				
            case _ATESIM://测试SIM卡是否存在
				ret = _ATERROR;
				if(strstr((char*)usart2_rx_buff.pdata, "OK"))
				{
					if((p = strstr((char*)usart2_rx_buff.pdata, "+ESIMS: ")) != 0)
					{
						p += 8;
						if(1 == (*p -'0'))
							ret = _ATOK;	
					}
					goto GU906_SENDATRET;
				}
				break;
            
            case _ATCMGS: //发送消息
                if(strstr((char*)usart2_rx_buff.pdata, ">"))
				{
//                    GetFreeBuff(1);
                    ret = _ATOK;
                    goto GU906_SENDATRET;
                }
				break;

            case _ATCSQ://查询信号质量
				if(strstr((char*)usart2_rx_buff.pdata, "OK"))
				{
					if((p = strstr((char*)usart2_rx_buff.pdata, "+CSQ:")) != 0)
					{
						gprs_ascii_to_hex((u8 *)(p+6), dat, 2);
						csq = dat[0]*10 + dat[1];

						if (csq < 99 && csq >= GPRSCSQ)//网络信号要大于GPRSCSQ(18)
						{ 
							ret = _ATOK;
							goto GU906_SENDATRET;
						} 
						else 
						{
							ret = _ATERROR;
							goto GU906_SENDATRET;
						}	
					}
				}
				else
				{
					ret = _ATERROR;
					goto GU906_SENDATRET;
				}
				break;

            case _ATCIPSTARTOK:	//查询当前模块是否有网络连接 
				if(strstr((char*)usart2_rx_buff.pdata, "OK"))
				{
					if (strstr((char*)usart2_rx_buff.pdata, "+CIPSTART:")) 
					{
						ret = _ATOK;
						goto GU906_SENDATRET;
					}	
					ret = _ATERROR;
					goto GU906_SENDATRET;					
				}else if(strstr((char*)usart2_rx_buff.pdata, "ERROR")) 
				{
					ret = _ATERROR;
                    goto GU906_SENDATRET;
				}
				break;				
			
            case _ATCREG://查询网络注册信息
				if(strstr((char*)usart2_rx_buff.pdata, "OK"))
				{
					if ((p = strstr((char*)usart2_rx_buff.pdata, "+CREG: ")) != 0)
					{
						p += 7;
						if(('0' == *p) || ('5' == *p)) 
						{
							ret = _ATOK;
							goto GU906_SENDATRET;
						}
					}	
					ret = _ATERROR;
					goto GU906_SENDATRET;					
				}
				else if(strstr((char*)usart2_rx_buff.pdata, "ERROR")) 
				{
					ret = _ATERROR;
                    goto GU906_SENDATRET;
				}
				break;

            case _ATCIPSEND://发送数据命令
                if (strstr((char*)usart2_rx_buff.pdata, ">")) 
				{
                    ret = _ATOK;
                    goto GU906_SENDATRET;
                }
                else if (strstr((char*)usart2_rx_buff.pdata, "ERROR")){
                    ret = _ATERROR;
                    goto GU906_SENDATRET;
                }
            break;

            case _ATCIPMUX://设置多链接命令
                if(strstr((char*)usart2_rx_buff.pdata, "+CIPMUX: 0") && strstr(usart2_rx_buff.pdata, "OK")) 
				{
                    ret = _ATOK;
                    goto GU906_SENDATRET;
                }
				else if (strstr((char*)usart2_rx_buff.pdata, "ERROR"))
				{
                    ret = _ATERROR;
                    goto GU906_SENDATRET;
                }
				break;

            case _ATCIPMODE://设置数据透传模式
                if(strstr((char*)usart2_rx_buff.pdata, "+CIPMODE: ") && strstr((char*)usart2_rx_buff.pdata, "OK")) 
				{
                    ret = _ATOK;
                    goto GU906_SENDATRET;
                }else if (strstr((char*)usart2_rx_buff.pdata, "ERROR"))
				{
                    ret = _ATERROR;
                    goto GU906_SENDATRET;
                }
				break;

            case _GPRSSEND:
                if(strstr((char*)usart2_rx_buff.pdata, "SEND OK")) 
				{
                   ret = _ATOK;
                   goto GU906_SENDATRET;
                }
            break;

            case _ATCMGR://读出消息
                gu906_GetRecvData((char*)usart2_rx_buff.pdata, &siz);
                ret = _ATOK;
                goto GU906_SENDATRET;
            //break; 

            case _ATCIPCLOSE://关闭链接命令
                if (strstr((char*)usart2_rx_buff.pdata, "CLOSE OK") || strstr((char*)usart2_rx_buff.pdata, "+CME ERROR:")) 
				{
                    ret = _ATOK;
                    goto GU906_SENDATRET;
                }
                else if(strstr((char*)usart2_rx_buff.pdata, "ERROR")){
                    ret = _ATERROR;
                    goto GU906_SENDATRET;   
                }
            break;

            case _ATCIPSTART://打开TCP或者UDP链接命令
                if(!GPRS_Dtu_ConLock)
                {
                    if(strstr((char*)usart2_rx_buff.pdata, "CONNECT OK"))
					{
                        ret = _ATOK;
                        goto GU906_SENDATRET;
                    }
                    else if(strstr((char*)usart2_rx_buff.pdata, "RECONNECTING") || strstr(usart2_rx_buff.pdata, "ERROR") || strstr(usart2_rx_buff.pdata, "CONNECT FAIL"))
					{
                        GetFreeBuff(100);
                        ret = _ATERROR;
                        goto GU906_SENDATRET;
                    }                    
                }
                else if(strstr((char*)usart2_rx_buff.pdata, "OK")){
                    ret = _ATOK;
                    goto GU906_SENDATRET;
                }
				else if(strstr((char*)usart2_rx_buff.pdata, "ERROR")){
                    ret = _ATERROR;
                    goto GU906_SENDATRET;   
                }
				break;
            
            case _GSMSENDEND: 
                GetFreeBuff(100);
                ret = _ATOK;
                goto GU906_SENDATRET; //
			
			case _ATCIPSCONT_C:
				if(strstr((char*)usart2_rx_buff.pdata, "OK"))
				{
					if(0 != (p = strstr((char*)usart2_rx_buff.pdata, "+CIPMODE: ")))
					{
						p += 10;

						if(1 == (*p -'0'))
						{

							if(0 != (p = strstr((char*)usart2_rx_buff.pdata, "+CIPSTART: ")))
							{

								if(strstr((char*)usart2_rx_buff.pdata,"218.66.59.201") && strstr((char*)usart2_rx_buff.pdata,"8888"))
								{								
									GPRS_Dtu_ConLock = 1;
									ret = _ATOK;
									goto GU906_SENDATRET;
								}
							}						
						}
					}
					GPRS_Dtu_ConLock = 0;
					ret = _ATOK;
					goto GU906_SENDATRET;
				}
				else if(strstr((char*)usart2_rx_buff.pdata, "ERROR"))
				{
                    ret = _ATERROR;
                    goto GU906_SENDATRET;   
                }
				break;
				
            default: break; 
        }   
    }
    GU906_SENDATRET:
    return ret;
}

/*********************************************************
  * @function  GU906_ExecuteOrder
  * @role      执行命令
  * @input     None
  * @output    None
  * @return    成功返回：_ATOK，失败返回：_ATERROR，超时返回：_ATOTIME
  ********************************************************/
static s8 GU906_ExecuteOrder(char *Order, u32 len, enum order type, u32 num)
{
	u32 i = 0;
	u32 delay_time = 1000;
	s8 ret = _ATOTIME;
    struct GprsData gprs;
	
    if(type == _ATCIPSTART)
        delay_time = 4000;
    if(type == _GPRSSEND)
        delay_time = 10;
	
    gprs.order = Order;
    gprs.olen = len;
    gprs.type = type;
	while((ret = SendAT(&gprs, NULL, delay_time)) != _ATOK)
	{
		if(ret == _ATERROR) 
		{
//			if(++i >= num)
//			{
//				return _ATERROR;
//			}
		}
		else 
		{
			return _ATOTIME;
		}
	}
	return _ATOK;
}

/*********************************************************
  * @function  GU906_init
  * @role      GSM???
  * @input     None
  * @output    None
  * @return    ????:_ATOK,????:_ATERROR,????:_ATOTIME
  ********************************************************/
s8 gu906_init(void)
{
	s8 ret = _ATOTIME;

    // 开回显:ATE1 关回显:ATE0
	if(_ATOK != (ret = GU906_ExecuteOrder(ATE(0), strlen(ATE(0)), _ATE, 2)))
		return ret;
//	
//	// 查询卡是否存在
//	if(_ATOK != (ret = GU906_ExecuteOrder(ATESIM, strlen(ATESIM), _ATESIM, 10))) 
//		return ret;
//    
//    //查询信号强度 信号强度大于等于18才行
//	while(_ATOK != (ret = GU906_ExecuteOrder(ATCSQ, strlen(ATCSQ), _ATCSQ, 60)))
//	{
//		if(ret == _ATOTIME) return ret;
//	}
    return _ATOK;  
}

/*********************************************************
  * @function  GU906_Module_State
  * @role      判断GU906的状态
  * @input     None
  * @output    None
  * @return    成功返回：_ATOK，失败返回：_ATERROR，超时返回：_ATOTIME
  ********************************************************/
s8 GU906_Module_State(void)
{
	return GU906_ExecuteOrder(AT, strlen(AT), _AT, 0);
}

/*********************************************************
  * @function  GU906_TCP_Socket
  * @role      进行TCP连接
  * @input     IP地址与端口
  * @output    None
  * @return    成功返回：_ATOK，失败返回：_ATERROR，超时返回：_ATOTIME
  ********************************************************/
s8 GU906_TCP_Socket(struct Gprs_Config *GprsCon)
{
    char cipstart[100] = {0};
    s8 ret = _ATOTIME;
	
    if(GprsCon->server_ip == NULL || !GprsCon->server_port) return ret;
    if(!strlen((char *)GprsCon->server_ip)) return ret;
	
    //确保模块以及注册到GSM网络
	if(_ATOK != (ret = GU906_ExecuteOrder(ATCREG, strlen(ATCREG), _ATCREG, 2))) 
		return ret;

    //让模块激活 GPRS 网络，在需要反复建立 TCP 链接的场合可提高速度
	if(_ATOK != (ret = GU906_ExecuteOrder(ATCIICR, strlen(ATCIICR), _ATCIICR, 2))) 
		return ret;
	
    //查询当前是否有网络连接
	while(_ATOK == GU906_ExecuteOrder(ATCIPSTARTOK, strlen(ATCIPSTARTOK), _ATCIPSTARTOK, 0)) 
	{
		//关闭网络连接
		if(_ATOK != (ret = GU906_ExecuteOrder(ATCIPCLOSE, strlen(ATCIPCLOSE), _ATCIPCLOSE, 2))) 
			return ret;
		
		//保存设置
		if(_ATOK != (ret = GU906_ExecuteOrder(ATCIPSCONT(0), strlen(ATCIPSCONT(0)), _ATCIPSCONT, 2))) 
			return ret;
	}
 
    //单链接模式
	if(_ATOK != (ret = GU906_ExecuteOrder(ATCIPMUX, strlen(ATCIPMUX), _ATCIPMUX, 2))) 
		return ret;

    //非数据透传输模式
	if(_ATOK != (ret = GU906_ExecuteOrder(ATCIPMODE(0), strlen(ATCIPMODE(0)), _ATCIPMODE, 2))) 
		return ret;

    //自动启动连接命令
	if(_ATOK != (ret = GU906_ExecuteOrder(ATCIPCFG(0), strlen(ATCIPCFG(0)), _ATCIPCFG, 2))) 
		return ret;

    //心跳包设置
	if(_ATOK != (ret = GU906_ExecuteOrder(ATCIPPACK(0), strlen(ATCIPPACK(0)), _ATCIPPACK, 2))) 
		return ret;
	
    //连接到服务器
    sprintf(cipstart, ATCIPSTART,"TCP", GprsCon->server_ip, GprsCon->server_port);
	ret = GU906_ExecuteOrder(cipstart, strlen(cipstart), _ATCIPSTART, 3);
	
    return ret;
}

/*********************************************************
  * @function  GU906_DTU_Socket
  * @role      设置透传模式
  * @input     IP地址与端口
  * @output    None
  * @return    成功返回：_ATOK，失败返回：_ATERROR，超时返回：_ATOTIME
  ********************************************************/
s8 GU906_DTU_Socket(struct Gprs_Config *GprsCon)
{
    char atorder[100] = "";
    s8 ret = _ATOTIME;
    
    if(GprsCon->server_ip == NULL || !GprsCon->server_port) return ret;
    if(!strlen((char *)GprsCon->server_ip)) return ret;
    
    //查询数据透设置情况
	if(_ATOK != (ret = GU906_ExecuteOrder(ATCIPSCONT_C, strlen(ATCIPSCONT_C), _ATCIPSCONT_C, 2))) 
		goto GU906_DTU_SOCKETEND;
 
    if(!GPRS_Dtu_ConLock)
	{
		//账号配置
		if(_ATOK != (ret = GU906_ExecuteOrder(ATCSTT, strlen(ATCSTT), _ATCSTT, 2))) 
			goto GU906_DTU_SOCKETEND;
		
		//透传参数设置
		if(_ATOK != (ret = GU906_ExecuteOrder(ATCIPCFG(1), strlen(ATCIPCFG(1)), _ATCIPCFG, 2))) 
			goto GU906_DTU_SOCKETEND;
		
		//设置心跳
		if(_ATOK != (ret = GU906_ExecuteOrder(ATCIPPACK(0), strlen(ATCIPPACK(0)), _ATCIPPACK, 2))) 
			goto GU906_DTU_SOCKETEND;
		
		//设置设备注册包
		if(_ATOK != (ret = GU906_ExecuteOrder(ATCIPPACK(1), strlen(ATCIPPACK(1)), _ATCIPPACK, 2))) 
			goto GU906_DTU_SOCKETEND;
		
		//单链接模式
		if(_ATOK != (ret = GU906_ExecuteOrder(ATCIPMUX, strlen(ATCIPMUX), _ATCIPMUX, 2))) 
			goto GU906_DTU_SOCKETEND;

		//数据透传输模式
		if(_ATOK != (ret = GU906_ExecuteOrder(ATCIPMODE(1), strlen(ATCIPMODE(1)), _ATCIPMODE, 2))) 
			goto GU906_DTU_SOCKETEND;

		//保存设置
		sprintf(atorder, ATCIPSCONT(1),"TCP", GprsCon->server_ip, GprsCon->server_port);
		if(_ATOK != (ret = GU906_ExecuteOrder(atorder, strlen(atorder), _ATCIPSCONT, 2))) 
			goto GU906_DTU_SOCKETEND;
		
		GPRS_Dtu_ConLock = 1;
	}

    //建立数据透连接
    sprintf(atorder, ATCIPSTART, "TCP", GprsCon->server_ip, GprsCon->server_port);
	if(_ATOK != (ret = GU906_ExecuteOrder(atorder, strlen(atorder), _ATCIPSTART, 2))) 
		goto GU906_DTU_SOCKETEND;

    GU906_DTU_SOCKETEND:

    return ret;
}

/*********************************************************
  * @function  GU906_DtuOrAT
  * @role      透传模式与AT模式转换
  * @input     None
  * @output    None
  * @return    成功返回：_ATOK，失败返回：_ATERROR，超时返回：_ATOTIME
  ********************************************************/
s8 gu906_DtuOrAT(u8 type)
{
    s8 ret = _ATERROR;
	if(type)
	{
		while(!GPRS_Dtu_ConLock)
		{
			//打开透传
			if(_ATOK != (ret = GU906_ExecuteOrder(OPENDTU, strlen(OPENDTU), _OPENDTU, 0))) 
				goto GU906_DTUOFFONEND;
			GPRS_Dtu_ConLock = 1;
		}
	}
	else
	{
		while(GPRS_Dtu_ConLock)
		{
			//关闭透传
			if(_ATOK != (ret = GU906_ExecuteOrder(CLOSEDTU, strlen(CLOSEDTU), _CLOSEDTU, 0)))
			{
				if(_ATOK != (GU906_Module_State()))
					goto GU906_DTUOFFONEND;	
			}
			GPRS_Dtu_ConLock = 0;
		}	
	}
	
	GU906_DTUOFFONEND:
	return ret;
}
/*********************************************************
  * @function  GU906_GPRS_write
  * @role      gprs发送数据
  * @input     要发送的数据与数据长度
  * @output    None
  * @return    成功返回：_ATOK，失败返回：_ATERROR，超时返回：_ATOTIME
  ********************************************************/
s8 gu906_gprs_write(char* pdat, int len)
{
    char atorder[20] = "";
    s8 ret = -1;
    if(strlen(pdat) == 0) return 0;
	
 
	if(!GPRS_Dtu_ConLock)//非数据透模式
	{
		//设置数据长度
		sprintf(atorder, ATCIPSEND(1), len);
		if(_ATOK != (ret = GU906_ExecuteOrder(atorder, strlen(atorder), _ATCIPSEND, 0))) 
			goto GU906_GPRS_WRITERET;
		
		//发送数据
		if(_ATOK != (ret = GU906_ExecuteOrder(pdat, len, _GPRSSEND, 0))) 
			goto GU906_GPRS_WRITERET;
	}
	else		//数据透传模式
	{
		//发送数据
		usart_send(USART2, pdat, len);
		ret = _ATOK;
	}
	
    GU906_GPRS_WRITERET:
    return ret;
}

/*********************************************************
  * @function  GU906_GPRS_read
  * @role      查询是否接收到数据
  * @input     输出缓存大小
  * @output    接收到的数据
  * @return    接收到的数据长度
  ********************************************************/
u32 gu906_gprs_read(char *pout, int len)
{
    int i = 0;
	
	if(!GPRS_Dtu_ConLock)
	{
		GPRSREAD:
		if(GPRS_Dlen)
		{
			for(i = 0;(i < GPRS_Dlen) && (i < (len -1)); i++)
			{
				pout[i] = GPRS_Data[i];
			}
			memset(GPRS_Data, 0, sizeof(GPRS_Data));
			GPRS_Dlen = 0;
			return i;
		}
		else
		{
			GetFreeBuff(1);
			if(GPRS_Dlen)
				goto GPRSREAD;
		}	
	}
	else
	{
		
		usart2_recv_data();
	}
    return 0;
}


static void gprs_init_task_fun(struct Gprs_Config *GprsCon)
{

	u8 atorder[100] = {0};
	s8 ret;
	
	static uint8_t gprs_init_flag = true;		//
		
	while(1)
	{
		
		switch(gprs_status)
		{
			case 0:
				gprs_power_on();
				gprs_status = 1;
				gprs_err_cnt = 0;
			break;
					
			case 1://查询数据透设置情况
				ret = GU906_ExecuteOrder(ATCIPSCONT_C, strlen(ATCIPSCONT_C), _ATCIPSCONT_C, 2);
				if (ret == _ATOK)
				{
					gprs_status++;
					gprs_err_cnt = 0;
				}
				else
				{
					gprs_err_cnt++;
					if (gprs_err_cnt > 5)
					{
						gprs_status = 0;
					}
				}
			break;
			
			case 2://账号配置
				ret = ret = GU906_ExecuteOrder(ATCSTT, strlen(ATCSTT), _ATCSTT, 2);
				if(ret == _ATOK)
				{
					gprs_status++;
					gprs_err_cnt = 0;
				}
				else
				{
					gprs_err_cnt++;
					if (gprs_err_cnt > 5)
					{
						gprs_status = 0;
					}
				}
			break;
			
			case 3:	//透传参数设置	
				ret = GU906_ExecuteOrder(ATCIPCFG(1), strlen(ATCIPCFG(1)), _ATCIPCFG, 2);
				if (ret == _ATOK)
				{
					gprs_status++;
					gprs_err_cnt = 0;
				}
				else
				{
					gprs_err_cnt++;
					if (gprs_err_cnt > 5)
					{
						gprs_status = 0;
					}
				}
			break;
			
			case 4://单链接模式
				ret = GU906_ExecuteOrder(ATCIPMUX, strlen(ATCIPMUX), _ATCIPMUX, 2);
				if (ret == _ATOK)
				{
					gprs_status++;
					gprs_err_cnt = 0;
				}
				else
				{
					gprs_err_cnt++;
					if (gprs_err_cnt > 5)
					{
						gprs_status = 0;
					}
				}
			break;
			
			case 5://数据透传输模式				
				ret = GU906_ExecuteOrder(ATCIPMODE(1), strlen(ATCIPMODE(1)), _ATCIPMODE, 2); 
				if (ret == _ATOK)
				{
					gprs_status++;
					gprs_err_cnt = 0;
				}
				else
				{
					if (gprs_err_cnt > 5)
					{
						gprs_status = 0;
					}
				}
			break;
			
			case 6://建立数据透传连接
				sprintf(atorder, ATCIPSTART, "TCP", GprsCon->server_ip, GprsCon->server_port);
				ret = GU906_ExecuteOrder(atorder, strlen(atorder), _ATCIPSTART, 2);

				if (ret == _ATOK)
				{
					gprs_status++;
					gprs_err_cnt = 0;
				}
				else
				{
					gprs_err_cnt++;
					if (gprs_err_cnt > 5)
					{
						gprs_status = 0;
					}
				}
			break;
			
			case 7: //GPRS初始化成功
				gprs_status = 255;	
			break;
				
			default:
			break;		
		}	//switch end			
	} // while end
	
		
}




