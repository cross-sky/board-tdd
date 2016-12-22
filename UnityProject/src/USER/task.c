#include "cominc.h"

//add task
//1.add init 2.add task.h->task 3.add taskcomps

TPC_TASK TaskComps[]=
{
	{0, 0, 201,  201, LED1on},			// 5*200=1s   ok	
	{0, 0, 10,  10, TaskUart2TxStrData},	//task 50ms/ÿ�Σ�
	{0, 0, 10,  10, TaskUart1PopProcess},	//50msÿ��
	{0, 0, 2,  2, Task1RegularSimpling},	//10msÿ��
	{0, 0, 20,  20, Task2InputProcess},	//100msÿ��
	{0, 0, 7,  7, vTask_valveProcess},	//30msÿ��
	{0, 0, 10,  10, vTaskQUEProcess},		//5*10msÿ��
	{0, 0, 20,  20, vTask_valveCalc},	//50msÿ��
};


void TaskInit(void)
{
	//��2�飺���2λ����ָ����ռʽ���ȼ�-PreemptionPriority�����2λ����ָ����Ӧ���ȼ�-SubPriority
	//���顱���ȼ���>������ռ���ȼ���>���������ȼ���
	//��ֵԽС����������ȼ���Խ��,�б�Ҫ������0
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_0);

	UB_LED_Init();

	UART1_Init();
	UART2_Init();

	vADCInit();
	vInp_init();

	vCd4051Init();
	vRelayInit();

	vQUEInit();
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

void vTPCSuspendTask(uint8_t ntasks,uint8_t state)
{
	if (ntasks>=TASKS_MAX)
	{
		return;
	}
	TaskComps[ntasks].Suspend = state;
}

