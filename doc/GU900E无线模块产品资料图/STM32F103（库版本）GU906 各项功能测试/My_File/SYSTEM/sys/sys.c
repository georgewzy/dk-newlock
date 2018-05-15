#include "sys.h"


void NVIC_Configuration(void)
{
    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);	//设置NVIC中断分组2:2位抢占优先级，2位响应优先级
}
void SoftReset(void)
{
	__set_FAULTMASK(1);      // 关闭所有中端
	NVIC_SystemReset();// 复位
}
