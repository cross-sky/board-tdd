#include "cominc.h"

#define VALVE_STEPS_ONECE	16	//默认每次运行最大步数

uint8_t valveCalcStartFlag= STATE_OFF;

typedef enum {
	IndexInitStep,
	IndexCloseStep,
	IndexMinStep,
	IndexMaxStep,
	ValveEnumMax
}ValveEnum;

int16_t TabValveParamsHeat[ValveKindsMax][ValveEnumMax]={
	{VALVE_INIT_STEP,VALVE_CLOSE_STEP,VALVE_MIN_STEP,VALVE_MAX_STEP},
	{VALVE_INIT_STEP,VALVE_CLOSE_STEP,VALVE_MIN_STEP,VALVE_MAX_STEP}
};

uint16_t TabValveDirect[IndexDirectMax]={
	DirectBack,DirectHold,DirectForward
};

//定义指向n维数组的指针，n就是除第一维的另外维度
//默认制热的步数
int16_t (*ptrTabValveParams)[ValveEnumMax] = TabValveParamsHeat;

//only change main valve
void ValveCalc_ChangeTabValveParams(int16_t maxValveStep)
{
	ptrTabValveParams[ValveMainA][IndexMaxStep] = maxValveStep;
}


//1.在funon中init退出前设置on，
//2.exit设置off
//3.funoff设置off
void ValveCalc_setStartFlag(uint8_t state)
{
	valveCalcStartFlag = state;
}

uint8_t ValveClac_getStartFlag(void)
{
	return valveCalcStartFlag;
}


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



int16_t getLowerLimit(void)
{
	return iQUE_getUpperLimit()-80;
}

int16_t getValveTotalStep(uint16_t valveKind)
{
	return valveStatus[valveKind].totalSteps;
}

uint8_t ValveCalc_popSig(ValveSig_t *sig)
{
	if (valveProcess.out != valveProcess.in)
	{
		if (++valveProcess.out >= ValveBuffLen)
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
		i=valveProcess.out;
		i++;
		if (i >= ValveBuffLen)
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
			ptrvalveStatus->runStep = ptrTabValveParams[tvalveKind][IndexInitStep];
			ptrvalveStatus->totalSteps = 0;
		}
		else
		{
			//close run total + 8;反向走
			ptrvalveStatus->runStep = 0-(ptrvalveStatus->totalSteps + ptrTabValveParams[tvalveKind][IndexCloseStep]);
			ptrvalveStatus->totalSteps = 0;
		}
	}
	//开度有限制 确保totalstep和runstep在范围内
	else if (tsig == valveRun)
	{
		if ((ptrvalveStatus->totalSteps + tcode) < ptrTabValveParams[tvalveKind][IndexMinStep] )
		{
			ptrvalveStatus->runStep = ptrTabValveParams[tvalveKind][IndexMinStep] - ptrvalveStatus->totalSteps;
			ptrvalveStatus->totalSteps = ptrTabValveParams[tvalveKind][IndexMinStep];
		}
		else if ((ptrvalveStatus->totalSteps + tcode) > ptrTabValveParams[tvalveKind][IndexMaxStep])
		{
			ptrvalveStatus->runStep = ptrTabValveParams[tvalveKind][IndexMaxStep] - ptrvalveStatus->totalSteps;
			ptrvalveStatus->totalSteps = ptrTabValveParams[tvalveKind][IndexMaxStep];
		}
		else{
			ptrvalveStatus->runStep = tcode;
			ptrvalveStatus->totalSteps += tcode;
		}
	}

	iQUE_ValveChanges((ValveKinds)tvalveKind, ptrvalveStatus->totalSteps);
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
			//push get pop 中最大值要一致，都是ValveBuffLen，不要定义太多，
			//否则深度不一样，会重复入队出队
			//要类似uart中统一一个函数确定最大最小值
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

void valveClac_runDone(ValveKinds valveKind)
{
	static uint8_t i=0;
	if (getValveState(valveKind) == statusRun)
	{
		//结束励磁保持300ms，
		if (i>=10)
		{
			i=0;
			//1. 步数已走完
			setValveState(statusDone, valveKind);
			//2. 步数结束时，先将电子膨胀阀的值清0，避免通电发热
			//3.刚开机时，会从步数1开始
			vRelay_backValveValue(valveKind);
		}
		i++;
	}
}

void ValveCalc_valveRun(ValveKinds valveKind)
{
	int16_t runStep;
	//static uint8_t i=0;
	runStep = getValveStep(valveKind);
	if (runStep == 0)
	{
		valveClac_runDone(valveKind);
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

			return;
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
	vTask4RelayOutProcess();
}


void valveDirectHold(int16_t subT, int16_t superHeat,ValveKinds valveKind )
{
	//目标过热度范围T+0.5 
	if (subT > superHeat+5)
	{
		//1.步数保持上次，开度加大，次数+1 TabValveDirect[IndexDirectHold],IndexDirectBack,IndexDirectHold,IndexDirectForward,
		//DirectBack,DirectHold,DirectForward
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
	int16_t workModel = iQUE_getWorkerModel();
	int16_t superHeat,subT;
	ValveSig_t sig;


	//0.1 排气温度>100,
	if (iQUE_getAirOutTemper() > AirOutTemperMax100)
	{
		sig.code = (iQUE_getAirOutTemper()- AirOutTemperMax100+10)/10;
		if (workModel == SIG_MAKE_COLD)
		{
			//制冷时电子膨胀阀要往小开
			sig.code = -sig.code;
		}
		sig.kindValue = ValveMainA;
		sig.sig = valveRun;
		ValveCalc_pushSig(&sig);
		return;
	}

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
		valveDirectBack(subT,superHeat,valveKind);
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
	if (workModel == SIG_MAKE_COLD)
	{
		sig.code = -sig.code;
	}
	sig.sig = valveRun;
	sig.kindValue = valveKind;
	ValveCalc_pushSig(&sig);
}

void ValveCalc_command5PushSig(int16_t data, uint16_t valveKind)
{
	ValveSig_t sig;
	sig.code = data;
	sig.sig = valveRun;
	sig.kindValue = valveKind;

	ValveCalc_pushSig(&sig);
}


void ValveCalc_valveClose(ValveKinds valveKind)
{
	//关机时关闭
	ValveSig_t valveSig;
	//..找到0位置
	valveSig.sig = valveClose;
	valveSig.code = 100;
	valveSig.kindValue = valveKind;
	ValveCalc_pushSig(&valveSig);
}


//计算补气电子膨胀阀
void ValveCalc_calcValveSub(ValveKinds valveKind)
{
	int16_t airoutT = iQUE_getAirOutTemper();
	int16_t value = airoutT - iQUE_getWaterBankTemper();
	int16_t steps=0;
	ValveSig_t sig;

	if (airoutT >= AirOutTemperMax100)
	{
		//1. run (airoutT - 110 )/20 steps,
		steps = (airoutT- AirOutTemperMax100 - 10)/20;
		sig.code = steps;
		sig.kindValue = ValveSubB;
		sig.sig = valveRun;
		ValveCalc_pushSig(&sig);
		return;
	}

/*
	//环温>=5度，关阀；增加如果排气-水温<10度--暂时不能  || value <= (iQUE_getUpperLimit() - 100)
	if (iQUE_getEvirTemper() >= EnvirTemper5 )
	{
		if (getValveTotalStep(valveKind) != 0)
		{
			sig.code = 500;	//any value but 0
			sig.kindValue = ValveSubB;
			sig.sig = valveClose;
			ValveCalc_pushSig(&sig);
		}
		return;
	}else
	{
		//补气电子膨胀阀初始开度设置
		//1.查询到补气开度为0时，重新设置初始开度
		if (getValveTotalStep(ValveSubB) == 0)
		{
			sig.code = 300;
			sig.kindValue = ValveSubB;
			sig.sig = valveRun;
			ValveCalc_pushSig(&sig);
			return;
		}
	}*/

	
	//1. >20度，阀开大
	//2. <16度，阀开小
	//3. 16-20之间，阀开度不变
	if (value > iQUE_getUpperLimit()  )
	{
		//run (value- upperLimit)/20 steps, max run 4 steps
		steps = (value - iQUE_getUpperLimit())/20;
		if (steps>4)
		{
			steps = 4;
		}
	}else if (value < getLowerLimit())
	{
		steps = (value - getLowerLimit())/20;
		if (steps < -4)
		{
			steps = -4;
		}
	}else{
		steps = 0;
	}

	sig.code = steps;	//any value but 0
	sig.kindValue = ValveSubB;
	sig.sig = valveRun;
	ValveCalc_pushSig(&sig);
}

void ValveCalc_valveInit(void)
{
	//1.上电初始化，2.进入除霜进行一次初始化
	ValveSig_t valveSig;

	//..找到0位置 往回走
	valveSig.sig = valveInit;
	valveSig.code = 300;
	valveSig.kindValue = ValveMainA;
	ValveCalc_pushSig(&valveSig);
	valveSig.kindValue = ValveSubB;
	ValveCalc_pushSig(&valveSig);
	//主膨胀阀初始开度
	valveSig.sig = valveRun;
	valveSig.code = VALVE_INITRUN_STEP;
	valveSig.kindValue = ValveMainA;
	ValveCalc_pushSig(&valveSig);
	//..补气阀默认关闭，上电时，应该不会有温度，不能根据环温设置初始开度
}

void ValveCalc_stepsSetTo(ValveKinds valveKind, int16_t steps)
{
	ValveSig_t valveSig;
	valveSig.sig = valveRun ;
	valveSig.code = steps - getValveTotalStep(ValveMainA);
	valveSig.kindValue = valveKind;
	ValveCalc_pushSig(&valveSig);
}

//除霜时进行一次调零
void ValveCalc_defrostValveSet(void)
{
	ValveCalc_valveInit();
	ValveCalc_stepsSetTo(ValveMainA, VALVE_MIN_STEP);
}

//制冷制热模式切换时，重新设置参数
void ValveCalc_WorkerModelChangeParams(void)
{
	int16_t workModel = iQUE_getWorkerModel();

	switch(workModel)
	{
	case SIG_MAKE_COLD:
		{
			//1.制冷时电子膨胀阀最大步数120
			ValveCalc_ChangeTabValveParams(VALVE_COLDMAX_STEP);
			//2.主电子膨胀阀开度设置到30
			ValveCalc_stepsSetTo(ValveMainA, VALVE_MIN_STEP);
			/*valveSig.sig = valveRun ;
			valveSig.code = VALVE_MIN_STEP - getValveTotalStep(ValveMainA);
			valveSig.kindValue = ValveMainA;
			ValveCalc_pushSig(&valveSig);*/
			break;
		}
	case SIG_MAKE_HotWater:
		{
			//1.制冷时电子膨胀阀最大步数470
			ValveCalc_ChangeTabValveParams(VALVE_MAX_STEP);
			//2.主电子膨胀阀开度设置到100
			/*valveSig.sig = valveRun;
			valveSig.code = VALVE_INITRUN_STEP  - getValveTotalStep(ValveMainA);
			valveSig.kindValue = ValveMainA;
			ValveCalc_pushSig(&valveSig);*/
			ValveCalc_stepsSetTo(ValveMainA, VALVE_INITRUN_STEP);
			break;
		}
	default:break;
	}
}



void vTask_valveCalc(void)
{	
	//100ms * 10 * 120s
	static uint16_t i=0;
	ValveSig_t sig;
	if (ValveClac_getStartFlag() == STATE_ON)
	{
		i++;
		if (i>=900)
		{
			ValveCalc_calcValveMain(ValveMainA);

			if (iQUE_getWorkerModel() == SIG_MAKE_HotWater)
			{
				ValveCalc_calcValveSub(ValveSubB);
			}
			else{
				//1. 如果补齐阀是打开的，则关闭，
				if (getValveTotalStep(ValveSubB) != 0)
				{
					sig.code = 500;	//any value but 0
					sig.kindValue = ValveSubB;
					sig.sig = valveClose;
					ValveCalc_pushSig(&sig);
				}
			}
			i=0;
		}

	}else{
		i=0;
	}
}
