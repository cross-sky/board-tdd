#include "cominc.h"

#define VALVE_STEPS_ONECE	16	//默认每次运行最大步数
//int16_t LastStep=VALVE_STEPS_ONECE;	//最后一次步数
//int16_t ValveDirection=0;	//运行方向direct**
//int16_t ValveCounts=0;		//连续增加或减少次数

ValveStatus_t valveStatus[ValveKindsMax]={
	{statusDone,0,0,VALVE_STEPS_ONECE,DirectHold,0},
	{statusDone,0,0,VALVE_STEPS_ONECE,DirectHold,0}
};
ValveStatus_t* prtvalveStatus= valveStatus;

ValveProcess_t valveProcess={
	0,0
};

void ValveCalv_changeValveData(ValveStatus_t* src)
{
	prtvalveStatus = src;
}

ValveProcess_t* ValveCalc_getSigNodes(void)
{
	return &valveProcess;
}

uint8_t isValueKindUsed(ValveKinds valveKind)
{
	return valveKind < ValveKindsMax ;
}

void setValveState(VALVE_ENUM state, ValveKinds valveKind)
{
	if (isValueKindUsed(valveKind))
	{
		prtvalveStatus[valveKind].valveStatus = state;
	}
}

uint16_t getValveState(ValveKinds valveKind)
{
	return prtvalveStatus[valveKind].valveStatus;
}

int16_t getValveStep(ValveKinds valveKind)
{
	return prtvalveStatus[valveKind].runStep;
}

uint8_t ValveCalc_popSig(ValveSig_t *sig)
{
	if (valveProcess.out != valveProcess.in)
	{
		if (++valveProcess.out >= ValveBuffLenMask)
		{
			valveProcess.out = 0;
		}
		sig->code = valveProcess.buff[valveProcess.out].code;
		sig->sig = valveProcess.buff[valveProcess.out].sig;
		sig->kindValue = valveProcess.buff[valveProcess.out].kindValue;
		return TRUE;
	}
	return FALSE;
}

uint8_t ValveCalc_getSig(ValveSig_t *sig)
{
	uint8_t i;
	if (valveProcess.out != valveProcess.in)
	{
		i=++valveProcess.out;
		if (i >= ValveBuffLenMask)
		{
			i = 0;
		}
		sig->code = valveProcess.buff[i].code;
		sig->sig = valveProcess.buff[i].sig;
		sig->kindValue = valveProcess.buff[i].kindValue;
		return TRUE;
	}
	return FALSE;
}

void checkTotalSteps(ValveSig_t *sig)
{
	uint16_t tsig=sig->sig;
	int16_t tcode=sig->code;
	uint16_t tvalveKind = sig->kindValue;

	ValveStatus_t *ptrvalveStatus = &prtvalveStatus[tvalveKind];
	//判断是初始或结束时，开度范围加大
	if (tsig == valveInit || tsig == valveClose)
	{
		if (tsig == valveInit)
		{
			ptrvalveStatus->runStep = VALVE_INIT_STEP;
			ptrvalveStatus->totalSteps = 0;
		}
		else
		{
			ptrvalveStatus->runStep = VALVE_CLOSE_STEP - ptrvalveStatus->totalSteps;
			ptrvalveStatus->totalSteps = VALVE_CLOSE_STEP;
		}
	}
	//开度有限制 确保totalstep和runstep在范围内
	else if (tsig == valveRun)
	{
		if ((ptrvalveStatus->totalSteps + tcode) < VALVE_MIN_STEP )
		{
			ptrvalveStatus->runStep = VALVE_MIN_STEP - ptrvalveStatus->totalSteps;
			ptrvalveStatus->totalSteps = VALVE_MIN_STEP;
		}
		else if ((ptrvalveStatus->totalSteps + tcode) > VALVE_MAX_STEP)
		{
			ptrvalveStatus->runStep = VALVE_MAX_STEP - ptrvalveStatus->totalSteps;
			ptrvalveStatus->totalSteps = VALVE_MAX_STEP;
		}
		else{
			ptrvalveStatus->runStep = tcode;
			ptrvalveStatus->totalSteps += tcode;
		}
	}
}

//队列有个缺点，a 正在工作，而队列下一个是A事件，当b已完成时，无法查找下一个B事件
uint8_t ValveCalc_pushSig(ValveSig_t *srcSig)
{
	int16_t code = srcSig->code;
	uint16_t sig = srcSig->sig;
	ValveKinds valveKind =(ValveKinds)srcSig->kindValue;
	if (((valveProcess.in+1)&ValveBuffLenMask) == valveProcess.out)
	{
		return FALSE;
	}
	else{
		if (code != 0)
		{
			//valveProcess.in++;
			if (++valveProcess.in >= ValveBuffLen)
			{
				valveProcess.in=0;
			}
			valveProcess.buff[valveProcess.in].code = code;
			valveProcess.buff[valveProcess.in].sig = sig;
			valveProcess.buff[valveProcess.in].kindValue = valveKind;
			return TRUE;
		}
		return FALSE;
	}
}

void stepChange(VALVE_ENUM addsOrDecr, ValveKinds valveKind)
{
	if (addsOrDecr == adds)
	{
		prtvalveStatus[valveKind].runStep++;
	}
	else if (addsOrDecr == decr)
	{
		prtvalveStatus[valveKind].runStep--;
	}
}

void ValveCalc_valveRun(ValveKinds valveKind)
{
	int16_t runStep;
	runStep = getValveStep(valveKind);
	if (runStep == 0)
	{
		//步数已走完
		setValveState(statusDone, valveKind);
		return;
	}
	if (runStep<0)
	{
		//返回走，总步数减少
		//运行步数增加到0
		stepChange(adds, valveKind);
		HardValve_runDirect(adds, valveKind);
	}
	else{
		//往前走，总步数增加
		//运行步数减少到0
		stepChange(decr, valveKind);
		HardValve_runDirect(decr, valveKind);
	}
}

void ValveCalc_checkProcess(ValveKinds valveKind)
{
	ValveSig_t sig;
	//1.查询valve状态是否完成
	if (!isValueKindUsed(valveKind))
	{
		return;
	}
	if (getValveState(valveKind) == statusDone)
	{
		//2.检查步数
		if ((ValveCalc_getSig(&sig) == TRUE) && sig.kindValue==valveKind)
		{
			//2.5 检查步数在范围内
			sig.kindValue = valveKind;
			checkTotalSteps(&sig);
			//3.设置状态run
			setValveState(statusRun,valveKind);
			ValveCalc_popSig(&sig);
		}
	}
	//4.根据步数运行
	ValveCalc_valveRun(valveKind);
}

void vTask_valveProcess(void)
{
	ValveCalc_checkProcess(ValveMainA);
	ValveCalc_checkProcess(ValveSubB);
	//IO output.........
}


void valveDirectHold(int16_t subT, int16_t superHeat,ValveKinds valveKind )
{
	//目标过热度范围T+0.5 
	if (subT > superHeat+5)
	{
		//1.步数保持上次，开度加大，次数+1
		prtvalveStatus[valveKind].valveDirection = DirectForward;
		prtvalveStatus[valveKind].valveCounts +=1;
	}
	else if (subT < superHeat - 5)
	{
		//2.步数保持上次，开度减少，次数+1
		prtvalveStatus[valveKind].valveDirection = DirectBack;
		prtvalveStatus[valveKind].valveCounts +=1;
	}
	else{
		//步数，开度保持，次数清0
		prtvalveStatus[valveKind].valveDirection = DirectHold;
		prtvalveStatus[valveKind].valveCounts = 0;
	}
}

void valveDirectForward(int16_t subT, int16_t superHeat,ValveKinds valveKind)
{
	//目标过热度范围T+0.5 
	if (subT > superHeat+5)
	{
		//1.步数保持上次，开度加大，次数+1
		prtvalveStatus[valveKind].valveDirection = DirectForward;
		prtvalveStatus[valveKind].valveCounts +=1;
	}
	else if (subT < superHeat - 5)
	{
		//2.步数减半，开度减少，次数保持
		prtvalveStatus[valveKind].lastStep >>=1;
		prtvalveStatus[valveKind].valveDirection = DirectBack;
	}
	else{
		//步数，开度保持，次数清0
		prtvalveStatus[valveKind].valveDirection = DirectHold;
		prtvalveStatus[valveKind].valveCounts = 0;
	}
}

void valveDirectBack(int16_t subT, int16_t superHeat,ValveKinds valveKind)
{
	//目标过热度范围T+0.5 
	if (subT > superHeat+5)
	{
		//1.步数减半，开度加大，次数保持
		prtvalveStatus[valveKind].lastStep >>=1;
		prtvalveStatus[valveKind].valveDirection = DirectForward;
	}
	else if (subT < superHeat - 5)
	{
		//2.步数保持上次，开度减少，次数+1
		prtvalveStatus[valveKind].valveDirection = DirectBack;
		prtvalveStatus[valveKind].valveCounts +=1;
	}
	else{
		//步数，开度保持上次，次数清0
		prtvalveStatus[valveKind].valveDirection = DirectHold;
		prtvalveStatus[valveKind].valveCounts = 0;
	}
}

//计算主电子膨胀阀
void ValveCalc_calcValveMain(ValveKinds valveKind)
{
	int16_t superHeat,subT;
	ValveSig_t sig;
	//1. 计算吸气-蒸发
	subT = iQUE_getInTemper() - iQUE_getEvaporateTemper();
	//2.获取目标过热度
	superHeat = iQUE_getSuperheat();

	//3.1上次电子膨胀阀维持开度
	if (prtvalveStatus[valveKind].valveDirection == DirectHold)
	{
		valveDirectHold(subT,superHeat,valveKind);
	}
	//3.2上次电子膨胀阀开度增加
	else if (prtvalveStatus[valveKind].valveDirection == DirectForward)
	{
		valveDirectForward(subT,superHeat,valveKind);
	}
	//3.3上次电子膨胀阀开度减少
	else
	{
		valveDirectHold(subT,superHeat,valveKind);
	}
	//4.连续增加或减少次数>=4，重置步数和次数
	if (prtvalveStatus[valveKind].valveCounts >=4)
	{
		if (prtvalveStatus[valveKind].valveDirection == DirectForward || prtvalveStatus[valveKind].valveDirection == DirectBack)
		{
			prtvalveStatus[valveKind].lastStep = VALVE_STEPS_ONECE;
			prtvalveStatus[valveKind].valveCounts = 0;
		}
	}

	//步数<1,步数置1
	if (prtvalveStatus[valveKind].lastStep <= 1 )
	{
		prtvalveStatus[valveKind].lastStep = 1;
	}

	//5.发送步数LastStep*ValveDirection, valveRun,
	//步数*方向 区分正反转
	sig.code = prtvalveStatus[valveKind].lastStep*(int16_t)(prtvalveStatus[valveKind].valveDirection - DirectHold);
	sig.sig = valveRun;
	sig.kindValue = valveKind;
	ValveCalc_pushSig(&sig);
}



/*
void vTask_valveCalc(void)
{	
}
*/
