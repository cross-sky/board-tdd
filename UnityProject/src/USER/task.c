#include "cominc.h"

//add task
//1.add init 2.add task.h->task 3.add taskcomps

TPC_TASK TaskComps[]=
{
	{0, 0, 201,  201, LED1on},			// 5*200=1s   ok	
	{0, 0, 10,  10, TaskUart2TxStrData},	//task 50ms/每次，
	{0, 0, 10,  10, TaskUart1PopProcess},	//50ms每次
	{0, 0, 2,  2, Task1RegularSimpling},	//50ms每次
	{0, 0, 20,  20, Task2InputProcess}	//50ms每次
};


void TaskInit(void)
{
	//第2组：最高2位用于指定抢占式优先级-PreemptionPriority，最低2位用于指定响应优先级-SubPriority
	//“组”优先级别>“抢”占优先级别>“副”优先级别
	//数值越小所代表的优先级就越高,有必要设置组0
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_0);

	UB_LED_Init();

	UART1_Init();
	UART2_Init();

	vADCInit();
	vInp_init();

	vCd4051Init();

	//vrelayinit();
	//vqueinit();

	SystickInit();		//start at lsat
}



void TPCRemarks( TPC_TASK *pTask)
{
	u8 i;
	
	for ( i=0; i<TASKS_MAX; i++)
	{
		if( pTask[i].Timer > 0 && pTask[i].Suspend == 0)
		{
			pTask[i].Timer--;
			if (pTask[i].Timer == 0)
			{
				pTask[i].Timer = pTask[i].ItvTime;
				pTask[i].Run = TPC_RUN_STM; 
			}
		}
	}
}

void TPCProcess(TPC_TASK *pTask)
{
	u8 i;

	for (i=0; i<TASKS_MAX; i++)
	{
		if (pTask[i].Run == TPC_RUN_STM && pTask[i].Suspend == 0)
		{
			pTask[i].TaskHook();
			pTask[i].Run = TPC_RUN_CLM;
		}

	}
}



void vTPCChangeTaskRunTime(uint8_t ntasks,uint8_t newtime)
{
	if (ntasks>=TASKS_MAX)
	{
		return;
	}
	TaskComps[ntasks].Timer = newtime;
}

