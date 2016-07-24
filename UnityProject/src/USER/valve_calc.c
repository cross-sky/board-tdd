#include "cominc.h"

#define VALVE_STEPS_ONECE	16	//Ĭ��ÿ�����������
//int16_t LastStep=VALVE_STEPS_ONECE;	//���һ�β���
//int16_t ValveDirection=0;	//���з���direct**
//int16_t ValveCounts=0;		//�������ӻ���ٴ���

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
	//�ж��ǳ�ʼ�����ʱ�����ȷ�Χ�Ӵ�
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
	//���������� ȷ��totalstep��runstep�ڷ�Χ��
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

//�����и�ȱ�㣬a ���ڹ�������������һ����A�¼�����b�����ʱ���޷�������һ��B�¼�
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
		//����������
		setValveState(statusDone, valveKind);
		return;
	}
	if (runStep<0)
	{
		//�����ߣ��ܲ�������
		//���в������ӵ�0
		stepChange(adds, valveKind);
		HardValve_runDirect(adds, valveKind);
	}
	else{
		//��ǰ�ߣ��ܲ�������
		//���в������ٵ�0
		stepChange(decr, valveKind);
		HardValve_runDirect(decr, valveKind);
	}
}

void ValveCalc_checkProcess(ValveKinds valveKind)
{
	ValveSig_t sig;
	//1.��ѯvalve״̬�Ƿ����
	if (!isValueKindUsed(valveKind))
	{
		return;
	}
	if (getValveState(valveKind) == statusDone)
	{
		//2.��鲽��
		if ((ValveCalc_getSig(&sig) == TRUE) && sig.kindValue==valveKind)
		{
			//2.5 ��鲽���ڷ�Χ��
			sig.kindValue = valveKind;
			checkTotalSteps(&sig);
			//3.����״̬run
			setValveState(statusRun,valveKind);
			ValveCalc_popSig(&sig);
		}
	}
	//4.���ݲ�������
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
	//Ŀ����ȶȷ�ΧT+0.5 
	if (subT > superHeat+5)
	{
		//1.���������ϴΣ����ȼӴ󣬴���+1
		prtvalveStatus[valveKind].valveDirection = DirectForward;
		prtvalveStatus[valveKind].valveCounts +=1;
	}
	else if (subT < superHeat - 5)
	{
		//2.���������ϴΣ����ȼ��٣�����+1
		prtvalveStatus[valveKind].valveDirection = DirectBack;
		prtvalveStatus[valveKind].valveCounts +=1;
	}
	else{
		//���������ȱ��֣�������0
		prtvalveStatus[valveKind].valveDirection = DirectHold;
		prtvalveStatus[valveKind].valveCounts = 0;
	}
}

void valveDirectForward(int16_t subT, int16_t superHeat,ValveKinds valveKind)
{
	//Ŀ����ȶȷ�ΧT+0.5 
	if (subT > superHeat+5)
	{
		//1.���������ϴΣ����ȼӴ󣬴���+1
		prtvalveStatus[valveKind].valveDirection = DirectForward;
		prtvalveStatus[valveKind].valveCounts +=1;
	}
	else if (subT < superHeat - 5)
	{
		//2.�������룬���ȼ��٣���������
		prtvalveStatus[valveKind].lastStep >>=1;
		prtvalveStatus[valveKind].valveDirection = DirectBack;
	}
	else{
		//���������ȱ��֣�������0
		prtvalveStatus[valveKind].valveDirection = DirectHold;
		prtvalveStatus[valveKind].valveCounts = 0;
	}
}

void valveDirectBack(int16_t subT, int16_t superHeat,ValveKinds valveKind)
{
	//Ŀ����ȶȷ�ΧT+0.5 
	if (subT > superHeat+5)
	{
		//1.�������룬���ȼӴ󣬴�������
		prtvalveStatus[valveKind].lastStep >>=1;
		prtvalveStatus[valveKind].valveDirection = DirectForward;
	}
	else if (subT < superHeat - 5)
	{
		//2.���������ϴΣ����ȼ��٣�����+1
		prtvalveStatus[valveKind].valveDirection = DirectBack;
		prtvalveStatus[valveKind].valveCounts +=1;
	}
	else{
		//���������ȱ����ϴΣ�������0
		prtvalveStatus[valveKind].valveDirection = DirectHold;
		prtvalveStatus[valveKind].valveCounts = 0;
	}
}

//�������������ͷ�
void ValveCalc_calcValveMain(ValveKinds valveKind)
{
	int16_t superHeat,subT;
	ValveSig_t sig;
	//1. ��������-����
	subT = iQUE_getInTemper() - iQUE_getEvaporateTemper();
	//2.��ȡĿ����ȶ�
	superHeat = iQUE_getSuperheat();

	//3.1�ϴε������ͷ�ά�ֿ���
	if (prtvalveStatus[valveKind].valveDirection == DirectHold)
	{
		valveDirectHold(subT,superHeat,valveKind);
	}
	//3.2�ϴε������ͷ���������
	else if (prtvalveStatus[valveKind].valveDirection == DirectForward)
	{
		valveDirectForward(subT,superHeat,valveKind);
	}
	//3.3�ϴε������ͷ����ȼ���
	else
	{
		valveDirectHold(subT,superHeat,valveKind);
	}
	//4.�������ӻ���ٴ���>=4�����ò����ʹ���
	if (prtvalveStatus[valveKind].valveCounts >=4)
	{
		if (prtvalveStatus[valveKind].valveDirection == DirectForward || prtvalveStatus[valveKind].valveDirection == DirectBack)
		{
			prtvalveStatus[valveKind].lastStep = VALVE_STEPS_ONECE;
			prtvalveStatus[valveKind].valveCounts = 0;
		}
	}

	//����<1,������1
	if (prtvalveStatus[valveKind].lastStep <= 1 )
	{
		prtvalveStatus[valveKind].lastStep = 1;
	}

	//5.���Ͳ���LastStep*ValveDirection, valveRun,
	//����*���� ��������ת
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
