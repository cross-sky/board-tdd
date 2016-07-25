#include "cominc.h"

QueueProcess_t queueProcess={
	0,0
};

//1.init @@@@@@@@
CoreProcess_t coreProcess;

//����/�˳�״̬ �����������0
uint32_t timeFlag=0;

//!!!!!!!ÿ��case ���������һ��break/return ����ᴦ�������������������

#define Time0s	0
#define Time3s	20*3
#define Time10s 20*10
#define Time60s	20*60
#define Time180s	20*180
#define Time360S	20*360

CoreProcess_t* xQue_getCoreData(void)
{
	return &coreProcess;
}

uint32_t getTimeFlag(void)
{
	return ++timeFlag;
}

void clearTimeFlag(void)
{
	timeFlag = 0;
}

uint8_t xQUESigPop(void)
{
	QueueSig_t sig;

	if (queueProcess.out != queueProcess.in)
	{
		queueProcess.out++;
		if (queueProcess.out>=QueueBuffLen)
		{
			queueProcess.out=0;
		}
		sig.sig=queueProcess.buff[queueProcess.out].sig;
	}
	else
	{
		sig.sig=SIG_NULL;
	}
	return sig.sig;
}

uint8_t xQUEgetSig(void)
{
	uint8_t sig;
	uint8_t tout = queueProcess.out;

	if (tout != queueProcess.in)
	{
		tout++;
		if (tout>=QueueBuffLen)
		{
			tout=0;
		}
		sig=queueProcess.buff[tout].sig;
	}
	else
	{
		sig=SIG_NULL;
	}
	return sig;
}



uint8_t xQUESigPush(uint8_t sig)
{
	if (((queueProcess.in+1)&QueueBuffLenMask) == queueProcess.out)
	{
		return FALSE;
	}

	if (sig != SIG_NULL)
	{
		queueProcess.in++;
		if (queueProcess.in >= QueueBuffLen)
		{
			queueProcess.in=0;
		}
		queueProcess.buff[queueProcess.in].sig=sig;
		return TRUE;
	}
	return FALSE;
}

//��ȡ�����¶�
int16_t iQUE_getInTemper(void)
{
	return coreProcess.coreParems.machineA.inTemper;
}

//��ȡ�����¶�
int16_t iQUE_getEvaporateTemper(void)
{
	return coreProcess.coreParems.machineA.evaporateTemper;
}

//���ݻ��£������¶ȣ�ȷ�����ʵĹ��ȶ�
int16_t iQUE_getSuperheat(void)
{
	return 30;
}


///���ݻ������ý��������¶�ֵ     
void vQUEDefrostInTemper(void)
{
	int16_t tdata;
	if (coreProcess.coreParems.environT > -1)
	{
		tdata=-6*10 ;
	}
	else
	{
		tdata=(int16_t)(coreProcess.coreParems.environT*0.77-54.22);
	}

	coreProcess.coreParems.setDefrostInTemper=tdata-coreProcess.coreParems.setDefrostInOffset;

}


void vqueSetMachineState(SigState state)
{
	coreProcess.runState.machineState = state;
}

uint8_t vqueGetMachineState(void)
{
	return coreProcess.runState.machineState;
}

uint8_t vqueGetFunState(void)
{
	return coreProcess.funState;
}

void vqueSetNewFuntion(ptrfuntion newFun)
{
	coreProcess.prefunction=coreProcess.functions;
	coreProcess.functions=newFun;
}

uint8_t vQueCheck3MinDelay(void)
{
	static uint8_t flag=STATE_OFF;
	static uint16_t delay3Min=STATE_OFF;
	if (vqueGetMachineState() == SIG_ON || vqueGetMachineState() == SIG_DEFROST)
	{
		flag = STATE_OFF;
	}
	else
	{
		if (flag != STATE_ON)
		{
			delay3Min++;
			if (delay3Min >= Time180s)
			{
				flag = STATE_ON;
				delay3Min=STATE_OFF;
			}
		}
	}

	return flag;
}

void vInpSetCoreParamADC(ptrInputProcessStruct srcData)
{
	coreProcess.coreParems.machineA.inTemper=srcData->realNtc[ADC03_AIN];
	coreProcess.coreParems.machineA.evaporateTemper=srcData->realNtc[ADC04_AMI];
	coreProcess.coreParems.machineA.outTemper=srcData->realNtc[ADC05_AOUT];
	coreProcess.coreParems.machineA.current=srcData->realNtc[ADC09_current];
	coreProcess.coreParems.waterIn=srcData->realNtc[ADC06_WIN];
	coreProcess.coreParems.waterOut=srcData->realNtc[ADC07_WOUT];
	coreProcess.coreParems.environT=srcData->realNtc[ADC01_ENVT];
	coreProcess.coreParems.waterBank=srcData->realNtc[ADC02_WTANK];
	coreProcess.coreParems.innerTemper=srcData->realNtc[ADC8_INTER];
	vQUEDefrostInTemper();
}

void vInpSetCoreParamCd4051(uint16_t srcData)
{
	coreProcess.coreParems.cd4051DectState = srcData;
}

void vQueSetCoreParamErrstate(uint16_t errType)
{
	coreProcess.coreParems.errState = errType;
}

//�����¼�����ʱת�룬 
uint8_t vqueFunErr(void)
{
	switch(coreProcess.funState)
	{
	case FUN_STATE_INIT:
		{
			//@@@@ֹͣ�̵���������������ͷ���0��
			//ִ��1������,���ص�ǰ״̬
			//500ms��˸
			//vTPCChangeTaskRunTime(TASK_LED1ON, 100);
			return FUN_STATE_INIT;
		}
	case FUN_STATE_RUN:
		{
			//�޶���
			//���ؿ��źţ���Ҫ�ⲿ�����޴����¼������ⲿ�¼�����ǿ���˳�
			return SIG_NULL;
		}
	case FUN_STATE_EXIT:
		{
			//�˳��󣬰�ǰһ������ָ�븳ֵ����ǰ����ָ��
			//ִ��1������,���ص�ǰ״̬
			//����������1s��˸
			//vTPCChangeTaskRunTime(TASK_LED1ON, 200);
			return FUN_STATE_EXIT;
		}
	default:
		{
			//�޶���
			return SIG_NULL;
		}
	}
}


uint8_t vqueFunOff(void)
{
	switch(coreProcess.funState)
	{
	case FUN_STATE_INIT:
		{
			//�޶��� ֱ���˳�
			vRelaySet(Relay10Motor, STATE_OFF);
			vRelaySet(Relay03Pump, STATE_OFF);
			vRelaySet(Relay02Valve4way, STATE_OFF);
			vRelaySet(Relay01Compressor, STATE_OFF);

			return FUN_STATE_INIT;
		}
	case FUN_STATE_RUN:
		{
			//�޶��� ֱ���˳�
			return SIG_NULL;
		}
	case FUN_STATE_EXIT:
		{
			//�޶��� ֱ���˳�
			return FUN_STATE_EXIT;
		}
	default:
		{
			//�޶���
			return SIG_NULL;
		}
	}
}

uint8_t vqueFunOn(void)
{
	switch(coreProcess.funState)
	{
	case FUN_STATE_INIT:
		{
			
			//�޶��� ֱ���˳�
			switch(timeFlag)
			{
			case Time3s:
				{
					vRelaySet(Relay03Pump, STATE_ON);
					IODECT_startCheckWaterOpen();
					break;
				}
			case Time10s:
				{
					//2@@@@@@@@@@@���ˮ���պ�
					vRelaySet(Relay10Motor, STATE_ON);
					if (vQueCheck3MinDelay() == STATE_ON)
					{
						timeFlag = Time180s-1;
					}
					break;
				}
			case Time180s:
				{
					//����ѹ����
					vRelaySet(Relay01Compressor,STATE_ON);
					timeFlag=0;
					return FUN_STATE_INIT;
				}
			default:break;
			}

			timeFlag++;
			return SIG_NULL;
		}
	case FUN_STATE_RUN:
		{
			//�޶��� ֱ���˳�
			//�����¶�<�趨ֵ �����˪
			if (coreProcess.coreParems.machineA.evaporateTemper <= coreProcess.coreParems.setDefrostInTemper
				&& timeFlag>= coreProcess.coreParems.setDefrostCycleTimes)
			{
				timeFlag=0;
				xQUESigPush(SIG_DEFROST);
				return SIG_NULL;
			}
			//ˮ���¶�>�趨�¶� ���뱣��
			if (coreProcess.coreParems.waterBank >= coreProcess.coreParems.SetWaterTSet)
			{
				timeFlag=0;
				xQUESigPush(SIG_HOLD);
				return SIG_NULL;
			}
			timeFlag++;
			return SIG_NULL;
		}
	case FUN_STATE_EXIT:
		{
			//�޶��� ֱ���˳�

			switch(timeFlag)
			{
			case Time3s:
				{
					vRelaySet(Relay01Compressor,STATE_OFF);
					IODECT_stopCheckWaterOpen();
					break;
				}
			case Time10s:
				{
					vRelaySet(Relay10Motor, STATE_OFF);
					vRelaySet(Relay03Pump, STATE_OFF);
					timeFlag=0;
					return FUN_STATE_EXIT;
				}
			default:break;
			}
			timeFlag++;
			return SIG_NULL;
		}
	default:
		{
			//�޶���
			return SIG_NULL;
		}
	}
}


uint8_t vqueFunNoErr(void)
{
	switch(coreProcess.funState)
	{
	case FUN_STATE_INIT:
		{
			//�޶��� ֱ���˳�
			return FUN_STATE_INIT;
		}
	case FUN_STATE_RUN:
		{
			//��Ҫ���ص��˳�֮ǰ�ĺ���@@@@@@@@@
			break;
		}
	case FUN_STATE_EXIT:
		{
			//�޶��� ֱ���˳�
			return FUN_STATE_EXIT;
		}
	default:
		{
			//�޶���
			return SIG_NULL;
		}
	}
	return SIG_NULL;
}


uint8_t vqueFunDefrost(void)
{
	switch(coreProcess.funState)
	{
	case FUN_STATE_INIT:
		{
			switch(timeFlag)
			{
			case Time3s:
				{
					//vRelaySet(Relay10Motor, STATE_ON);
					vRelaySet(Relay03Pump, STATE_ON);
					IODECT_startCheckWaterOpen();
					break;
				}
			case Time10s:
				{
					//2@@@@@@@@@@@���ˮ���պ� ��д.....
					//vRelaySet(Relay10Motor, STATE_ON);
					break;
				}
			case Time60s:
				{
					vRelaySet(Relay02Valve4way,STATE_ON);
					break;
				}
			case Time180s:
				{
					//����ѹ����
					vRelaySet(Relay01Compressor,STATE_ON);
					timeFlag=0;
					return FUN_STATE_INIT;
				}
			/*case Time360S:
				{
					timeFlag=0;
					return FUN_STATE_INIT;
				}*/
			default:break;
			}

			timeFlag++;
			return SIG_NULL;
		}
	case FUN_STATE_RUN:
		{
			//T��>T�趨��|| T��˪ʱ��>��˪�趨ʱ��
			if (coreProcess.coreParems.machineA.evaporateTemper >= coreProcess.coreParems.setDefrostOutTemper
				|| (timeFlag) >= coreProcess.coreParems.setDefrostRunTimes)
			{
				timeFlag=0;
				//Tˮ��>T�趨�����뱣��
				if (coreProcess.coreParems.waterBank >= coreProcess.coreParems.SetWaterTSet)
				{
					xQUESigPush(SIG_HOLD);
					return SIG_NULL;
				}
				else
				{
					xQUESigPush(SIG_ON);
					return SIG_NULL;
				}
			}
			timeFlag++;
			return SIG_NULL;
		}
	case FUN_STATE_EXIT:
		{
			//��ѹ����-60s����ͨ���ϵ�-��ˮ��
			switch(timeFlag)
			{
			case Time3s:
				{
					vRelaySet(Relay01Compressor,STATE_OFF);
					vRelaySet(Relay03Pump, STATE_OFF);
					IODECT_stopCheckWaterOpen();
					break;
				}
			case Time10s:
				{
					//vRelaySet(Relay10Motor, STATE_OFF);
					vRelaySet(Relay02Valve4way, STATE_OFF);
					timeFlag=0;
					return FUN_STATE_EXIT;
				}
			}
			timeFlag++;
			return SIG_NULL;
		}
	default:
		{
			//�޶���
			return SIG_NULL;
		}
	}
}

uint8_t vqueFunHold(void)
{
	switch(coreProcess.funState)
	{
	case FUN_STATE_INIT:
		{
			//�޶��� ֱ���˳�
			return FUN_STATE_INIT;
		}
	case FUN_STATE_RUN:
		{
			//Tˮ��<T�趨-�ز�������
			if (coreProcess.coreParems.waterBank <= (coreProcess.coreParems.SetWaterTSet
				-coreProcess.coreParems.setWaterCycleTemper))
			{
				xQUESigPush(SIG_ON);
			}
			return SIG_NULL;
		}
	case FUN_STATE_EXIT:
		{
			//�޶��� ֱ���˳�
			return FUN_STATE_EXIT;
		}
	default:
		{
			//�޶���
			return SIG_NULL;
		}
	}
}

uint8_t vqueFunLowT(void)
{
	switch(coreProcess.funState)
	{
	case FUN_STATE_INIT:
		{
			//�޶��� ֱ���˳�
			return FUN_STATE_INIT;
		}
	case FUN_STATE_RUN:
		{
			//�޶��� ֱ���˳�
			return SIG_NULL;
		}
	case FUN_STATE_EXIT:
		{
			//�޶��� ֱ���˳�
			return FUN_STATE_EXIT;
		}
	default:
		{
			//�޶���
			return SIG_NULL;
		}
	}
}

void vqueNormalEventProcess(void)
{
	uint8_t tSig;
	uint8_t tmstate;
	//������˳����е�run��־(״̬ת����ɱ�־)
	coreProcess.funExcuted = 0;
	//�ⲿ/�ڲ��¼�����ǿ��ת��
	coreProcess.funState=FUN_STATE_EXIT;

	tSig=xQUESigPop();

	if (tSig != SIG_NULL)
	{
		tmstate = vqueGetMachineState();
		vqueSetMachineState(tSig);

		timeFlag=0;
		switch(tSig)
		{
		case SIG_ERR:
			{
				//����ǰfun��Err�����˳�
				if (coreProcess.functions == vqueFunErr)
				{
					coreProcess.funExcuted = FUN_EXCUTED;
					coreProcess.funState=FUN_STATE_RUN;
				}
				else
				{
					vqueSetNewFuntion(vqueFunErr);
				}
				break;
			}
			//������no err
		case SIG_NOERR:
			{
				//ǰһ��������ΪҪ���еĺ���
				//vqueSetNewFuntion(vqueFunOn);
				break;
			}
		case SIG_OFF:
			{
				vqueSetNewFuntion(vqueFunOff);
				break;
			}
		default:
			{
				switch(tSig)
				{
				case SIG_ON:
					{
						vqueSetNewFuntion(vqueFunOn);
						break;
					}
				case SIG_DEFROST:
					{
						vqueSetNewFuntion(vqueFunDefrost);
						break;
					}
				case SIG_HOLD:
					{
						vqueSetNewFuntion(vqueFunHold);
						break;
					}
				case SIG_LOWT:
					{
						vqueSetNewFuntion(vqueFunLowT);
						break;
					}
				default:
					{
						//sig_null
						coreProcess.funExcuted = FUN_EXCUTED;
						coreProcess.funState=FUN_STATE_RUN;
						vqueSetMachineState(tmstate);
						break;
					}
				}
				break;
			}
		}
	}
}

void vqueExternalOffEvent(void)
{
	uint8_t tSig;
	//������init �� exit�׶Σ�����ǿ���˳�
	//1.get sig��popsig,timeflag to 0,set funoff,set funstate_init, set fun_excuted;
	//2.set machinestate to off
	tSig = xQUEgetSig();
	if (tSig == SIG_OFF)
	{
		tSig = xQUESigPop();
		timeFlag=0;
		vqueSetNewFuntion(vqueFunOff);
		//����init�׶�ʱ��Ҫ��temfun����ΪҪ���еĺ���
		coreProcess.funState=FUN_STATE_INIT;
		coreProcess.tempfun=coreProcess.functions;

		coreProcess.funExcuted = FUN_EXCUTED;
		vqueSetMachineState(SIG_OFF);
		IODECT_stopCheckWaterOpen();

	}
}

void vqueFunStateChangeFromFunction(uint8_t tSig)
{
	switch(tSig)
	{
	case FUN_STATE_EXIT:
		{
			//�˳�֮ǰ������һ�κ���ת��
			coreProcess.funState= FUN_STATE_INIT;
			coreProcess.tempfun=coreProcess.functions;
			break;
		}
	case FUN_STATE_INIT:
		{
			coreProcess.funState = FUN_STATE_RUN;
			break;
		}
	case FUN_STATE_RUN:
		{
			//�ڲ��¼��˳�-Ԥ�Ʋ���ִ�У���Ϊ�ڲ�ת��Ҳ���¼�����
			break;
		}
	default:
		{
			//��Ҫִ�ж�����ڣ���ת����ֱ���˳�
			break;
		}
	}
}

void vTask3QUEProcess(void)
{
	uint8_t tSig;
	if (coreProcess.funExcuted == FUN_EXCUTED)
	{
		vqueNormalEventProcess();
	}
	else{
		vqueExternalOffEvent();
	}

	tSig = coreProcess.tempfun();
	//״̬���е�runʱ Ӧ�����ñ���ת�����
	if (coreProcess.funExcuted != FUN_EXCUTED)
	{
		if (coreProcess.funState==FUN_STATE_RUN)
		{
			coreProcess.funExcuted = FUN_EXCUTED;
		}
	}

	vqueFunStateChangeFromFunction(tSig);
	
}


void vQUEInit(void)
{
	coreProcess.coreParems.SetWaterTSet= 55*10;
	coreProcess.coreParems.setWaterCycleTemper=5*10;
	coreProcess.coreParems.setDefrostInTemper=-7*10;
	coreProcess.coreParems.setDefrostRunTimes=8*60*20;
	coreProcess.coreParems.setDefrostOutTemper=10*10;

	coreProcess.coreParems.setDefrostCycleTimes=5*30*60*20;
	coreProcess.coreParems.setDefrostInOffset=3*10;

	coreProcess.funExcuted=FUN_EXCUTED;
	coreProcess.funState=FUN_STATE_RUN;
	coreProcess.tempfun=vqueFunOff;
	coreProcess.functions=vqueFunOff;
	coreProcess.prefunction=vqueFunOff;

	vqueSetMachineState(SIG_OFF);

}






//....................................................................
void vQUEGetTemperParams(Command3ReturnDataStruct *dstData)
{
	dstData->waterIn = coreProcess.coreParems.waterIn;
	dstData->waterOut = coreProcess.coreParems.waterOut;
	dstData->waterBank = coreProcess.coreParems.waterBank;

	dstData->evironT = coreProcess.coreParems.environT;
	dstData->innerTemper = coreProcess.coreParems.innerTemper;
	dstData->valvesteps = coreProcess.coreParems.valvesteps;


	dstData->machineA.inTemper = coreProcess.coreParems.machineA.inTemper;
	dstData->machineA.outTemper = coreProcess.coreParems.machineA.outTemper;
	dstData->machineA.evaporateTemper = coreProcess.coreParems.machineA.evaporateTemper;

	dstData->runState.machineState = coreProcess.runState.machineState;
	dstData->runState.elecState = coreProcess.runState.elecState;
	dstData->errType = coreProcess.coreParems.errState;
	//�� �� �� ˮ�� �� �� �� �������� adc���� cd4051���� �ڲ��¶�  (��λ��ǰ)
	
}

void vQueSetCoreParams(Command2RequestDataStruct *srcData)
{
	coreProcess.coreParems.SetWaterTSet= (srcData->setWaterT) * 10;
	coreProcess.coreParems.setWaterCycleTemper=(srcData->setWaterCycleTemper)* 10;
	coreProcess.coreParems.setDefrostInTemper=(srcData->setDefrostInTemper)* 10;
	coreProcess.coreParems.setDefrostRunTimes=(srcData->setDefrostRunTimes)*60*20;

	coreProcess.coreParems.setDefrostOutTemper=(srcData->setDefrostOutTemper)*10;
	coreProcess.coreParems.setDefrostCycleTimes=(srcData->setDefrostCycleTimes)*30*60*20;
	coreProcess.coreParems.setDefrostInOffset=(srcData->setDefrostInOffset)*10;
}


