//                            _ooOoo_  
//                           o8888888o  
//                           88" . "88  
//                           (| -_- |)  
//                            O\ = /O  
//                        ____/`---'\____  
//                      .   ' \\| |// `.  
//                       / \\||| : |||// \  
//                     / _||||| -:- |||||- \  
//                       | | \\\ - /// | |  
//                     | \_| ''\---/'' | |  
//                      \ .-\__ `-` ___/-. /  
//                   ___`. .' /--.--\ `. . __  
//                ."" '< `.___\_<|>_/___.' >'"".  
//               | | : `- \`.;`\ _ /`;.`/ - ` : | |  
//                 \ \ `-. \_ __\ /__ _/ .-` / /  
//         ======`-.____`-.___\_____/___.-`____.-'======  
//                            `=---='  
//  
//         .............................................  
//                  佛祖保佑             永无BUG 
//          佛曰:  
//                 写字楼里写字间，写字间里程序员；  
//                 程序人员写程序，又拿程序换酒钱。  
//                 酒醒只在网上坐，酒醉还来网下眠；  
//                 酒醉酒醒日复日，网上网下年复年。  
//                 但愿老死电脑间，不愿鞠躬老板前；  
//                 奔驰宝马贵者趣，公交自行程序员。  
//                 别人笑我忒疯癫，我笑自己命太贱；  
//                 不见满街漂亮妹，哪个归得程序员？  
//////////////////////////////////////////////////////////
#include <string.h>
#include <stdlib.h>
#include "stdio.h"
#include "delay.h"
#include "config.h"
#include "usart4.h"
#include "gu906.h"

#define DEBUG_EN  1

//接收缓存
#define MAXRECVBUFF  USART4_BUFF

#define AT                  "AT\r\n"                                        //测试命令
#define ATE(x)              ((x)?("ATE1\r\n"):("ATE0\r\n"))                 //开回显、关回显 
#define ATESIM              "AT+ESIMS?\r\n"                                 //检测卡是否存在
#define ATCNMI              "AT+CNMI=2,1\r\n"                               //设置这组参数来了新信息存储起来 
#define ATCMGD              "AT+CMGD=1,4\r\n"                               //删除当前存储器中全部短信 
#define ATCMGF              "AT+CMGF=1\r\n"                                 //0设置短消息为PDU模式, 1设置短消息为txet模式
#define ATCSMP              "AT+CSMP=17,167,2,25\r\n"                       //设置文本模式的参数
#define ATUCS2              "AT+CSCS=\"UCS2\"\r\n"                          //设置为 UCS2 编码字符集
#define ATGB2312            "AT+CSCS=\"GB2312\"\r\n"                        //设置GB2312编码
#define ATATD               "ATD%s;\r\n"                                    //对指定手机拨号
#define ATATH               "ATH\r\n"                                       //挂机
#define ATGSM               "AT+CSCS=\"GSM\"\r\n"                           //设置GSM字符集  
#define ATCPMS              "AT+CPMS=\"SM\",\"SM\",\"SM\"\r\n"              //设置短信存储单元为SIM卡 
#define ATCSQ               "AT+CSQ\r\n"                                    //获取信号强度
#define ATCREG              "AT+CREG?\r\n"                                  //确保模块以及注册到GSM网络
#define ATCIICR             "AT+CIICR\r\n"                                  //让模块激活 GPRS 网络，在需要反复建立 TCP 链接的场合可提高速度       
#define ATCIPSTARTOK        "AT+CIPSTART?\r\n"                              //查询当前是否有网络连接
#define ATCIPCLOSE          "AT+CIPCLOSE=0\r\n"                             //关闭网络连接
#define ATCIPSCONT(x)       ((x)?("AT+CIPSCONT=0,\"%s\",\"%s\",%d,2")\
                                :("AT+CIPSCONT\r\n"))                       //保存设置     
#define ATCIPSTART          "AT+CIPSTART=\"%s\",\"%s\",%d\r\n"              //设置TCP连接的IP和端口
#define ATCIPMUX            "AT+CIPMUX=0\r\n"                               //单链接模式
#define ATCIPMODE(x)        ((x)?("AT+CIPMODE=1,0\r\n")\
                                :("AT+CIPMODE=0,0\r\n"))                    //数据透传输模式、非透传
#define ATCIPCFG(x)         ((x)?("AT+CIPCFG=1,50,0\r\n")\
                                :("AT+CIPCFG=0,50,0\r\n"))                  //自动启动连接命令                      
#define ATCIPPACK(x)        ((x)?("AT+CIPPACK=1,\"4C4F47494E3A31303031\"\r\n")\
                                :("AT+CIPPACK=0,\"0102A0\"\r\n"))           //设备注册包、心跳包设置
#define ATCIPSEND(x)        ((x)?("AT+CIPSEND=%d\r\n")\
                                :("AT+CIPSEND\r\n"))                        //设置发送的数据长度
#define ATCGMR              "AT+CGMR\r\n"                                   //获取基站信息          
#define ATCMGS              "AT+CMGS=\"%s\"\r\n"                            //设置需要发送短信的手机号
#define ATCMGR              "AT+CMGR=%s\r\n"                                //设置要读取的短信的位置
#define ATCSTT              "AT+CSTT=\"CMNET\"\r\n"                         //账号配置
#define ATCIPSCONT_C        "AT+CIPSCONT?\r\n"                              //查询透传连接情况            
#define GPRSSEND            0x1A                                 
#define CLOSEDTU            "+++"                                            //关闭透传
#define OPENDTU             "ATO0\r\n"                                      //重新进入透传

enum order{
    //与命令对应的
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
static int GPRS_ascii_to_hex(u8 *asc_data, u8 *hex_data, int len)
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
  * @role      pow库函数的实现，计算num的n次幂，其中n为整数 
  * @input     num
  * @output    n
  * @return    计算结果
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
  * @role      删除字符串中的字串，支持16进制数据，无视结束符
  * @input     字符串、字符串总长度、开始删除的起始位置、要删除的长度
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
  * @role      解析SIM卡中的短信数据
  * @input     卡中的数据
  * @output    None
  * @return    成功返回：0，失败返回：-1
    @data      
    +CMGR: "REC READ","18750895002",,"2015/03/14 20:02:15+32"
     124abcABC

    OK
  ********************************************************/
static int GU906_ParsingSIM(char *pinput)
{
    char *p = pinput;
    int i;
    #if DEBUG_EN
    printf("\n分离手机号\n");
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
    printf("sms.phone[%s]\r\n",sim.phone);
    printf("\n分离设备类型\n");
    #endif
    
    p +=2;
    memset(sim.dev,0,sizeof(sim.dev));
    for (i = 0; (*p != ',') && (*p != '\0'); ++i,p++){
        sim.dev[i] = *p;
    }
    #if DEBUG_EN
    printf("sms.dev[%s]\r\n",sim.dev);
    printf("\n分离时间\n");
    #endif
    
    p += 2;
    memset(sim.date,0,sizeof(sim.date));
    for (i = 0; (*p != '\"') && (*p != '\0'); ++i,p++){
        sim.date[i] = *p;
    }
    #if DEBUG_EN
    printf("sms.date[%s]\r\n",sim.date);
    printf("\n分离数据\n");
    #endif
    
    p++;
    memset(sim.data,0,sizeof(sim.data));
    while((*p != '\0') && ((*p == '\n') || (*p == '\r')) ) p++;
    for (i = 0; (*p != '\0') && (*p != '\n') && (*p != '\r'); ++i,p++){
        sim.data[i] = *p;
    }
    sim.data[i] = '\0';
    #if DEBUG_EN
    printf("sms.data:[%s]\r\n",sim.data );
    #endif
    return 0;
}
#endif

/*********************************************************
  * @function  GetRecvData
  * @role      提取字符串中跟命令无关的数据,有时在进行命令操作时，
               会突然收到短信，什么的，这里要做的就是处理并过滤掉这些数据。
               还有模块突然复位了，这里也做判断，并复位CPU。
  * @input     数据和数据长度
  * @output    None
  * @return    None
  ********************************************************/
static void GetRecvData(char *pBuff, int *pLen)
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
        printf("B[%d][%s]\r\n",*pLen, pBuff);
        #endif
    }
    #if GU906GSM_EN
    else if (strstr(pBuff, "+CMTI:") && ((p1 = strchr(pBuff, ',')) != 0)){   //+CMTI: "SM",2 有短信消息到来  
        rlen = 0;
        p1++;
        for(i = 0; *p1 != '\r' && *p1 != '\n' && *p1 != '\0' && rlen < sizeof(SIMDataID);i++, p1++){
            if(*p1 >= '0' && *p1 <= '9')
                SIMDataID[rlen++] = *p1;
        }
        SIMDataID[rlen] = '\0'; 
    }
    else if ((p1 = strstr(pBuff, "+CMGR:")) != 0){ //读取到短消息
        GU906_ParsingSIM(p1);
    }
    #endif
    else if(strstr(pBuff,"[0000]") || strstr(pBuff,"Build Time")) 
    {
        #if (DEBUG_EN == 1)
        printf("restart...\r\n\r\n");
        #endif
        RestartGprs = 1;
    }
}

/*********************************************************
  * @function  GetFreeBuff
  * @role      处理掉缓存中多余的数据,同时也起到延时200ms的作用，
               读取数据函数自带延时10ms，所以这里num=20,
               GU906发送命令不能太快，不然GU906会因为处理不过来，而导致出错。
  * @input     None
  * @output    None
  * @return    None
  ********************************************************/
static void GetFreeBuff(int num)
{
    char buff[MAXRECVBUFF] = {0};
    int siz = 0;
    while(num--)
    {
        siz = usart4_Receive(buff,MAXRECVBUFF);
        if(siz)
        {
            GetRecvData(buff, &siz);    
        }
    }
}

    
/*********************************************************
  * @function  SendAT
  * @role      发送AT指令并接收
  * @input     gprs：要发送的参数
  * @output    out：返回的参数
  * @return    成功返回：_ATOK，失败返回：_ATERROR
  ********************************************************/
static s8 SendAT(struct GprsData *gprs, char *out, u32 Delay)
{
    int siz = 0;
    int i = 0;
    char *p = gprs->order;  
    u8 dat[2];
    u8 csq = 0;
    s8 ret = _ATERROR;
    char buff[MAXRECVBUFF] = {0};
    RestartGprs = 0;

#if (DEBUG_EN == 1)
    printf("\r\n------------------------------\r\n");
    printf("len[%d]\r\n", gprs->olen);
    for(i = 0; i< gprs->olen; i++,++p)
        printf("%c", *p);
    printf("\r\n");
#endif
    i = 0;
    p = NULL;
    GetFreeBuff(10);
    usart4_Send(gprs->order,gprs->olen);
    if((gprs->type == _GSMSEND) || (gprs->type == _ATATD)) 
    {
        ret = _ATOK;
        goto GU906_SENDATRET;
    }

    while(1)
    {
        for(i = 0;i<sizeof(buff);i++) 
			buff[i]=0;
        siz = 0; i = 0;
        while(siz == 0)
        {
            siz = usart4_Receive(buff,MAXRECVBUFF);
            if(siz){
				#if (DEBUG_EN == 1)
				printf("\r\nrecv:\r\n");
				printf("[%s]\r\n",buff);
				#endif
                GetRecvData(buff, &siz);
            }
            if(i++ > Delay) 
            {
                ret = _ATOTIME;
                goto GU906_SENDATRET;
            }
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
            case _ATGB2312:
                if(strstr(buff, "OK")){
                    ret = _ATOK;
                    goto GU906_SENDATRET;
                }else if(strstr(buff, "ERROR") || strstr(buff,"NO CARRIER")) {
                    GetFreeBuff(100);
                    ret = _ATERROR;
                    goto GU906_SENDATRET;
                }
            break;
                
            case _ATCPMS:
				if(strstr(buff, "OK") && strstr(buff, "+CPMS:")){
					 ret = _ATOK;
                     goto GU906_SENDATRET;
				}else if(strstr(buff, "ERROR")){
					ret = _ATERROR;
                    goto GU906_SENDATRET;
				}
				break;
				
            case _ATESIM:
				ret = _ATERROR;
				if(strstr(buff, "OK"))
				{
					if((p = strstr(buff, "+ESIMS: ")) != 0)
					{
						p += 8;
						if(1 == (*p -'0'))
							ret = _ATOK;	
					}
					goto GU906_SENDATRET;
				}
				break;
            
            case _ATCMGS:
                if(strstr(buff, ">")){
                    GetFreeBuff(1);
                    ret = _ATOK;
                    goto GU906_SENDATRET;
                }
				break;

            case _ATCSQ:
				if(strstr(buff, "OK"))
				{
					if((p = strstr(buff, "+CSQ:")) != 0)
					{
						GPRS_ascii_to_hex((u8 *)(p+6), dat, 2);
						csq = dat[0]*10 + dat[1];
						#if DEBUG_EN
						printf("信号:[%d]\r\n", csq);
						#endif	
						if (csq < 99 && csq >= GPRSCSQ){ //网络信号要大于GPRSCSQ(18)
							ret = _ATOK;
							goto GU906_SENDATRET;
						} else {
							ret = _ATERROR;
							goto GU906_SENDATRET;
						}	
					}
				}
				else{
					ret = _ATERROR;
					goto GU906_SENDATRET;
				}
				break;

            case _ATCIPSTARTOK:
				if(strstr(buff, "OK"))
				{
					if (strstr(buff, "+CIPSTART:")) {
						ret = _ATOK;
						goto GU906_SENDATRET;
					}	
					ret = _ATERROR;
					goto GU906_SENDATRET;					
				}else if(strstr(buff, "ERROR")) {
					ret = _ATERROR;
                    goto GU906_SENDATRET;
				}
				break;				
			
            case _ATCREG:
				if(strstr(buff, "OK"))
				{
					if ((p = strstr(buff, "+CREG: ")) != 0)
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
				}else if(strstr(buff, "ERROR")) {
					ret = _ATERROR;
                    goto GU906_SENDATRET;
				}
				break;

            case _ATCIPSEND:
                if (strstr(buff, ">")) {
                    ret = _ATOK;
                    goto GU906_SENDATRET;
                }
                else if (strstr(buff, "ERROR")){
                    ret = _ATERROR;
                    goto GU906_SENDATRET;
                }
            break;

            case _ATCIPMUX:
                if(strstr(buff, "+CIPMUX: 0") && strstr(buff, "OK")) {
                    ret = _ATOK;
                    goto GU906_SENDATRET;
                }else if (strstr(buff, "ERROR")){
                    ret = _ATERROR;
                    goto GU906_SENDATRET;
                }
				break;

            case _ATCIPMODE:
                if(strstr(buff, "+CIPMODE: ") && strstr(buff, "OK")) {
                    ret = _ATOK;
                    goto GU906_SENDATRET;
                }else if (strstr(buff, "ERROR")){
                    ret = _ATERROR;
                    goto GU906_SENDATRET;
                }
				break;

            case _GPRSSEND:
                if(strstr(buff, "SEND OK")) {
                   ret = _ATOK;
                   goto GU906_SENDATRET;
                }
            break;

            case _ATCMGR:
                GetRecvData(buff, &siz);
                ret = _ATOK;
                goto GU906_SENDATRET;
            //break; 

            case _ATCIPCLOSE:
                if (strstr(buff, "CLOSE OK") || strstr(buff, "+CME ERROR:")) {
                    ret = _ATOK;
                    goto GU906_SENDATRET;
                }
                else if(strstr(buff, "ERROR")){
                    ret = _ATERROR;
                    goto GU906_SENDATRET;   
                }
            break;

            case _ATCIPSTART:
                if(!GPRS_Dtu_ConLock)
                {
                    if(strstr(buff, "CONNECT OK")){
                        ret = _ATOK;
                        goto GU906_SENDATRET;
                    }
                    else if(strstr(buff, "RECONNECTING") || strstr(buff, "ERROR") || strstr(buff, "CONNECT FAIL")){
                        GetFreeBuff(100);
                        ret = _ATERROR;
                        goto GU906_SENDATRET;
                    }                    
                }
                else if(strstr(buff, "OK")){
                    ret = _ATOK;
                    goto GU906_SENDATRET;
                }
				else if(strstr(buff, "ERROR")){
                    ret = _ATERROR;
                    goto GU906_SENDATRET;   
                }
				break;
            
            case _GSMSENDEND: 
                GetFreeBuff(100);
                ret = _ATOK;
                goto GU906_SENDATRET; //忽略返回信息
                /*
                if(strstr(buff, "+CMGS:")) {
                    if(strstr(buff, "OK"))
                        return _ATOK;
                    lock = 1;
                }
                else if(lock && strstr(buff, "OK")) {
                    return _ATOK;
                }else return _ATOK; //忽略返回信息
                break;
                */
			case _ATCIPSCONT_C:
				if(strstr(buff,"OK"))
				{
					printf("Line:%d\r\n",__LINE__);
					if(0 != (p = strstr(buff,"+CIPMODE: ")))
					{
						p += 10;
						printf("Line:%d\r\n",__LINE__);
						if(1 == (*p -'0'))
						{
							printf("Line:%d\r\n",__LINE__);
							if(0 != (p = strstr(buff,"+CIPSTART: ")))
							{
								printf("Line:%d\r\n",__LINE__);
								if(strstr(buff,"218.66.59.201") && strstr(buff,"8888"))
								{
									printf("DTU OK\r\n");
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
				}else if(strstr(buff, "ERROR")){
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
		if(ret == _ATERROR) {
			if(++i >= num) return _ATERROR;
			delay_s(1);
		}else return _ATOTIME;
	}
	return _ATOK;
}

/*********************************************************
  * @function  GU906_init
  * @role      GSM初始化
  * @input     None
  * @output    None
  * @return    成功返回：_ATOK，失败返回：_ATERROR，超时返回：_ATOTIME
  ********************************************************/
s8 GU906_init(void)
{
	s8 ret = _ATOTIME;

    // 开回显:ATE1 关回显:ATE0
	if(_ATOK != (ret = GU906_ExecuteOrder(ATE(0), strlen(ATE(0)), _ATE, 2)))
		return ret;
	
	// 查询卡是否存在
	if(_ATOK != (ret = GU906_ExecuteOrder(ATESIM, strlen(ATESIM), _ATESIM, 10))) 
		return ret;

#if GU906GSM_EN
    // 设置短信模式为text模式
	if(_ATOK != (ret = GU906_ExecuteOrder(ATCMGF, strlen(ATCMGF), _ATCMGF, 2))) 
		return ret;

    // 设置短信存储单元为SIM卡
	if(_ATOK != (ret = GU906_ExecuteOrder(ATCPMS, strlen(ATCPMS), _ATCPMS, 2))) 
		return ret;

    // 设置这组参数来了新信息存储起来
	if(_ATOK != (ret = GU906_ExecuteOrder(ATCNMI, strlen(ATCNMI), _ATCNMI, 2))) 
		return ret;
#endif
    
    //删除SIM卡中的所有短信
	if(_ATOK != (ret = GU906_ExecuteOrder(ATCMGD, strlen(ATCMGD), _ATCMGD, 2))) 
		return ret;

    //查询信号强度 信号强度大于等于18才行
	while(_ATOK != (ret = GU906_ExecuteOrder(ATCSQ, strlen(ATCSQ), _ATCSQ, 60)))
	{
		if(ret == _ATOTIME) return ret;
	}
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
    //cipstart=(char *)mymalloc(100); 
    //if(cipstart==NULL) return -1; 
    sprintf(cipstart, ATCIPSTART,"TCP", GprsCon->server_ip, GprsCon->server_port);
	ret = GU906_ExecuteOrder(cipstart, strlen(cipstart), _ATCIPSTART, 3);
	
    //myfree(cipstart);
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
    
    //atorder=(char *)mymalloc(100); 
    //if(atorder==NULL) return -1; 
    
    //查询数据透设置情况
	if(_ATOK != (ret = GU906_ExecuteOrder(ATCIPSCONT_C, strlen(ATCIPSCONT_C), _ATCIPSCONT_C, 2))) 
		goto GU906_DTU_SOCKETEND;
 
    if(!GPRS_Dtu_ConLock)
	{
		//设置账号
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
    //myfree(atorder);
    return ret;
}

/*********************************************************
  * @function  GU906_DtuOrAT
  * @role      透传模式与AT模式转换
  * @input     None
  * @output    None
  * @return    成功返回：_ATOK，失败返回：_ATERROR，超时返回：_ATOTIME
  ********************************************************/
s8 GU906_DtuOrAT(u8 type)
{
    s8 ret = _ATERROR;
	if(type)
	{
		while(!GPRS_Dtu_ConLock)
		{
			//打开透传
			delay_s(2);
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
			delay_s(2);
			if(_ATOK != (ret = GU906_ExecuteOrder(CLOSEDTU, strlen(CLOSEDTU), _CLOSEDTU, 0)))
			{
				delay_s(1);
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
s8 GU906_GPRS_write(char* pdat, int len)
{
    char atorder[20] = "";
    s8 ret = -1;
    if(strlen(pdat) == 0) return 0;
	
    //atorder = (char *)mymalloc(20); 
    //if(atorder == NULL) return -1; 
	
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
	else
	{
		//发送数据
		usart4_Send(pdat, len);
		ret = _ATOK;
	}
    GU906_GPRS_WRITERET:
    //myfree(atorder);
    return ret;
}

/*********************************************************
  * @function  GU906_GPRS_read
  * @role      查询是否接收到数据
  * @input     输出缓存大小
  * @output    接收到的数据
  * @return    接收到的数据长度
  ********************************************************/
u32 GU906_GPRS_read(char *pout, int len)
{
    int i = 0;
	
	if(!GPRS_Dtu_ConLock)
	{
		GPRSREAD:
		if(GPRS_Dlen){
			for(i = 0;(i < GPRS_Dlen) && (i < (len -1)); i++){
				pout[i] = GPRS_Data[i];
			}
			memset(GPRS_Data, 0, sizeof(GPRS_Data));
			GPRS_Dlen = 0;
			return i;
		}else{
			GetFreeBuff(1);
			if(GPRS_Dlen)
				goto GPRSREAD;
		}	
	}
	else
	{
		return usart4_Receive(pout,len);
	}
    return 0;
}

/*********************************************************
  * @function  GU906_make_phone
  * @role      向指定的手机拨号
  * @input     手机号
  * @output    None
  * @return    成功返回：_ATOK，失败返回：_ATERROR，超时返回：_ATOTIME
  ********************************************************/
s8 GU906_make_phone(char *phone)
{
    char mphone[20]="";
    sprintf(mphone, ATATD, phone);  
    return GU906_ExecuteOrder(mphone, strlen(mphone), _ATATD, 0);
}

/*********************************************************
  * @function  GU906_Answer_Phone
  * @role      等待电话被接听
  * @input     手机号
  * @output    None
  * @return    成功返回：_ATOK，失败返回：_ATERROR，超时返回：_ATOTIME
  ********************************************************/
s8 GU906_Answer_Phone(u32 Delay)
{
	int siz = 0;
	u32 i = 0;
	char buff[MAXRECVBUFF] = "";
	
	i = 0;
	while(1)
	{
		siz = 0;
		siz = usart4_Receive(buff,MAXRECVBUFF);
		if(siz){
			GetRecvData(buff, &siz);
			if(strstr(buff, "+COLP:") && strstr(buff, "OK")){
				return _ATOK;
			}else if(strstr(buff, "NO CARRIER") || strstr(buff, "+CREG: 1") || strstr(buff, "ERROR")){
				return _ATERROR;
			}
		}
		if(i++ > Delay) 
		{
			return _ATOTIME;
		}
	}
}		
/*********************************************************
  * @function  GU906_end_phone
  * @role      挂机
  * @input     None
  * @output    None
  * @return    成功返回：_ATOK，失败返回：_ATERROR，超时返回：_ATOTIME
  ********************************************************/
s8 GU906_end_phone(void)
{
    return GU906_ExecuteOrder(ATATH, strlen(ATATH), _ATATH, 0);
}

#if GU906GSM_EN
/*********************************************************
  * @function  GU906_Chinese_text
  * @role      向指定的手机发送中文短信
  * @input     phone 手机号指针，pmsg 短消息指针
  * @output    None
  * @return    成功返回：_ATOK，失败返回：_ATERROR，超时返回：_ATOTIME
  ********************************************************/
s8 GU906_Chinese_text(char *phone,char* pmsg)
{
	s8 ret = _ATOTIME;
    char atphone[50] = "";
    char end[]={0x1A,0x00};
	
    if(strlen(phone) != 11)  return _ATERROR;
    //atphone = (char *)mymalloc(50); 
    //if(atphone == NULL) return -1; 
	
    //设置短消息为txet模式
	if(_ATOK != (ret = GU906_ExecuteOrder(ATCMGF, strlen(ATCMGF), _ATCMGF, 2))) 
		goto GU906_CHINESE_TEXTEND;
    
    //设置GB2312编码
	if(_ATOK != (ret = GU906_ExecuteOrder(ATGB2312, strlen(ATGB2312), _ATGB2312, 2))) 
		goto GU906_CHINESE_TEXTEND;
	
    //设置这组参数来了新信息存储起来 
	if(_ATOK != (ret = GU906_ExecuteOrder(ATCNMI, strlen(ATCNMI), _ATCNMI, 2))) 
		goto GU906_CHINESE_TEXTEND;
	
    //设置用户手机号
	sprintf(atphone,ATCMGS,phone);
	if(_ATOK != (ret = GU906_ExecuteOrder(atphone, strlen(atphone), _ATCMGS, 2))) 
		goto GU906_CHINESE_TEXTEND;
	
    //发送数据
	if(_ATOK == (ret = GU906_ExecuteOrder(pmsg, strlen(pmsg), _GSMSEND, 0))) 
	{
		ret = GU906_ExecuteOrder(end, 1, _GSMSENDEND, 0);
	}
	GU906_CHINESE_TEXTEND:
	//myfree(atphone);
    return ret;
}

/*********************************************************
  * @function  GU906_Read_SIM
  * @role      读取短信信息
  * @input     短信在SIM卡中的位置
  * @output    None
  * @return    成功返回：_ATOK，失败返回：_ATERROR，超时返回：_ATOTIME 
  ********************************************************/
static s8 GU906_Read_SIM(char *pnum)
{
	s8 ret = _ATOTIME;
    char cmgr[20]="";
    //读取短信的编码格式为GB2312
	if(_ATOK != (ret = GU906_ExecuteOrder(ATGB2312, strlen(ATGB2312), _ATGB2312, 2))) 
		return ret;
	
    //读取短消息
    sprintf(cmgr,ATCMGR,pnum);
    return GU906_ExecuteOrder(cmgr, strlen(cmgr), _ATCMGR, 2);
}

/*********************************************************
  * @function  GU906_DeleteSms
  * @role      删除SIM卡中的所有短信
  * @input     None
  * @output    None
  * @return    成功返回：_ATOK，失败返回：_ATERROR，超时返回：_ATOTIME 
  ********************************************************/
static int GU906_DeleteSms(void)
{
    return GU906_ExecuteOrder(ATCMGD, strlen(ATCMGD), _ATCMGD, 2);
}

/*********************************************************
  * @function  GU906_Read_UserSMS
  * @role      查询并读取短信数据
  * @input     None
  * @output    None
  * @return    0,接收到新数据，-1,未接收到新数据
  ********************************************************/
s8 GU906_Read_UserSMS(void)
{
    SMSREAD:
    if(strlen(SIMDataID)){
        #if DEBUG_EN
        printf("SIMDataID[%s]\r\n",SIMDataID);
        #endif
        GU906_Read_SIM(SIMDataID);
        GU906_DeleteSms();
        memset(SIMDataID,0,sizeof(SIMDataID));
        return 0;
    }else{
        GetFreeBuff(1);
        if(strlen(SIMDataID))
            goto SMSREAD;
    }
    return -1;
}
#endif


/*
int main(void)
{    
	u32 ret = 0;
	char buff[200]="";
	struct Gprs_Config GprsCon;
	delay_init();
	usart4_Configuration(115200);    //GU900默认通信波特率是115200
	usart1_Configuration(115200);   //调试输出端口波特率设置
	delay_s(5);//刚上电 要等待10秒，等待GU906模块初始化完成
	
	printf("\r\nBegin...\r\n");
	GprsCon.server_ip = (u8 *)"218.66.59.201";
	GprsCon.server_port = atoi("8888");
	
	//GSM初始化
	while(1)
	{
		if(_ATOK == GU906_init()){
			printf("GU906 init ok.\r\n\r\n");
			break;
		}
		printf("init error.\r\n");
		delay_s(1);
	}
	
	*****************************************************************************
	//GU906 GPRS TCP 非透传模式通信测试
	while(1)
	{
		if(_ATOK == GU906_TCP_Socket(&GprsCon))
		{
			printf("socket ok\r\n\r\n");
			delay_s(3);	
			while(1)
			{
				ret = GU906_GPRS_read(buff, 200);
				if(ret)
				{
					printf("GPRS:[%d][%s]\r\n", ret,buff);
					if(_ATOK != GU906_GPRS_write((char *)"OK", 2))
					{
						printf("Send Error.\r\n");
					}					
				}
			}
		}
		printf("GU906_TCP_Socket ERROR.\r\n");
		while(1);
	}
	*******************************************************************************
	
	*****************************************************************************
	//GU906 GPRS TCP 透传模式通信测试
	while(1)
	{
		if(_ATOK == GU906_DTU_Socket(&GprsCon))
		{
			printf("socket ok\r\n\r\n");
			delay_s(3);	
			while(1)
			{
				ret = GU906_GPRS_read(buff, 200);
				if(ret)
				{
					printf("GPRS:[%d][%s]\r\n", ret,buff);
					if(_ATOK != GU906_GPRS_write((char *)buff, ret))
					{
						printf("Send Error.\r\n");
					}					
					
					if(strstr(buff,"CLOSE"))
					{
						GU906_DtuOrAT(0);
					}
					if(strstr(buff,"OPEN"))
					{
						GU906_DtuOrAT(1);
					}
				}
			}
		}
		printf("GU906_TCP_Socket ERROR.\r\n");
		while(1);
	}
	*******************************************************************************
	
	*****************************************************************************
	//发送短信测试
    while(_ATOK != GU906_Chinese_text("18750895002", "123abd 测试"))
    {
        delay_s(5);
    }

	//接收短信测试
    while(1)
    {
        if(0 == GU906_Read_UserSMS())
        {
            printf("------------------------------\r\n");
            printf("号码:%s\r\n",sim.phone);
            printf("设备:%s\r\n",sim.dev);
            printf("时间:%s\r\n",sim.date);
            printf("信息:%s\r\n",sim.data);
        }
        delay_ms(50);
    }
	******************************************************************************
	
	*****************************************************************************
	//打电话测试
	if (_ATOK == GU906_make_phone("18750895002"))
	{
		//等待接听
		while(_ATOTIME == GU906_Answer_Phone(1000))
		{
			printf("make ok\r\n");
			GU906_end_phone();			
		}
		printf("make ok\r\n");
	}
	else 
	{
		printf("make error\r\n");
		//SoftReset();
	}
	*****************************************************************************
	while(1);
}
*/








