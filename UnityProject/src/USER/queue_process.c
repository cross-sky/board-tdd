#include "cominc.h"

//.............
//满足单次高低压，排气启动条件,在init阶段检测,还必须返回信息给显示板
//.................

QueueProcess_t queueProcess={
	0,0
};

//1.init @@@@@@@@
CoreProcess_t coreProcess;

//进入/退出状态 结束后必须清0
uint32_t timeFlag=0;

uint8_t vqueFunOn(void);
ptrfuntion ptrWorkerModel = vqueFunOn;
void setWorkerModelFun( ptrfuntion  newModelFun)
{
	ptrWorkerModel = newModelFun;
}

//!!!!!!!每个case 后面必须配一个break/return 否则会处理出错！！！！！！！！

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

//获取吸气温度
int16_t iQUE_getInTemper(void)
{
	return coreProcess.coreParems.machineA.inTemper;
}

//获取蒸发温度
int16_t iQUE_getEvaporateTemper(void)
{
	uint16_t workModel = iQUE_getWorkerModel();
	if (workModel == SIG_MAKE_COLD)
	{
		//制冷时蒸发温度，选择进水探头
		return coreProcess.coreParems.waterIn;
	}
	else
	{
		//制热时蒸发温度，选择蒸发探头
		return coreProcess.coreParems.machineA.evaporateTemper;
	}
}

//获取排气温度
int16_t iQUE_getAirOutTemper(void)
{
	return coreProcess.coreParems.machineA.outTemper;
}

//获取环境温度
int16_t iQUE_getEvirTemper(void)
{
	return coreProcess.coreParems.environT;
}

//获取水箱温度
int16_t iQUE_getWaterBankTemper(void)
{
	return coreProcess.coreParems.waterBank;
}

int16_t iQUE_getWorkerModel(void)
{
	return coreProcess.coreParems.setWorkMode;
}

void iQUE_setWorkerModel(int16_t newstate)
{
	coreProcess.coreParems.setWorkMode = newstate;
}

int16_t iQUE_getColdModelSuperHeat(void)
{
	//排气-水温差来判断
	int16_t data=0;
	int16_t temp = iQUE_getAirOutTemper() - iQUE_getWaterBankTemper();
	if (temp <= 400)
	{
		data = 50;
	}else if(temp >= 700)
	{
		data = 20;
	}else{
		data = (int16_t)(temp*(-0.1)) + 90;
	}
	return data;
}

int16_t iQUE_getHotWaterModelSuperHeat(void)
{
	int16_t data=0;
	int16_t envirT = iQUE_getEvirTemper();
	if (envirT > 300)
	{
		data = 60;
	}
	else if (envirT < -100)
	{
		data = 20;
	}else{
		//-10时过热度2，0度时过热度3
		data = (int16_t)(envirT*0.1) + 30;
	}

	//水温>40度，过热度-1
	if (iQUE_getWaterBankTemper() >= 400)
	{
		data -=10;
	}
	return data;
}

//根据环温，水温确定合适的过热度
int16_t iQUE_getSuperheat(void)
{
	uint16_t workModel = iQUE_getWorkerModel();
	int16_t value=0;
	if (workModel == SIG_MAKE_HotWater)
	{
		value = iQUE_getHotWaterModelSuperHeat();
	}
	else if (workModel == SIG_MAKE_COLD)
	{
		value = iQUE_getColdModelSuperHeat();
	}
	return value;
}

void iQUE_ValveChanges(ValveKinds valveKind,uint16_t step)
{
	switch(valveKind)
	{
	case ValveMainA:
		coreProcess.coreParems.machineA.valveMainStep = step;
		break;
	case ValveSubB:
		coreProcess.coreParems.machineA.valveSubStep = step;
	default:
		break;
	}
}


///根据环温设置进入蒸发温度值     
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

int16_t iQUE_getUpperLimit(void)
{
	return coreProcess.coreParems.setAirout_water;
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

//运行在off and hold fun中
uint8_t vQueCheck3MinDelay(void)
{
	static uint8_t flag=STATE_OFF;
	static uint16_t delay3Min=STATE_OFF;
	//在funon and fundefrost 的runstate 状态下，重置为1
	if ((vqueGetMachineState() == SIG_ON || vqueGetMachineState() == SIG_DEFROST) && coreProcess.funState==FUN_STATE_RUN)
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

void vInpSetErrState(void)
{
	ErrStruct err;
	ERR_getErr(&err);
	coreProcess.coreParems.errState = err.errType;
}

void vQueSetCoreParamErrstate(uint16_t errType)
{
	coreProcess.coreParems.errState = errType;
}

//错误事件发生时转入， 
uint8_t vqueFunErr(void)
{
	switch(coreProcess.funState)
	{
	case FUN_STATE_INIT:
		{
			//@@@@停止继电器输出，电子膨胀阀调0？
			//执行1个周期,返回当前状态
			//500ms闪烁
			//vTPCChangeTaskRunTime(TASK_LED1ON, 100);
			return FUN_STATE_INIT;
		}
	case FUN_STATE_RUN:
		{
			//无动作
			//返回空信号，需要外部发出无错误事件，由外部事件进行强制退出
			return SIG_NULL;
		}
	case FUN_STATE_EXIT:
		{
			//退出后，把前一个函数指针赋值到当前运行指针
			//执行1个周期,返回当前状态
			//返回正常的1s闪烁
			//vTPCChangeTaskRunTime(TASK_LED1ON, 200);
			return FUN_STATE_EXIT;
		}
	default:
		{
			//无动作
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
			//无动作 直接退出
			vRelaySet(Relay09Motor, STATE_OFF);
			vRelaySet(Relay03CyclePump, STATE_OFF);
			vRelaySet(Relay02Valve4way, STATE_OFF);
			vRelaySet(Relay01Compressor, STATE_OFF);

			ValveCalc_setStartFlag(STATE_OFF);

			return FUN_STATE_INIT;
		}
	case FUN_STATE_RUN:
		{
			//无动作 直接退出
			//1.检查是否有模式切换，在关机状态下，电子膨胀阀队列应该执行玩了。
			checkWorkModelIsChange();

			vQueCheck3MinDelay();
			return SIG_NULL;
		}
	case FUN_STATE_EXIT:
		{
			//无动作 直接退出
			return FUN_STATE_EXIT;
		}
	default:
		{
			//无动作
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
			
			//无动作 直接退出
			switch(timeFlag)
			{
			case Time3s:
				{
					vRelaySet(Relay03CyclePump, STATE_ON);
					//vRelaySet(Relay02Valve4way, STATE_ON);
					IODECT_startCheckWaterOpen();
					break;
				}
			case Time10s:
				{
					//2@@@@@@@@@@@检测水流闭合,无温度异常，高低压闭合....



					vRelaySet(Relay09Motor, STATE_ON);
					if (vQueCheck3MinDelay() == STATE_ON)
					{
						timeFlag = Time180s-1;
					}
					break;
				}
			case Time180s:
				{
					//开启压缩机
					vRelaySet(Relay01Compressor,STATE_ON);
					timeFlag=0;

					ValveCalc_setStartFlag(STATE_ON);
					return FUN_STATE_INIT;
				}
			default:break;
			}

			timeFlag++;
			return SIG_NULL;
		}
	case FUN_STATE_RUN:
		{
			//无动作 直接退出
			//蒸发温度<设定值 进入除霜
			if (coreProcess.coreParems.machineA.evaporateTemper <= coreProcess.coreParems.setDefrostInTemper
				&& timeFlag>= (coreProcess.coreParems.setDefrostCycleTimes >>1))
			{
				timeFlag=0;
				xQUESigPush(SIG_DEFROST);
				return SIG_NULL;
			}
			//水箱温度>设定温度 进入保温
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
			//无动作 直接退出

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
					vRelaySet(Relay02Valve4way, STATE_OFF);
					vRelaySet(Relay09Motor, STATE_OFF);
					vRelaySet(Relay03CyclePump, STATE_OFF);
					timeFlag=0;
					ValveCalc_setStartFlag(STATE_OFF);
					return FUN_STATE_EXIT;
				}
			default:break;
			}
			timeFlag++;
			return SIG_NULL;
		}
	default:
		{
			//无动作
			return SIG_NULL;
		}
	}
}

//制冷模式
uint8_t vqueFunColdOn(void)
{

	switch(coreProcess.funState)
	{
	case FUN_STATE_INIT:
		{

			//无动作 直接退出
			switch(timeFlag)
			{
			case Time3s:
				{
					vRelaySet(Relay03CyclePump, STATE_ON);
					vRelaySet(Relay02Valve4way, STATE_ON);
					IODECT_startCheckWaterOpen();
					break;
				}
			case Time10s:
				{
					//2@@@@@@@@@@@检测水流闭合,无温度异常，高低压闭合....

					vRelaySet(Relay09Motor, STATE_ON);
					if (vQueCheck3MinDelay() == STATE_ON)
					{
						timeFlag = Time180s-1;
					}
					break;
				}
			case Time180s:
				{
					//开启压缩机
					vRelaySet(Relay01Compressor,STATE_ON);
					timeFlag=0;

					ValveCalc_setStartFlag(STATE_ON);
					return FUN_STATE_INIT;
				}
			default:break;
			}

			timeFlag++;
			return SIG_NULL;
		}
	case FUN_STATE_RUN:
		{
			//无动作 直接退出
			//蒸发温度<设定值 制冷模式没有除霜
			/*if (coreProcess.coreParems.machineA.evaporateTemper <= coreProcess.coreParems.setDefrostInTemper
				&& timeFlag>= coreProcess.coreParems.setDefrostCycleTimes>>1)
			{
				timeFlag=0;
				xQUESigPush(SIG_DEFROST);
				return SIG_NULL;
			}*/
			//水箱温度<设定温度 进入保温
			if (coreProcess.coreParems.waterBank <= coreProcess.coreParems.setColdWaterT)
			{
				timeFlag=0;
				xQUESigPush(SIG_COLDHOLD);
				return SIG_NULL;
			}
			timeFlag++;
			return SIG_NULL;
		}
	case FUN_STATE_EXIT:
		{
			//无动作 直接退出

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
					vRelaySet(Relay02Valve4way, STATE_OFF);
					vRelaySet(Relay09Motor, STATE_OFF);
					vRelaySet(Relay03CyclePump, STATE_OFF);
					timeFlag=0;
					ValveCalc_setStartFlag(STATE_OFF);
					return FUN_STATE_EXIT;
				}
			default:break;
			}
			timeFlag++;
			return SIG_NULL;
		}
	default:
		{
			//无动作
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
			//无动作 直接退出
			return FUN_STATE_INIT;
		}
	case FUN_STATE_RUN:
		{
			//需要返回到退出之前的函数@@@@@@@@@
			break;
		}
	case FUN_STATE_EXIT:
		{
			//无动作 直接退出
			return FUN_STATE_EXIT;
		}
	default:
		{
			//无动作
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
					//vRelaySet(Relay09Motor, STATE_ON);
					vRelaySet(Relay03CyclePump, STATE_ON);
					IODECT_startCheckWaterOpen();
					break;
				}
			case Time10s:
				{
					//2@@@@@@@@@@@检测水流闭合 待写.....
					//vRelaySet(Relay09Motor, STATE_ON);

					//ValveCalc_defrostValveSet();
					//除霜开度要调到最小
					ValveCalc_command5PushSig(-100, ValveMainA);
					break;
				}
			case Time60s:
				{
					vRelaySet(Relay02Valve4way,STATE_ON);
					break;
				}
			case Time180s:
				{
					//开启压缩机
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
			//T蒸>T设定：|| T除霜时间>除霜设定时间
			if (coreProcess.coreParems.machineA.evaporateTemper >= coreProcess.coreParems.setDefrostOutTemper
				|| (timeFlag) >= coreProcess.coreParems.setDefrostRunTimes)
			{
				timeFlag=0;
				//T水箱>T设定：进入保温
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
			//关压缩机-60s后四通阀断电-关水泵
			switch(timeFlag)
			{
			case Time3s:
				{
					vRelaySet(Relay01Compressor,STATE_OFF);
					vRelaySet(Relay03CyclePump, STATE_OFF);
					IODECT_stopCheckWaterOpen();
					ValveCalc_valveClose(ValveMainA);
					ValveCalc_stepsSetTo(ValveMainA, VALVE_INITRUN_STEP);
					break;
				}
			case Time10s:
				{
					//vRelaySet(Relay09Motor, STATE_OFF);
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
			//无动作
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
			//无动作 直接退出
			return FUN_STATE_INIT;
		}
	case FUN_STATE_RUN:
		{
			//T水箱<T设定-回差：进入加热
			if (coreProcess.coreParems.waterBank <= (coreProcess.coreParems.SetWaterTSet
				-coreProcess.coreParems.setWaterCycleTemper))
			{
				xQUESigPush(SIG_ON);
			}


			vQueCheck3MinDelay();
			return SIG_NULL;
		}
	case FUN_STATE_EXIT:
		{
			//无动作 直接退出
			return FUN_STATE_EXIT;
		}
	default:
		{
			//无动作
			return SIG_NULL;
		}
	}
}

uint8_t vqueFunColdHold(void)
{
	switch(coreProcess.funState)
	{
	case FUN_STATE_INIT:
		{
			//无动作 直接退出
			return FUN_STATE_INIT;
		}
	case FUN_STATE_RUN:
		{
			//T水箱<T设定-回差：进入加热
			if (coreProcess.coreParems.waterBank >= (coreProcess.coreParems.setColdWaterT
				+coreProcess.coreParems.setWaterCycleTemper))
			{
				xQUESigPush(SIG_COLDON);
			}


			vQueCheck3MinDelay();
			return SIG_NULL;
		}
	case FUN_STATE_EXIT:
		{
			//无动作 直接退出
			return FUN_STATE_EXIT;
		}
	default:
		{
			//无动作
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
			//无动作 直接退出
			return FUN_STATE_INIT;
		}
	case FUN_STATE_RUN:
		{
			//无动作 直接退出
			return SIG_NULL;
		}
	case FUN_STATE_EXIT:
		{
			//无动作 直接退出
			return FUN_STATE_EXIT;
		}
	default:
		{
			//无动作
			return SIG_NULL;
		}
	}
}

void vqueNormalEventProcess(void)
{
	uint8_t tSig;
	uint8_t tmstate;

	tSig=xQUESigPop();

	if (tSig != SIG_NULL)
	{
		//清除从退出运行到run标志(状态转换完成标志)
		coreProcess.funExcuted = 0;
		//外部/内部事件进行强制转换
		coreProcess.funState=FUN_STATE_EXIT;


		tmstate = vqueGetMachineState();
		vqueSetMachineState((SigState)tSig);

		timeFlag=0;
		switch(tSig)
		{
		case SIG_ERR:
			{
				//若当前fun是Err，则退出
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
			//？？？no err
		case SIG_NOERR:
			{
				//前一个函数置为要运行的函数
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
						vqueSetNewFuntion(ptrWorkerModel);
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
						vqueSetMachineState((SigState)tmstate);
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
	//在这里或者funoff中init设置所有关闭
	//程序处在init 或 exit阶段，进行强制退出
	//1.get sig，popsig,timeflag to 0,set funoff,set funstate_init, set fun_excuted;
	//2.set machinestate to off
	tSig = xQUEgetSig();
	if (tSig == SIG_OFF)
	{
		tSig = xQUESigPop();
		timeFlag=0;
		vqueSetNewFuntion(vqueFunOff);
		//进入init阶段时，要把temfun设置为要运行的函数
		coreProcess.funState=FUN_STATE_INIT;

		//运行funoff
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
			//退出之前进行下一次函数转换
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
			//内部事件退出-预计不会执行，因为内部转换也以事件产生
			break;
		}
	default:
		{
			//需要执行多个周期，无转换，直接退出
			break;
		}
	}
}

void vTaskQUEProcess(void)
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
	//状态运行到run时 应该设置本次转换完成
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

	coreProcess.coreParems.setAirout_water = 20*10;

	coreProcess.coreParems.setColdWaterT = 15*10;
	coreProcess.coreParems.setWorkMode = SIG_MAKE_HotWater;

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
	RelayAndValveDataStruct* relasValue;
	relasValue = vRelay_getDataStruct();
	dstData->waterSet = coreProcess.coreParems.SetWaterTSet;
	dstData->waterIn = coreProcess.coreParems.waterIn;
	dstData->waterOut = coreProcess.coreParems.waterOut;
	dstData->waterBank = coreProcess.coreParems.waterBank;

	dstData->evironT = coreProcess.coreParems.environT;
	dstData->innerTemper = coreProcess.coreParems.innerTemper;


	dstData->machineA.inTemper = coreProcess.coreParems.machineA.inTemper;
	dstData->machineA.outTemper = coreProcess.coreParems.machineA.outTemper;
	dstData->machineA.evaporateTemper = coreProcess.coreParems.machineA.evaporateTemper;
	dstData->machineA.valveMainStep = coreProcess.coreParems.machineA.valveMainStep;
	dstData->machineA.valveSubStep = coreProcess.coreParems.machineA.valveSubStep;
	dstData->machineA.current = coreProcess.coreParems.machineA.current;

	dstData->runState.machineState = coreProcess.runState.machineState;
	
	dstData->runState.elecState = coreProcess.runState.elecState;
	dstData->errType = coreProcess.coreParems.errState;
	dstData->cd4051DectState = coreProcess.coreParems.cd4051DectState;

	dstData->relaysValue = relasValue->relaysAndValveMainA;

	//进 出 环 水箱 吸 排 蒸 电流互感 adc错误 cd4051错误 内部温度  (高位在前)
	
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
	coreProcess.coreParems.setAirout_water = (srcData->setAirOut_WaterT)*10;
}


//only in off model then change work model
void vQUE_changeWorkerModel(void)
{
	int16_t workModel = iQUE_getWorkerModel();
	
	if (workModel == SIG_MAKE_COLD)
	{
		setWorkerModelFun(vqueFunColdOn);
	}
	else if (workModel == SIG_MAKE_HotWater)
	{
		setWorkerModelFun(vqueFunOn);
	}
}

void checkWorkModelIsChange(void)
{
	static uint16_t tvalue = SIG_MAKE_HotWater;
	uint16_t workModel = iQUE_getWorkerModel();
	if (workModel != tvalue)
	{
		tvalue = workModel;
		ValveCalc_WorkerModelChangeParams();
		vQUE_changeWorkerModel();
	}
}
