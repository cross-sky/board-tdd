
#include "cominc.h"

/*
 * 函数名：main;
 * 描述  ：主函数;
 * 输入  ：无;
 * 输出  ：无;
 */




int main(void)
{	
	/* 初始化 */;
//=======
	TaskInit();
	
	while (1)
	{
		TPCProcess(TaskComps);
		
	}
}




/******************* (C) COPYRIGHT 2012 WildFire Team *****END OF FILE************/
